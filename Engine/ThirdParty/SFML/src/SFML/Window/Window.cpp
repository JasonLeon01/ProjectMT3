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
#include <SFML/Window/GlContext.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/WindowImpl.hpp>

#include <SFML/System/Err.hpp>
#ifdef SFML_SYSTEM_EMSCRIPTEN
#include <SFML/System/Emscripten/Browser.hpp>
#endif
#include <SFML/System/Sleep.hpp>

#include <ostream>
#include <utility>

#ifdef SFML_SYSTEM_ANDROID
#include <SFML/System/Android/Activity.hpp>
#endif


namespace sf
{
////////////////////////////////////////////////////////////
Window::Window() = default;


////////////////////////////////////////////////////////////
Window::Window(VideoMode mode, const String& title, std::uint32_t style, State state, const ContextSettings& settings)
{
    Window::create(mode, title, style, state, settings);
}


////////////////////////////////////////////////////////////
Window::Window(VideoMode mode, const String& title, State state, const ContextSettings& settings)
{
    Window::create(mode, title, sf::Style::Default, state, settings);
}


////////////////////////////////////////////////////////////
Window::Window(WindowHandle handle, const ContextSettings& settings)
{
    Window::create(handle, settings);
}


////////////////////////////////////////////////////////////
Window::~Window()
{
    Window::close();
}


////////////////////////////////////////////////////////////
Window::Window(Window&&) noexcept = default;


////////////////////////////////////////////////////////////
Window& Window::operator=(Window&& other) noexcept
{
    if (this == &other)
        return *this;

    close();
    m_surface        = std::move(other.m_surface);
    m_clock          = other.m_clock;
    m_frameTimeLimit = other.m_frameTimeLimit;
    WindowBase::operator=(std::move(other));
    return *this;
}


////////////////////////////////////////////////////////////
void Window::create(VideoMode mode, const String& title, std::uint32_t style, State state)
{
    create(mode, title, style, state, ContextSettings{});
}


////////////////////////////////////////////////////////////
void Window::create(VideoMode mode, const String& title, std::uint32_t style, State state, const ContextSettings& settings)
{
    // Ensure the open window is closed first
    close();

    ContextSettings       effective = settings;
    std::shared_ptr<void> sharedContext;
    if (priv::validGlSettings(settings))
    {
        sharedContext = priv::GlContext::prepareSharedContext(effective);
        if (!sharedContext)
            return;
    }

    m_impl = priv::WindowImpl::create(mode, title, style, state, effective);
    if (m_impl)
        m_surface = createSurface(effective, *m_impl, mode.bitsPerPixel);
    if (m_surface)
        initialize();
    else
        close();
}


////////////////////////////////////////////////////////////
void Window::create(VideoMode mode, const String& title, State state)
{
    create(mode, title, sf::Style::Default, state, ContextSettings{});
}


////////////////////////////////////////////////////////////
void Window::create(VideoMode mode, const String& title, State state, const ContextSettings& settings)
{
    create(mode, title, sf::Style::Default, state, settings);
}


////////////////////////////////////////////////////////////
void Window::create(WindowHandle handle)
{
    create(handle, ContextSettings{});
}


////////////////////////////////////////////////////////////
void Window::create(WindowHandle handle, const ContextSettings& settings)
{
    // Ensure the open window is closed first
    close();

    m_impl = priv::WindowImpl::create(handle);
    if (m_impl)
        m_surface = createSurface(settings, *m_impl, VideoMode::getDesktopMode().bitsPerPixel);
    if (m_surface)
        initialize();
    else
        close();
}


////////////////////////////////////////////////////////////
void Window::close()
{
#ifdef SFML_SYSTEM_ANDROID
    if (m_surface)
    {
        auto&                 activity = priv::getActivity();
        const std::lock_guard lock(activity.mutex);
        if (activity.surface == m_surface.get())
            activity.surface = nullptr;
    }
#endif

    // Release the rendering surface before destroying the native window.
    m_surface.reset();

    // Close the base window
    WindowBase::close();
}


////////////////////////////////////////////////////////////
const ContextSettings& Window::getSettings() const
{
    static constexpr ContextSettings empty{/* depthBits */ 0, /* stencilBits */ 0, /* antiAliasingLevel */ 0};

    return m_surface ? m_surface->getSettings() : empty;
}


////////////////////////////////////////////////////////////
void Window::setVerticalSyncEnabled(bool enabled)
{
    if (setActive())
        m_surface->setVerticalSyncEnabled(enabled);
}


////////////////////////////////////////////////////////////
void Window::setFramerateLimit(unsigned int limit)
{
    if (limit > 0)
        m_frameTimeLimit = seconds(1.f / static_cast<float>(limit));
    else
        m_frameTimeLimit = Time::Zero;
}


////////////////////////////////////////////////////////////
bool Window::setActive(bool active) const
{
    if (m_surface)
    {
        if (m_surface->setActive(active))
        {
            return true;
        }

        err() << "Failed to activate the window's context" << std::endl;
        return false;
    }

    return false;
}


////////////////////////////////////////////////////////////
void Window::display()
{
    // Display the backbuffer on screen
    if (setActive())
        m_surface->display();
#ifdef SFML_SYSTEM_EMSCRIPTEN
    else
        priv::sleepImpl(Time::Zero);
#endif

    // Limit the framerate if needed
    if (m_frameTimeLimit != Time::Zero)
    {
        sleep(m_frameTimeLimit - m_clock.getElapsedTime());
        m_clock.restart();
    }
}


////////////////////////////////////////////////////////////
std::unique_ptr<priv::WindowSurface> Window::createSurface(const ContextSettings&  settings,
                                                           const priv::WindowImpl& owner,
                                                           unsigned int            bitsPerPixel)
{
    return priv::GlContext::create(settings, owner, bitsPerPixel);
}


////////////////////////////////////////////////////////////
priv::WindowSurface* Window::getSurface() const
{
    return m_surface.get();
}


////////////////////////////////////////////////////////////
void Window::initialize()
{
#ifdef SFML_SYSTEM_ANDROID
    {
        auto&                 activity = priv::getActivity();
        const std::lock_guard lock(activity.mutex);
        activity.surface = m_surface.get();
    }
#endif

    // Setup default behaviors (to get a consistent behavior across different implementations)
    setVerticalSyncEnabled(false);
    setFramerateLimit(0);

    // Reset frame time
    m_clock.restart();

    // Activate the window
    if (!setActive())
    {
        err() << "Failed to set window as active during initialization" << std::endl;
    }

    WindowBase::initialize();
}

} // namespace sf
