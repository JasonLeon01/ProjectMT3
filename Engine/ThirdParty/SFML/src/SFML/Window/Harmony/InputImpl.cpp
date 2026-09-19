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

#include <SFML/Window/Harmony/InputMethodImpl.hpp>
#include <SFML/Window/Harmony/NativeAppImpl.hpp>
#include <SFML/Window/InputImpl.hpp>
#include <SFML/Window/WindowBase.hpp>

#include <SFML/System/String.hpp>

#include <string>

#include <cmath>

#ifdef SFML_HARMONY_2IN1
#include <multimodalinput/oh_input_manager.h>
#include <window_manager/oh_display_manager.h>
#include <window_manager/oh_window.h>
#endif


#ifdef SFML_HARMONY_2IN1
namespace
{
bool getClientOrigin(sf::Vector2i& origin)
{
    auto&        host = sf::priv::Harmony::getHostState();
    std::int32_t windowId{};
    {
        const std::lock_guard lock(host.mutex);
        windowId = host.windowId;
    }

    WindowManager_WindowProperties properties{};
    if (windowId <= 0 || OH_WindowManager_GetWindowProperties(windowId, &properties) != OK)
        return false;

    std::int32_t displayX{};
    std::int32_t displayY{};
    if (OH_NativeDisplayManager_GetDisplayPosition(properties.displayId, &displayX, &displayY) != DISPLAY_MANAGER_OK)
    {
        displayX = 0;
        displayY = 0;
    }

    origin = {displayX + properties.windowRect.posX + properties.drawableRect.posX,
              displayY + properties.windowRect.posY + properties.drawableRect.posY};
    return true;
}
} // namespace
#endif


