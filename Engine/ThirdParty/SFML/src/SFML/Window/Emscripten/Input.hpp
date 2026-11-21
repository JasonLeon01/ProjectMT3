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
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <SFML/System/Vector2.hpp>

#include <array>
#include <optional>
#include <string_view>
namespace sf::priv::WebInput
{
struct KeyMapping
{
    std::string_view   code;
    Keyboard::Key      key;
    Keyboard::Scancode scan;
};
struct TouchPoint
{
    std::optional<int> identifier;
    Vector2i           position;
};
const KeyMapping*                                findCode(std::string_view code);
const KeyMapping*                                findKey(Keyboard::Key key);
const KeyMapping*                                findScan(Keyboard::Scancode scan);
extern std::array<bool, Keyboard::KeyCount>      keys;
extern std::array<bool, Keyboard::ScancodeCount> scans;
extern std::array<bool, Mouse::ButtonCount>      buttons;
extern Vector2i                                  mouse;
extern std::array<TouchPoint, 32>                touches;
} // namespace sf::priv::WebInput
