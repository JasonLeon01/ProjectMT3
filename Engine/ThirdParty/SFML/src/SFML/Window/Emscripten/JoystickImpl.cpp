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

#include <SFML/Window/JoystickImpl.hpp>

#include <algorithm>
#include <array>
#include <emscripten/html5.h>
#include <string_view>

#include <cmath>

namespace sf::priv
{
namespace
{
bool readGamepad(unsigned int index, EmscriptenGamepadEvent& event)
{
    return index < Joystick::Count && emscripten_sample_gamepad_data() == EMSCRIPTEN_RESULT_SUCCESS &&
           static_cast<int>(index) < emscripten_get_num_gamepads() &&
           emscripten_get_gamepad_status(static_cast<int>(index), &event) == EMSCRIPTEN_RESULT_SUCCESS &&
           event.connected && std::string_view(event.mapping) == "standard";
}

float axisPosition(double value, double minimum)
{
    return std::isfinite(value) ? static_cast<float>(std::clamp(value, minimum, 1.0) * 100.0) : 0.f;
}
} // namespace

void JoystickImpl::initialize()
{
}

void JoystickImpl::cleanup()
{
}

bool JoystickImpl::isConnected(unsigned int index)
{
    EmscriptenGamepadEvent event{};
    return readGamepad(index, event);
}

bool JoystickImpl::open(unsigned int index)
{
    EmscriptenGamepadEvent event{};
    if (!readGamepad(index, event))
        return false;
    m_index                    = index;
    m_capabilities             = {};
    m_capabilities.buttonCount = static_cast<unsigned int>(
        std::clamp(event.numButtons, 0, static_cast<int>(Joystick::ButtonCount)));
    constexpr std::array axes{Joystick::Axis::X, Joystick::Axis::Y, Joystick::Axis::Z, Joystick::Axis::R};
    for (std::size_t axis = 0; axis < axes.size(); ++axis)
        m_capabilities.axes[axes[axis]] = static_cast<int>(axis) < event.numAxes;
    m_capabilities.axes[Joystick::Axis::U]    = event.numButtons > 6;
    m_capabilities.axes[Joystick::Axis::V]    = event.numButtons > 7;
    m_capabilities.axes[Joystick::Axis::PovX] = event.numButtons > 15;
    m_capabilities.axes[Joystick::Axis::PovY] = event.numButtons > 13;
    const std::string_view name(event.id);
    m_identification = {String::fromUtf8(name.begin(), name.end()), 0, 0};
    return true;
}

void JoystickImpl::close()
{
    m_capabilities   = {};
    m_identification = {};
}

JoystickCaps JoystickImpl::getCapabilities() const
{
    return m_capabilities;
}

Joystick::Identification JoystickImpl::getIdentification() const
{
    return m_identification;
}

JoystickState JoystickImpl::update() const
{
    EmscriptenGamepadEvent event{};
    JoystickState          state;
    if (!readGamepad(m_index, event))
        return state;
    state.connected = true;
    constexpr std::array axes{Joystick::Axis::X, Joystick::Axis::Y, Joystick::Axis::Z, Joystick::Axis::R};
    for (std::size_t axis = 0; axis < axes.size(); ++axis)
        if (static_cast<int>(axis) < event.numAxes)
            state.axes[axes[axis]] = axisPosition(event.axis[axis], -1.0);
    if (event.numButtons > 6)
        state.axes[Joystick::Axis::U] = axisPosition(event.analogButton[6], 0.0);
    if (event.numButtons > 7)
        state.axes[Joystick::Axis::V] = axisPosition(event.analogButton[7], 0.0);
    if (event.numButtons > 15)
        state.axes[Joystick::Axis::PovX] = 100.f * static_cast<float>(static_cast<int>(event.digitalButton[15]) -
                                                                      static_cast<int>(event.digitalButton[14]));
    if (event.numButtons > 13)
        state.axes[Joystick::Axis::PovY] = 100.f * static_cast<float>(static_cast<int>(event.digitalButton[12]) -
                                                                      static_cast<int>(event.digitalButton[13]));
    const auto buttons = static_cast<unsigned int>(std::clamp(event.numButtons, 0, static_cast<int>(Joystick::ButtonCount)));
    for (unsigned int button = 0; button < buttons; ++button)
        state.buttons[button] = event.digitalButton[button];
    return state;
}
} // namespace sf::priv
