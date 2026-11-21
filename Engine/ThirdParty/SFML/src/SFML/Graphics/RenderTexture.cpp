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
#include <SFML/Graphics/GraphicsDevice.hpp>
#include <SFML/Graphics/RHI/Access.hpp>
#include <SFML/Graphics/RenderTargetBackend.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/Exception.hpp>

#include <memory>
#include <ostream>
#include <utility>


namespace sf
{
////////////////////////////////////////////////////////////
RenderTexture::RenderTexture() = default;


////////////////////////////////////////////////////////////
RenderTexture::RenderTexture(Vector2u size, const ContextSettings& settings)
{
    if (!resize(size, settings))
        throw Exception("Failed to create render texture");
}


////////////////////////////////////////////////////////////
RenderTexture::~RenderTexture() = default;


////////////////////////////////////////////////////////////
RenderTexture::RenderTexture(RenderTexture&&) noexcept = default;


////////////////////////////////////////////////////////////
RenderTexture& RenderTexture::operator=(RenderTexture&&) noexcept = default;


////////////////////////////////////////////////////////////
bool RenderTexture::resize(Vector2u size, const ContextSettings& settings)
{
    return priv::withGraphicsDevice(settings,
                                    [&](const priv::GraphicsDevice& device, const ContextSettings& effective)
                                    {
                                        Texture texture;
                                        texture.setRepeated(m_texture.isRepeated());
                                        const auto createTarget = [&](bool srgb) -> std::shared_ptr<rhi::RenderTarget>
                                        {
                                            if (!texture.resize(size, srgb))
                                                return {};
                                            ContextSettings actual = effective;
                                            actual.sRgbCapable     = texture.isSrgb();
                                            return device.device->createRenderTarget(texture.m_resource, actual);
                                        };
                                        auto target = createTarget(effective.sRgbCapable);
                                        if (texture.isSrgb() && (!target || !target->settings().sRgbCapable) &&
                                            (device.backend == GraphicsBackend::OpenGL ||
                                             device.backend == GraphicsBackend::OpenGLES))
                                        {
                                            target = createTarget(false);
                                        }
                                        if (!target || target->settings().sRgbCapable != texture.isSrgb())
                                            return false;
                                        m_backend = std::static_pointer_cast<priv::RenderTargetBackend>(target);
                                        m_texture = std::move(texture);
                                        RenderTarget::initialize();
                                        return true;
                                    });
}


////////////////////////////////////////////////////////////
unsigned int RenderTexture::getMaximumAntiAliasingLevel()
{
    const auto device = rhi::getDevice();
    return device ? device->capabilities().maximumAntiAliasingLevel : 0;
}


////////////////////////////////////////////////////////////
void RenderTexture::setSmooth(bool smooth)
{
    m_texture.setSmooth(smooth);
}


////////////////////////////////////////////////////////////
bool RenderTexture::isSmooth() const
{
    return m_texture.isSmooth();
}


////////////////////////////////////////////////////////////
void RenderTexture::setRepeated(bool repeated)
{
    m_texture.setRepeated(repeated);
}


////////////////////////////////////////////////////////////
bool RenderTexture::isRepeated() const
{
    return m_texture.isRepeated();
}


////////////////////////////////////////////////////////////
bool RenderTexture::generateMipmap()
{
    return m_texture.generateMipmap();
}


////////////////////////////////////////////////////////////
bool RenderTexture::setActive(bool active)
{
    return m_backend && m_backend->setActive(active);
}


////////////////////////////////////////////////////////////
void RenderTexture::display()
{
    if (m_backend)
    {
        m_backend->finish();
        m_texture.invalidateMipmap();
        if (const auto device = rhi::getDevice())
            (void)device->submit(false);
    }
}


////////////////////////////////////////////////////////////
Vector2u RenderTexture::getSize() const
{
    return m_texture.getSize();
}


////////////////////////////////////////////////////////////
bool RenderTexture::isSrgb() const
{
    return m_backend ? m_backend->settings().sRgbCapable : m_texture.isSrgb();
}


////////////////////////////////////////////////////////////
const Texture& RenderTexture::getTexture() const
{
    return m_texture;
}

} // namespace sf
