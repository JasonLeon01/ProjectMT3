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

#include <SFML/Window/Emscripten/Input.hpp>
#include <SFML/Window/InputImpl.hpp>

#include <SFML/System/String.hpp>

#include <algorithm>
namespace sf::priv::WebInput
{
std::array<bool, Keyboard::KeyCount>      keys{};
std::array<bool, Keyboard::ScancodeCount> scans{};
std::array<bool, Mouse::ButtonCount>      buttons{};
Vector2i                                  mouse;
std::array<TouchPoint, 32>                touches{};
constexpr std::array                      mappings{
    KeyMapping{"KeyA", Keyboard::Key::A, Keyboard::Scancode::A},
    KeyMapping{"KeyB", Keyboard::Key::B, Keyboard::Scancode::B},
    KeyMapping{"KeyC", Keyboard::Key::C, Keyboard::Scancode::C},
    KeyMapping{"KeyD", Keyboard::Key::D, Keyboard::Scancode::D},
    KeyMapping{"KeyE", Keyboard::Key::E, Keyboard::Scancode::E},
    KeyMapping{"KeyF", Keyboard::Key::F, Keyboard::Scancode::F},
    KeyMapping{"KeyG", Keyboard::Key::G, Keyboard::Scancode::G},
    KeyMapping{"KeyH", Keyboard::Key::H, Keyboard::Scancode::H},
    KeyMapping{"KeyI", Keyboard::Key::I, Keyboard::Scancode::I},
    KeyMapping{"KeyJ", Keyboard::Key::J, Keyboard::Scancode::J},
    KeyMapping{"KeyK", Keyboard::Key::K, Keyboard::Scancode::K},
    KeyMapping{"KeyL", Keyboard::Key::L, Keyboard::Scancode::L},
    KeyMapping{"KeyM", Keyboard::Key::M, Keyboard::Scancode::M},
    KeyMapping{"KeyN", Keyboard::Key::N, Keyboard::Scancode::N},
    KeyMapping{"KeyO", Keyboard::Key::O, Keyboard::Scancode::O},
    KeyMapping{"KeyP", Keyboard::Key::P, Keyboard::Scancode::P},
    KeyMapping{"KeyQ", Keyboard::Key::Q, Keyboard::Scancode::Q},
    KeyMapping{"KeyR", Keyboard::Key::R, Keyboard::Scancode::R},
    KeyMapping{"KeyS", Keyboard::Key::S, Keyboard::Scancode::S},
    KeyMapping{"KeyT", Keyboard::Key::T, Keyboard::Scancode::T},
    KeyMapping{"KeyU", Keyboard::Key::U, Keyboard::Scancode::U},
    KeyMapping{"KeyV", Keyboard::Key::V, Keyboard::Scancode::V},
    KeyMapping{"KeyW", Keyboard::Key::W, Keyboard::Scancode::W},
    KeyMapping{"KeyX", Keyboard::Key::X, Keyboard::Scancode::X},
    KeyMapping{"KeyY", Keyboard::Key::Y, Keyboard::Scancode::Y},
    KeyMapping{"KeyZ", Keyboard::Key::Z, Keyboard::Scancode::Z},
    KeyMapping{"Digit0", Keyboard::Key::Num0, Keyboard::Scancode::Num0},
    KeyMapping{"Digit1", Keyboard::Key::Num1, Keyboard::Scancode::Num1},
    KeyMapping{"Digit2", Keyboard::Key::Num2, Keyboard::Scancode::Num2},
    KeyMapping{"Digit3", Keyboard::Key::Num3, Keyboard::Scancode::Num3},
    KeyMapping{"Digit4", Keyboard::Key::Num4, Keyboard::Scancode::Num4},
    KeyMapping{"Digit5", Keyboard::Key::Num5, Keyboard::Scancode::Num5},
    KeyMapping{"Digit6", Keyboard::Key::Num6, Keyboard::Scancode::Num6},
    KeyMapping{"Digit7", Keyboard::Key::Num7, Keyboard::Scancode::Num7},
    KeyMapping{"Digit8", Keyboard::Key::Num8, Keyboard::Scancode::Num8},
    KeyMapping{"Digit9", Keyboard::Key::Num9, Keyboard::Scancode::Num9},
    KeyMapping{"F1", Keyboard::Key::F1, Keyboard::Scancode::F1},
    KeyMapping{"F2", Keyboard::Key::F2, Keyboard::Scancode::F2},
    KeyMapping{"F3", Keyboard::Key::F3, Keyboard::Scancode::F3},
    KeyMapping{"F4", Keyboard::Key::F4, Keyboard::Scancode::F4},
    KeyMapping{"F5", Keyboard::Key::F5, Keyboard::Scancode::F5},
    KeyMapping{"F6", Keyboard::Key::F6, Keyboard::Scancode::F6},
    KeyMapping{"F7", Keyboard::Key::F7, Keyboard::Scancode::F7},
    KeyMapping{"F8", Keyboard::Key::F8, Keyboard::Scancode::F8},
    KeyMapping{"F9", Keyboard::Key::F9, Keyboard::Scancode::F9},
    KeyMapping{"F10", Keyboard::Key::F10, Keyboard::Scancode::F10},
    KeyMapping{"F11", Keyboard::Key::F11, Keyboard::Scancode::F11},
    KeyMapping{"F12", Keyboard::Key::F12, Keyboard::Scancode::F12},
    KeyMapping{"F13", Keyboard::Key::F13, Keyboard::Scancode::F13},
    KeyMapping{"F14", Keyboard::Key::F14, Keyboard::Scancode::F14},
    KeyMapping{"F15", Keyboard::Key::F15, Keyboard::Scancode::F15},
    KeyMapping{"Escape", Keyboard::Key::Escape, Keyboard::Scancode::Escape},
    KeyMapping{"Space", Keyboard::Key::Space, Keyboard::Scancode::Space},
    KeyMapping{"Enter", Keyboard::Key::Enter, Keyboard::Scancode::Enter},
    KeyMapping{"Tab", Keyboard::Key::Tab, Keyboard::Scancode::Tab},
    KeyMapping{"Backspace", Keyboard::Key::Backspace, Keyboard::Scancode::Backspace},
    KeyMapping{"Insert", Keyboard::Key::Insert, Keyboard::Scancode::Insert},
    KeyMapping{"Delete", Keyboard::Key::Delete, Keyboard::Scancode::Delete},
    KeyMapping{"Home", Keyboard::Key::Home, Keyboard::Scancode::Home},
    KeyMapping{"End", Keyboard::Key::End, Keyboard::Scancode::End},
    KeyMapping{"PageUp", Keyboard::Key::PageUp, Keyboard::Scancode::PageUp},
    KeyMapping{"PageDown", Keyboard::Key::PageDown, Keyboard::Scancode::PageDown},
    KeyMapping{"Pause", Keyboard::Key::Pause, Keyboard::Scancode::Pause},
    KeyMapping{"Comma", Keyboard::Key::Comma, Keyboard::Scancode::Comma},
    KeyMapping{"Period", Keyboard::Key::Period, Keyboard::Scancode::Period},
    KeyMapping{"Slash", Keyboard::Key::Slash, Keyboard::Scancode::Slash},
    KeyMapping{"Backslash", Keyboard::Key::Backslash, Keyboard::Scancode::Backslash},
    KeyMapping{"Semicolon", Keyboard::Key::Semicolon, Keyboard::Scancode::Semicolon},
    KeyMapping{"Equal", Keyboard::Key::Equal, Keyboard::Scancode::Equal},
    KeyMapping{"BracketLeft", Keyboard::Key::LBracket, Keyboard::Scancode::LBracket},
    KeyMapping{"BracketRight", Keyboard::Key::RBracket, Keyboard::Scancode::RBracket},
    KeyMapping{"Quote", Keyboard::Key::Apostrophe, Keyboard::Scancode::Apostrophe},
    KeyMapping{"Backquote", Keyboard::Key::Grave, Keyboard::Scancode::Grave},
    KeyMapping{"Minus", Keyboard::Key::Hyphen, Keyboard::Scancode::Hyphen},
    KeyMapping{"ControlLeft", Keyboard::Key::LControl, Keyboard::Scancode::LControl},
    KeyMapping{"ControlRight", Keyboard::Key::RControl, Keyboard::Scancode::RControl},
    KeyMapping{"ShiftLeft", Keyboard::Key::LShift, Keyboard::Scancode::LShift},
    KeyMapping{"ShiftRight", Keyboard::Key::RShift, Keyboard::Scancode::RShift},
    KeyMapping{"AltLeft", Keyboard::Key::LAlt, Keyboard::Scancode::LAlt},
    KeyMapping{"AltRight", Keyboard::Key::RAlt, Keyboard::Scancode::RAlt},
    KeyMapping{"MetaLeft", Keyboard::Key::LSystem, Keyboard::Scancode::LSystem},
    KeyMapping{"MetaRight", Keyboard::Key::RSystem, Keyboard::Scancode::RSystem},
    KeyMapping{"ContextMenu", Keyboard::Key::Menu, Keyboard::Scancode::Application},
    KeyMapping{"ArrowLeft", Keyboard::Key::Left, Keyboard::Scancode::Left},
    KeyMapping{"ArrowRight", Keyboard::Key::Right, Keyboard::Scancode::Right},
    KeyMapping{"ArrowUp", Keyboard::Key::Up, Keyboard::Scancode::Up},
    KeyMapping{"ArrowDown", Keyboard::Key::Down, Keyboard::Scancode::Down},
    KeyMapping{"Numpad0", Keyboard::Key::Numpad0, Keyboard::Scancode::Numpad0},
    KeyMapping{"Numpad1", Keyboard::Key::Numpad1, Keyboard::Scancode::Numpad1},
    KeyMapping{"Numpad2", Keyboard::Key::Numpad2, Keyboard::Scancode::Numpad2},
    KeyMapping{"Numpad3", Keyboard::Key::Numpad3, Keyboard::Scancode::Numpad3},
    KeyMapping{"Numpad4", Keyboard::Key::Numpad4, Keyboard::Scancode::Numpad4},
    KeyMapping{"Numpad5", Keyboard::Key::Numpad5, Keyboard::Scancode::Numpad5},
    KeyMapping{"Numpad6", Keyboard::Key::Numpad6, Keyboard::Scancode::Numpad6},
    KeyMapping{"Numpad7", Keyboard::Key::Numpad7, Keyboard::Scancode::Numpad7},
    KeyMapping{"Numpad8", Keyboard::Key::Numpad8, Keyboard::Scancode::Numpad8},
    KeyMapping{"Numpad9", Keyboard::Key::Numpad9, Keyboard::Scancode::Numpad9},
    KeyMapping{"NumpadAdd", Keyboard::Key::Add, Keyboard::Scancode::NumpadPlus},
    KeyMapping{"NumpadSubtract", Keyboard::Key::Subtract, Keyboard::Scancode::NumpadMinus},
    KeyMapping{"NumpadMultiply", Keyboard::Key::Multiply, Keyboard::Scancode::NumpadMultiply},
    KeyMapping{"NumpadDivide", Keyboard::Key::Divide, Keyboard::Scancode::NumpadDivide}};
const KeyMapping* findCode(std::string_view code)
{
    const auto it = std::find_if(mappings.begin(),
                                 mappings.end(),
                                 [code](const auto& mapping) { return mapping.code == code; });
    return it == mappings.end() ? nullptr : &*it;
}
const KeyMapping* findKey(Keyboard::Key key)
{
    const auto it = std::find_if(mappings.begin(),
                                 mappings.end(),
                                 [key](const auto& mapping) { return mapping.key == key; });
    return it == mappings.end() ? nullptr : &*it;
}
const KeyMapping* findScan(Keyboard::Scancode scan)
{
    const auto it = std::find_if(mappings.begin(),
                                 mappings.end(),
                                 [scan](const auto& mapping) { return mapping.scan == scan; });
    return it == mappings.end() ? nullptr : &*it;
}
} // namespace sf::priv::WebInput
namespace sf::priv::InputImpl
{
bool isKeyPressed(Keyboard::Key key)
{
    return key >= Keyboard::Key::A && static_cast<unsigned int>(key) < Keyboard::KeyCount &&
           WebInput::keys[static_cast<std::size_t>(key)];
}
bool isKeyPressed(Keyboard::Scancode code)
{
    return code >= Keyboard::Scancode::A && static_cast<unsigned int>(code) < Keyboard::ScancodeCount &&
           WebInput::scans[static_cast<std::size_t>(code)];
}
Keyboard::Key localize(Keyboard::Scancode code)
{
    const auto* mapping = WebInput::findScan(code);
    return mapping ? mapping->key : Keyboard::Key::Unknown;
}
Keyboard::Scancode delocalize(Keyboard::Key key)
{
    const auto* mapping = WebInput::findKey(key);
    return mapping ? mapping->scan : Keyboard::Scancode::Unknown;
}
String getDescription(Keyboard::Scancode code)
{
    const auto* mapping = WebInput::findScan(code);
    return mapping ? String(std::string(mapping->code)) : String{};
}
void setVirtualKeyboardVisible(bool)
{
}
bool isMouseButtonPressed(Mouse::Button button)
{
    return static_cast<unsigned int>(button) < Mouse::ButtonCount && WebInput::buttons[static_cast<std::size_t>(button)];
}
Vector2i getMousePosition()
{
    return WebInput::mouse;
}
Vector2i getMousePosition(const WindowBase&)
{
    return getMousePosition();
}
void setMousePosition(Vector2i)
{
}
void setMousePosition(Vector2i, const WindowBase&)
{
}
bool isTouchDown(unsigned int finger)
{
    return finger < WebInput::touches.size() && WebInput::touches[finger].identifier.has_value();
}
Vector2i getTouchPosition(unsigned int finger)
{
    return finger < WebInput::touches.size() ? WebInput::touches[finger].position : Vector2i{};
}
Vector2i getTouchPosition(unsigned int finger, const WindowBase&)
{
    return getTouchPosition(finger);
}
} // namespace sf::priv::InputImpl
