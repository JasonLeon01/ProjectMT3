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
#include <SFML/Window/GlContext.hpp>

#include <emscripten/html5_webgl.h>
namespace sf::priv
{
class WebContext : public GlContext
{
public:
    explicit WebContext(WebContext* shared);
    WebContext(WebContext* shared, const ContextSettings& settings, Vector2u size);
    WebContext(WebContext* shared, const ContextSettings& settings, const WindowImpl& owner, unsigned int bitsPerPixel);
    ~WebContext() override;
    static GlFunctionPointer getFunction(const char* name);
    static const char*       canvasSelector();
    static bool              isLost();
    bool                     makeCurrent(bool current) override;
    void                     display() override;
    void                     setVerticalSyncEnabled(bool enabled) override;

private:
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_handle{};
    bool                            m_owner{};
    bool                            m_verticalSync{};
};
} // namespace sf::priv
