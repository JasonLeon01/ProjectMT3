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

#include <variant>

namespace sf
{
/// The profile requested for a desktop OpenGL context.
enum class OpenGLProfile
{
    Automatic,
    Compatibility,
    Core
};

struct OpenGLSettings
{
    unsigned int  majorVersion{2};
    unsigned int  minorVersion{};
    OpenGLProfile profile{OpenGLProfile::Automatic};
    bool          debug{};
};

struct OpenGLESSettings
{
    unsigned int majorVersion{3};
    unsigned int minorVersion{};
};

struct VulkanSettings
{
    unsigned int majorVersion{1};
    unsigned int minorVersion{};
};

/// Minimum Direct3D feature level, using the Direct3D 11 API.
/// Supported levels are 10_0, 10_1, 11_0, 11_1, 12_0 and 12_1.
struct DirectXSettings
{
    unsigned int majorVersion{10};
    unsigned int minorVersion{};
};

/// Selects the Metal backend on macOS and iOS.
/// The available Metal version is determined by the deployment target
/// (macOS 13.0 / iOS 16.0 at minimum), so no version is requested here.
struct MetalSettings
{
};

/// Framebuffer options and minimum API version for a rendering target.
/// An empty backend setting inherits the current graphics device's configuration.
/// Window::getSettings() reports the actual settings selected by the driver.
struct ContextSettings
{
    using BackendSettings = std::variant<std::monostate, OpenGLSettings, OpenGLESSettings, VulkanSettings, DirectXSettings, MetalSettings>;

    unsigned int    depthBits{};
    unsigned int    stencilBits{};
    unsigned int    antiAliasingLevel{};
    bool            sRgbCapable{};
    BackendSettings backend{std::monostate{}};
};
} // namespace sf
