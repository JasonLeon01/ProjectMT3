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

#include <SFML/Graphics/DirectX/DirectXProgram.hpp>
#include <SFML/Graphics/DirectX/DirectXRHI.hpp>
#include <SFML/Graphics/DirectX/DirectXReadback.hpp>
#include <SFML/Graphics/DirectX/DirectXTimer.hpp>
#include <SFML/Graphics/DirectX/DirectXWindowSurface.hpp>
#include <SFML/Graphics/GraphicsDevice.hpp>

#include <SFML/Window/WindowImpl.hpp>

#include <limits>
#include <sstream>
#include <thread>

#include <cstring>

namespace sf::priv
{
template <>
bool DirectXBufferResource::upload(const void* data, std::size_t size, std::size_t offset)
{
    if (!data || !fits(size, offset))
        return false;
    const auto lock = device->lock();
    if (device->isLost())
        return false;
    if (!size)
        return true;
    const D3D11_BOX box{static_cast<UINT>(offset), 0, 0, static_cast<UINT>(offset + size), 1, 1};
    device->context()->UpdateSubresource(buffer->buffer.get(), 0, &box, data, 0, 0);
    return device->check(device->device()->GetDeviceRemovedReason(), "update vertex buffer");
}
template <>
bool DirectXBufferResource::copy(const rhi::Buffer& source)
{
    const auto* other = resourceCast<const DirectXBufferResource>(&source);
    if (!other || other->device != device || other->bytes != bytes)
        return false;
    const auto lock = device->lock();
    if (device->isLost())
        return false;
    if (other != this && bytes)
        device->context()->CopyResource(buffer->buffer.get(), other->buffer->buffer.get());
    return device->check(device->device()->GetDeviceRemovedReason(), "copy vertex buffer");
}
template <>
bool DirectXBufferResource::read(void* data, std::size_t size, std::size_t offset) const
{
    if (!data || !fits(size, offset))
        return false;
    const auto lock = device->lock();
    if (device->isLost())
        return false;
    if (!size)
        return true;
    D3D11_BUFFER_DESC desc{};
    buffer->buffer->GetDesc(&desc);
    desc.BindFlags      = 0;
    desc.MiscFlags      = 0;
    desc.Usage          = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11Buffer> staging;
    if (!device->check(device->device()->CreateBuffer(&desc, nullptr, staging.put()), "create readback buffer"))
        return false;
    device->context()->CopyResource(staging.get(), buffer->buffer.get());
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (!device->check(device->context()->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped), "read vertex buffer"))
        return false;
    std::memcpy(data, static_cast<const std::byte*>(mapped.pData) + offset, size);
    device->context()->Unmap(staging.get(), 0);
    return true;
}

