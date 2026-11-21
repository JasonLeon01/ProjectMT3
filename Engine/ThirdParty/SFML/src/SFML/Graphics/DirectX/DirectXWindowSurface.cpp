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

#include <SFML/Graphics/DirectX/DirectXWindowSurface.hpp>

namespace sf::priv
{
std::unique_ptr<DirectXWindowSurface> DirectXWindowSurface::create(
    const std::shared_ptr<DirectXDevice>& device,
    WindowHandle                          handle,
    Vector2u                              size,
    const ContextSettings&                settings)
{
    const auto guard = device->lock();
    if (!handle || !size.x || !size.y || device->isLost())
        return {};
    auto surface      = std::unique_ptr<DirectXWindowSurface>(new DirectXWindowSurface);
    surface->m_device = device;
    surface->m_size   = size;
    surface->m_target = DirectXRenderTarget::create(device, device->makeTexture(size, settings.sRgbCapable), settings);
    if (!surface->m_target)
        return {};
    ComPtr<IDXGIDevice>  dxgiDevice;
    ComPtr<IDXGIAdapter> adapter;
    ComPtr<IDXGIFactory> factory;
    if (!device->check(device->device()->QueryInterface(IID_IDXGIDevice, reinterpret_cast<void**>(dxgiDevice.put())),
                       "query DXGI device") ||
        !device->check(dxgiDevice->GetAdapter(adapter.put()), "query DXGI adapter") ||
        !device->check(adapter->GetParent(IID_IDXGIFactory, reinterpret_cast<void**>(factory.put())),
                       "query DXGI factory"))
        return {};
    DXGI_SWAP_CHAIN_DESC description{};
    description.BufferDesc.Width  = size.x;
    description.BufferDesc.Height = size.y;
    description.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    description.SampleDesc.Count  = 1;
    description.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    description.BufferCount       = 1;
    description.OutputWindow      = handle;
    description.Windowed          = TRUE;
    description.SwapEffect        = DXGI_SWAP_EFFECT_DISCARD;
    if (!device->check(factory->CreateSwapChain(device->device(), &description, surface->m_swapchain.put()),
                       "create swapchain") ||
        !device->check(factory->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER), "configure window association"))
        return {};
    return surface;
}

DirectXWindowSurface::~DirectXWindowSurface()
{
    const auto guard = m_device->lock();
    m_device->unbindTextures();
    m_swapchain.reset();
}
const ContextSettings& DirectXWindowSurface::getSettings() const
{
    return m_target->settings();
}
bool DirectXWindowSurface::setActive(bool active)
{
    return m_target->setActive(active);
}
void DirectXWindowSurface::setVerticalSyncEnabled(bool enabled)
{
    m_vsync = enabled;
}
const std::shared_ptr<DirectXRenderTarget>& DirectXWindowSurface::renderTarget() const
{
    return m_target;
}

void DirectXWindowSurface::resize(Vector2u size)
{
    const auto guard = m_device->lock();
    if (size == m_size)
        return;
    m_size  = size;
    m_dirty = true;
    if (size.x && size.y)
        (void)m_target->resize(size);
}

void DirectXWindowSurface::display()
{
    const auto guard = m_device->lock();
    if (m_device->isLost() || !m_size.x || !m_size.y)
        return;
    if (m_dirty)
    {
        m_device->unbindTextures();
        if (m_target->image()->size != m_size && !m_target->resize(m_size))
            return;
        if (!m_device->check(m_swapchain->ResizeBuffers(0, m_size.x, m_size.y, DXGI_FORMAT_UNKNOWN, 0),
                             "resize swapchain"))
            return;
        m_dirty = false;
    }
    m_target->finish();
    m_device->unbindTextures();
    ComPtr<ID3D11Texture2D> backbuffer;
    if (!m_device->check(m_swapchain->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(backbuffer.put())),
                         "get backbuffer"))
        return;
    const D3D11_BOX area{0, 0, 0, m_size.x, m_size.y, 1};
    m_device->context()->CopySubresourceRegion(backbuffer.get(), 0, 0, 0, 0, m_target->image()->texture.get(), 0, &area);
    (void)m_device->check(m_swapchain->Present(m_vsync ? 1 : 0, 0), "present window");
}

bool DirectXWindowSurface::copyTo(const std::shared_ptr<DirectXImage>& destination, Vector2u offset) const
{
    const auto guard = m_device->lock();
    m_target->finish();
    return m_device->copy(*m_target->image(), *destination, offset);
}
} // namespace sf::priv