namespace sf::priv::InputImpl
{
bool isKeyPressed(Keyboard::Key key)
{
    auto&                 state = Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    return state.keys.count(key) != 0;
}


bool isKeyPressed(Keyboard::Scancode code)
{
    auto&                 state = Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    return state.scancodes.count(code) != 0;
}


Keyboard::Key localize(Keyboard::Scancode code)
{
    if (code >= Keyboard::Scan::A && code <= Keyboard::Scan::Z)
        return static_cast<Keyboard::Key>(
            static_cast<int>(Keyboard::Key::A) + (static_cast<int>(code) - static_cast<int>(Keyboard::Scan::A)));
    if (code >= Keyboard::Scan::F1 && code <= Keyboard::Scan::F15)
        return static_cast<Keyboard::Key>(
            static_cast<int>(Keyboard::Key::F1) + (static_cast<int>(code) - static_cast<int>(Keyboard::Scan::F1)));
    switch (code)
    {
        case Keyboard::Scan::Num0:
            return Keyboard::Key::Num0;
        case Keyboard::Scan::Num1:
            return Keyboard::Key::Num1;
        case Keyboard::Scan::Num2:
            return Keyboard::Key::Num2;
        case Keyboard::Scan::Num3:
            return Keyboard::Key::Num3;
        case Keyboard::Scan::Num4:
            return Keyboard::Key::Num4;
        case Keyboard::Scan::Num5:
            return Keyboard::Key::Num5;
        case Keyboard::Scan::Num6:
            return Keyboard::Key::Num6;
        case Keyboard::Scan::Num7:
            return Keyboard::Key::Num7;
        case Keyboard::Scan::Num8:
            return Keyboard::Key::Num8;
        case Keyboard::Scan::Num9:
            return Keyboard::Key::Num9;
        case Keyboard::Scan::Escape:
            return Keyboard::Key::Escape;
        case Keyboard::Scan::LControl:
            return Keyboard::Key::LControl;
        case Keyboard::Scan::LShift:
            return Keyboard::Key::LShift;
        case Keyboard::Scan::LAlt:
            return Keyboard::Key::LAlt;
        case Keyboard::Scan::LSystem:
            return Keyboard::Key::LSystem;
        case Keyboard::Scan::RControl:
            return Keyboard::Key::RControl;
        case Keyboard::Scan::RShift:
            return Keyboard::Key::RShift;
        case Keyboard::Scan::RAlt:
            return Keyboard::Key::RAlt;
        case Keyboard::Scan::RSystem:
            return Keyboard::Key::RSystem;
        case Keyboard::Scan::Menu:
            return Keyboard::Key::Menu;
        case Keyboard::Scan::LBracket:
            return Keyboard::Key::LBracket;
        case Keyboard::Scan::RBracket:
            return Keyboard::Key::RBracket;
        case Keyboard::Scan::Semicolon:
            return Keyboard::Key::Semicolon;
        case Keyboard::Scan::Comma:
            return Keyboard::Key::Comma;
        case Keyboard::Scan::Period:
            return Keyboard::Key::Period;
        case Keyboard::Scan::Apostrophe:
            return Keyboard::Key::Apostrophe;
        case Keyboard::Scan::Slash:
            return Keyboard::Key::Slash;
        case Keyboard::Scan::Backslash:
            return Keyboard::Key::Backslash;
        case Keyboard::Scan::Grave:
            return Keyboard::Key::Grave;
        case Keyboard::Scan::Equal:
            return Keyboard::Key::Equal;
        case Keyboard::Scan::Hyphen:
            return Keyboard::Key::Hyphen;
        case Keyboard::Scan::Space:
            return Keyboard::Key::Space;
        case Keyboard::Scan::Enter:
        case Keyboard::Scan::NumpadEnter:
            return Keyboard::Key::Enter;
        case Keyboard::Scan::Backspace:
            return Keyboard::Key::Backspace;
        case Keyboard::Scan::Tab:
            return Keyboard::Key::Tab;
        case Keyboard::Scan::PageUp:
            return Keyboard::Key::PageUp;
        case Keyboard::Scan::PageDown:
            return Keyboard::Key::PageDown;
        case Keyboard::Scan::End:
            return Keyboard::Key::End;
        case Keyboard::Scan::Home:
            return Keyboard::Key::Home;
        case Keyboard::Scan::Insert:
            return Keyboard::Key::Insert;
        case Keyboard::Scan::Delete:
            return Keyboard::Key::Delete;
        case Keyboard::Scan::NumpadPlus:
            return Keyboard::Key::Add;
        case Keyboard::Scan::NumpadMinus:
            return Keyboard::Key::Subtract;
        case Keyboard::Scan::NumpadMultiply:
            return Keyboard::Key::Multiply;
        case Keyboard::Scan::NumpadDivide:
            return Keyboard::Key::Divide;
        case Keyboard::Scan::Left:
            return Keyboard::Key::Left;
        case Keyboard::Scan::Right:
            return Keyboard::Key::Right;
        case Keyboard::Scan::Up:
            return Keyboard::Key::Up;
        case Keyboard::Scan::Down:
            return Keyboard::Key::Down;
        case Keyboard::Scan::Numpad0:
            return Keyboard::Key::Numpad0;
        case Keyboard::Scan::Numpad1:
            return Keyboard::Key::Numpad1;
        case Keyboard::Scan::Numpad2:
            return Keyboard::Key::Numpad2;
        case Keyboard::Scan::Numpad3:
            return Keyboard::Key::Numpad3;
        case Keyboard::Scan::Numpad4:
            return Keyboard::Key::Numpad4;
        case Keyboard::Scan::Numpad5:
            return Keyboard::Key::Numpad5;
        case Keyboard::Scan::Numpad6:
            return Keyboard::Key::Numpad6;
        case Keyboard::Scan::Numpad7:
            return Keyboard::Key::Numpad7;
        case Keyboard::Scan::Numpad8:
            return Keyboard::Key::Numpad8;
        case Keyboard::Scan::Numpad9:
            return Keyboard::Key::Numpad9;
        case Keyboard::Scan::Pause:
            return Keyboard::Key::Pause;
        default:
            return Keyboard::Key::Unknown;
    }
}


Keyboard::Scancode delocalize(Keyboard::Key key)
{
    if (key >= Keyboard::Key::A && key <= Keyboard::Key::Z)
        return static_cast<Keyboard::Scancode>(
            static_cast<int>(Keyboard::Scan::A) + (static_cast<int>(key) - static_cast<int>(Keyboard::Key::A)));

    for (unsigned int index = 0; index < Keyboard::ScancodeCount; ++index)
    {
        const auto scan = static_cast<Keyboard::Scancode>(index);
        if (InputImpl::localize(scan) == key)
            return scan;
    }
    return Keyboard::Scan::Unknown;
}


String getDescription(Keyboard::Scancode code)
{
    if (code >= Keyboard::Scan::A && code <= Keyboard::Scan::Z)
    {
        const char character = static_cast<char>('A' + static_cast<int>(code) - static_cast<int>(Keyboard::Scan::A));
        return {character};
    }
    if (code >= Keyboard::Scan::Num1 && code <= Keyboard::Scan::Num9)
    {
        const char character = static_cast<char>('1' + static_cast<int>(code) - static_cast<int>(Keyboard::Scan::Num1));
        return {character};
    }
    if (code >= Keyboard::Scan::F1 && code <= Keyboard::Scan::F24)
        return {"F" + std::to_string(static_cast<int>(code) - static_cast<int>(Keyboard::Scan::F1) + 1)};
    switch (code)
    {
        case Keyboard::Scan::Num0:
            return "0";
        case Keyboard::Scan::Enter:
            return "Enter";
        case Keyboard::Scan::Escape:
            return "Escape";
        case Keyboard::Scan::Backspace:
            return "Backspace";
        case Keyboard::Scan::Tab:
            return "Tab";
        case Keyboard::Scan::Space:
            return "Space";
        case Keyboard::Scan::Hyphen:
            return "-";
        case Keyboard::Scan::Equal:
            return "=";
        case Keyboard::Scan::LBracket:
            return "[";
        case Keyboard::Scan::RBracket:
            return "]";
        case Keyboard::Scan::Backslash:
            return "\\";
        case Keyboard::Scan::Semicolon:
            return ";";
        case Keyboard::Scan::Apostrophe:
            return "'";
        case Keyboard::Scan::Grave:
            return "`";
        case Keyboard::Scan::Comma:
            return ",";
        case Keyboard::Scan::Period:
            return ".";
        case Keyboard::Scan::Slash:
            return "/";
        case Keyboard::Scan::CapsLock:
            return "Caps Lock";
        case Keyboard::Scan::PrintScreen:
            return "Print Screen";
        case Keyboard::Scan::ScrollLock:
            return "Scroll Lock";
        case Keyboard::Scan::Pause:
            return "Pause";
        case Keyboard::Scan::Insert:
            return "Insert";
        case Keyboard::Scan::Home:
            return "Home";
        case Keyboard::Scan::PageUp:
            return "Page Up";
        case Keyboard::Scan::Delete:
            return "Delete";
        case Keyboard::Scan::End:
            return "End";
        case Keyboard::Scan::PageDown:
            return "Page Down";
        case Keyboard::Scan::Left:
            return "Left Arrow";
        case Keyboard::Scan::Right:
            return "Right Arrow";
        case Keyboard::Scan::Up:
            return "Up Arrow";
        case Keyboard::Scan::Down:
            return "Down Arrow";
        case Keyboard::Scan::NumLock:
            return "Num Lock";
        case Keyboard::Scan::NumpadDivide:
            return "Numpad /";
        case Keyboard::Scan::NumpadMultiply:
            return "Numpad *";
        case Keyboard::Scan::NumpadMinus:
            return "Numpad -";
        case Keyboard::Scan::NumpadPlus:
            return "Numpad +";
        case Keyboard::Scan::NumpadEqual:
            return "Numpad =";
        case Keyboard::Scan::NumpadEnter:
            return "Numpad Enter";
        case Keyboard::Scan::NumpadDecimal:
            return "Numpad .";
        case Keyboard::Scan::Numpad0:
            return "Numpad 0";
        case Keyboard::Scan::Numpad1:
            return "Numpad 1";
        case Keyboard::Scan::Numpad2:
            return "Numpad 2";
        case Keyboard::Scan::Numpad3:
            return "Numpad 3";
        case Keyboard::Scan::Numpad4:
            return "Numpad 4";
        case Keyboard::Scan::Numpad5:
            return "Numpad 5";
        case Keyboard::Scan::Numpad6:
            return "Numpad 6";
        case Keyboard::Scan::Numpad7:
            return "Numpad 7";
        case Keyboard::Scan::Numpad8:
            return "Numpad 8";
        case Keyboard::Scan::Numpad9:
            return "Numpad 9";
        case Keyboard::Scan::NonUsBackslash:
            return "Non-US Backslash";
        case Keyboard::Scan::Application:
            return "Application";
        case Keyboard::Scan::Execute:
            return "Execute";
        case Keyboard::Scan::ModeChange:
            return "Mode Change";
        case Keyboard::Scan::Help:
            return "Help";
        case Keyboard::Scan::Menu:
            return "Menu";
        case Keyboard::Scan::Select:
            return "Select";
        case Keyboard::Scan::Redo:
            return "Redo";
        case Keyboard::Scan::Undo:
            return "Undo";
        case Keyboard::Scan::Cut:
            return "Cut";
        case Keyboard::Scan::Copy:
            return "Copy";
        case Keyboard::Scan::Paste:
            return "Paste";
        case Keyboard::Scan::VolumeMute:
            return "Volume Mute";
        case Keyboard::Scan::VolumeUp:
            return "Volume Up";
        case Keyboard::Scan::VolumeDown:
            return "Volume Down";
        case Keyboard::Scan::MediaPlayPause:
            return "Media Play/Pause";
        case Keyboard::Scan::MediaStop:
            return "Media Stop";
        case Keyboard::Scan::MediaNextTrack:
            return "Media Next Track";
        case Keyboard::Scan::MediaPreviousTrack:
            return "Media Previous Track";
        case Keyboard::Scan::LControl:
            return "Left Control";
        case Keyboard::Scan::LShift:
            return "Left Shift";
        case Keyboard::Scan::LAlt:
            return "Left Alt";
        case Keyboard::Scan::LSystem:
            return "Left System";
        case Keyboard::Scan::RControl:
            return "Right Control";
        case Keyboard::Scan::RShift:
            return "Right Shift";
        case Keyboard::Scan::RAlt:
            return "Right Alt";
        case Keyboard::Scan::RSystem:
            return "Right System";
        case Keyboard::Scan::Back:
            return "Back";
        case Keyboard::Scan::Forward:
            return "Forward";
        case Keyboard::Scan::Refresh:
            return "Refresh";
        case Keyboard::Scan::Stop:
            return "Stop";
        case Keyboard::Scan::Search:
            return "Search";
        case Keyboard::Scan::Favorites:
            return "Favorites";
        case Keyboard::Scan::HomePage:
            return "Home Page";
        case Keyboard::Scan::LaunchApplication1:
            return "Launch Application 1";
        case Keyboard::Scan::LaunchApplication2:
            return "Launch Application 2";
        case Keyboard::Scan::LaunchMail:
            return "Launch Mail";
        case Keyboard::Scan::LaunchMediaSelect:
            return "Launch Media Select";
        default:
            return "Unknown key";
    }
}


void setVirtualKeyboardVisible(bool visible)
{
    // A native proxy remains the authoritative IME backend even when a show
    // or hide request is rejected because the keyboard is already in that
    // state. Only use the ArkTS compatibility path when native attach failed.
    if (Harmony::setInputMethodVisible(visible) != Harmony::InputMethodVisibilityResult::Unavailable)
        return;

    auto&                            state = Harmony::getHostState();
    sf::priv::Harmony::HostCallbacks callbacks;
    OH_NativeXComponent*             component = nullptr;
    {
        const std::lock_guard lock(state.mutex);
        callbacks = state.callbacks;
        component = state.component;
    }

    if (callbacks.setVirtualKeyboardVisible)
        callbacks.setVirtualKeyboardVisible(visible, callbacks.userData);
    else if (component)
        OH_NativeXComponent_SetNeedSoftKeyboard(component, visible);
}


bool isMouseButtonPressed(Mouse::Button button)
{
    auto&                 state = Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    return state.mouseButtons[button];
}


Vector2i getMousePosition()
{
    auto& state = Harmony::getHostState();
#ifdef SFML_HARMONY_2IN1
    bool queryGlobalPointer{};
    {
        const std::lock_guard lock(state.mutex);
        queryGlobalPointer = state.pointerLocationAvailable.value_or(true);
    }
    if (queryGlobalPointer)
    {
        std::int32_t displayId{};
        double       displayX{};
        double       displayY{};
        if (OH_Input_GetPointerLocation(&displayId, &displayX, &displayY) == INPUT_SUCCESS)
        {
            std::int32_t originX{};
            std::int32_t originY{};
            if (displayId < 0 ||
                OH_NativeDisplayManager_GetDisplayPosition(static_cast<std::uint64_t>(displayId), &originX, &originY) !=
                    DISPLAY_MANAGER_OK)
            {
                originX = 0;
                originY = 0;
            }
            {
                const std::lock_guard lock(state.mutex);
                state.pointerLocationAvailable = true;
            }
            return {originX + static_cast<int>(std::lround(displayX)), originY + static_cast<int>(std::lround(displayY))};
        }

        const std::lock_guard lock(state.mutex);
        state.pointerLocationAvailable = false;
    }

    Vector2i local;
    {
        const std::lock_guard lock(state.mutex);
        local = state.mousePosition;
    }
    Vector2i origin;
    if (getClientOrigin(origin))
        return origin + local;
    return local;
#else
    const std::lock_guard lock(state.mutex);
    return state.mousePosition;
#endif
}


Vector2i getMousePosition(const WindowBase& relativeTo)
{
#ifdef SFML_HARMONY_2IN1
    const auto global = getMousePosition();
    Vector2i   origin;
    if (getClientOrigin(origin))
        return global - origin;
    return global - relativeTo.getPosition();
#else
    (void)relativeTo;
    return getMousePosition();
#endif
}


void setMousePosition(Vector2i)
{
    // Input injection is intentionally unsupported for ordinary applications.
}


void setMousePosition(Vector2i position, const WindowBase&)
{
    setMousePosition(position);
}


bool isTouchDown(unsigned int finger)
{
    auto&                 state = Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    return state.touches.count(finger) != 0;
}


Vector2i getTouchPosition(unsigned int finger)
{
    auto&                 state = Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    if (const auto found = state.touches.find(finger); found != state.touches.end())
        return found->second;
    return {-1, -1};
}


Vector2i getTouchPosition(unsigned int finger, const WindowBase&)
{
    return getTouchPosition(finger);
}

} // namespace sf::priv::InputImpl
