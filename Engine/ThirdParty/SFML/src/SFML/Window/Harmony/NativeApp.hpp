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
#include <SFML/Window/Export.hpp>

#include <SFML/System/Vector2.hpp>

#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include <cstdint>


namespace sf::priv::Harmony
{
enum class WindowCommandType : std::uint32_t
{
    Configure,
    Restore,
    SetPosition,
    SetSize,
    SetMinimumSize,
    SetMaximumSize,
    SetTitle,
    SetIcon,
    SetVisible,
    RequestFocus
};


struct WindowCommand
{
    std::uint32_t                         requestId{};
    std::chrono::steady_clock::time_point deadline;
    WindowCommandType                     type{};
    Vector2i                              position;
    Vector2u                              size;
    std::optional<Vector2u>               sizeLimit;
    std::string                           title;
    Vector2u                              iconSize;
    std::vector<std::uint8_t>             iconPixels;
    std::uint32_t                         style{};
    bool                                  fullscreen{};
    bool                                  visible{true};
    bool                                  keepScreenOn{};
};


struct WindowState
{
    Vector2i position;
    Vector2u clientSize;
    bool     visible{true};
    bool     focused{};
    bool     fullscreen{};
};


struct HostCallbacks
{
    void (*setPointerVisible)(bool visible, void* userData){};
    void (*setSystemPointer)(unsigned int type, void* userData){};
    void (*setCustomPointer)(const std::uint8_t* pixels, Vector2u size, Vector2u hotspot, void* userData){};
    void (*setVirtualKeyboardVisible)(bool visible, void* userData){};
    void (*configureWindow)(bool fullscreen, bool keepScreenOn, void* userData){};
    bool (*executeWindowCommand)(const WindowCommand& command, void* userData){};
    void (*requestExit)(void* userData){};
    void* userData{};
};

SFML_WINDOW_API bool registerNativeXComponent(void* component);
SFML_WINDOW_API void unregisterNativeXComponent(void* component);

SFML_WINDOW_API void notifyForeground();
SFML_WINDOW_API void notifyBackground();
SFML_WINDOW_API void notifyDestroy();
SFML_WINDOW_API void submitText(char32_t unicode);
SFML_WINDOW_API void setHostCallbacks(const HostCallbacks& callbacks);

SFML_WINDOW_API bool completeWindowCommand(std::uint32_t requestId, bool success, const WindowState& state);
SFML_WINDOW_API void updateWindowState(const WindowState& state);

} // namespace sf::priv::Harmony
