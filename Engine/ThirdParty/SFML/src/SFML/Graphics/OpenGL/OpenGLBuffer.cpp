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
#include <SFML/Graphics/OpenGL/OpenGLBuffer.hpp>
#include <SFML/Graphics/OpenGL/OpenGLResource.hpp>

#include <limits>

#include <cstring>

namespace sf::priv
{
OpenGLBuffer::~OpenGLBuffer()
{
    const OpenGLResource::TransientContextLock lock;
    if (m_buffer)
        glCheck(GLEXT_glDeleteBuffers(1, &m_buffer));
}
bool OpenGLBuffer::create(std::size_t bytes, rhi::BufferUsage usage)
{
    if (bytes > static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max()))
        return false;
    const OpenGLResource::TransientContextLock lock;
    ensureExtensionsInit();
    glCheck(GLEXT_glGenBuffers(1, &m_buffer));
    if (!m_buffer)
        return false;
    const GLenum hint = usage == rhi::BufferUsage::Static    ? GLEXT_GL_STATIC_DRAW
                        : usage == rhi::BufferUsage::Dynamic ? GLEXT_GL_DYNAMIC_DRAW
                                                             : GLEXT_GL_STREAM_DRAW;
    glCheck(GLEXT_glBindBuffer(GLEXT_GL_ARRAY_BUFFER, m_buffer));
    glCheck(GLEXT_glBufferData(GLEXT_GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bytes), nullptr, hint));
    glCheck(GLEXT_glBindBuffer(GLEXT_GL_ARRAY_BUFFER, 0));
    m_size = bytes;
    return true;
}
bool OpenGLBuffer::upload(const void* data, std::size_t bytes, std::size_t offset)
{
    if (!data || offset > m_size || bytes > m_size - offset)
        return false;
    const OpenGLResource::TransientContextLock lock;
    glCheck(GLEXT_glBindBuffer(GLEXT_GL_ARRAY_BUFFER, m_buffer));
    if (bytes)
        glCheck(
            GLEXT_glBufferSubData(GLEXT_GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(bytes), data));
    glCheck(GLEXT_glBindBuffer(GLEXT_GL_ARRAY_BUFFER, 0));
    return true;
}
bool OpenGLBuffer::read(void* data, std::size_t bytes, std::size_t offset) const
{
    if (!data || offset > m_size || bytes > m_size - offset)
        return false;
    if (!bytes)
        return true;
    const OpenGLResource::TransientContextLock lock;
    glCheck(GLEXT_glBindBuffer(GLEXT_GL_ARRAY_BUFFER, m_buffer));
#ifdef SFML_OPENGL_ES
    const void* source = glCheck(
        glMapBufferRange(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(bytes), GL_MAP_READ_BIT));
    if (source)
        std::memcpy(data, source, bytes);
#else
    const void* source = glCheck(GLEXT_glMapBuffer(GLEXT_GL_ARRAY_BUFFER, GLEXT_GL_READ_ONLY));
    if (source)
        std::memcpy(data, static_cast<const std::byte*>(source) + offset, bytes);
#endif
    const bool success = source && glCheck(glUnmapBuffer(GLEXT_GL_ARRAY_BUFFER)) == GL_TRUE;
    glCheck(GLEXT_glBindBuffer(GLEXT_GL_ARRAY_BUFFER, 0));
    return success;
}
bool OpenGLBuffer::copy(const rhi::Buffer& source)
{
    const auto* other = resourceCast<const OpenGLBuffer>(&source);
    if (!other || other->m_size != m_size)
        return false;
    if (other == this || !m_size)
        return true;
    const OpenGLResource::TransientContextLock lock;
    ensureExtensionsInit();
    if (GLEXT_copy_buffer)
    {
        glCheck(GLEXT_glBindBuffer(GLEXT_GL_COPY_READ_BUFFER, other->m_buffer));
        glCheck(GLEXT_glBindBuffer(GLEXT_GL_COPY_WRITE_BUFFER, m_buffer));
        glCheck(
            GLEXT_glCopyBufferSubData(GLEXT_GL_COPY_READ_BUFFER, GLEXT_GL_COPY_WRITE_BUFFER, 0, 0, static_cast<GLsizeiptr>(m_size)));
        glCheck(GLEXT_glBindBuffer(GLEXT_GL_COPY_READ_BUFFER, 0));
        glCheck(GLEXT_glBindBuffer(GLEXT_GL_COPY_WRITE_BUFFER, 0));
        return true;
    }
    std::vector<std::byte> data(m_size);
    return other->read(data.data(), m_size, 0) && upload(data.data(), m_size, 0);
}
void OpenGLBuffer::bind(const rhi::Buffer* value)
{
    const OpenGLResource::TransientContextLock lock;
    ensureExtensionsInit();
    const auto* buffer = resourceCast<const OpenGLBuffer>(value);
    glCheck(GLEXT_glBindBuffer(GLEXT_GL_ARRAY_BUFFER, buffer ? buffer->m_buffer : 0));
}
} // namespace sf::priv
