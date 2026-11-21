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

#include <SFML/Window/ContextSettings.hpp>

#include <tuple>
#include <utility>

#include <cstdint>

namespace sf::priv
{
// Normalized settings shared by the native context implementations.
struct GlSettings : ContextSettings
{
    enum Attribute : std::uint32_t
    {
        Default = 0,
        Core    = 1 << 0,
        Debug   = 1 << 2
    };

    GlSettings() = default;

    explicit GlSettings(const ContextSettings& settings)
    {
        depthBits         = settings.depthBits;
        stencilBits       = settings.stencilBits;
        antiAliasingLevel = settings.antiAliasingLevel;
        sRgbCapable       = settings.sRgbCapable;

#ifdef SFML_OPENGL_ES
        if (const auto* es = std::get_if<OpenGLESSettings>(&settings.backend))
        {
            majorVersion = es->majorVersion;
            minorVersion = es->minorVersion;
        }
#else
        if (const auto* gl = std::get_if<OpenGLSettings>(&settings.backend))
        {
            majorVersion = gl->majorVersion;
            minorVersion = gl->minorVersion;
            profile      = gl->profile;
            if (profile == OpenGLProfile::Core)
            {
                attributeFlags |= Core;
                if (std::pair{majorVersion, minorVersion} < std::pair{3u, 2u})
                {
                    majorVersion = 3;
                    minorVersion = 2;
                }
            }
            if (gl->debug)
                attributeFlags |= Debug;
        }
#endif
    }

    [[nodiscard]] ContextSettings actual() const
    {
#ifdef SFML_OPENGL_ES
        const OpenGLESSettings settings{majorVersion, minorVersion};
#else
        const OpenGLSettings settings{majorVersion,
                                      minorVersion,
                                      (attributeFlags & Core) ? OpenGLProfile::Core : OpenGLProfile::Compatibility,
                                      (attributeFlags & Debug) != 0};
#endif
        return {depthBits, stencilBits, antiAliasingLevel, sRgbCapable, settings};
    }

    [[nodiscard]] bool supports(const GlSettings& requested) const
    {
        const bool versionSupported = std::tie(majorVersion, minorVersion) >=
                                      std::tie(requested.majorVersion, requested.minorVersion);
#ifdef SFML_OPENGL_ES
        return versionSupported;
#else
        const bool debugSupported   = !(requested.attributeFlags & Debug) || (attributeFlags & Debug);
        const bool profileSupported = requested.profile == OpenGLProfile::Automatic ||
                                      ((attributeFlags & Core) != 0) == (requested.profile == OpenGLProfile::Core);
        return versionSupported && debugSupported && profileSupported;
#endif
    }

#ifdef SFML_OPENGL_ES
    unsigned int majorVersion{3};
#else
    unsigned int majorVersion{2};
#endif
    unsigned int  minorVersion{};
    std::uint32_t attributeFlags{Default};
    OpenGLProfile profile{OpenGLProfile::Automatic};
};

[[nodiscard]] inline bool validGlSettings(const ContextSettings& settings)
{
    const GlSettings gl(settings);
#ifdef SFML_OPENGL_ES
    return (std::holds_alternative<std::monostate>(settings.backend) ||
            std::holds_alternative<OpenGLESSettings>(settings.backend)) &&
           gl.majorVersion >= 3;
#else
    return (std::holds_alternative<std::monostate>(settings.backend) ||
            std::holds_alternative<OpenGLSettings>(settings.backend)) &&
           gl.majorVersion >= 2;
#endif
}
} // namespace sf::priv
