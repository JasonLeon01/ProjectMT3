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

#include <SFML/Graphics/GraphicsDevice.hpp>
#include <SFML/Graphics/Metal/MetalRHI.hpp>
#include <SFML/Graphics/Metal/MetalReadback.hpp>
#include <SFML/Graphics/Metal/MetalRenderTarget.hpp>
#include <SFML/Graphics/Metal/MetalTimer.hpp>
#include <SFML/Graphics/Metal/MetalWindowSurface.hpp>

#include <SFML/Window/WindowImpl.hpp>

#include <cstring>

namespace sf::priv
{
template <>
bool MetalBufferResource::upload(const void* data, std::size_t size, std::size_t offset)
{
    if (!data || !fits(size, offset))
        return false;
    const auto lock = device->lock();
    return !size || device->write(*buffer, data, offset, size);
}
template <>
bool MetalBufferResource::copy(const rhi::Buffer& source)
{
    const auto* other = resourceCast<const MetalBufferResource>(&source);
    if (!other || other->device != device || other->bytes != bytes)
        return false;
    const auto lock = device->lock();
    return other == this || !bytes || device->copy(*other->buffer, *buffer, bytes);
}
template <>
bool MetalBufferResource::read(void* data, std::size_t size, std::size_t offset) const
{
    if (!data || !fits(size, offset))
        return false;
    const auto lock = device->lock();
    if (!size)
        return true;
    @autoreleasepool
    {
        const id<MTLBuffer> staging = [device->impl().device
            newBufferWithLength:size
                        options:MTLResourceStorageModeShared];
        if (!staging)
            return false;
        const id<MTLBlitCommandEncoder> encoder = device->impl().blit();
        if (!encoder)
            return false;
        [encoder copyFromBuffer:buffer->buffer sourceOffset:offset toBuffer:staging destinationOffset:0 size:size];
        if (!device->submit(true))
            return false;
        std::memcpy(data, staging.contents, size);
        return true;
    }
}
namespace
{
class MetalRhi final : public NativeDeviceResource<MetalDevice, MetalImage, MetalBuffer, MetalProgram, MetalRenderTarget>
{
public:
    using NativeDeviceResource::createShader;

    explicit MetalRhi(std::shared_ptr<MetalDevice> value) :
        NativeDeviceResource(std::move(value), GraphicsBackend::Metal, MetalSettings{})
    {
    }
    [[nodiscard]] rhi::Capabilities capabilities() const override
    {
        return {m_native->maximumTextureSize(), m_native->maximumSamples(), true, false, true, true, true, true, true};
    }
    [[nodiscard]] rhi::CacheIdentity getCacheIdentity() const override
    {
        const auto guard = m_native->lock();
        @autoreleasepool
        {
            const auto device = m_native->impl().device;
            return {std::string(device.name.UTF8String) + ":" + std::to_string(device.registryID),
                    NSProcessInfo.processInfo.operatingSystemVersionString.UTF8String};
        }
    }
    std::shared_ptr<rhi::Texture> createFloatTexture(Vector2u size, const float* data) override
    {
        auto image = m_native->makeTexture(size, false, false, true);
        if (!image || (data && !m_native->uploadFloats(*image, data, size, {})))
            return {};
        return std::make_shared<MetalTextureResource>(m_native, std::move(image), size, false);
    }
    std::shared_ptr<rhi::ShaderProgram> createShader(std::string_view          vertex,
                                                     std::string_view          geometry,
                                                     std::string_view          fragment,
                                                     const rhi::ShaderOptions& options) override
    {
        return MetalProgram::create(*m_native, vertex, geometry, fragment, options);
    }
    bool transformVertices(const std::shared_ptr<rhi::ShaderProgram>&   shader,
                           const std::vector<rhi::VertexBufferBinding>& bindings,
                           std::size_t                                  count,
                           const std::shared_ptr<rhi::Buffer>&          output) override
    {
        const auto program = resourcePointerCast<MetalProgram>(shader);
        return program && output && program->transformVertices(*m_native, bindings, count, *output);
    }
    std::shared_ptr<rhi::Readback> readTextureAsync(const std::shared_ptr<rhi::Texture>& texture, IntRect area) override
    {
        const auto resource = resourcePointerCast<MetalTextureResource>(texture);
        return resource && resource->device == m_native ? MetalReadback::create(m_native, *resource->image, area) : nullptr;
    }
    std::shared_ptr<rhi::Timer> beginTimer() override
    {
        return MetalTimer::create(m_native);
    }
    std::unique_ptr<rhi::WindowSurface> createSurface(const WindowImpl& owner, unsigned int, const ContextSettings& settings) override
    {
        auto surface = MetalWindowSurface::create(m_native, owner.getMetalLayer(), owner.getSize(), settings);
        return surface ? std::make_unique<NativeSurface<MetalTextureResource, MetalWindowSurface>>(std::move(surface))
                       : nullptr;
    }
    bool submit(bool wait) override
    {
        return m_native->submit(wait);
    }
    bool loadPipelineCache(const void* data, std::size_t size) override
    {
        return m_native->loadPipelineCache(data, size);
    }
    std::optional<std::vector<std::uint8_t>> getPipelineCacheData() override
    {
        return m_native->getPipelineCacheData();
    }
};
} // namespace
std::shared_ptr<rhi::Device> createMetalRhi()
{
    auto device = MetalDevice::create();
    return device ? std::make_shared<MetalRhi>(std::move(device)) : nullptr;
}
} // namespace sf::priv
