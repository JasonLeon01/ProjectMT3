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
#include <SFML/Audio/OpenHarmonyPlaybackDevice.hpp>

#include <SFML/System/Err.hpp>

#include <miniaudio.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <ohaudio/native_audiostreambuilder.h>
#include <ostream>
#include <semaphore.h>
#include <system_error>
#include <thread>
#include <vector>

#include <cerrno>
#include <cstring>


namespace sf::priv
{
struct OpenHarmonyPlaybackDevice::Impl
{
    enum Event : std::uint32_t
    {
        NoEvent         = 0,
        RouteChanged    = 1 << 0,
        Interruption    = 1 << 1,
        StreamError     = 1 << 2,
        EngineReadError = 1 << 3,
        VolumeChanged   = 1 << 4,
        StopEventWorker = 1 << 5
    };

    Impl(std::mutex& readingMutexRef, NotificationSink sink) :
        readingMutex(&readingMutexRef),
        notificationSink(sink),
        eventSemaphoreInitialized(sem_init(&eventSemaphore, 0, 0) == 0)
    {
    }

    ~Impl()
    {
        // Make every subsequently-entering real-time callback a silent no-op,
        // then wait for an already-running engine read to leave its critical
        // section before either the renderer or the engine can be destroyed.
        callbacksEnabled = false;
        engine.store(nullptr, std::memory_order_release);
        if (readingMutex)
        {
            const std::lock_guard lock(*readingMutex);
        }

        wakeEventWorker(StopEventWorker);
        if (eventThread.joinable())
            eventThread.join();

        if (eventSemaphoreInitialized)
            sem_destroy(&eventSemaphore);

        nullSinkRunning = false;
        if (nullSinkThread.joinable())
            nullSinkThread.join();

        if (renderer)
        {
            if (started)
                OH_AudioRenderer_Stop(renderer);

            OH_AudioRenderer_Release(renderer);
        }

        if (started.exchange(false))
            notify(PlaybackDevice::Notification::DeviceStopped);
    }

    void notify(PlaybackDevice::Notification notification) const
    {
        if (notificationSink)
            notificationSink(notification);
    }

    [[nodiscard]] bool startEventWorker()
    {
        try
        {
            eventThread = std::thread(
                [this]
                {
                    while (true)
                    {
                        int waitResult{};
                        do
                        {
                            waitResult = sem_wait(&eventSemaphore);
                        } while ((waitResult == -1) && (errno == EINTR));

                        if (waitResult == -1)
                            break;

                        const auto events = pendingEvents.exchange(NoEvent, std::memory_order_acq_rel);

                        if (events & RouteChanged)
                            notify(PlaybackDevice::Notification::DeviceRerouted);

                        if (events & Interruption)
                            processInterruptions();

                        if (events & StreamError)
                        {
                            const auto error = lastError.exchange(AUDIOSTREAM_SUCCESS);
                            if (error != AUDIOSTREAM_SUCCESS)
                                err() << "OpenHarmony audio renderer failed: "
                                      << getOpenHarmonyAudioStreamResultDescription(error) << std::endl;

                            stopAfterError();
                        }

                        if (events & EngineReadError)
                        {
                            const auto error = engineReadError.exchange(MA_SUCCESS);
                            if (error != MA_SUCCESS)
                                err() << "Failed to read PCM frames from the OpenHarmony audio engine: "
                                      << ma_result_description(error) << std::endl;

                            stopAfterError();
                        }

                        if (events & VolumeChanged)
                            applyRendererVolume(false);

                        if (events & StopEventWorker)
                            break;
                    }
                });
        } catch (const std::system_error& exception)
        {
            err() << "Failed to create the OpenHarmony audio event worker: " << exception.what() << std::endl;
            return false;
        }

        return true;
    }

    void queueEvent(Event event)
    {
        if (!callbacksEnabled.load(std::memory_order_acquire))
            return;

        wakeEventWorker(event);
    }

    void wakeEventWorker(Event event)
    {
        const auto previous = pendingEvents.fetch_or(event, std::memory_order_acq_rel);
        if ((previous == NoEvent) && eventSemaphoreInitialized)
            static_cast<void>(sem_post(&eventSemaphore));
    }

    void setRendererVolume(float volume)
    {
        requestedVolume.store(std::clamp(volume, 0.f, 1.f), std::memory_order_relaxed);
        queueEvent(VolumeChanged);
    }

