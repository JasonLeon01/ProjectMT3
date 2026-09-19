////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2026 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Audio/OpenHarmonyAudio.hpp>
#include <SFML/Audio/SoundRecorder.hpp>

#include <SFML/System/Err.hpp>

#include <miniaudio.h>

#include <accesstoken/ability_access_control.h>
#include <algorithm>
#include <atomic>
#include <ohaudio/native_audiostreambuilder.h>
#include <ostream>
#include <semaphore.h>
#include <system_error>
#include <thread>

#include <cassert>
#include <cerrno>
#include <cstring>


namespace sf
{
struct SoundRecorder::Impl
{
    explicit Impl(SoundRecorder* ownerPtr) : owner(ownerPtr), deviceName(SoundRecorder::getDefaultDevice())
    {
        workerSemaphoreInitialized = sem_init(&workerSemaphore, 0, 0) == 0;
    }

    ~Impl()
    {
        requestWorkerStop();
        if (worker.joinable())
            worker.join();

        if (capturer)
            OH_AudioCapturer_Release(capturer);

        if (converter)
            ma_data_converter_uninit(&*converter, nullptr);

        if (workerSemaphoreInitialized)
            sem_destroy(&workerSemaphore);
    }

    bool initialize()
    {
        if (startedSession)
            return false;

        if (!workerSemaphoreInitialized)
        {
            err() << "Failed to initialize the OpenHarmony capture worker semaphore" << std::endl;
            return false;
        }

        const auto devices = priv::getOpenHarmonyAudioDevices(priv::OpenHarmonyAudioDeviceKind::Capture);
        if (deviceName.empty())
        {
            const auto defaultDevice = std::find_if(devices.begin(),
                                                    devices.end(),
                                                    [](const auto& device) { return device.isDefault; });
            if (defaultDevice != devices.end())
                deviceName = defaultDevice->name;
        }

        const auto iter = std::find_if(devices.begin(),
                                       devices.end(),
                                       [this](const auto& device) { return device.name == deviceName; });
        if (iter == devices.end())
        {
            err() << "The selected OpenHarmony audio capture device is not available: " << deviceName << std::endl;
            return false;
        }

        if (!priv::selectOpenHarmonyAudioCaptureDevice(selectedDeviceId))
            return false;

        if (converter)
        {
            ma_data_converter_uninit(&*converter, nullptr);
            converter.reset();
        }

        if (capturer)
        {
            OH_AudioCapturer_Release(capturer);
            capturer = nullptr;
        }

        const auto selectSupported =
            [](unsigned int requested, const std::vector<std::uint32_t>& supported, std::initializer_list<unsigned int> fallbacks)
        {
            if (supported.empty() || std::find(supported.begin(), supported.end(), requested) != supported.end())
                return requested;

            for (const auto fallback : fallbacks)
            {
                if (std::find(supported.begin(), supported.end(), fallback) != supported.end())
                    return fallback;
            }

            return supported.front();
        };

        nativeSampleRate   = selectSupported(sampleRate, iter->sampleRates, {48000, 44100});
        nativeChannelCount = selectSupported(channelCount, iter->channelCounts, {1, 2});

        if (!createCapturer(true))
        {
            const auto sampleRateCandidates = iter->sampleRates.empty() ? std::vector<std::uint32_t>{48000, 44100}
                                                                        : iter->sampleRates;
            const auto channelCandidates = iter->channelCounts.empty() ? std::vector<std::uint32_t>{1, 2} : iter->channelCounts;
            bool created{};
            for (const auto rate : sampleRateCandidates)
            {
                for (const auto channels : channelCandidates)
                {
                    if ((rate == nativeSampleRate) && (channels == nativeChannelCount))
                        continue;

                    nativeSampleRate   = rate;
                    nativeChannelCount = channels;
                    if ((nativeSampleRate > 0) && (nativeChannelCount > 0) && createCapturer(false))
                    {
                        created = true;
                        break;
                    }
                }

                if (created)
                    break;
            }

            if (!created)
            {
                err() << "Failed to find a usable native OpenHarmony capture format for the requested " << sampleRate
                      << " Hz, " << channelCount << " channel stream" << std::endl;
                return false;
            }
        }

        std::int32_t actualSampleRate{};
        std::int32_t actualChannelCount{};
        if ((OH_AudioCapturer_GetSamplingRate(capturer, &actualSampleRate) != AUDIOSTREAM_SUCCESS) ||
            (OH_AudioCapturer_GetChannelCount(capturer, &actualChannelCount) != AUDIOSTREAM_SUCCESS) ||
            (actualSampleRate <= 0) || (actualChannelCount <= 0))
        {
            err() << "OpenHarmony did not report a valid native capture format" << std::endl;
            OH_AudioCapturer_Release(capturer);
            capturer = nullptr;
            return false;
        }

        nativeSampleRate   = static_cast<unsigned int>(actualSampleRate);
        nativeChannelCount = static_cast<unsigned int>(actualChannelCount);

        if ((nativeSampleRate != sampleRate) || (nativeChannelCount != channelCount))
        {
            auto config = ma_data_converter_config_init(ma_format_s16,
                                                        ma_format_s16,
                                                        nativeChannelCount,
                                                        channelCount,
                                                        nativeSampleRate,
                                                        sampleRate);
            converter.emplace();
            if (const auto result = ma_data_converter_init(&config, nullptr, &*converter); result != MA_SUCCESS)
            {
                err() << "Failed to initialize OpenHarmony capture format conversion: " << ma_result_description(result)
                      << std::endl;
                converter.reset();
                OH_AudioCapturer_Release(capturer);
                capturer = nullptr;
                return false;
            }
        }

        // Two seconds absorbs scheduling jitter without allocating from the
        // OHAudio real-time callback. Conversion is performed by the worker.
        const auto  requiredRingSampleCount = std::max<std::size_t>(static_cast<std::size_t>(nativeSampleRate) *
                                                                       nativeChannelCount * 2,
                                                                   8192 * nativeChannelCount);
        std::size_t ringSampleCount{1};
        while (ringSampleCount < requiredRingSampleCount)
            ringSampleCount <<= 1;

        ringBuffer.assign(ringSampleCount, 0);
        ringIndexMask = ringSampleCount - 1;

        constexpr std::size_t nativeProcessFrameCount = 4096;
        nativeProcessBuffer.resize(nativeProcessFrameCount * nativeChannelCount);
        const auto outputFrameCount = std::max<std::size_t>((nativeProcessFrameCount * static_cast<std::size_t>(sampleRate) +
                                                             nativeSampleRate -
                                                             1) / nativeSampleRate +
                                                                64,
                                                            nativeProcessFrameCount);
        processBuffer.resize(outputFrameCount * channelCount);
        resetQueue();
        return true;
    }

