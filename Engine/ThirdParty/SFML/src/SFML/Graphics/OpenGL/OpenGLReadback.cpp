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

#include <SFML/Graphics/OpenGL/OpenGLReadback.hpp>
#include <SFML/Graphics/OpenGL/OpenGLResource.hpp>
#include <SFML/Graphics/OpenGL/OpenGLTexture.hpp>

#include <limits>

#include <cstring>
namespace sf::priv
{
std::shared_ptr<OpenGLReadback> OpenGLReadback::create(std::shared_ptr<void> context, const OpenGLTexture& texture, IntRect area)
{
    const auto size = texture.getSize();
    if (area.position.x < 0 || area.position.y < 0 || area.size.x <= 0 || area.size.y <= 0 ||
        area.position.x > static_cast<int>(size.x) - area.size.x ||
        area.position.y > static_cast<int>(size.y) - area.size.y)
        return {};
    constexpr auto maximumBytes = static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max());
    const auto     width        = static_cast<std::size_t>(area.size.x);
    const auto     height       = static_cast<std::size_t>(area.size.y);
    if (width > maximumBytes / 4)
        return {};
    const std::size_t rowBytes = width * 4;
    if (height > maximumBytes / rowBytes)
        return {};
    const std::size_t                          byteCount = rowBytes * height;
    const OpenGLResource::TransientContextLock lock;
    ensureExtensionsInit();
    if (!glFenceSync || !glMapBufferRange || !glGenFramebuffers)
        return {};
    auto result         = std::make_shared<OpenGLReadback>();
    result->m_context   = std::move(context);
    result->m_size      = Vector2u(area.size);
    result->m_rowBytes  = rowBytes;
    result->m_byteCount = byteCount;
    result->m_flip      = texture.coordinateMatrix(CoordinateType::Normalized)[5] < 0;
    GLint framebuffer{};
    GLint buffer{};
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &framebuffer);
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &buffer);
    const std::array<GLenum, 4> parameters{GL_PACK_ALIGNMENT, GL_PACK_ROW_LENGTH, GL_PACK_SKIP_ROWS, GL_PACK_SKIP_PIXELS};
    std::array<GLint, 4> values{};
    for (std::size_t i = 0; i < parameters.size(); ++i)
    {
        glGetIntegerv(parameters[i], &values[i]);
        glPixelStorei(parameters[i], i == 0 ? 1 : 0);
    }
    GLuint target{};
    glGenFramebuffers(1, &target);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, target);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.nativeHandle(), 0);
    const bool valid = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    if (valid)
    {
        glGenBuffers(1, &result->m_buffer);
        glBindBuffer(GL_PIXEL_PACK_BUFFER, result->m_buffer);
        glBufferData(GL_PIXEL_PACK_BUFFER, static_cast<GLsizeiptr>(result->m_byteCount), nullptr, GL_STREAM_READ);
        const GLint y = result->m_flip ? static_cast<GLint>(size.y) - area.position.y - area.size.y : area.position.y;
        glReadPixels(area.position.x, y, area.size.x, area.size.y, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        result->m_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        glFlush();
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GLuint>(framebuffer));
    glDeleteFramebuffers(1, &target);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, static_cast<GLuint>(buffer));
    for (std::size_t i = 0; i < parameters.size(); ++i)
        glPixelStorei(parameters[i], values[i]);
    return valid && result->m_fence ? result : nullptr;
}
OpenGLReadback::~OpenGLReadback()
{
    const OpenGLResource::TransientContextLock lock;
    if (m_fence)
        glDeleteSync(m_fence);
    if (m_buffer)
        glDeleteBuffers(1, &m_buffer);
}
rhi::ReadbackStatus OpenGLReadback::poll()
{
    if (m_status != rhi::ReadbackStatus::Pending)
        return m_status;
    const OpenGLResource::TransientContextLock lock;
    const auto                                 status = glClientWaitSync(m_fence, 0, 0);
    if (status == GL_TIMEOUT_EXPIRED)
        return m_status;
    if (status != GL_ALREADY_SIGNALED && status != GL_CONDITION_SATISFIED)
        return m_status = rhi::ReadbackStatus::Failed;
    GLint buffer{};
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &buffer);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_buffer);
    const auto* source = static_cast<const std::uint8_t*>(
        glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, static_cast<GLsizeiptr>(m_byteCount), GL_MAP_READ_BIT));
    if (source)
    {
        m_data.resize(m_byteCount);
        for (unsigned int row = 0; row < m_size.y; ++row)
            std::memcpy(m_data.data() + row * m_rowBytes, source + (m_flip ? m_size.y - row - 1 : row) * m_rowBytes, m_rowBytes);
        m_status = glUnmapBuffer(GL_PIXEL_PACK_BUFFER) ? rhi::ReadbackStatus::Ready : rhi::ReadbackStatus::Failed;
    }
    else
        m_status = rhi::ReadbackStatus::Failed;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, static_cast<GLuint>(buffer));
    return m_status;
}
const std::vector<std::uint8_t>& OpenGLReadback::data() const
{
    return m_data;
}
} // namespace sf::priv