    void queueInterruption(OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint)
    {
        const auto event = (static_cast<std::uint32_t>(type) << 16) | static_cast<std::uint32_t>(hint);
        if (!interruptionEvents.push(event))
        {
            overflowInterruption = event;
            interruptionOverflow = true;
        }

        queueEvent(Interruption);
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
        enum Effect : std::uint32_t
        {
            PauseEffect = 1 << 0,
            DuckEffect  = 1 << 1,
            MuteEffect  = 1 << 2,
            StopEffect  = 1 << 3
        };

        const bool applicationMustAct = type == AUDIOSTREAM_INTERRUPT_SHARE;
        bool       begins{};
        bool       ends{};

        switch (hint)
        {
            case AUDIOSTREAM_INTERRUPT_HINT_PAUSE:
                begins = true;
                if (applicationMustAct && !nativePaused)
                {
                    if (started && renderer)
                    {
                        const auto result = OH_AudioRenderer_Pause(renderer);
                        if (result != AUDIOSTREAM_SUCCESS)
                        {
                            failStream(result, "pause after an interruption");
                            break;
                        }
                    }
                    nativePaused = true;
                }
                else if (!applicationMustAct)
                {
                    forcedEffects |= PauseEffect;
                }
                break;

            case AUDIOSTREAM_INTERRUPT_HINT_RESUME:
                ends = true;
                if (applicationMustAct && nativePaused)
                {
                    if (started && renderer)
                    {
                        const auto result = OH_AudioRenderer_Start(renderer);
                        if (result != AUDIOSTREAM_SUCCESS)
                        {
                            failStream(result, "resume after an interruption");
                            break;
                        }
                    }
                    nativePaused = false;
                }
                else if (!applicationMustAct)
                {
                    forcedEffects &= ~(PauseEffect | StopEffect);
                }
                break;

            case AUDIOSTREAM_INTERRUPT_HINT_STOP:
                begins = true;
                if (applicationMustAct && started && renderer)
                {
                    const auto result = OH_AudioRenderer_Stop(renderer);
                    if ((result != AUDIOSTREAM_SUCCESS) && (result != AUDIOSTREAM_ERROR_ILLEGAL_STATE))
                    {
                        failStream(result, "stop after an interruption");
                        break;
                    }
                }
                if (started.exchange(false))
                    notify(PlaybackDevice::Notification::DeviceStopped);
                nativePaused = false;
                if (!applicationMustAct)
                    forcedEffects |= StopEffect;
                break;

            case AUDIOSTREAM_INTERRUPT_HINT_DUCK:
                begins = true;
                if (applicationMustAct)
                {
                    ducked = true;
                    applyRendererVolume(true);
                }
                else
                {
                    forcedEffects |= DuckEffect;
                }
                break;

            case AUDIOSTREAM_INTERRUPT_HINT_UNDUCK:
                ends = true;
                if (applicationMustAct)
                {
                    ducked = false;
                    applyRendererVolume(true);
                }
                else
                {
                    forcedEffects &= ~DuckEffect;
                }
                break;

            case AUDIOSTREAM_INTERRUPT_HINT_MUTE:
                begins = true;
                if (applicationMustAct)
                {
                    muted = true;
                    applyRendererVolume(true);
                }
                else
                {
                    forcedEffects |= MuteEffect;
                }
                break;

            case AUDIOSTREAM_INTERRUPT_HINT_UNMUTE:
                ends = true;
                if (applicationMustAct)
                {
                    muted = false;
                    applyRendererVolume(true);
                }
                else
                {
                    forcedEffects &= ~MuteEffect;
                }
                break;

            default:
                break;
        }

        if (begins && !interruptionActive)
        {
            interruptionActive = true;
            notify(PlaybackDevice::Notification::DeviceInterruptionBegan);
        }

        if (ends && interruptionActive && !nativePaused && !ducked && !muted && (forcedEffects == 0))
        {
            interruptionActive = false;
            notify(PlaybackDevice::Notification::DeviceInterruptionEnded);
        }
    }

    void applyRendererVolume(bool fatalOnFailure)
    {
        if (!renderer || !started)
            return;

        const auto volume = muted ? 0.f : requestedVolume.load(std::memory_order_relaxed) * (ducked ? 0.2f : 1.f);
        if (const auto result = OH_AudioRenderer_SetVolume(renderer, volume); result != AUDIOSTREAM_SUCCESS)
        {
            if (fatalOnFailure)
                failStream(result, "set the interruption-adjusted volume");
            else
                err() << "Failed to set the OpenHarmony audio renderer volume: "
                      << getOpenHarmonyAudioStreamResultDescription(result) << std::endl;
        }
    }