    bool createCapturer(bool reportFailure)
    {
        capturer = nullptr;
        OH_AudioStreamBuilder* builder{};
        if (const auto result = OH_AudioStreamBuilder_Create(&builder, AUDIOSTREAM_TYPE_CAPTURER);
            result != AUDIOSTREAM_SUCCESS)
        {
            if (reportFailure)
                logStreamError("create the capture stream builder", result);
            return false;
        }

        const auto failBuilderOperation = [&builder, reportFailure](OH_AudioStream_Result result, const char* operation)
        {
            if (result == AUDIOSTREAM_SUCCESS)
                return false;

            if (reportFailure)
                logStreamError(operation, result);
            OH_AudioStreamBuilder_Destroy(builder);
            return true;
        };

        OH_AudioCapturer_Callbacks callbacks{};
        callbacks.OH_AudioCapturer_OnReadData = [](OH_AudioCapturer*, void* userData, void* buffer, std::int32_t length)
        {
            auto& impl = *static_cast<Impl*>(userData);
            if (buffer && (length > 0) && impl.running)
                impl.pushSamples(static_cast<const std::int16_t*>(buffer),
                                 static_cast<std::size_t>(length) / sizeof(std::int16_t));
            return 0;
        };
        callbacks.OH_AudioCapturer_OnStreamEvent = [](OH_AudioCapturer*, void* userData, OH_AudioStream_Event event)
        {
            auto& impl = *static_cast<Impl*>(userData);
            if (event == AUDIOSTREAM_EVENT_ROUTING_CHANGED)
            {
                impl.routeChanged = true;
                impl.wakeWorker();
            }
            return 0;
        };
        callbacks.OH_AudioCapturer_OnInterruptEvent =
            [](OH_AudioCapturer*, void* userData, OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint)
        {
            static_cast<Impl*>(userData)->queueInterruption(type, hint);
            return 0;
        };
        callbacks.OH_AudioCapturer_OnError = [](OH_AudioCapturer*, void* userData, OH_AudioStream_Result error)
        {
            auto& impl       = *static_cast<Impl*>(userData);
            impl.streamError = error;
            impl.fatalStop   = true;
            impl.wakeWorker();
            return 0;
        };

        if (failBuilderOperation(OH_AudioStreamBuilder_SetSamplingRate(builder, static_cast<std::int32_t>(nativeSampleRate)),
                                 "set the capture sample rate") ||
            failBuilderOperation(OH_AudioStreamBuilder_SetChannelCount(builder, static_cast<std::int32_t>(nativeChannelCount)),
                                 "set the capture channel count") ||
            failBuilderOperation(OH_AudioStreamBuilder_SetSampleFormat(builder, AUDIOSTREAM_SAMPLE_S16LE),
                                 "set the capture sample format") ||
            failBuilderOperation(OH_AudioStreamBuilder_SetEncodingType(builder, AUDIOSTREAM_ENCODING_TYPE_RAW),
                                 "set the capture encoding") ||
            failBuilderOperation(OH_AudioStreamBuilder_SetLatencyMode(builder, AUDIOSTREAM_LATENCY_MODE_NORMAL),
                                 "set the capture latency mode") ||
            failBuilderOperation(OH_AudioStreamBuilder_SetCapturerInfo(builder, AUDIOSTREAM_SOURCE_TYPE_MIC),
                                 "set the capture source") ||
            failBuilderOperation(OH_AudioStreamBuilder_SetCapturerCallback(builder, callbacks, this),
                                 "set capture callbacks") ||
            failBuilderOperation(OH_AudioStreamBuilder_GenerateCapturer(builder, &capturer),
                                 "generate the capture stream"))
        {
            if (capturer)
                OH_AudioCapturer_Release(capturer);
            capturer = nullptr;
            return false;
        }

        OH_AudioStreamBuilder_Destroy(builder);
        return true;
    }

