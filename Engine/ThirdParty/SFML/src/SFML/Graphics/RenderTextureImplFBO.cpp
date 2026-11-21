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
#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/Graphics/GLExtensions.hpp>
#include <SFML/Graphics/RenderTextureImplFBO.hpp>

#include <SFML/Window/Context.hpp>
#include <SFML/Window/ContextSettings.hpp>

#include <SFML/System/Err.hpp>

#include <ostream>
#include <utility>


namespace sf::priv
{
////////////////////////////////////////////////////////////
struct RenderTextureImplFBO::FrameBufferObject
{
    FrameBufferObject()
    {
        // Create the framebuffer object
        glCheck(GLEXT_glGenFramebuffers(1, &object));
    }

    ~FrameBufferObject()
    {
        if (object)
            glCheck(GLEXT_glDeleteFramebuffers(1, &object));
    }

    GLuint object{};
};


////////////////////////////////////////////////////////////
RenderTextureImplFBO::RenderTextureImplFBO() = default;


////////////////////////////////////////////////////////////
RenderTextureImplFBO::~RenderTextureImplFBO()
{
    const TransientContextLock contextLock;

    // Destroy the color buffer
    if (m_colorBuffer)
    {
        const GLuint colorBuffer = m_colorBuffer;
        glCheck(GLEXT_glDeleteRenderbuffers(1, &colorBuffer));
    }

    // Destroy the depth/stencil buffer
    if (m_depthBuffer)
    {
        const GLuint depthStencilBuffer = m_depthBuffer;
        glCheck(GLEXT_glDeleteRenderbuffers(1, &depthStencilBuffer));
    }

    if (m_stencilBuffer && m_stencilBuffer != m_depthBuffer)
    {
        const GLuint stencilBuffer = m_stencilBuffer;
        glCheck(GLEXT_glDeleteRenderbuffers(1, &stencilBuffer));
    }

    // Unregister FBOs with the contexts if they haven't already been destroyed
    for (auto& entry : m_frameBuffers)
    {
        auto frameBuffer = entry.second.lock();

        if (frameBuffer)
            unregisterUnsharedGlObject(std::move(frameBuffer));
    }

    for (auto& entry : m_multisampleFrameBuffers)
    {
        auto frameBuffer = entry.second.lock();

        if (frameBuffer)
            unregisterUnsharedGlObject(std::move(frameBuffer));
    }
}


////////////////////////////////////////////////////////////
bool RenderTextureImplFBO::isAvailable()
{
    const TransientContextLock lock;

    // Make sure that extensions are initialized
    ensureExtensionsInit();

    return GLEXT_framebuffer_object != 0;
}


////////////////////////////////////////////////////////////
unsigned int RenderTextureImplFBO::getMaximumAntiAliasingLevel()
{

    const TransientContextLock lock;
    ensureExtensionsInit();
    if (!GLEXT_framebuffer_multisample_blit)
        return 0;

    GLint samples = 0;
    glCheck(glGetIntegerv(GLEXT_GL_MAX_SAMPLES, &samples));
    return static_cast<unsigned int>(samples);
}


////////////////////////////////////////////////////////////
void RenderTextureImplFBO::unbind()
{
    glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, 0));
}


