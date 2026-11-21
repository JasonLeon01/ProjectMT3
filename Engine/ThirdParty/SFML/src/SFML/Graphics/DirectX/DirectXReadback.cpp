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

#include <SFML/Graphics/DirectX/DirectXReadback.hpp>

#include <cstring>
namespace sf::priv
{
std::shared_ptr<DirectXReadback> DirectXReadback::create(const std::shared_ptr<DirectXDevice>& device,
                                                         const DirectXImage&                   image,
                                                         IntRect                               area)
{
    if (area.position.x < 0 || area.position.y < 0 || area.size.x <= 0 || area.size.y <= 0 ||
        area.position.x > static_cast<int>(image.size.x) - area.size.x ||
        area.position.y > static_cast<int>(image.size.y) - area.size.y ||
        (image.format != DXGI_FORMAT_R8G8B8A8_UNORM && image.format != DXGI_FORMAT_R8G8B8A8_UNORM_SRGB))
        return {};
    const auto guard = device->lock();
    auto       value = std::make_shared<DirectXReadback>();
    value->m_device  = device;
    value->m_size    = Vector2u(area.size);
    D3D11_TEXTURE2D_DESC desc{};
    image.texture->GetDesc(&desc);
    desc.Width          = value->m_size.x;
    desc.Height         = value->m_size.y;
    desc.MipLevels      = 1;
    desc.BindFlags      = 0;
    desc.MiscFlags      = 0;
    desc.Usage          = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    const D3D11_QUERY_DESC query{D3D11_QUERY_EVENT, 0};
    if (!device->check(device->device()->CreateTexture2D(&desc, nullptr, value->m_staging.put()),
                       "create asynchronous texture readback") ||
        !device->check(device->device()->CreateQuery(&query, value->m_fence.put()), "create readback fence"))
        return {};
    const D3D11_BOX box{static_cast<UINT>(area.position.x),
                        static_cast<UINT>(area.position.y),
                        0,
                        static_cast<UINT>(area.position.x + area.size.x),
                        static_cast<UINT>(area.position.y + area.size.y),
                        1};
    device->unbindTextures();
    device->context()->CopySubresourceRegion(value->m_staging.get(), 0, 0, 0, 0, image.texture.get(), 0, &box);
    device->context()->End(value->m_fence.get());
    device->context()->Flush();
    return value;
}
rhi::ReadbackStatus DirectXReadback::poll()
{
    if (m_status != rhi::ReadbackStatus::Pending)
        return m_status;
    const auto    guard = m_device->lock();
    const HRESULT ready = m_device->context()->GetData(m_fence.get(), nullptr, 0, D3D11_ASYNC_GETDATA_DONOTFLUSH);
    if (ready == S_FALSE)
        return m_status;
    if (!m_device->check(ready, "poll texture readback"))
        return m_status = rhi::ReadbackStatus::Failed;
    D3D11_MAPPED_SUBRESOURCE mapped{};
    const auto result = m_device->context()->Map(m_staging.get(), 0, D3D11_MAP_READ, D3D11_MAP_FLAG_DO_NOT_WAIT, &mapped);
    if (result == DXGI_ERROR_WAS_STILL_DRAWING)
        return m_status;
    if (!m_device->check(result, "map texture readback"))
        return m_status = rhi::ReadbackStatus::Failed;
    const std::size_t stride = static_cast<std::size_t>(m_size.x) * 4;
    m_data.resize(stride * m_size.y);
    for (unsigned int row = 0; row < m_size.y; ++row)
        std::memcpy(m_data.data() + row * stride,
                    static_cast<const std::uint8_t*>(mapped.pData) + row * mapped.RowPitch,
                    stride);
    m_device->context()->Unmap(m_staging.get(), 0);
    return m_status = rhi::ReadbackStatus::Ready;
}
const std::vector<std::uint8_t>& DirectXReadback::data() const
{
    return m_data;
}
} // namespace sf::priv