    void pushSamples(const std::int16_t* samples, std::size_t sampleCount)
    {
        if (ringBuffer.empty() || !samples)
            return;

        // Preserve complete sample frames if OHAudio ever supplies a partial
        // trailing frame.
        sampleCount -= sampleCount % nativeChannelCount;
        const auto write     = writeIndex.load(std::memory_order_relaxed);
        const auto read      = readIndex.load(std::memory_order_acquire);
        const auto queued    = std::min<std::uint32_t>(write - read, static_cast<std::uint32_t>(ringBuffer.size()));
        const auto available = ringBuffer.size() - static_cast<std::size_t>(queued);
        const auto toWrite   = std::min(sampleCount, available - (available % nativeChannelCount));

        const auto offset    = static_cast<std::size_t>(write) & ringIndexMask;
        const auto firstPart = std::min(toWrite, ringBuffer.size() - offset);
        std::memcpy(ringBuffer.data() + offset, samples, firstPart * sizeof(std::int16_t));
        std::memcpy(ringBuffer.data(), samples + firstPart, (toWrite - firstPart) * sizeof(std::int16_t));

        writeIndex.store(write + static_cast<std::uint32_t>(toWrite), std::memory_order_release);
        if (toWrite < sampleCount)
            droppedSamples.fetch_add(static_cast<std::uint32_t>(sampleCount - toWrite), std::memory_order_relaxed);

        wakeWorker();
    }

    void queueInterruption(OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint)
    {
        const auto event = (static_cast<std::uint32_t>(type) << 16) | static_cast<std::uint32_t>(hint);
        if (!interruptionEvents.push(event))
        {
            overflowInterruption = event;
            interruptionOverflow = true;
        }

        interruptionChanged = true;
        wakeWorker();
    }

    void processInterruptions()
    {
        std::uint32_t event{};
        while (interruptionEvents.pop(event))
            processInterruption(static_cast<OH_AudioInterrupt_ForceType>(event >> 16),
                                static_cast<OH_AudioInterrupt_Hint>(event & 0xFFFF));

        if (interruptionOverflow.exchange(false, std::memory_order_acq_rel))
        {
            event = overflowInterruption.load(std::memory_order_acquire);
            processInterruption(static_cast<OH_AudioInterrupt_ForceType>(event >> 16),
                                static_cast<OH_AudioInterrupt_Hint>(event & 0xFFFF));
        }
    }

