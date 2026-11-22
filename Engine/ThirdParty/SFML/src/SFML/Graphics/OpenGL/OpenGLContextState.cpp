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

#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/Graphics/GLExtensions.hpp>
#include <SFML/Graphics/OpenGL/OpenGLContextState.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <SFML/Window/GlContext.hpp>

#include <memory>

#include <cstddef>

namespace
{
void configureVertexAttributes()
{
    glCheck(glVertexAttribPointer(0,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(sf::Vertex),
                                  reinterpret_cast<const void*>(offsetof(sf::Vertex, position))));
    glCheck(glVertexAttribPointer(1,
                                  4,
                                  GL_UNSIGNED_BYTE,
                                  GL_TRUE,
                                  sizeof(sf::Vertex),
                                  reinterpret_cast<const void*>(offsetof(sf::Vertex, color))));
    glCheck(glVertexAttribPointer(2,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(sf::Vertex),
                                  reinterpret_cast<const void*>(offsetof(sf::Vertex, texCoords))));
    for (GLuint index = 0; index < 3; ++index)
        glCheck(glEnableVertexAttribArray(index));
}
} // namespace

namespace sf::priv
{
OpenGLContextState& OpenGLContextState::get()
{
    auto& state = GlContext::getActiveContextRenderData();
    if (!state)
        state = std::make_shared<OpenGLContextState>();

    return *static_cast<OpenGLContextState*>(state.get());
}

OpenGLContextState::OpenGLContextState() : m_dispatchSlot(GlContext::getActiveContextGraphicsData())
{
}

OpenGLContextState::~OpenGLContextState()
{
    // Native context loss can prevent activation during teardown.
    if (!m_dispatchSlot || !*m_dispatchSlot || GlContext::getActiveContextGraphicsData() != m_dispatchSlot)
        return;
    if (m_streamArray)
        glCheck(glDeleteVertexArrays(1, &m_streamArray));
    if (m_bufferArray)
        glCheck(glDeleteVertexArrays(1, &m_bufferArray));
    if (m_customArray)
        glCheck(glDeleteVertexArrays(1, &m_customArray));
    if (m_stream)
        glCheck(glDeleteBuffers(1, &m_stream));
}

void OpenGLContextState::bindStream()
{
    const bool initialize = !m_streamArray;
    if (initialize)
    {
        glCheck(glGenVertexArrays(1, &m_streamArray));
        glCheck(glGenBuffers(1, &m_stream));
    }
    glCheck(glBindVertexArray(m_streamArray));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_stream));
    if (initialize)
        configureVertexAttributes();
}

void OpenGLContextState::bindVertexBuffer(unsigned int buffer)
{
    if (!m_bufferArray)
        glCheck(glGenVertexArrays(1, &m_bufferArray));
    glCheck(glBindVertexArray(m_bufferArray));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    configureVertexAttributes();
}

void OpenGLContextState::bindCustomLayout()
{
    if (!m_customArray)
        glCheck(glGenVertexArrays(1, &m_customArray));
    glCheck(glBindVertexArray(m_customArray));
}
unsigned int OpenGLContextState::maximumVertexAttributes()
{
    if (!m_maximumVertexAttributes)
        glCheck(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &m_maximumVertexAttributes));
    return static_cast<unsigned int>(m_maximumVertexAttributes);
}
} // namespace sf::priv
