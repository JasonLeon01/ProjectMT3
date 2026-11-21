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

#include <SFML/Graphics/Metal/MetalReadback.hpp>

#include <cstring>

namespace sf::priv
{
std::shared_ptr<MetalReadback> MetalReadback::create(std::shared_ptr<MetalDevice> device, const MetalImage& image, IntRect area)
{
    const auto guard = device->lock();
    if (image.texture.pixelFormat != MTLPixelFormatRGBA8Unorm && image.texture.pixelFormat != MTLPixelFormatRGBA8Unorm_sRGB)
        return {};
    if (area.position.x < 0 || area.position.y < 0 || area.size.x <= 0 || area.size.y <= 0 ||
        area.position.x > static_cast<int>(image.size.x) || area.position.y > static_cast<int>(image.size.y) ||
        area.size.x > static_cast<int>(image.size.x) - area.position.x ||
        area.size.y > static_cast<int>(image.size.y) - area.position.y)
        return {};
    @autoreleasepool
    {
        auto result               = std::make_shared<MetalReadback>();
        result->m_device          = std::move(device);
        result->m_size            = Vector2u(area.size);
        result->m_rowBytes        = (static_cast<std::size_t>(area.size.x) * 4 + 255) & ~std::size_t{255};
        auto&             context = result->m_device->impl();
        const std::size_t bytes   = result->m_rowBytes * result->m_size.y;
        result->m_buffer          = [context.device newBufferWithLength:bytes options:MTLResourceStorageModeShared];
        if (!result->m_buffer)
            return {};
        result->m_commands = context.commandBuffer();
        if (!result->m_commands)
            return {};
        const id<MTLBlitCommandEncoder> encoder = context.blit();
        if (!encoder)
            return {};
        [encoder copyFromTexture:image.texture
                         sourceSlice:0
                         sourceLevel:0
                        sourceOrigin:MTLOriginMake(static_cast<NSUInteger>(area.position.x),
                                                   static_cast<NSUInteger>(area.position.y),
                                                   0)
                          sourceSize:MTLSizeMake(static_cast<NSUInteger>(area.size.x), static_cast<NSUInteger>(area.size.y), 1)
                            toBuffer:result->m_buffer
                   destinationOffset:0
              destinationBytesPerRow:result->m_rowBytes
            destinationBytesPerImage:bytes];
        if (!context.submit(false))
            return {};
        return result;
    }
}

rhi::ReadbackStatus MetalReadback::poll()
{
    if (m_status != rhi::ReadbackStatus::Pending)
        return m_status;
    const auto status = m_commands.status;
    if (status == MTLCommandBufferStatusError)
        return m_status = rhi::ReadbackStatus::Failed;
    if (status != MTLCommandBufferStatusCompleted)
        return m_status;
    const auto* source = static_cast<const std::uint8_t*>(m_buffer.contents);
    if (!source)
        return m_status = rhi::ReadbackStatus::Failed;
    const std::size_t packed = static_cast<std::size_t>(m_size.x) * 4;
    m_data.resize(packed * m_size.y);
    for (unsigned int row = 0; row < m_size.y; ++row)
        std::memcpy(m_data.data() + row * packed, source + row * m_rowBytes, packed);
    m_buffer        = nil;
    m_commands      = nil;
    return m_status = rhi::ReadbackStatus::Ready;
}

const std::vector<std::uint8_t>& MetalReadback::data() const
{
    return m_data;
}
} // namespace sf::priv