    void processInterruption(OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint)
    {
        enum PauseReason : std::uint32_t
        {
            PauseReasonHint = 1 << 0,
            MuteReasonHint  = 1 << 1
        };

        const bool applicationMustAct = type == AUDIOSTREAM_INTERRUPT_SHARE;
        switch (hint)
        {
            case AUDIOSTREAM_INTERRUPT_HINT_PAUSE:
            case AUDIOSTREAM_INTERRUPT_HINT_MUTE:
            {
                err() << "OpenHarmony interrupted the active audio capture stream" << std::endl;
                if (!applicationMustAct || !running || !capturer)
                    break;

                const auto reason     = hint == AUDIOSTREAM_INTERRUPT_HINT_PAUSE ? PauseReasonHint : MuteReasonHint;
                const auto oldReasons = applicationPauseReasons;
                applicationPauseReasons |= reason;
                if ((oldReasons == 0) && !nativePaused)
                {
                    const auto result = OH_AudioCapturer_Pause(capturer);
                    if (result == AUDIOSTREAM_SUCCESS)
                        nativePaused = true;
                    else
                    {
                        streamError = result;
                        fatalStop   = true;
                    }
                }
                break;
            }

            case AUDIOSTREAM_INTERRUPT_HINT_RESUME:
            case AUDIOSTREAM_INTERRUPT_HINT_UNMUTE:
            {
                if (!applicationMustAct || !running || !capturer)
                    break;

                const auto reason = hint == AUDIOSTREAM_INTERRUPT_HINT_RESUME ? PauseReasonHint : MuteReasonHint;
                applicationPauseReasons &= ~reason;
                if ((applicationPauseReasons == 0) && nativePaused)
                {
                    const auto result = OH_AudioCapturer_Start(capturer);
                    if (result == AUDIOSTREAM_SUCCESS)
                        nativePaused = false;
                    else
                    {
                        streamError = result;
                        fatalStop   = true;
                    }
                }
                break;
            }

            case AUDIOSTREAM_INTERRUPT_HINT_STOP:
                err() << "OpenHarmony stopped the active audio capture stream after an interruption" << std::endl;
                stopNativeStreamFromWorker();
                workerStop = true;
                readIndex.store(writeIndex.load(std::memory_order_acquire), std::memory_order_release);
                break;

            case AUDIOSTREAM_INTERRUPT_HINT_DUCK:
                err() << "OpenHarmony ducked another stream while audio capture remains active" << std::endl;
                break;

            default:
                break;
        }
    }

