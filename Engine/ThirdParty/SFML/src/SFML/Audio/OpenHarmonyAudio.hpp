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
#include <array>
#include <atomic>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include <cstddef>
#include <cstdint>


namespace sf::priv
{
////////////////////////////////////////////////////////////
/// \brief Fixed-capacity lock-free queue for native audio callbacks
///
/// The queue is multi-producer/single-consumer. It performs no allocation
/// and only uses pointer-sized atomics, including on the 32-bit ABI.
///
////////////////////////////////////////////////////////////
template <std::size_t Capacity>
class OpenHarmonyAudioEventQueue
{
    static_assert(Capacity > 1);
    static_assert(std::atomic<std::size_t>::is_always_lock_free);

public:
    OpenHarmonyAudioEventQueue()
    {
        for (std::size_t i = 0; i < Capacity; ++i)
            m_slots[i].sequence.store(i, std::memory_order_relaxed);
    }

    OpenHarmonyAudioEventQueue(const OpenHarmonyAudioEventQueue&)            = delete;
    OpenHarmonyAudioEventQueue& operator=(const OpenHarmonyAudioEventQueue&) = delete;

    [[nodiscard]] bool push(std::uint32_t value)
    {
        auto position = m_enqueuePosition.load(std::memory_order_relaxed);
        while (true)
        {
            auto&      slot       = m_slots[position % Capacity];
            const auto sequence   = slot.sequence.load(std::memory_order_acquire);
            const auto difference = static_cast<std::make_signed_t<std::size_t>>(sequence - position);

            if (difference == 0)
            {
                if (m_enqueuePosition.compare_exchange_weak(position, position + 1, std::memory_order_relaxed, std::memory_order_relaxed))
                {
                    slot.value = value;
                    slot.sequence.store(position + 1, std::memory_order_release);
                    return true;
                }
            }
            else if (difference < 0)
            {
                return false;
            }
            else
            {
                position = m_enqueuePosition.load(std::memory_order_relaxed);
            }
        }
    }

    [[nodiscard]] bool pop(std::uint32_t& value)
    {
        auto&      slot       = m_slots[m_dequeuePosition % Capacity];
        const auto sequence   = slot.sequence.load(std::memory_order_acquire);
        const auto difference = static_cast<std::make_signed_t<std::size_t>>(sequence - (m_dequeuePosition + 1));
        if (difference != 0)
            return false;

        value = slot.value;
        slot.sequence.store(m_dequeuePosition + Capacity, std::memory_order_release);
        ++m_dequeuePosition;
        return true;
    }

private:
    struct Slot
    {
        std::atomic<std::size_t> sequence{};
        std::uint32_t            value{};
    };

    std::array<Slot, Capacity> m_slots{};
    std::atomic<std::size_t>   m_enqueuePosition{};
    std::size_t                m_dequeuePosition{};
};

////////////////////////////////////////////////////////////
/// \brief Device information obtained from OHAudio's routing manager
///
////////////////////////////////////////////////////////////
struct OpenHarmonyAudioDevice
{
    std::string                name;
    std::uint32_t              id{};
    std::int32_t               type{};
    bool                       isDefault{};
    std::vector<std::uint32_t> sampleRates;
    std::vector<std::uint32_t> channelCounts;
};

enum class OpenHarmonyAudioDeviceKind
{
    Playback,
    Capture
};

////////////////////////////////////////////////////////////
/// \brief Enumerate media devices through OHAudio
///
////////////////////////////////////////////////////////////
[[nodiscard]] std::vector<OpenHarmonyAudioDevice> getOpenHarmonyAudioDevices(OpenHarmonyAudioDeviceKind kind);

////////////////////////////////////////////////////////////
/// \brief Select an API 21 media input device, or clear the selection
///
////////////////////////////////////////////////////////////
[[nodiscard]] bool selectOpenHarmonyAudioCaptureDevice(std::optional<std::uint32_t> deviceId);

////////////////////////////////////////////////////////////
/// \brief Convert an OHAudio stream result to a diagnostic string
///
////////////////////////////////////////////////////////////
[[nodiscard]] const char* getOpenHarmonyAudioStreamResultDescription(std::int32_t result);

} // namespace sf::priv
