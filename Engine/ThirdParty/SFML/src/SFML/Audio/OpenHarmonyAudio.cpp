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

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <ohaudio/native_audio_routing_manager.h>
#include <ohaudio/native_audio_session_manager.h>
#include <ostream>
#include <unordered_map>
#include <unordered_set>


namespace sf::priv
{
namespace
{
class DeviceArray
{
public:
    explicit DeviceArray(OH_AudioRoutingManager* managerPtr) : manager(managerPtr)
    {
    }

    ~DeviceArray()
    {
        if (manager && array)
            OH_AudioRoutingManager_ReleaseDevices(manager, array);
    }

    DeviceArray(const DeviceArray&)            = delete;
    DeviceArray& operator=(const DeviceArray&) = delete;

    OH_AudioRoutingManager*        manager{};
    OH_AudioDeviceDescriptorArray* array{};
};

class SessionDeviceArray
{
public:
    explicit SessionDeviceArray(OH_AudioSessionManager* managerPtr) : manager(managerPtr)
    {
    }

    ~SessionDeviceArray()
    {
        if (manager && array)
            OH_AudioSessionManager_ReleaseDevices(manager, array);
    }

    SessionDeviceArray(const SessionDeviceArray&)            = delete;
    SessionDeviceArray& operator=(const SessionDeviceArray&) = delete;

