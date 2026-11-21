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

#include <SFML/Window/Harmony/NativeAppImpl.hpp>
#include <SFML/Window/Harmony/WindowImplHarmony.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/Exception.hpp>
#include <SFML/System/String.hpp>

#include <limits>
#include <ostream>
#include <utility>
#include <window_manager/oh_window.h>


namespace
{
std::string toUtf8(const sf::String& value)
{
    const auto utf8 = value.toUtf8();
    return {reinterpret_cast<const char*>(utf8.data()), utf8.size()};
}
} // namespace


namespace sf::priv
{
WindowImplHarmony::WindowImplHarmony(WindowHandle handle)
{
    initialize(handle);
}


WindowImplHarmony::WindowImplHarmony(VideoMode mode, const String& title, std::uint32_t style, State state, const ContextSettings&)
{
    initialize(nullptr);

    const bool                       fullscreen = state == State::Fullscreen;
    Harmony::HostState&              host       = Harmony::getHostState();
    sf::priv::Harmony::HostCallbacks callbacks;
    {
        const std::lock_guard lock(host.mutex);
        host.windowConfigurationSet = true;
        host.fullscreen             = fullscreen;
        host.keepScreenOn           = true;
        callbacks                   = host.callbacks;
    }

    if (Harmony::usesDesktopWindow())
    {
        Harmony::WindowCommand command;
        command.type         = Harmony::WindowCommandType::Configure;
        command.size         = mode.size;
        command.title        = toUtf8(title);
        command.style        = style;
        command.fullscreen   = fullscreen;
        command.visible      = true;
        command.keepScreenOn = true;
        if (!Harmony::requestWindowCommand(std::move(command)))
        {
            {
                const std::lock_guard lock(host.mutex);
                host.windowConfigurationSet = false;
                host.fullscreen             = false;
                host.keepScreenOn           = false;
            }
            Harmony::releaseWindow();
            m_claimed = false;
            throw Exception("The Harmony 2-in-1 host failed to configure the Stage window");
        }
    }
    else
    {
        // Android's native entry point keeps the display awake for the lifetime
        // of the mobile window and applies immersive mode for fullscreen windows.
        // Ask the Stage host to provide the same behavior without blocking this
        // application thread on ArkUI work.
        if (callbacks.configureWindow)
            callbacks.configureWindow(fullscreen, true, callbacks.userData);
    }
}


WindowImplHarmony::~WindowImplHarmony()
{
    if (!m_claimed)
        return;

    if (Harmony::usesDesktopWindow())
    {
        if (m_cursorGrabDesired || m_cursorGrabActual)
            setMouseCursorGrabbed(false);
    }

    sf::priv::Harmony::HostCallbacks callbacks;
    {
        auto&                 host = Harmony::getHostState();
        const std::lock_guard lock(host.mutex);
        if (!host.destroyed && host.windowConfigurationSet)
        {
            host.windowConfigurationSet = false;
            host.fullscreen             = false;
            host.keepScreenOn           = false;
            callbacks                   = host.callbacks;
        }
    }

    if (Harmony::usesDesktopWindow())
    {
        if (callbacks.executeWindowCommand)
        {
            Harmony::WindowCommand command;
            command.type         = Harmony::WindowCommandType::Restore;
            command.fullscreen   = false;
            command.keepScreenOn = false;
            (void)Harmony::requestWindowCommand(std::move(command));
        }
    }
    else
    {
        if (callbacks.configureWindow)
            callbacks.configureWindow(false, false, callbacks.userData);
    }
    Harmony::releaseWindow();
}


void WindowImplHarmony::initialize(WindowHandle requestedHandle)
{
    if (!Harmony::claimWindow())
        throw Exception("Only one owning SFML window is supported by the Harmony host");

    m_claimed          = true;
    const auto surface = Harmony::getSurfaceSnapshot();
    if (!surface.window)
    {
        Harmony::releaseWindow();
        m_claimed = false;
        throw Exception("The Harmony XComponent surface is not ready");
    }
    if (requestedHandle && requestedHandle != surface.window)
    {
        Harmony::releaseWindow();
        m_claimed = false;
        throw Exception("A Harmony WindowHandle must refer to the registered XComponent surface");
    }

    m_size                     = surface.size;
    auto&                 host = Harmony::getHostState();
    const std::lock_guard lock(host.mutex);
    m_hasFocus = host.focused;
}


WindowHandle WindowImplHarmony::getNativeHandle() const
{
    return Harmony::getSurfaceSnapshot().window;
}


Vector2i WindowImplHarmony::getPosition() const
{
    if (Harmony::usesDesktopWindow())
        return Harmony::getWindowState().position;
    return {};
}


void WindowImplHarmony::setPosition(Vector2i position)
{
    if (Harmony::usesDesktopWindow())
    {
        Harmony::WindowCommand command;
        command.type     = Harmony::WindowCommandType::SetPosition;
        command.position = position;
        (void)Harmony::requestWindowCommand(std::move(command));
    }
}


Vector2u WindowImplHarmony::getSize() const
{
    if (Harmony::usesDesktopWindow())
        return Harmony::getWindowState().clientSize;
    return Harmony::getSurfaceSnapshot().size;
}


void WindowImplHarmony::setSize(Vector2u size)
{
    if (Harmony::usesDesktopWindow())
    {
        Harmony::WindowCommand command;
        command.type = Harmony::WindowCommandType::SetSize;
        command.size = size;
        (void)Harmony::requestWindowCommand(std::move(command));
    }
}


void WindowImplHarmony::setMinimumSize(const std::optional<Vector2u>& minimumSize)
{
    if (Harmony::usesDesktopWindow())
    {
        Harmony::WindowCommand command;
        command.type      = Harmony::WindowCommandType::SetMinimumSize;
        command.sizeLimit = minimumSize;
        if (Harmony::requestWindowCommand(std::move(command)))
            WindowImpl::setMinimumSize(minimumSize);
    }
}


void WindowImplHarmony::setMaximumSize(const std::optional<Vector2u>& maximumSize)
{
    if (Harmony::usesDesktopWindow())
    {
        Harmony::WindowCommand command;
        command.type      = Harmony::WindowCommandType::SetMaximumSize;
        command.sizeLimit = maximumSize;
        if (Harmony::requestWindowCommand(std::move(command)))
            WindowImpl::setMaximumSize(maximumSize);
    }
}


void WindowImplHarmony::setTitle(const String& title)
{
    if (Harmony::usesDesktopWindow())
    {
        Harmony::WindowCommand command;
        command.type  = Harmony::WindowCommandType::SetTitle;
        command.title = toUtf8(title);
        (void)Harmony::requestWindowCommand(std::move(command));
    }
}


void WindowImplHarmony::setIcon(Vector2u size, const std::uint8_t* pixels)
{
    if (Harmony::usesDesktopWindow())
    {
        const auto pixelCount = static_cast<std::uint64_t>(size.x) * size.y * 4u;
        if (!pixels || !size.x || !size.y || pixelCount > std::numeric_limits<std::size_t>::max())
            return;

        Harmony::WindowCommand command;
        command.type     = Harmony::WindowCommandType::SetIcon;
        command.iconSize = size;
        command.iconPixels.assign(pixels, pixels + static_cast<std::size_t>(pixelCount));
        (void)Harmony::requestWindowCommand(std::move(command));
    }
}


void WindowImplHarmony::setVisible(bool visible)
{
    if (Harmony::usesDesktopWindow())
    {
        Harmony::WindowCommand command;
        command.type    = Harmony::WindowCommandType::SetVisible;
        command.visible = visible;
        (void)Harmony::requestWindowCommand(std::move(command));
    }
}


void WindowImplHarmony::setMouseCursorVisible(bool visible)
{
    Harmony::HostState&              host = Harmony::getHostState();
    sf::priv::Harmony::HostCallbacks callbacks;
    {
        const std::lock_guard lock(host.mutex);
        callbacks = host.callbacks;
    }
    if (callbacks.setPointerVisible)
        callbacks.setPointerVisible(visible, callbacks.userData);
}


void WindowImplHarmony::setMouseCursorGrabbed(bool grabbed)
{
    if (Harmony::usesDesktopWindow())
    {
        m_cursorGrabDesired = grabbed;
        if ((grabbed && (!m_hasFocus || m_cursorGrabActual)) || (!grabbed && !m_cursorGrabActual))
            return;

        auto&        host = Harmony::getHostState();
        std::int32_t windowId{};
        {
            const std::lock_guard lock(host.mutex);
            windowId = host.windowId;
        }
        if (windowId <= 0)
            return;

        const auto result = grabbed ? OH_WindowManager_LockCursor(windowId, true) : OH_WindowManager_UnlockCursor(windowId);
        if (result == OK)
            m_cursorGrabActual = grabbed;
        else
            err() << "Failed to " << (grabbed ? "lock" : "unlock") << " the Harmony cursor (error " << result << ')'
                  << std::endl;
    }
    else
    {
        // Pointer confinement is unavailable to ordinary mobile applications.
    }
}


void WindowImplHarmony::setMouseCursor(const CursorImpl& cursor)
{
    Harmony::HostState&              host = Harmony::getHostState();
    sf::priv::Harmony::HostCallbacks callbacks;
    {
        const std::lock_guard lock(host.mutex);
        callbacks = host.callbacks;
    }

    if (const auto type = cursor.getSystemType())
    {
        if (callbacks.setSystemPointer)
            callbacks.setSystemPointer(static_cast<unsigned int>(*type), callbacks.userData);
    }
    else if (callbacks.setCustomPointer && !cursor.getPixels().empty())
    {
        callbacks.setCustomPointer(cursor.getPixels().data(), cursor.getSize(), cursor.getHotspot(), callbacks.userData);
    }
}


void WindowImplHarmony::setKeyRepeatEnabled(bool enabled)
{
    Harmony::HostState&   host = Harmony::getHostState();
    const std::lock_guard lock(host.mutex);
    host.keyRepeatEnabled = enabled;
}


void WindowImplHarmony::requestFocus()
{
    if (Harmony::usesDesktopWindow())
    {
        Harmony::WindowCommand command;
        command.type = Harmony::WindowCommandType::RequestFocus;
        (void)Harmony::requestWindowCommand(std::move(command));
    }
    else
    {
        // ArkUI owns focus traversal. XComponent focus must be requested by the host.
    }
}


bool WindowImplHarmony::hasFocus() const
{
    auto&                 host = Harmony::getHostState();
    const std::lock_guard lock(host.mutex);
    return host.focused;
}


void WindowImplHarmony::processEvents()
{
    for (auto& event : Harmony::drainEvents())
    {
        if (const auto* resized = event.getIf<Event::Resized>())
            m_size = resized->size;
        else if (event.is<Event::FocusGained>())
        {
            m_hasFocus = true;
            if (m_cursorGrabDesired && !m_cursorGrabActual)
                setMouseCursorGrabbed(true);
        }
        else if (event.is<Event::FocusLost>())
        {
            m_hasFocus = false;
            if (Harmony::usesDesktopWindow())
            {
                // Harmony automatically releases confinement as focus leaves the
                // window. Preserve the desired state so it can be restored later.
                m_cursorGrabActual = false;
            }
        }

        pushEvent(event);
    }
}

} // namespace sf::priv