    void runWorker()
    {
        while (true)
        {
            int waitResult{};
            do
            {
                waitResult = sem_wait(&workerSemaphore);
            } while ((waitResult == -1) && (errno == EINTR));

            if (waitResult == -1)
                break;

            workerWakePending = false;

            if (workerStop)
            {
                stopNativeStreamFromWorker();
                readIndex.store(writeIndex.load(std::memory_order_acquire), std::memory_order_release);
            }

            if (routeChanged.exchange(false))
                err() << "OpenHarmony rerouted the active audio capture stream" << std::endl;

            if (interruptionChanged.exchange(false))
                processInterruptions();

            const auto dropped = droppedSamples.exchange(0);
            if (dropped > 0)
                err() << "OpenHarmony audio capture ring buffer overflowed; dropped " << dropped << " samples" << std::endl;

            if (fatalStop.exchange(false))
            {
                const auto error = streamError.exchange(AUDIOSTREAM_SUCCESS);
                if (error != AUDIOSTREAM_SUCCESS)
                    logStreamError("continue the capture stream", static_cast<OH_AudioStream_Result>(error));

                stopNativeStreamFromWorker();
                workerStop = true;
                readIndex.store(writeIndex.load(std::memory_order_acquire), std::memory_order_release);
            }

            while (readIndex.load(std::memory_order_relaxed) != writeIndex.load(std::memory_order_acquire))
            {
                if (workerStop)
                {
                    stopNativeStreamFromWorker();
                    readIndex.store(writeIndex.load(std::memory_order_acquire), std::memory_order_release);
                    break;
                }

                const auto read      = readIndex.load(std::memory_order_relaxed);
                const auto write     = writeIndex.load(std::memory_order_acquire);
                auto       available = static_cast<std::size_t>(write - read);
                available            = std::min(available, nativeProcessBuffer.size());
                available -= available % nativeChannelCount;
                if (available == 0)
                    break;

                const auto offset    = static_cast<std::size_t>(read) & ringIndexMask;
                const auto firstPart = std::min(available, ringBuffer.size() - offset);
                std::memcpy(nativeProcessBuffer.data(), ringBuffer.data() + offset, firstPart * sizeof(std::int16_t));
                std::memcpy(nativeProcessBuffer.data() + firstPart,
                            ringBuffer.data(),
                            (available - firstPart) * sizeof(std::int16_t));

                std::size_t consumedSamples{};
                std::size_t producedSamples{};
                if (converter)
                {
                    ma_uint64  inputFrames  = available / nativeChannelCount;
                    ma_uint64  outputFrames = processBuffer.size() / channelCount;
                    const auto result       = ma_data_converter_process_pcm_frames(&*converter,
                                                                             nativeProcessBuffer.data(),
                                                                             &inputFrames,
                                                                             processBuffer.data(),
                                                                             &outputFrames);
                    if (result != MA_SUCCESS)
                    {
                        err() << "Failed to convert OpenHarmony capture samples: " << ma_result_description(result)
                              << std::endl;
                        stopNativeStreamFromWorker();
                        workerStop = true;
                        readIndex.store(writeIndex.load(std::memory_order_acquire), std::memory_order_release);
                        break;
                    }

                    consumedSamples = static_cast<std::size_t>(inputFrames) * nativeChannelCount;
                    producedSamples = static_cast<std::size_t>(outputFrames) * channelCount;
                }
                else
                {
                    consumedSamples = available;
                    producedSamples = available;
                    std::memcpy(processBuffer.data(), nativeProcessBuffer.data(), available * sizeof(std::int16_t));
                }

                if (consumedSamples == 0)
                    break;

                readIndex.store(read + static_cast<std::uint32_t>(consumedSamples), std::memory_order_release);

                if ((producedSamples > 0) && !owner->onProcessSamples(processBuffer.data(), producedSamples))
                {
                    stopNativeStreamFromWorker();
                    workerStop = true;
                    readIndex.store(writeIndex.load(std::memory_order_acquire), std::memory_order_release);
                    break;
                }
            }

            if (workerStop && (readIndex.load(std::memory_order_relaxed) == writeIndex.load(std::memory_order_acquire)))
                break;
        }

        // This is deliberately the final operation: after publishing an idle
        // session the worker must never enter another derived-class callback.
        startedSession.store(false, std::memory_order_release);
    }

    void stopNativeStreamFromWorker()
    {
        if (running.exchange(false) && capturer)
        {
            const auto result = OH_AudioCapturer_Stop(capturer);
            if (result != AUDIOSTREAM_SUCCESS && result != AUDIOSTREAM_ERROR_ILLEGAL_STATE)
                logStreamError("stop the capture stream", result);
        }
    }

    void requestWorkerStop()
    {
        workerStop = true;
        wakeWorker();
    }

    void wakeWorker()
    {
        if (!workerWakePending.exchange(true, std::memory_order_acq_rel) && workerSemaphoreInitialized)
            static_cast<void>(sem_post(&workerSemaphore));
    }

    void resetQueue()
    {
        readIndex               = 0;
        writeIndex              = 0;
        droppedSamples          = 0;
        fatalStop               = false;
        routeChanged            = false;
        interruptionChanged     = false;
        interruptionOverflow    = false;
        streamError             = AUDIOSTREAM_SUCCESS;
        workerStop              = false;
        workerWakePending       = false;
        nativePaused            = false;
        applicationPauseReasons = 0;

        std::uint32_t ignoredEvent{};
        while (interruptionEvents.pop(ignoredEvent))
        {
        }
    }