    OH_AudioSessionManager*        manager{};
    OH_AudioDeviceDescriptorArray* array{};
};

std::unordered_set<std::uint32_t> getPreferredDeviceIds(OH_AudioRoutingManager* manager, OpenHarmonyAudioDeviceKind kind)
{
    DeviceArray           preferred(manager);
    OH_AudioCommon_Result result{};
    if (kind == OpenHarmonyAudioDeviceKind::Playback)
        result = OH_AudioRoutingManager_GetPreferredOutputDevice(manager, AUDIOSTREAM_USAGE_GAME, &preferred.array);
    else
        result = OH_AudioRoutingManager_GetPreferredInputDevice(manager, AUDIOSTREAM_SOURCE_TYPE_MIC, &preferred.array);

    std::unordered_set<std::uint32_t> ids;
    if ((result != AUDIOCOMMON_RESULT_SUCCESS) || !preferred.array)
        return ids;

    ids.reserve(preferred.array->size);
    for (std::uint32_t i = 0; i < preferred.array->size; ++i)
    {
        std::uint32_t id{};
        if (OH_AudioDeviceDescriptor_GetDeviceId(preferred.array->descriptors[i], &id) == AUDIOCOMMON_RESULT_SUCCESS)
            ids.insert(id);
    }

    return ids;
}

std::vector<OpenHarmonyAudioDevice> describeDevices(const OH_AudioDeviceDescriptorArray*     devices,
                                                    const std::unordered_set<std::uint32_t>& preferredIds)
{
    if (!devices)
        return {};

    std::vector<OpenHarmonyAudioDevice> result;
    result.reserve(devices->size);

    std::unordered_map<std::string, unsigned int> nameCounts;
    nameCounts.reserve(devices->size);

    for (std::uint32_t i = 0; i < devices->size; ++i)
    {
        auto* descriptor = devices->descriptors[i];
        if (!descriptor)
            continue;

        std::uint32_t       id{};
        OH_AudioDevice_Type type{AUDIO_DEVICE_TYPE_INVALID};
        char*               rawName{};

        if (OH_AudioDeviceDescriptor_GetDeviceId(descriptor, &id) != AUDIOCOMMON_RESULT_SUCCESS ||
            OH_AudioDeviceDescriptor_GetDeviceType(descriptor, &type) != AUDIOCOMMON_RESULT_SUCCESS ||
            OH_AudioDeviceDescriptor_GetDeviceName(descriptor, &rawName) != AUDIOCOMMON_RESULT_SUCCESS)
            continue;

        std::string name  = rawName && *rawName ? rawName : "OpenHarmony audio device " + std::to_string(id);
        auto&       count = nameCounts[name];
        ++count;
        if (count > 1)
            name += ' ' + std::to_string(count);

        std::uint32_t* rawSampleRates{};
        std::uint32_t  sampleRateCount{};
        std::uint32_t* rawChannelCounts{};
        std::uint32_t  channelCountCount{};
        static_cast<void>(OH_AudioDeviceDescriptor_GetDeviceSampleRates(descriptor, &rawSampleRates, &sampleRateCount));
        static_cast<void>(
            OH_AudioDeviceDescriptor_GetDeviceChannelCounts(descriptor, &rawChannelCounts, &channelCountCount));

        std::vector<std::uint32_t> sampleRates;
        std::vector<std::uint32_t> channelCounts;
        if (rawSampleRates)
            sampleRates.assign(rawSampleRates, rawSampleRates + sampleRateCount);
        if (rawChannelCounts)
            channelCounts.assign(rawChannelCounts, rawChannelCounts + channelCountCount);

        result.push_back({std::move(name),
                          id,
                          static_cast<std::int32_t>(type),
                          preferredIds.count(id) != 0,
                          std::move(sampleRates),
                          std::move(channelCounts)});
    }

    if (!result.empty() && std::none_of(result.begin(), result.end(), [](const auto& device) { return device.isDefault; }))
        result.front().isDefault = true;

    std::stable_partition(result.begin(), result.end(), [](const auto& device) { return device.isDefault; });
    return result;
}
} // namespace


////////////////////////////////////////////////////////////
bool selectOpenHarmonyAudioCaptureDevice(std::optional<std::uint32_t> deviceId)
{
    OH_AudioSessionManager* manager{};
    if ((OH_AudioManager_GetAudioSessionManager(&manager) != AUDIOCOMMON_RESULT_SUCCESS) || !manager)
    {
        err() << "Failed to get the OpenHarmony audio session manager" << std::endl;
        return false;
    }

    if (!deviceId)
    {
        if (OH_AudioSessionManager_SelectMediaInputDevice(manager, nullptr) == AUDIOCOMMON_RESULT_SUCCESS)
            return true;

        err() << "Failed to clear the OpenHarmony media input device selection" << std::endl;
        return false;
    }

    SessionDeviceArray devices(manager);
    if ((OH_AudioSessionManager_GetAvailableDevices(manager, AUDIO_DEVICE_USAGE_MEDIA_INPUT, &devices.array) !=
         AUDIOCOMMON_RESULT_SUCCESS) ||
        !devices.array)
    {
        err() << "Failed to enumerate selectable OpenHarmony media input devices" << std::endl;
        return false;
    }

    for (std::uint32_t i = 0; i < devices.array->size; ++i)
    {
        auto*         descriptor = devices.array->descriptors[i];
        std::uint32_t id{};
        if (descriptor && (OH_AudioDeviceDescriptor_GetDeviceId(descriptor, &id) == AUDIOCOMMON_RESULT_SUCCESS) &&
            (id == *deviceId))
        {
            if (OH_AudioSessionManager_SelectMediaInputDevice(manager, descriptor) == AUDIOCOMMON_RESULT_SUCCESS)
                return true;

            err() << "OpenHarmony rejected the selected media input device" << std::endl;
            return false;
        }
    }

    err() << "The selected OpenHarmony media input device is no longer available" << std::endl;
    return false;
}


////////////////////////////////////////////////////////////
std::vector<OpenHarmonyAudioDevice> getOpenHarmonyAudioDevices(OpenHarmonyAudioDeviceKind kind)
{
    OH_AudioRoutingManager* manager{};
    if (OH_AudioManager_GetAudioRoutingManager(&manager) != AUDIOCOMMON_RESULT_SUCCESS || !manager)
    {
        err() << "Failed to get the OpenHarmony audio routing manager" << std::endl;
        return {};
    }

    const auto preferredIds = getPreferredDeviceIds(manager, kind);

    if (kind == OpenHarmonyAudioDeviceKind::Capture)
    {
        OH_AudioSessionManager* sessionManager{};
        if ((OH_AudioManager_GetAudioSessionManager(&sessionManager) != AUDIOCOMMON_RESULT_SUCCESS) || !sessionManager)
        {
            err() << "Failed to get the OpenHarmony audio session manager" << std::endl;
            return {};
        }

        SessionDeviceArray devices(sessionManager);
        if ((OH_AudioSessionManager_GetAvailableDevices(sessionManager, AUDIO_DEVICE_USAGE_MEDIA_INPUT, &devices.array) !=
             AUDIOCOMMON_RESULT_SUCCESS) ||
            !devices.array)
        {
            err() << "Failed to enumerate selectable OpenHarmony media input devices" << std::endl;
            return {};
        }

        return describeDevices(devices.array, preferredIds);
    }

    // HarmonyOS lets media renderers follow the system route, but its public
    // native API only supports application-selected output devices for voice
    // communication usages. Expose the current GAME route as the single
    // selectable SFML playback entry instead of advertising devices that
    // PlaybackDevice::setDevice() cannot actually select.
    DeviceArray devices(manager);
    const auto  preferredResult = OH_AudioRoutingManager_GetPreferredOutputDevice(manager,
                                                                                 AUDIOSTREAM_USAGE_GAME,
                                                                                 &devices.array);
    if ((preferredResult != AUDIOCOMMON_RESULT_SUCCESS) || !devices.array)
    {
        if (devices.array)
        {
            OH_AudioRoutingManager_ReleaseDevices(manager, devices.array);
            devices.array = nullptr;
        }

        if ((OH_AudioRoutingManager_GetAvailableDevices(manager, AUDIO_DEVICE_USAGE_MEDIA_OUTPUT, &devices.array) !=
             AUDIOCOMMON_RESULT_SUCCESS) ||
            !devices.array)
        {
            err() << "Failed to enumerate the current OpenHarmony media output device" << std::endl;
            return {};
        }
    }

    auto result = describeDevices(devices.array, preferredIds);
    if (result.size() > 1)
        result.resize(1);
    if (!result.empty())
        result.front().isDefault = true;
    return result;
}


////////////////////////////////////////////////////////////
const char* getOpenHarmonyAudioStreamResultDescription(std::int32_t result)
{
    switch (result)
    {
        case AUDIOSTREAM_SUCCESS:
            return "success";
        case AUDIOSTREAM_ERROR_INVALID_PARAM:
            return "invalid parameter";
        case AUDIOSTREAM_ERROR_ILLEGAL_STATE:
            return "illegal state";
        case AUDIOSTREAM_ERROR_SYSTEM:
            return "system error";
        case AUDIOSTREAM_ERROR_UNSUPPORTED_FORMAT:
            return "unsupported format";
        default:
            return "unknown error";
    }
}

} // namespace sf::priv