////////////////////////////////////////////////////////////
bool RenderTextureImplFBO::create(Vector2u size, unsigned int textureId, const ContextSettings& settings)
{
    // Store the dimensions
    m_size = size;

    {
        const TransientContextLock lock;

        // Make sure that extensions are initialized
        ensureExtensionsInit();

        if (settings.antiAliasingLevel && !GLEXT_framebuffer_multisample_blit)
            return false;

        m_sRgb = settings.sRgbCapable && GLEXT_texture_sRGB;
#ifndef SFML_OPENGL_ES
        // Desktop GL additionally requires opt-in framebuffer encoding.
        m_sRgb = m_sRgb && GLEXT_framebuffer_sRGB;
#endif
        if (settings.sRgbCapable && !m_sRgb)
            return false;

        // Check if the requested anti-aliasing level is supported
        if (settings.antiAliasingLevel)
        {
            GLint samples = 0;
            glCheck(glGetIntegerv(GLEXT_GL_MAX_SAMPLES, &samples));

            if (settings.antiAliasingLevel > static_cast<unsigned int>(samples))
            {
                err() << "Impossible to create render texture (unsupported anti-aliasing level)"
                      << " Requested: " << settings.antiAliasingLevel << " Maximum supported: " << samples << std::endl;
                return false;
            }
        }


        const auto createRenderbuffer = [&](GLenum format, const char* label)
        {
            GLuint buffer{};
            glCheck(GLEXT_glGenRenderbuffers(1, &buffer));
            if (!buffer)
            {
                err() << "Impossible to create render texture (failed to create the " << label << " buffer)" << std::endl;
                return buffer;
            }

            glCheck(GLEXT_glBindRenderbuffer(GLEXT_GL_RENDERBUFFER, buffer));
            const auto width  = static_cast<GLsizei>(size.x);
            const auto height = static_cast<GLsizei>(size.y);
            if (settings.antiAliasingLevel)
            {
                glCheck(GLEXT_glRenderbufferStorageMultisample(GLEXT_GL_RENDERBUFFER,
                                                               static_cast<GLsizei>(settings.antiAliasingLevel),
                                                               format,
                                                               width,
                                                               height));
            }
            else
                glCheck(GLEXT_glRenderbufferStorage(GLEXT_GL_RENDERBUFFER, format, width, height));

            return buffer;
        };

        m_multisample = settings.antiAliasingLevel != 0;
        if (m_multisample)
        {
            m_colorBuffer = createRenderbuffer(m_sRgb ? GLEXT_GL_SRGB8_ALPHA8 : GL_RGBA8, "multisample color");
            if (!m_colorBuffer)
                return false;
        }

        if (settings.depthBits && settings.stencilBits && GLEXT_packed_depth_stencil)
        {
            m_depthBuffer   = createRenderbuffer(GLEXT_GL_DEPTH24_STENCIL8, "depth/stencil");
            m_stencilBuffer = m_depthBuffer;
        }
        else
        {
            if (settings.depthBits)
                m_depthBuffer = createRenderbuffer(GLEXT_GL_DEPTH_COMPONENT, "depth");
            if (settings.stencilBits)
                m_stencilBuffer = createRenderbuffer(GLEXT_GL_STENCIL_INDEX8, "stencil");
        }

        if ((settings.depthBits && !m_depthBuffer) || (settings.stencilBits && !m_stencilBuffer))
            return false;
    }

    // Save our texture ID in order to be able to attach it to an FBO at any time
    m_textureId = textureId;

    // We can't create an FBO now if there is no active context
    if (!Context::getActiveContextId())
        return true;


    // EXT_framebuffer_object alone exposes a single binding for reads and draws.
    if (!GLEXT_framebuffer_blit)
    {
        GLint framebuffer{};
        glCheck(glGetIntegerv(GLEXT_GL_FRAMEBUFFER_BINDING, &framebuffer));
        const bool created = createFrameBuffer();
        glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, static_cast<GLuint>(framebuffer)));
        return created;
    }

    GLint readFramebuffer{};
    GLint drawFramebuffer{};
    glCheck(glGetIntegerv(GLEXT_GL_READ_FRAMEBUFFER_BINDING, &readFramebuffer));
    glCheck(glGetIntegerv(GLEXT_GL_DRAW_FRAMEBUFFER_BINDING, &drawFramebuffer));

    const bool created = createFrameBuffer();
    glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_READ_FRAMEBUFFER, static_cast<GLuint>(readFramebuffer)));
    glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(drawFramebuffer)));
    return created;
}


////////////////////////////////////////////////////////////
void RenderTextureImplFBO::attachDepthStencil() const
{
    if (m_depthBuffer)
        glCheck(
            GLEXT_glFramebufferRenderbuffer(GLEXT_GL_FRAMEBUFFER, GLEXT_GL_DEPTH_ATTACHMENT, GLEXT_GL_RENDERBUFFER, m_depthBuffer));
    if (m_stencilBuffer)
        glCheck(GLEXT_glFramebufferRenderbuffer(GLEXT_GL_FRAMEBUFFER,
                                                GLEXT_GL_STENCIL_ATTACHMENT,
                                                GLEXT_GL_RENDERBUFFER,
                                                m_stencilBuffer));
}


////////////////////////////////////////////////////////////
bool RenderTextureImplFBO::createFrameBuffer()
{
    // Create the framebuffer object
    auto frameBuffer = std::make_shared<FrameBufferObject>();

    if (!frameBuffer->object)
    {
        err() << "Impossible to create render texture (failed to create the frame buffer object)" << std::endl;
        return false;
    }
    glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, frameBuffer->object));

    // Link the depth/stencil renderbuffer to the frame buffer
    if (!m_multisample)
        attachDepthStencil();

    // Link the texture to the frame buffer
    glCheck(GLEXT_glFramebufferTexture2D(GLEXT_GL_FRAMEBUFFER, GLEXT_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureId, 0));

    // A final check, just to be sure...
    if (glCheck(GLEXT_glCheckFramebufferStatus(GLEXT_GL_FRAMEBUFFER)) != GLEXT_GL_FRAMEBUFFER_COMPLETE)
    {
        glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, 0));
        err() << "Impossible to create render texture (failed to link the target texture to the frame buffer)" << std::endl;
        return false;
    }

    // Insert the FBO into our map
    m_frameBuffers.try_emplace(Context::getActiveContextId(), frameBuffer);

    // Register the object with the current context so it is automatically destroyed
    registerUnsharedGlObject(std::move(frameBuffer));


    if (m_multisample)
    {
        // Create the multisample framebuffer object
        auto multisampleFrameBuffer = std::make_shared<FrameBufferObject>();

        if (!multisampleFrameBuffer->object)
        {
            err() << "Impossible to create render texture (failed to create the multisample frame buffer object)"
                  << std::endl;
            return false;
        }
        glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, multisampleFrameBuffer->object));

        // Link the multisample color buffer to the frame buffer
        glCheck(GLEXT_glBindRenderbuffer(GLEXT_GL_RENDERBUFFER, m_colorBuffer));
        glCheck(
            GLEXT_glFramebufferRenderbuffer(GLEXT_GL_FRAMEBUFFER, GLEXT_GL_COLOR_ATTACHMENT0, GLEXT_GL_RENDERBUFFER, m_colorBuffer));

        // Link the depth/stencil renderbuffer to the frame buffer
        attachDepthStencil();

        // A final check, just to be sure...
        if (glCheck(GLEXT_glCheckFramebufferStatus(GLEXT_GL_FRAMEBUFFER)) != GLEXT_GL_FRAMEBUFFER_COMPLETE)
        {
            glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, 0));
            err() << "Impossible to create render texture (failed to link the render buffers to the multisample frame "
                     "buffer)"
                  << std::endl;
            return false;
        }

        // Insert the FBO into our map
        m_multisampleFrameBuffers.try_emplace(Context::getActiveContextId(), multisampleFrameBuffer);

        // Register the object with the current context so it is automatically destroyed
        registerUnsharedGlObject(std::move(multisampleFrameBuffer));
    }


    return true;
}


