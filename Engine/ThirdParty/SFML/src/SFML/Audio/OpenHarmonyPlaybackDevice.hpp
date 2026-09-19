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

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Audio/PlaybackDevice.hpp>

#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include <cstdint>


// NOLINTNEXTLINE(readability-identifier-naming)
struct ma_engine;


namespace sf::priv
{
////////////////////////////////////////////////////////////
/// \brief OpenHarmony playback sink for a no-device miniaudio engine
///
////////////////////////////////////////////////////////////
class OpenHarmonyPlaybackDevice
{
public:
    using NotificationSink = void (*)(PlaybackDevice::Notification);

    ////////////////////////////////////////////////////////////
    /// \brief Create an OHAudio renderer or a clocked null sink
    ///
    ////////////////////////////////////////////////////////////
    [[nodiscard]] static std::unique_ptr<OpenHarmonyPlaybackDevice> create(bool             useNull,
                                                                           std::mutex&      readingMutex,
                                                                           NotificationSink notificationSink);

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~OpenHarmonyPlaybackDevice();

    OpenHarmonyPlaybackDevice(const OpenHarmonyPlaybackDevice&)            = delete;
    OpenHarmonyPlaybackDevice& operator=(const OpenHarmonyPlaybackDevice&) = delete;

    ////////////////////////////////////////////////////////////
    /// \brief Start pulling frames from a configured miniaudio engine
    ///
    ////////////////////////////////////////////////////////////
    [[nodiscard]] bool start(ma_engine& engine);

    ////////////////////////////////////////////////////////////
    /// \brief Set the OHAudio renderer volume in the range [0, 1]
    ///
    ////////////////////////////////////////////////////////////
    void setVolume(float volume);

    ////////////////////////////////////////////////////////////
    /// \brief Get the selected device name
    ///
    ////////////////////////////////////////////////////////////
    [[nodiscard]] std::optional<std::string> getName() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the native stream sample rate
    ///
    ////////////////////////////////////////////////////////////
    [[nodiscard]] std::uint32_t getSampleRate() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the native stream channel count
    ///
    ////////////////////////////////////////////////////////////
    [[nodiscard]] std::uint32_t getChannelCount() const;

    ////////////////////////////////////////////////////////////
    /// \brief Check whether the selected device is the system default
    ///
    ////////////////////////////////////////////////////////////
    [[nodiscard]] bool isDefault() const;

private:
    struct Impl;

    OpenHarmonyPlaybackDevice(std::mutex& readingMutex, NotificationSink notificationSink);

    std::unique_ptr<Impl> m_impl;
};

} // namespace sf::priv
