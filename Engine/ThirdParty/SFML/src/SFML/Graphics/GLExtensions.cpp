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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/GLExtensions.hpp>

#include <SFML/Window/Context.hpp>
#include <SFML/Window/GlContext.hpp>

#include <SFML/System/Exception.hpp>

// We check for this definition in order to avoid multiple definitions of GLAD
// entities during unity builds of SFML.
#ifndef SF_GLAD_GL_IMPLEMENTATION_INCLUDED
#define SF_GLAD_GL_IMPLEMENTATION_INCLUDED
#define SF_GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef SF_GLAD_GL_IMPLEMENTATION
#endif

#include <memory>
#include <type_traits>

namespace
{
////////////////////////////////////////////////////////////
void extensionSanityCheck()
{
    static const auto check = [](auto&& flag, auto... entryPoints)
    {
        // If a required entry point is missing, flag the whole extension as unavailable
        if (!(entryPoints && ...))
        {
            if constexpr (std::is_lvalue_reference_v<decltype(flag)> &&
                          !std::is_const_v<std::remove_reference_t<decltype(flag)>>)
                flag = 0;
        }
    };
#ifdef SFML_OPENGL_ES
    check(GLEXT_multitexture_dependencies);
    check(GLEXT_vertex_buffer_object_dependencies);
    check(GLEXT_blend_subtract_dependencies);
    check(GLEXT_blend_func_separate_dependencies);
    check(GLEXT_blend_equation_separate_dependencies);
    check(GLEXT_framebuffer_object_dependencies);
    check(GLEXT_blend_minmax_dependencies);
#else
    check(GLEXT_blend_minmax_dependencies);
    check(GLEXT_multitexture_dependencies);
    check(GLEXT_blend_func_separate_dependencies);
    check(GLEXT_vertex_buffer_object_dependencies);
    check(GLEXT_blend_equation_separate_dependencies);
    check(GLEXT_framebuffer_object_dependencies);
    check(GLEXT_framebuffer_blit_dependencies);
    check(GLEXT_framebuffer_multisample_dependencies);
    check(GLEXT_copy_buffer_dependencies);
#endif
}
} // namespace

namespace sf::priv
{
////////////////////////////////////////////////////////////
GladGLContext& getGlDispatch()
{
    // The window module tracks the native context per thread. Its slot survives
    // switching targets and follows the context when it moves between threads.
    auto* storage = GlContext::getActiveContextGraphicsData();
    if (!storage)
        throw Exception("Cannot initialize OpenGL entry points without an active context");
    if (*storage)
        return *static_cast<GladGLContext*>(storage->get());

    auto dispatch = std::make_shared<GladGLContext>();
#ifdef SFML_OPENGL_ES
    const int     loadedVersion = gladLoadGLES2Context(dispatch.get(), Context::getFunction);
    constexpr int minimumMajor  = 3;
#else
    const int     loadedVersion = gladLoadGLContext(dispatch.get(), Context::getFunction);
    constexpr int minimumMajor  = 2;
#endif
    if (!loadedVersion || GLAD_VERSION_MAJOR(loadedVersion) < minimumMajor)
        throw Exception("Failed to load the OpenGL entry points required by sfml-graphics");

    *storage = dispatch;
    extensionSanityCheck();
    return *dispatch;
}

void ensureExtensionsInit()
{
    (void)getGlDispatch();
}

} // namespace sf::priv