////////////////////////////////////////////////////////////
bool RenderTextureImplFBO::activate(bool active)
{
    // Unbind the FBO if requested
    if (!active)
    {
        glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, 0));
        return true;
    }

    std::uint64_t contextId = Context::getActiveContextId();

    // In the odd case we have to activate and there is no active
    // context yet, we have to create one
    if (!contextId)
    {
        if (!m_context)
            m_context = std::make_unique<Context>();

        if (!m_context->setActive(true))
        {
            err() << "Failed to set context as active during render texture activation" << std::endl;
            return false;
        }

        contextId = Context::getActiveContextId();

        if (!contextId)
        {
            err() << "Impossible to activate render texture (failed to create backup context)" << std::endl;
            return false;
        }
    }

    // Lookup the FBO corresponding to the currently active context
    // If none is found, there is no FBO corresponding to the
    // currently active context so we will have to create a new FBO
    if (m_multisample)
    {
        const auto it = m_multisampleFrameBuffers.find(contextId);

        if (it != m_multisampleFrameBuffers.end())
        {
            const auto frameBuffer = it->second.lock();

            if (frameBuffer)
            {
                glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, frameBuffer->object));

                return true;
            }
        }
    }
    else
    {
        const auto it = m_frameBuffers.find(contextId);

        if (it != m_frameBuffers.end())
        {
            const auto frameBuffer = it->second.lock();

            if (frameBuffer)
            {
                glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, frameBuffer->object));

                return true;
            }
        }
    }

    return createFrameBuffer();
}


////////////////////////////////////////////////////////////
bool RenderTextureImplFBO::isSrgb() const
{
    return m_sRgb;
}


////////////////////////////////////////////////////////////
void RenderTextureImplFBO::updateTexture(unsigned int)
{
    // If multisampling is enabled, we need to resolve by blitting
    // from our FBO with multisample renderbuffer attachments
    // to our FBO to which our target texture is attached


    // In case of multisampling, make sure both FBOs
    // are already available within the current context
    if (m_multisample && m_size.x && m_size.y && activate(true))
    {
        const std::uint64_t contextId = Context::getActiveContextId();

        const auto frameBufferIt = m_frameBuffers.find(contextId);
        const auto multisampleIt = m_multisampleFrameBuffers.find(contextId);

        if ((frameBufferIt != m_frameBuffers.end()) && (multisampleIt != m_multisampleFrameBuffers.end()))
        {
            const auto frameBuffer            = frameBufferIt->second.lock();
            const auto multiSampleFrameBuffer = multisampleIt->second.lock();

            if (frameBuffer && multiSampleFrameBuffer)
            {
                // Scissor testing affects framebuffer blits as well
                // Since we don't want scissor testing to interfere with our copying, we temporarily disable it for the blit if it is enabled
                GLboolean scissorEnabled = GL_FALSE;
                glCheck(glGetBooleanv(GL_SCISSOR_TEST, &scissorEnabled));

                if (scissorEnabled == GL_TRUE)
                    glCheck(glDisable(GL_SCISSOR_TEST));

                // Set up the blit target (draw framebuffer) and blit (from the read framebuffer, our multisample FBO)
                glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_DRAW_FRAMEBUFFER, frameBuffer->object));
                glCheck(GLEXT_glBlitFramebuffer(0,
                                                0,
                                                static_cast<GLint>(m_size.x),
                                                static_cast<GLint>(m_size.y),
                                                0,
                                                0,
                                                static_cast<GLint>(m_size.x),
                                                static_cast<GLint>(m_size.y),
                                                GL_COLOR_BUFFER_BIT,
                                                GL_NEAREST));
                glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_DRAW_FRAMEBUFFER, multiSampleFrameBuffer->object));

                // Re-enable scissor testing if it was previously enabled
                if (scissorEnabled == GL_TRUE)
                    glCheck(glEnable(GL_SCISSOR_TEST));
            }
        }
    }
}

} // namespace sf::priv
