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

#include <SFML/Window/Emscripten/WebContext.hpp>

#include <SFML/System/Emscripten/Browser.hpp>

#include <GLES3/gl3.h>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <string>

#include <cstdlib>

EM_JS_DEPS(sfmlCanvasDependencies, "$stringToNewUTF8");

// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(char*, sfmlGetCanvasSelector, (), {
    var canvas = Module['canvas'] || document.getElementById('canvas');
    if (!canvas)
        return 0;
    Module['canvas'] = canvas;
    if (!canvas.id)
        canvas.id = 'sfml-canvas';
    return stringToNewUTF8('#' + CSS.escape(canvas.id));
});
// clang-format on

namespace sf::priv
{
namespace
{
namespace WebContextImpl
{
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE handle{};
}
} // namespace
const char* WebContext::canvasSelector()
{
    static const std::string selector = []
    {
        const std::unique_ptr<char, decltype(&std::free)> value(sfmlGetCanvasSelector(), std::free);
        return std::string(value ? value.get() : "");
    }();
    return selector.c_str();
}
WebContext::WebContext(WebContext* shared) : WebContext(shared, {}, Vector2u{1, 1})
{
}
WebContext::WebContext(WebContext* shared, const ContextSettings&, Vector2u)
{
    if (shared)
    {
        m_handle   = shared->m_handle;
        m_settings = shared->m_settings;
        return;
    }
    {
        EmscriptenWebGLContextAttributes attributes;
        emscripten_webgl_init_context_attributes(&attributes);
        attributes.majorVersion = 2;
        attributes.alpha        = false;
        attributes.depth        = true;
        attributes.stencil      = true;
        attributes.antialias    = true;
        m_handle                = emscripten_webgl_create_context(canvasSelector(), &attributes);
        m_owner                 = m_handle > 0;
        WebContextImpl::handle  = m_handle;
    }
    if (makeCurrent(true))
    {
        GLint depth   = 0;
        GLint stencil = 0;
        GLint samples = 0;
        // GLAD is initialized later. Resolve directly, including in Unity builds
        // where another source may already have defined the GLAD function macros.
        const auto getInteger = reinterpret_cast<void (*)(GLenum, GLint*)>(getFunction("glGetIntegerv"));
        if (!getInteger)
            return;
        getInteger(GL_DEPTH_BITS, &depth);
        getInteger(GL_STENCIL_BITS, &stencil);
        getInteger(GL_SAMPLES, &samples);
        m_settings.depthBits         = static_cast<unsigned int>(depth);
        m_settings.stencilBits       = static_cast<unsigned int>(stencil);
        m_settings.antiAliasingLevel = static_cast<unsigned int>(samples);
        m_settings.sRgbCapable       = false;
    }
}
WebContext::WebContext(WebContext* shared, const ContextSettings& settings, const WindowImpl&, unsigned int) :
    WebContext(shared, settings, Vector2u{1, 1})
{
}
WebContext::~WebContext()
{
    cleanupUnsharedResources();
    if (m_owner)
    {
        emscripten_webgl_destroy_context(m_handle);
        WebContextImpl::handle = 0;
    }
}
GlFunctionPointer WebContext::getFunction(const char* name)
{
    return reinterpret_cast<GlFunctionPointer>(emscripten_webgl_get_proc_address(name));
}
bool WebContext::isLost()
{
    return WebContextImpl::handle > 0 && emscripten_is_webgl_context_lost(WebContextImpl::handle);
}
bool WebContext::makeCurrent(bool current)
{
    return m_handle > 0 && (!current || !isLost()) &&
           emscripten_webgl_make_context_current(current ? m_handle : 0) == EMSCRIPTEN_RESULT_SUCCESS;
}
void WebContext::display()
{
    if (m_verticalSync)
        waitForBrowserFrame();
    else
        sleepImpl(Time::Zero);
}
void WebContext::setVerticalSyncEnabled(bool enabled)
{
    m_verticalSync = enabled;
}
} // namespace sf::priv
