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

#include <SFML/Graphics/OpenGL/OpenGLResource.hpp>
#include <SFML/Graphics/OpenGL/OpenGLTimer.hpp>

#include <SFML/Window/Context.hpp>
namespace sf::priv
{
OpenGLTimer::Functions OpenGLTimer::functions()
{
#ifdef SFML_OPENGL_ES
    return {glGenQueriesEXT,
            glDeleteQueriesEXT,
            glBeginQueryEXT,
            glEndQueryEXT,
            glGetQueryivEXT,
            glGetQueryObjectuivEXT,
            glGetQueryObjectui64vEXT};
#else
    return {glGenQueries, glDeleteQueries, glBeginQuery, glEndQuery, glGetQueryiv, glGetQueryObjectuiv, glGetQueryObjectui64v};
#endif
}
bool OpenGLTimer::isAvailable()
{
    const auto api = functions();
    return api.generate && api.erase && api.begin && api.end && api.current && api.ready && api.result;
}
std::shared_ptr<OpenGLTimer> OpenGLTimer::create(std::shared_ptr<void> context)
{
    const OpenGLResource::TransientContextLock lock;
    ensureExtensionsInit();
    if (!isAvailable())
        return {};
    GLint      active{};
    const auto api = functions();
    api.current(GL_TIME_ELAPSED, GL_CURRENT_QUERY, &active);
    if (active)
        return {};
    auto timer         = std::make_shared<OpenGLTimer>();
    timer->m_functions = api;
    timer->m_context   = std::move(context);
    timer->m_contextId = Context::getActiveContextId();
    api.generate(1, &timer->m_query);
    api.begin(GL_TIME_ELAPSED, timer->m_query);
    return timer;
}
OpenGLTimer::~OpenGLTimer()
{
    const OpenGLResource::TransientContextLock lock;
    if (Context::getActiveContextId() == m_contextId)
    {
        if (!m_ended)
            m_functions.end(GL_TIME_ELAPSED);
        m_functions.erase(1, &m_query);
    }
}
void OpenGLTimer::end()
{
    if (m_ended)
        return;
    const OpenGLResource::TransientContextLock lock;
    if (Context::getActiveContextId() == m_contextId)
    {
        m_functions.end(GL_TIME_ELAPSED);
        m_ended = true;
    }
}
std::optional<double> OpenGLTimer::elapsedMilliseconds()
{
    if (!m_ended)
        return {};
    const OpenGLResource::TransientContextLock lock;
    if (Context::getActiveContextId() != m_contextId)
        return -1.0;
    GLuint ready{};
    m_functions.ready(m_query, GL_QUERY_RESULT_AVAILABLE, &ready);
    if (!ready)
        return {};
#ifdef SFML_OPENGL_ES
    GLint disjoint{};
    glGetIntegerv(GL_GPU_DISJOINT_EXT, &disjoint);
    if (disjoint)
        return -1.0;
#endif
    GLuint64 elapsed{};
    m_functions.result(m_query, GL_QUERY_RESULT, &elapsed);
    return static_cast<double>(elapsed) / 1000000.0;
}
} // namespace sf::priv