    static void logStreamError(const char* operation, OH_AudioStream_Result result)
    {
        err() << "Failed to " << operation
              << " with OpenHarmony OHAudio: " << priv::getOpenHarmonyAudioStreamResultDescription(result);

        if ((result == AUDIOSTREAM_ERROR_SYSTEM) || (result == AUDIOSTREAM_ERROR_ILLEGAL_STATE))
            err() << ". Ensure ohos.permission.MICROPHONE is declared and granted before recording";

        err() << std::endl;
    }

    SoundRecorder* const                 owner;
    OH_AudioCapturer*                    capturer{};
    std::string                          deviceName;
    std::optional<std::uint32_t>         selectedDeviceId;
    unsigned int                         channelCount{1};
    unsigned int                         sampleRate{44100};
    std::vector<SoundChannel>            channelMap{SoundChannel::Mono};
    unsigned int                         nativeChannelCount{1};
    unsigned int                         nativeSampleRate{48000};
    std::vector<std::int16_t>            ringBuffer;
    std::size_t                          ringIndexMask{};
    std::vector<std::int16_t>            nativeProcessBuffer;
    std::vector<std::int16_t>            processBuffer;
    std::optional<ma_data_converter>     converter;
    std::atomic<std::uint32_t>           readIndex{};
    std::atomic<std::uint32_t>           writeIndex{};
    std::atomic<std::uint32_t>           droppedSamples{};
    std::atomic_bool                     running{};
    std::atomic_bool                     fatalStop{};
    std::atomic_bool                     routeChanged{};
    std::atomic_bool                     interruptionChanged{};
    priv::OpenHarmonyAudioEventQueue<32> interruptionEvents;
    std::atomic<std::uint32_t>           overflowInterruption{};
    std::atomic_bool                     interruptionOverflow{};
    std::atomic<std::int32_t>            streamError{AUDIOSTREAM_SUCCESS};
    sem_t                                workerSemaphore{};
    bool                                 workerSemaphoreInitialized{};
    std::atomic_bool                     workerWakePending{};
    std::thread                          worker;
    std::atomic_bool                     workerStop{};
    bool                                 nativePaused{};
    std::uint32_t                        applicationPauseReasons{};
    std::atomic_bool                     startedSession{};
    bool                                 completionPending{};
};


////////////////////////////////////////////////////////////
SoundRecorder::SoundRecorder() : m_impl(std::make_unique<Impl>(this))
{
}


////////////////////////////////////////////////////////////
SoundRecorder::~SoundRecorder()
{
    assert(!m_impl->startedSession &&
           "You must call stop() in the destructor of your derived class, so that the "
           "capture worker is stopped before your object is destroyed.");
}


////////////////////////////////////////////////////////////
bool SoundRecorder::start(unsigned int sampleRate)
{
    if (m_impl->startedSession)
    {
        err() << "Trying to start audio capture, but another capture is already running" << std::endl;
        return false;
    }

    // A previous session may have stopped itself because onProcessSamples()
    // returned false or OHAudio reported a fatal error. Reap its worker and
    // complete the derived-class lifecycle before starting a new session.
    if (m_impl->worker.joinable())
        m_impl->worker.join();

    if (m_impl->completionPending)
    {
        m_impl->completionPending = false;
        onStop();
    }

    if (!isAvailable())
    {
        err() << "Failed to start capture: OpenHarmony reports no permitted audio input device" << std::endl;
        return false;
    }

    if (sampleRate == 0)
    {
        err() << "Failed to start capture with a zero sample rate" << std::endl;
        return false;
    }

    m_impl->sampleRate = sampleRate;
    if (!m_impl->initialize())
        return false;

    if (!onStart())
        return false;

    m_impl->resetQueue();
    m_impl->completionPending = true;
    m_impl->startedSession    = true;
    m_impl->running           = true;

    const auto result = OH_AudioCapturer_Start(m_impl->capturer);
    if (result != AUDIOSTREAM_SUCCESS)
    {
        m_impl->running        = false;
        m_impl->startedSession = false;
        Impl::logStreamError("start the capture stream", result);
        m_impl->completionPending = false;
        onStop();
        return false;
    }

    try
    {
        m_impl->worker = std::thread([impl = m_impl.get()] { impl->runWorker(); });
    } catch (const std::system_error& exception)
    {
        static_cast<void>(OH_AudioCapturer_Stop(m_impl->capturer));
        m_impl->running           = false;
        m_impl->startedSession    = false;
        m_impl->completionPending = false;
        err() << "Failed to create the OpenHarmony capture worker: " << exception.what() << std::endl;
        onStop();
        return false;
    }

    return true;
}


////////////////////////////////////////////////////////////
void SoundRecorder::stop()
{
    if (!m_impl->startedSession && !m_impl->worker.joinable() && !m_impl->completionPending)
        return;

    m_impl->startedSession = false;
    m_impl->requestWorkerStop();
    if (m_impl->worker.joinable())
        m_impl->worker.join();

    if (m_impl->completionPending)
    {
        m_impl->completionPending = false;
        onStop();
    }
}


////////////////////////////////////////////////////////////
unsigned int SoundRecorder::getSampleRate() const
{
    return m_impl->sampleRate;
}


////////////////////////////////////////////////////////////
std::vector<std::string> SoundRecorder::getAvailableDevices()
{
    const auto               devices = priv::getOpenHarmonyAudioDevices(priv::OpenHarmonyAudioDeviceKind::Capture);
    std::vector<std::string> result;
    result.reserve(devices.size());
    for (const auto& device : devices)
        result.push_back(device.name);
    return result;
}


////////////////////////////////////////////////////////////
std::string SoundRecorder::getDefaultDevice()
{
    const auto devices = priv::getOpenHarmonyAudioDevices(priv::OpenHarmonyAudioDeviceKind::Capture);
    const auto iter = std::find_if(devices.begin(), devices.end(), [](const auto& device) { return device.isDefault; });
    return iter != devices.end() ? iter->name : std::string{};
}


////////////////////////////////////////////////////////////
bool SoundRecorder::setDevice(const std::string& name)
{
    const auto devices = priv::getOpenHarmonyAudioDevices(priv::OpenHarmonyAudioDeviceKind::Capture);
    const auto iter    = std::find_if(devices.begin(),
                                   devices.end(),
                                   [&name](const auto& device) { return device.name == name; });
    if (iter == devices.end())
    {
        if (m_impl->startedSession)
            stop();
        return false;
    }

    if (m_impl->deviceName == name)
        return true;

    if (!priv::selectOpenHarmonyAudioCaptureDevice(iter->id))
    {
        if (m_impl->startedSession)
            stop();
        return false;
    }

    m_impl->deviceName       = name;
    m_impl->selectedDeviceId = iter->id;
    return true;
}


////////////////////////////////////////////////////////////
const std::string& SoundRecorder::getDevice() const
{
    return m_impl->deviceName;
}


////////////////////////////////////////////////////////////
void SoundRecorder::setChannelCount(unsigned int channelCount)
{
    if (channelCount < 1 || channelCount > 2)
    {
        err() << "Unsupported channel count: " << channelCount
              << " Currently only mono (1) and stereo (2) recording is supported." << std::endl;
        return;
    }

    if (m_impl->channelCount == channelCount)
        return;

    if (m_impl->startedSession)
        stop();

    m_impl->channelCount = channelCount;
    m_impl->channelMap   = channelCount == 1 ? std::vector<SoundChannel>{SoundChannel::Mono}
                                             : std::vector<SoundChannel>{SoundChannel::FrontLeft, SoundChannel::FrontRight};
}


////////////////////////////////////////////////////////////
unsigned int SoundRecorder::getChannelCount() const
{
    return m_impl->channelCount;
}


////////////////////////////////////////////////////////////
const std::vector<SoundChannel>& SoundRecorder::getChannelMap() const
{
    return m_impl->channelMap;
}


////////////////////////////////////////////////////////////
bool SoundRecorder::isAvailable()
{
    // Device enumeration alone remains successful after a user denies the
    // microphone permission. API 21 exposes a cheap native self-check that
    // reflects the current runtime grant without opening a capture stream.
    return OH_AT_CheckSelfPermission("ohos.permission.MICROPHONE") &&
           !priv::getOpenHarmonyAudioDevices(priv::OpenHarmonyAudioDeviceKind::Capture).empty();
}


////////////////////////////////////////////////////////////
bool SoundRecorder::onStart()
{
    return true;
}


////////////////////////////////////////////////////////////
void SoundRecorder::onStop()
{
}

} // namespace sf
