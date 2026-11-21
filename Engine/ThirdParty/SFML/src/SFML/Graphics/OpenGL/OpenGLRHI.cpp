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
#include <SFML/Graphics/GraphicsDevice.hpp>
#include <SFML/Graphics/OpenGL/OpenGLBuffer.hpp>
#include <SFML/Graphics/OpenGL/OpenGLProgram.hpp>
#include <SFML/Graphics/OpenGL/OpenGLReadback.hpp>
#include <SFML/Graphics/OpenGL/OpenGLRenderTarget.hpp>
#include <SFML/Graphics/OpenGL/OpenGLResource.hpp>
#include <SFML/Graphics/OpenGL/OpenGLTexture.hpp>
#include <SFML/Graphics/OpenGL/OpenGLTimer.hpp>
#include <SFML/Graphics/RenderTextureImplFBO.hpp>

#include <SFML/Window/GlContext.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/WindowImpl.hpp>

namespace sf::priv
{
void bindOpenGLTexture(const rhi::Texture* texture, CoordinateType type, bool programmable)
{
    const OpenGLResource::TransientContextLock lock;
    ensureExtensionsInit();
    const auto* resource = resourceCast<const OpenGLTexture>(texture);
    glCheck(glBindTexture(GL_TEXTURE_2D, resource ? resource->nativeHandle() : 0));
#ifndef SFML_OPENGL_ES
    if (!programmable)
    {
        const auto matrix = texture ? texture->coordinateMatrix(type)
                                    : std::array<float, 16>{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        glCheck(glMatrixMode(GL_TEXTURE));
        glCheck(glLoadMatrixf(matrix.data()));
        glCheck(glMatrixMode(GL_MODELVIEW));
    }
#else
    (void)type;
    (void)programmable;
#endif
}
namespace
{
class OpenGLSurface final : public SurfaceResource
{
public:
    OpenGLSurface(std::shared_ptr<priv::WindowSurface> value, Vector2u size, bool programmable) :
        surface(std::move(value)),
        target(std::make_shared<OpenGLRenderTarget>(programmable))
    {
        target->attachWindow(surface, size);
    }
    [[nodiscard]] std::shared_ptr<rhi::RenderTarget> getRenderTarget() const override
    {
        return target;
    }
    bool setActive(bool active) override
    {
        return target->setActive(active);
    }
    void resize(Vector2u size) override
    {
        target->resizeWindow(size);
    }
    void display() override
    {
        if (target->setActive(true))
            surface->display();
    }
    void setVerticalSyncEnabled(bool enabled) override
    {
        surface->setVerticalSyncEnabled(enabled);
    }
#ifdef SFML_SYSTEM_ANDROID
    void setNativeWindow(WindowHandle handle) override
    {
        surface->setNativeWindow(handle);
    }
#endif
    bool copyTo(rhi::Texture& destination, Vector2u offset) const override
    {
        auto* texture = resourceCast<OpenGLTexture>(&destination);
        return texture && target->setActive(true) && texture->copyWindow(target->getSize(), offset);
    }
    std::shared_ptr<priv::WindowSurface> surface;
    std::shared_ptr<OpenGLRenderTarget>  target;
};
class OpenGLRhi final : public DeviceResource
{
public:
    OpenGLRhi(std::shared_ptr<void> context, ContextSettings::BackendSettings settings) :
        m_context(std::move(context)),
        m_settings(settings)
    {
        const auto* gl = std::get_if<OpenGLSettings>(&m_settings);
        m_programmable = !gl || gl->profile == OpenGLProfile::Core;
    }
    [[nodiscard]] GraphicsBackend backend() const override
    {
        return std::holds_alternative<OpenGLSettings>(m_settings) ? GraphicsBackend::OpenGL : GraphicsBackend::OpenGLES;
    }
    [[nodiscard]] ContextSettings::BackendSettings settings() const override
    {
        return m_settings;
    }
    [[nodiscard]] rhi::Capabilities capabilities() const override
    {
        return {OpenGLTexture::maximumSize(),
                RenderTextureImplFBO::isAvailable() ? RenderTextureImplFBO::getMaximumAntiAliasingLevel() : 0,
                true,
                OpenGLProgram::geometryAvailable(),
                glBeginTransformFeedback != nullptr,
                glDrawArraysInstanced && (glVertexAttribDivisor || glVertexAttribDivisorARB),
                true,
                glFenceSync && glMapBufferRange,
                OpenGLTimer::isAvailable()};
    }
    [[nodiscard]] rhi::CacheIdentity getCacheIdentity() const override
    {
        const OpenGLResource::TransientContextLock lock;
        ensureExtensionsInit();
        const auto value = [](GLenum name)
        {
            const auto* text = glGetString(name);
            return text ? std::string(reinterpret_cast<const char*>(text)) : std::string{};
        };
        return {value(GL_VENDOR) + ":" + value(GL_RENDERER), value(GL_VERSION) + ":" + value(GL_SHADING_LANGUAGE_VERSION)};
    }
    std::shared_ptr<rhi::Texture> createFloatTexture(Vector2u size, const float* data) override
    {
        auto texture = std::make_shared<OpenGLTexture>(m_context);
        return texture->resizeFloat(size, data) ? texture : nullptr;
    }
    std::shared_ptr<rhi::ShaderProgram> createShader(std::string_view          vertex,
                                                     std::string_view          geometry,
                                                     std::string_view          fragment,
                                                     const rhi::ShaderOptions& options) override
    {
        auto shader = std::make_shared<OpenGLProgram>(m_context, m_programmable);
        return shader->compile(vertex, geometry, fragment, options) ? shader : nullptr;
    }
    bool transformVertices(const std::shared_ptr<rhi::ShaderProgram>&   shader,
                           const std::vector<rhi::VertexBufferBinding>& bindings,
                           std::size_t                                  count,
                           const std::shared_ptr<rhi::Buffer>&          output) override
    {
        auto program = resourcePointerCast<OpenGLProgram>(shader);
        return program && output && program->transformVertices(bindings, count, *output);
    }
    std::shared_ptr<rhi::Readback> readTextureAsync(const std::shared_ptr<rhi::Texture>& texture, IntRect area) override
    {
        auto value = resourcePointerCast<OpenGLTexture>(texture);
        return value ? OpenGLReadback::create(m_context, *value, area) : nullptr;
    }
    std::shared_ptr<rhi::Timer> beginTimer() override
    {
        return OpenGLTimer::create(m_context);
    }
    std::shared_ptr<rhi::Texture> createTexture(Vector2u size, bool srgb) override
    {
        auto texture = std::make_shared<OpenGLTexture>(m_context);
        return texture->resize(size, srgb) ? texture : nullptr;
    }
    std::shared_ptr<rhi::Buffer> createBuffer(std::size_t size, rhi::BufferUsage usage) override
    {
        auto buffer = std::make_shared<OpenGLBuffer>(m_context);
        return buffer->create(size, usage) ? buffer : nullptr;
    }
    std::shared_ptr<rhi::ShaderProgram> createShader(std::string_view vertex,
                                                     std::string_view geometry,
                                                     std::string_view fragment) override
    {
        auto shader = std::make_shared<OpenGLProgram>(m_context, m_programmable);
        return shader->compile(vertex, geometry, fragment) ? shader : nullptr;
    }
    std::shared_ptr<rhi::RenderTarget> createRenderTarget(std::shared_ptr<rhi::Texture> color,
                                                          const ContextSettings&        settings) override
    {
        const auto texture = resourcePointerCast<OpenGLTexture>(color);
        return texture && acceptsSettings(settings) ? OpenGLRenderTarget::create(texture, settings, m_programmable) : nullptr;
    }
    std::unique_ptr<rhi::WindowSurface> createSurface(const WindowImpl& owner, unsigned int bits, const ContextSettings& settings) override
    {
        auto surface = GlContext::create(settings, owner, bits);
        return surface ? std::make_unique<OpenGLSurface>(std::move(surface), owner.getSize(), m_programmable) : nullptr;
    }
    bool submit(bool wait) override
    {
        const OpenGLResource::TransientContextLock lock;
        if (wait)
            glCheck(glFinish());
        else
            glCheck(glFlush());
        return true;
    }
    bool loadPipelineCache(const void*, std::size_t) override
    {
        return false;
    }
    std::optional<std::vector<std::uint8_t>> getPipelineCacheData() override
    {
        return std::nullopt;
    }
    void bindTexture(const rhi::Texture* texture, CoordinateType type) override
    {
        bindOpenGLTexture(texture, type, m_programmable);
    }
    void bindBuffer(const rhi::Buffer* buffer) override
    {
        OpenGLBuffer::bind(buffer);
    }
    void bindShader(const rhi::ShaderProgram* shader) override
    {
        OpenGLProgram::bind(shader);
    }
    bool copyWindow(const Window& window, rhi::Texture& value, Vector2u offset) override
    {
        auto* texture = resourceCast<OpenGLTexture>(&value);
        return texture && window.setActive(true) && texture->copyWindow(window.getSize(), offset);
    }

private:
    std::shared_ptr<void>            m_context;
    ContextSettings::BackendSettings m_settings;
    bool                             m_programmable;
};
} // namespace
std::shared_ptr<rhi::Device> createOpenGLRhi(const ContextSettings& requested)
{
    auto settings = requested;
    auto context  = GlContext::prepareSharedContext(settings);
    return context ? std::make_shared<OpenGLRhi>(std::move(context), settings.backend) : nullptr;
}
} // namespace sf::priv