    void failStream(OH_AudioStream_Result result, const char* operation)
    {
        err() << "Failed to " << operation << " for the OpenHarmony audio renderer: "
              << getOpenHarmonyAudioStreamResultDescription(result) << std::endl;

        if (renderer && started)
            static_cast<void>(OH_AudioRenderer_Stop(renderer));

        if (started.exchange(false))
            notify(PlaybackDevice::Notification::DeviceStopped);
    }

    void stopAfterError()
    {
        if (renderer && started)
            static_cast<void>(OH_AudioRenderer_Stop(renderer));

        if (started.exchange(false))
            notify(PlaybackDevice::Notification::DeviceStopped);
    }

    OH_AudioRenderer*              renderer{};
    std::mutex*                    readingMutex{};
    NotificationSink               notificationSink{};
    std::atomic<ma_engine*>        engine{};
    std::uint32_t                  sampleRate{48000};
    std::uint32_t                  channelCount{2};
    bool                           useNull{};
    std::vector<float>             conversionBuffer;
    std::atomic_bool               callbacksEnabled{};
    std::atomic_bool               started{};
    std::atomic_bool               nullSinkRunning{};
    std::atomic<float>             requestedVolume{1.f};
    std::atomic<std::int32_t>      lastError{AUDIOSTREAM_SUCCESS};
    std::atomic<ma_result>         engineReadError{MA_SUCCESS};
    OpenHarmonyAudioEventQueue<32> interruptionEvents;
    std::atomic<std::uint32_t>     overflowInterruption{};
    std::atomic_bool               interruptionOverflow{};
    std::atomic<std::uint32_t>     pendingEvents{};
    sem_t                          eventSemaphore{};
    bool                           eventSemaphoreInitialized{};
    bool                           nativePaused{};
    bool                           ducked{};
    bool                           muted{};
    bool                           interruptionActive{};
    std::uint32_t                  forcedEffects{};
    std::thread                    eventThread;
    std::thread                    nullSinkThread;
};


////////////////////////////////////////////////////////////
OpenHarmonyPlaybackDevice::OpenHarmonyPlaybackDevice(std::mutex& readingMutex, NotificationSink notificationSink) :
    m_impl(std::make_unique<Impl>(readingMutex, notificationSink))
{
}


////////////////////////////////////////////////////////////
OpenHarmonyPlaybackDevice::~OpenHarmonyPlaybackDevice() = default;


////////////////////////////////////////////////////////////
std::unique_ptr<OpenHarmonyPlaybackDevice> OpenHarmonyPlaybackDevice::create(bool             useNull,
                                                                             std::mutex&      readingMutex,
                                                                             NotificationSink notificationSink)
{
    auto playback = std::unique_ptr<OpenHarmonyPlaybackDevice>(
        new OpenHarmonyPlaybackDevice(readingMutex, notificationSink));
    auto& impl   = *playback->m_impl;
    impl.useNull = useNull;

    if (!impl.eventSemaphoreInitialized)
    {
        err() << "Failed to initialize the OpenHarmony audio event semaphore" << std::endl;
        return nullptr;
    }

    if (impl.useNull)
        return playback;

    OH_AudioStreamBuilder* builder{};
    if (const auto result = OH_AudioStreamBuilder_Create(&builder, AUDIOSTREAM_TYPE_RENDERER); result != AUDIOSTREAM_SUCCESS)
    {
        err() << "Failed to create OpenHarmony audio renderer builder: "
              << getOpenHarmonyAudioStreamResultDescription(result) << std::endl;
        return nullptr;
    }

    const auto failBuilderOperation = [&builder](OH_AudioStream_Result result, const char* operation)
    {
        if (result == AUDIOSTREAM_SUCCESS)
            return false;

        err() << "Failed to " << operation
              << " for OpenHarmony audio renderer: " << getOpenHarmonyAudioStreamResultDescription(result) << std::endl;
        OH_AudioStreamBuilder_Destroy(builder);
        return true;
    };

    OH_AudioRenderer_Callbacks callbacks{};
    callbacks.OH_AudioRenderer_OnWriteData = [](OH_AudioRenderer*, void* userData, void* buffer, std::int32_t length)
    {
        auto& callbackImpl = *static_cast<Impl*>(userData);
        if (!buffer || (length <= 0))
            return 0;

        std::memset(buffer, 0, static_cast<std::size_t>(length));

        if (!callbackImpl.callbacksEnabled.load(std::memory_order_acquire) || !callbackImpl.started ||
            !callbackImpl.readingMutex)
            return 0;

        if ((callbackImpl.channelCount == 0) || callbackImpl.conversionBuffer.empty())
            return 0;

        // A real-time renderer callback must never wait behind a resource
        // mutation or engine teardown. A missed cycle is emitted as the
        // silence already written above.
        const std::unique_lock lock(*callbackImpl.readingMutex, std::try_to_lock);
        if (!lock.owns_lock() || !callbackImpl.callbacksEnabled.load(std::memory_order_acquire))
            return 0;

        auto* const engine = callbackImpl.engine.load(std::memory_order_acquire);
        if (!engine)
            return 0;

        const auto  frameCount = static_cast<std::size_t>(length) / (sizeof(std::int16_t) * callbackImpl.channelCount);
        const auto  bufferFrameCapacity = callbackImpl.conversionBuffer.size() / callbackImpl.channelCount;
        auto*       output              = static_cast<std::int16_t*>(buffer);
        std::size_t frameOffset{};

        while (frameOffset < frameCount)
        {
            const auto framesToRead = std::min(frameCount - frameOffset, bufferFrameCapacity);
            ma_uint64  framesRead{};
            const auto result = ma_engine_read_pcm_frames(engine, callbackImpl.conversionBuffer.data(), framesToRead, &framesRead);
            if (result != MA_SUCCESS)
            {
                callbackImpl.engineReadError = result;
                callbackImpl.queueEvent(Impl::EngineReadError);
                break;
            }

            const auto sampleCount  = static_cast<std::size_t>(framesRead) * callbackImpl.channelCount;
            const auto sampleOffset = frameOffset * callbackImpl.channelCount;
            for (std::size_t i = 0; i < sampleCount; ++i)
            {
                const auto sample        = std::clamp(callbackImpl.conversionBuffer[i], -1.f, 1.f);
                output[sampleOffset + i] = static_cast<std::int16_t>(sample * 32767.f);
            }

            frameOffset += static_cast<std::size_t>(framesRead);
            if (framesRead < framesToRead)
                break;
        }

        return 0;
    };
    callbacks.OH_AudioRenderer_OnStreamEvent = [](OH_AudioRenderer*, void* userData, OH_AudioStream_Event event)
    {
        if (event == AUDIOSTREAM_EVENT_ROUTING_CHANGED)
            static_cast<Impl*>(userData)->queueEvent(Impl::RouteChanged);
        return 0;
    };
    callbacks.OH_AudioRenderer_OnInterruptEvent =
        [](OH_AudioRenderer*, void* userData, OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint)
    {
        static_cast<Impl*>(userData)->queueInterruption(type, hint);
        return 0;
    };
    callbacks.OH_AudioRenderer_OnError = [](OH_AudioRenderer*, void* userData, OH_AudioStream_Result error)
    {
        auto& callbackImpl     = *static_cast<Impl*>(userData);
        callbackImpl.lastError = error;
        callbackImpl.queueEvent(Impl::StreamError);
        return 0;
    };

    if (failBuilderOperation(OH_AudioStreamBuilder_SetSamplingRate(builder, static_cast<std::int32_t>(impl.sampleRate)),
                             "set the sample rate") ||
        failBuilderOperation(OH_AudioStreamBuilder_SetChannelCount(builder, static_cast<std::int32_t>(impl.channelCount)),
                             "set the channel count") ||
        failBuilderOperation(OH_AudioStreamBuilder_SetSampleFormat(builder, AUDIOSTREAM_SAMPLE_S16LE),
                             "set the sample format") ||
        failBuilderOperation(OH_AudioStreamBuilder_SetEncodingType(builder, AUDIOSTREAM_ENCODING_TYPE_RAW),
                             "set the encoding type") ||
        failBuilderOperation(OH_AudioStreamBuilder_SetLatencyMode(builder, AUDIOSTREAM_LATENCY_MODE_NORMAL),
                             "set the latency mode") ||
        failBuilderOperation(OH_AudioStreamBuilder_SetRendererInfo(builder, AUDIOSTREAM_USAGE_GAME),
                             "set the stream usage") ||
        failBuilderOperation(OH_AudioStreamBuilder_SetRendererCallback(builder, callbacks, &impl), "set callbacks") ||
        failBuilderOperation(OH_AudioStreamBuilder_GenerateRenderer(builder, &impl.renderer), "generate the stream"))
        return nullptr;

    OH_AudioStreamBuilder_Destroy(builder);

    std::int32_t actualSampleRate{};
    std::int32_t actualChannelCount{};
    if ((OH_AudioRenderer_GetSamplingRate(impl.renderer, &actualSampleRate) == AUDIOSTREAM_SUCCESS) &&
        (actualSampleRate > 0))
        impl.sampleRate = static_cast<std::uint32_t>(actualSampleRate);
    if ((OH_AudioRenderer_GetChannelCount(impl.renderer, &actualChannelCount) == AUDIOSTREAM_SUCCESS) &&
        (actualChannelCount > 0))
        impl.channelCount = static_cast<std::uint32_t>(actualChannelCount);

    constexpr std::size_t conversionFrameCapacity = 4096;
    impl.conversionBuffer.resize(conversionFrameCapacity * impl.channelCount);
    return playback;
}


////////////////////////////////////////////////////////////
bool OpenHarmonyPlaybackDevice::start(ma_engine& engine)
{
    auto& impl = *m_impl;

    if (impl.renderer)
    {
        impl.engine.store(&engine, std::memory_order_release);
        impl.callbacksEnabled = true;
        if (!impl.startEventWorker())
            return false;

        impl.started = true;
        if (const auto result = OH_AudioRenderer_Start(impl.renderer); result != AUDIOSTREAM_SUCCESS)
        {
            impl.started = false;
            err() << "Failed to start OpenHarmony audio renderer: " << getOpenHarmonyAudioStreamResultDescription(result)
                  << std::endl;
            return false;
        }

        impl.notify(PlaybackDevice::Notification::DeviceStarted);
        return true;
    }

    // Keep the node graph's clock advancing while the public null device is
    // selected, without opening an OHAudio renderer.
    impl.conversionBuffer.resize(256 * impl.channelCount);
    impl.engine.store(&engine, std::memory_order_release);
    impl.nullSinkRunning = true;

    try
    {
        impl.nullSinkThread = std::thread(
            [&impl]
            {
                const auto interval = std::chrono::duration<double>(256.0 / impl.sampleRate);
                auto       nextRead = std::chrono::steady_clock::now();

                while (impl.nullSinkRunning)
                {
                    nextRead += std::chrono::duration_cast<std::chrono::steady_clock::duration>(interval);
                    {
                        const std::lock_guard lock(*impl.readingMutex);
                        if (auto* const enginePtr = impl.engine.load(std::memory_order_acquire))
                            ma_engine_read_pcm_frames(enginePtr, impl.conversionBuffer.data(), 256, nullptr);
                    }
                    std::this_thread::sleep_until(nextRead);
                }
            });
    } catch (const std::system_error& exception)
    {
        impl.nullSinkRunning = false;
        impl.engine.store(nullptr, std::memory_order_release);
        err() << "Failed to create the OpenHarmony null audio worker: " << exception.what() << std::endl;
        return false;
    }

    impl.started = true;
    impl.notify(PlaybackDevice::Notification::DeviceStarted);
    return true;
}


////////////////////////////////////////////////////////////
void OpenHarmonyPlaybackDevice::setVolume(float volume)
{
    if (m_impl->renderer)
        m_impl->setRendererVolume(volume);
}


////////////////////////////////////////////////////////////
std::optional<std::string> OpenHarmonyPlaybackDevice::getName() const
{
    if (m_impl->useNull)
        return "Null Audio Device";

    const auto devices = getOpenHarmonyAudioDevices(OpenHarmonyAudioDeviceKind::Playback);
    const auto iter = std::find_if(devices.begin(), devices.end(), [](const auto& device) { return device.isDefault; });
    return iter != devices.end() ? std::optional<std::string>(iter->name) : std::nullopt;
}


////////////////////////////////////////////////////////////
std::uint32_t OpenHarmonyPlaybackDevice::getSampleRate() const
{
    return m_impl->sampleRate;
}


////////////////////////////////////////////////////////////
std::uint32_t OpenHarmonyPlaybackDevice::getChannelCount() const
{
    return m_impl->channelCount;
}


////////////////////////////////////////////////////////////
bool OpenHarmonyPlaybackDevice::isDefault() const
{
    if (m_impl->useNull)
        return false;

    const auto current = getName();
    if (!current)
        return false;

    const auto devices = getOpenHarmonyAudioDevices(OpenHarmonyAudioDeviceKind::Playback);
    const auto iter    = std::find_if(devices.begin(),
                                   devices.end(),
                                   [&current](const auto& device) { return device.name == *current; });
    return iter != devices.end() && iter->isDefault;
}

} // namespace sf::priv
