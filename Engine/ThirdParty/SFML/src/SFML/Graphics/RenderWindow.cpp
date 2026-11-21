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
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RHI/Access.hpp>
#include <SFML/Graphics/RenderTargetBackend.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowImpl.hpp>
#include <SFML/Window/WindowSurface.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
RenderWindow::RenderWindow(VideoMode mode, const String& title, std::uint32_t style, State state, const ContextSettings& settings)
{
    // Don't call the base class constructor because it contains virtual function calls
    create(mode, title, style, state, settings);
}


////////////////////////////////////////////////////////////
RenderWindow::RenderWindow(VideoMode mode, const String& title, State state, const ContextSettings& settings)
{
    // Don't call the base class constructor because it contains virtual function calls
    create(mode, title, sf::Style::Default, state, settings);
}


////////////////////////////////////////////////////////////
RenderWindow::RenderWindow(WindowHandle handle, const ContextSettings& settings)
{
    // Don't call the base class constructor because it contains virtual function calls
    create(handle, settings);
}


////////////////////////////////////////////////////////////
void RenderWindow::create(VideoMode mode, const String& title, std::uint32_t style, State state, const ContextSettings& settings)
{
    (void)priv::withGraphicsDevice(settings,
                                   [&](const priv::GraphicsDevice&, const ContextSettings& effective)
                                   {
                                       Window::create(mode, title, style, state, effective);
                                       return isOpen();
                                   });
}

void RenderWindow::create(WindowHandle handle, const ContextSettings& settings)
{
    (void)priv::withGraphicsDevice(settings,
                                   [&](const priv::GraphicsDevice&, const ContextSettings& effective)
                                   {
                                       Window::create(handle, effective);
                                       return isOpen();
                                   });
}

void RenderWindow::close()
{
    m_backend.reset();
    Window::close();
}

std::unique_ptr<priv::WindowSurface> RenderWindow::createSurface(const ContextSettings&  settings,
                                                                 const priv::WindowImpl& owner,
                                                                 unsigned int            bitsPerPixel)
{
    const auto* device = priv::getGraphicsDevice();
    if (!device)
        return {};
    auto surface = static_cast<priv::DeviceResource&>(*device->device).createSurface(owner, bitsPerPixel, settings);
    return surface ? std::make_unique<priv::GraphicsWindowSurface>(std::move(surface)) : nullptr;
}


////////////////////////////////////////////////////////////
Vector2u RenderWindow::getSize() const
{
    return Window::getSize();
}


////////////////////////////////////////////////////////////
void RenderWindow::setIcon(const Image& icon)
{
    setIcon(icon.getSize(), icon.getPixelsPtr());
}


////////////////////////////////////////////////////////////
bool RenderWindow::isSrgb() const
{
    return getSettings().sRgbCapable;
}


////////////////////////////////////////////////////////////
bool RenderWindow::setActive(bool active)
{
    return Window::setActive(active) && RenderTarget::setActive(active);
}


////////////////////////////////////////////////////////////
void RenderWindow::onCreate()
{
    if (auto* surface = priv::resourceCast<priv::GraphicsWindowSurface>(getSurface()))
        m_backend = std::static_pointer_cast<priv::RenderTargetBackend>(surface->surface->getRenderTarget());
    RenderTarget::initialize();
}


////////////////////////////////////////////////////////////
void RenderWindow::onResize()
{
    if (auto* surface = priv::resourceCast<priv::GraphicsWindowSurface>(getSurface()))
        surface->surface->resize(getSize());
    setView(getView());
}

} // namespace sf
