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
#include <SFML/Window/Event.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <array>
#include <deque>
#include <ostream>

#include <cstddef>


namespace sf::priv::Harmony
{
class EventQueue
{
public:
    void push(Event event)
    {
        if (event.is<Event::Closed>() &&
            std::any_of(m_events.begin(), m_events.end(), [](const Event& queued) { return queued.is<Event::Closed>(); }))
            return;

        // Coalescing is only safe for adjacent equivalent events. Crossing a
        // focus, key, button, or another finger's event changes causal order.
        if (!m_events.empty())
        {
            auto& previous = m_events.back();

            if (event.is<Event::MouseMoved>() && previous.is<Event::MouseMoved>())
            {
                previous = event;
                return;
            }

            if (const auto* moved = event.getIf<Event::TouchMoved>())
            {
                if (const auto* previousMoved = previous.getIf<Event::TouchMoved>();
                    previousMoved && previousMoved->finger == moved->finger)
                {
                    previous = event;
                    return;
                }
            }

            if (event.is<Event::Resized>() && previous.is<Event::Resized>())
            {
                previous = event;
                return;
            }
        }

        if (m_events.size() >= maximumQueuedEvents)
        {
            const auto droppable = std::find_if(m_events.begin(), m_events.end(), isDroppableEvent);
            if (droppable != m_events.end())
            {
                m_events.erase(droppable);
            }
            else if (event.is<Event::Closed>())
            {
                m_events.pop_front();
            }
            else
            {
                if (!m_overflowReported)
                {
                    m_overflowReported = true;
                    err() << "Harmony event queue reached its limit; dropping further non-lifecycle input" << std::endl;
                }
                return;
            }
        }

        m_events.emplace_back(event);
    }

    [[nodiscard]] std::deque<Event> drain()
    {
        std::deque<Event> result;
        result.swap(m_events);
        m_overflowReported = false;
        return result;
    }

private:
    static constexpr std::size_t maximumQueuedEvents = 65'536;

    static bool isDroppableEvent(const Event& event)
    {
        return event.is<Event::MouseMoved>() || event.is<Event::MouseWheelScrolled>() ||
               event.is<Event::TouchMoved>() || event.is<Event::Resized>();
    }

    std::deque<Event> m_events;
    bool              m_overflowReported{};
};


inline void releasePressedMouseButtons(std::array<bool, Mouse::ButtonCount>& buttons, Vector2i position, EventQueue& events)
{
    for (std::size_t index = 0; index < buttons.size(); ++index)
    {
        if (!buttons[index])
            continue;

        buttons[index] = false;
        events.push(Event::MouseButtonReleased{static_cast<Mouse::Button>(index), position});
    }
}

} // namespace sf::priv::Harmony