namespace
{
class DirectXRhi final :
    public NativeDeviceResource<DirectXDevice, DirectXImage, DirectXBuffer, DirectXProgram, DirectXRenderTarget>
{
public:
    using NativeDeviceResource::createShader;

    explicit DirectXRhi(const std::shared_ptr<DirectXDevice>& value) :
        NativeDeviceResource(value, GraphicsBackend::DirectX, value->settings())
    {
    }
    [[nodiscard]] rhi::Capabilities capabilities() const override
    {
        return {m_native->maximumTextureSize(),
                DirectXRenderTarget::getMaximumAntiAliasingLevel(*m_native),
                DirectXProgram::compilerAvailable(),
                DirectXProgram::compilerAvailable(),
                true,
                true,
                true,
                true,
                true};
    }
    [[nodiscard]] rhi::CacheIdentity getCacheIdentity() const override
    {
        const auto           guard = m_native->lock();
        ComPtr<IDXGIDevice>  device;
        ComPtr<IDXGIAdapter> adapter;
        if (FAILED(m_native->device()->QueryInterface(IID_IDXGIDevice, reinterpret_cast<void**>(device.put()))) ||
            FAILED(device->GetAdapter(adapter.put())))
            return {};
        DXGI_ADAPTER_DESC desc{};
        LARGE_INTEGER     version{};
        if (FAILED(adapter->GetDesc(&desc)))
            return {};
        std::ostringstream identity;
        std::ostringstream driver;
        identity << std::hex << desc.VendorId << ':' << desc.DeviceId << ':' << desc.SubSysId << ':' << desc.Revision;
        if (FAILED(adapter->CheckInterfaceSupport(IID_IDXGIDevice, &version)))
            return {identity.str(), {}};
        driver << std::hex << version.QuadPart;
        driver << ':' << m_native->settings().majorVersion << '.' << m_native->settings().minorVersion;
        return {identity.str(), driver.str()};
    }
    std::shared_ptr<rhi::Texture> createFloatTexture(Vector2u size, const float* data) override
    {
        auto image = m_native->makeFloatTexture(size, data);
        return image ? std::make_shared<DirectXTextureResource>(m_native, std::move(image), size, false) : nullptr;
    }
    std::shared_ptr<rhi::ShaderProgram> createShader(std::string_view          vertex,
                                                     std::string_view          geometry,
                                                     std::string_view          fragment,
                                                     const rhi::ShaderOptions& options) override
    {
        return DirectXProgram::create(*m_native, vertex, geometry, fragment, options);
    }
    bool transformVertices(const std::shared_ptr<rhi::ShaderProgram>&   shader,
                           const std::vector<rhi::VertexBufferBinding>& bindings,
                           std::size_t                                  count,
                           const std::shared_ptr<rhi::Buffer>&          output) override
    {
        const auto program = resourcePointerCast<DirectXProgram>(shader);
        return program && output && program->transformVertices(*m_native, bindings, count, *output);
    }
    std::shared_ptr<rhi::Readback> readTextureAsync(const std::shared_ptr<rhi::Texture>& texture, IntRect area) override
    {
        const auto value = resourcePointerCast<DirectXTextureResource>(texture);
        return value && value->device == m_native ? DirectXReadback::create(m_native, *value->image, area) : nullptr;
    }
    std::shared_ptr<rhi::Timer> beginTimer() override
    {
        return DirectXTimer::create(m_native);
    }
    std::unique_ptr<rhi::WindowSurface> createSurface(const WindowImpl& owner, unsigned int, const ContextSettings& settings) override
    {
        auto surface = DirectXWindowSurface::create(m_native, owner.getNativeHandle(), owner.getSize(), settings);
        return surface ? std::make_unique<NativeSurface<DirectXTextureResource, DirectXWindowSurface>>(std::move(surface))
                       : nullptr;
    }
    bool submit(bool wait) override
    {
        const auto lock = m_native->lock();
        if (m_native->isLost())
            return false;
        ComPtr<ID3D11Query> fence;
        if (wait)
        {
            const D3D11_QUERY_DESC desc{D3D11_QUERY_EVENT, 0};
            if (!m_native->check(m_native->device()->CreateQuery(&desc, fence.put()), "create submission fence"))
                return false;
            m_native->context()->End(fence.get());
        }
        m_native->context()->Flush();
        if (wait)
        {
            HRESULT result{};
            while ((result = m_native->context()->GetData(fence.get(), nullptr, 0, 0)) == S_FALSE)
                std::this_thread::yield();
            if (!m_native->check(result, "wait for submission"))
                return false;
        }
        return m_native->check(m_native->device()->GetDeviceRemovedReason(), "submit graphics commands");
    }
    bool loadPipelineCache(const void*, std::size_t) override
    {
        return false;
    }
    std::optional<std::vector<std::uint8_t>> getPipelineCacheData() override
    {
        return std::nullopt;
    }
};
} // namespace
std::shared_ptr<rhi::Device> createDirectXRhi(const DirectXSettings& settings)
{
    auto device = DirectXDevice::create(settings);
    return device ? std::make_shared<DirectXRhi>(std::move(device)) : nullptr;
}
} // namespace sf::priv
