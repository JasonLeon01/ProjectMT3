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

#include <SFML/Window/Emscripten/CursorImpl.hpp>
#include <SFML/Window/Emscripten/Input.hpp>
#include <SFML/Window/Emscripten/WebContext.hpp>
#include <SFML/Window/Emscripten/WindowImplWeb.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/String.hpp>

#include <algorithm>
#include <emscripten.h>
#include <ostream>
#include <string_view>

#include <cmath>


// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(void, sfmlSetVisibility, (const char* selector, int visible), {
    document.querySelector(UTF8ToString(selector)).style.visibility = visible ? 'visible' : 'hidden';
});
// clang-format on
// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(void, sfmlSetCursor, (const char* selector, const char* cursor), {
    document.querySelector(UTF8ToString(selector)).style.cursor = UTF8ToString(cursor);
});
// clang-format on
// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(void, sfmlFocusCanvas, (const char* selector), {
    var canvas      = document.querySelector(UTF8ToString(selector));
    canvas.tabIndex = 0;
    canvas.focus();
});
// clang-format on
// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(int, sfmlHasFocus, (const char* selector), {
    return document.hasFocus() && !document.hidden &&
        document.activeElement === document.querySelector(UTF8ToString(selector));
});
// clang-format on
// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(double, sfmlCanvasLeft, (const char* selector), {
    return document.querySelector(UTF8ToString(selector)).getBoundingClientRect().left;
});
// clang-format on
// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(double, sfmlCanvasTop, (const char* selector), {
    return document.querySelector(UTF8ToString(selector)).getBoundingClientRect().top;
});
// clang-format on
// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(void, sfmlInstallTouchInput, (const char* selector, bool enabled), {
    var canvas = document.querySelector(UTF8ToString(selector));
    if (enabled) {
        var state = {
            touchAction: canvas.style.touchAction,
            suppressMouse: (event) => {
                if (event.sourceCapabilities?.firesTouchEvents) {
                    event.preventDefault();
                    event.stopImmediatePropagation();
                }
            }
        };
        canvas.sfmlTouchInput = state;
        canvas.style.touchAction = 'none';
        for (var name of ['mousedown', 'mouseup', 'mousemove', 'click'])
            canvas.addEventListener(name, state.suppressMouse, true);
    } else if (canvas.sfmlTouchInput) {
        var state = canvas.sfmlTouchInput;
        canvas.style.touchAction = state.touchAction;
        for (var name of ['mousedown', 'mouseup', 'mousemove', 'click'])
            canvas.removeEventListener(name, state.suppressMouse, true);
        delete canvas.sfmlTouchInput;
    }
});
// clang-format on

namespace sf::priv
{
namespace
{
namespace WindowImplWebImpl
{
WindowImplWeb* window{};
}
} // namespace
bool WindowImplWeb::canCreate(WindowHandle handle)
{
    if (WindowImplWebImpl::window || !*WebContext::canvasSelector() ||
        (handle && std::string_view(handle) != WebContext::canvasSelector()))
    {
        err() << "Web supports one window on Module.canvas" << std::endl;
        return false;
    }
    return true;
}
WindowImplWeb::WindowImplWeb(VideoMode mode, const String& title, std::uint32_t, State state, const ContextSettings&)
{
    WindowImplWebImpl::window = this;
    setSize(mode.size);
    setTitle(title);
    setVisible(true);
    installCallbacks(true);
    requestFocus();
    if (state == State::Fullscreen)
        emscripten_request_fullscreen(getNativeHandle(), true);
}
WindowImplWeb::WindowImplWeb(WindowHandle)
{
    WindowImplWebImpl::window = this;
    int width                 = 0;
    int height                = 0;
    emscripten_get_canvas_element_size(getNativeHandle(), &width, &height);
    m_size = {static_cast<unsigned int>(std::max(width, 1)), static_cast<unsigned int>(std::max(height, 1))};
    setVisible(true);
    installCallbacks(true);
}
WindowImplWeb::~WindowImplWeb()
{
    cancelTouches();
    installCallbacks(false);
    setVisible(false);
    emscripten_exit_pointerlock();
    emscripten_exit_fullscreen();
    WebInput::keys.fill(false);
    WebInput::scans.fill(false);
    WebInput::buttons.fill(false);
    WindowImplWebImpl::window = nullptr;
}
WindowHandle WindowImplWeb::getNativeHandle() const
{
    return WebContext::canvasSelector();
}
Vector2i WindowImplWeb::getPosition() const
{
    return {};
}
void WindowImplWeb::setPosition(Vector2i)
{
}
Vector2u WindowImplWeb::getSize() const
{
    return m_size;
}
void WindowImplWeb::setSize(Vector2u size)
{
    m_size = {std::max(size.x, 1u), std::max(size.y, 1u)};
    emscripten_set_canvas_element_size(getNativeHandle(), static_cast<int>(m_size.x), static_cast<int>(m_size.y));
    const double scale = emscripten_get_device_pixel_ratio();
    emscripten_set_element_css_size(getNativeHandle(), m_size.x / scale, m_size.y / scale);
}
void WindowImplWeb::setTitle(const String& title)
{
    const auto utf8 = title.toUtf8();
    emscripten_set_window_title(reinterpret_cast<const char*>(utf8.c_str()));
}
void WindowImplWeb::setIcon(Vector2u, const std::uint8_t*)
{
}
void WindowImplWeb::setVisible(bool visible)
{
    if (!visible)
        cancelTouches();
    sfmlSetVisibility(getNativeHandle(), visible);
}
void WindowImplWeb::setMouseCursorVisible(bool visible)
{
    m_cursorVisible = visible;
    sfmlSetCursor(getNativeHandle(), visible ? m_cursor.c_str() : "none");
}
void WindowImplWeb::setMouseCursorGrabbed(bool grabbed)
{
    if (grabbed)
        emscripten_request_pointerlock(getNativeHandle(), true);
    else
        emscripten_exit_pointerlock();
}
void WindowImplWeb::setMouseCursor(const CursorImpl& cursor)
{
    m_cursor = cursor.css;
    setMouseCursorVisible(m_cursorVisible);
}
void WindowImplWeb::setKeyRepeatEnabled(bool enabled)
{
    m_keyRepeat = enabled;
}
void WindowImplWeb::requestFocus()
{
    sfmlFocusCanvas(getNativeHandle());
}
bool WindowImplWeb::hasFocus() const
{
    return sfmlHasFocus(getNativeHandle()) != 0;
}
void WindowImplWeb::installCallbacks(bool enabled)
{
    const auto* canvas = getNativeHandle();
    sfmlInstallTouchInput(canvas, enabled);
    emscripten_set_keydown_callback(canvas, this, false, enabled ? keyboardCallback : nullptr);
    emscripten_set_keyup_callback(canvas, this, false, enabled ? keyboardCallback : nullptr);
    emscripten_set_mousedown_callback(canvas, this, false, enabled ? mouseCallback : nullptr);
    emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, enabled ? mouseCallback : nullptr);
    emscripten_set_mousemove_callback(canvas, this, false, enabled ? mouseCallback : nullptr);
    emscripten_set_mouseenter_callback(canvas, this, false, enabled ? mouseCallback : nullptr);
    emscripten_set_mouseleave_callback(canvas, this, false, enabled ? mouseCallback : nullptr);
    emscripten_set_wheel_callback(canvas, this, false, enabled ? wheelCallback : nullptr);
    emscripten_set_focus_callback(canvas, this, false, enabled ? focusCallback : nullptr);
    emscripten_set_blur_callback(canvas, this, false, enabled ? focusCallback : nullptr);
    emscripten_set_focus_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, enabled ? focusCallback : nullptr);
    emscripten_set_blur_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, enabled ? focusCallback : nullptr);
    emscripten_set_touchstart_callback(canvas, this, false, enabled ? touchCallback : nullptr);
    emscripten_set_touchmove_callback(canvas, this, false, enabled ? touchCallback : nullptr);
    emscripten_set_touchend_callback(canvas, this, false, enabled ? touchCallback : nullptr);
    emscripten_set_touchcancel_callback(canvas, this, false, enabled ? touchCallback : nullptr);
    emscripten_set_visibilitychange_callback(this, false, enabled ? visibilityCallback : nullptr);
    if (enabled)
        updateFocus();
}
bool WindowImplWeb::keyboardCallback(int type, const EmscriptenKeyboardEvent* event, void* user)
{
    auto&                  self    = *static_cast<WindowImplWeb*>(user);
    const auto*            mapping = WebInput::findCode(event->code);
    auto                   key     = mapping ? mapping->key : Keyboard::Key::Unknown;
    const auto             scan    = mapping ? mapping->scan : Keyboard::Scancode::Unknown;
    const std::string_view value(event->key);
    if (value.size() == 1 && ((value[0] >= 'a' && value[0] <= 'z') || (value[0] >= 'A' && value[0] <= 'Z')))
        key = static_cast<Keyboard::Key>((value[0] | 32) - 'a');
    const bool down = type == EMSCRIPTEN_EVENT_KEYDOWN;
    if (key != Keyboard::Key::Unknown)
        WebInput::keys[static_cast<std::size_t>(key)] = down;
    if (scan != Keyboard::Scancode::Unknown)
        WebInput::scans[static_cast<std::size_t>(scan)] = down;
    if (!event->repeat || self.m_keyRepeat)
    {
        if (down)
            self.pushEvent(Event::KeyPressed{key, scan, event->altKey, event->ctrlKey, event->shiftKey, event->metaKey});
        else
            self.pushEvent(Event::KeyReleased{key, scan, event->altKey, event->ctrlKey, event->shiftKey, event->metaKey});
        const auto text = String::fromUtf8(value.begin(), value.end());
        if (down && !event->ctrlKey && !event->metaKey && text.getSize() == 1)
            self.pushEvent(Event::TextEntered{text[0]});
    }
    return true;
}
Vector2i WindowImplWeb::mousePosition(const EmscriptenMouseEvent& event) const
{
    return canvasPosition(event.clientX, event.clientY);
}
Vector2i WindowImplWeb::canvasPosition(int clientX, int clientY) const
{
    double width  = 1;
    double height = 1;
    emscripten_get_element_css_size(getNativeHandle(), &width, &height);
    const double left = sfmlCanvasLeft(getNativeHandle());
    const double top  = sfmlCanvasTop(getNativeHandle());
    return {static_cast<int>((clientX - left) * m_size.x / std::max(width, 1.0)),
            static_cast<int>((clientY - top) * m_size.y / std::max(height, 1.0))};
}
bool WindowImplWeb::mouseCallback(int type, const EmscriptenMouseEvent* event, void* user)
{
    auto&      self     = *static_cast<WindowImplWeb*>(user);
    const auto position = self.mousePosition(*event);
    WebInput::mouse     = position;
    if (type == EMSCRIPTEN_EVENT_MOUSEMOVE)
        self.pushEvent(Event::MouseMoved{position});
    else if (type == EMSCRIPTEN_EVENT_MOUSEENTER)
        self.pushEvent(Event::MouseEntered{});
    else if (type == EMSCRIPTEN_EVENT_MOUSELEAVE)
        self.pushEvent(Event::MouseLeft{});
    else if (event->button >= 0 && event->button < 5)
    {
        constexpr std::array buttons{Mouse::Button::Left,
                                     Mouse::Button::Middle,
                                     Mouse::Button::Right,
                                     Mouse::Button::Extra1,
                                     Mouse::Button::Extra2};
        const auto           button                         = buttons[static_cast<std::size_t>(event->button)];
        const bool           down                           = type == EMSCRIPTEN_EVENT_MOUSEDOWN;
        WebInput::buttons[static_cast<std::size_t>(button)] = down;
        if (down)
            self.pushEvent(Event::MouseButtonPressed{button, position});
        else
            self.pushEvent(Event::MouseButtonReleased{button, position});
    }
    return true;
}
bool WindowImplWeb::wheelCallback(int, const EmscriptenWheelEvent* event, void* user)
{
    auto&        self     = *static_cast<WindowImplWeb*>(user);
    const auto   position = self.mousePosition(event->mouse);
    const double divisor  = event->deltaMode == 0 ? 100.0 : 1.0;
    if (event->deltaY != 0)
        self.pushEvent(
            Event::MouseWheelScrolled{Mouse::Wheel::Vertical, static_cast<float>(-event->deltaY / divisor), position});
    if (event->deltaX != 0)
        self.pushEvent(
            Event::MouseWheelScrolled{Mouse::Wheel::Horizontal, static_cast<float>(-event->deltaX / divisor), position});
    return true;
}
bool WindowImplWeb::focusCallback(int, const EmscriptenFocusEvent*, void* user)
{
    static_cast<WindowImplWeb*>(user)->updateFocus();
    return false;
}
void WindowImplWeb::updateFocus()
{
    const bool focused = hasFocus();
    if (focused == m_focused)
        return;
    m_focused = focused;
    if (focused)
        pushEvent(Event::FocusGained{});
    else
    {
        WebInput::keys.fill(false);
        WebInput::scans.fill(false);
        WebInput::buttons.fill(false);
        cancelTouches();
        pushEvent(Event::FocusLost{});
    }
}
bool WindowImplWeb::touchCallback(int type, const EmscriptenTouchEvent* event, void* user)
{
    auto& self = *static_cast<WindowImplWeb*>(user);
    if (type == EMSCRIPTEN_EVENT_TOUCHSTART)
        self.requestFocus();
    for (int index = 0; index < event->numTouches; ++index)
    {
        const auto& touch = event->touches[index];
        if (!touch.isChanged)
            continue;
        auto slot = std::find_if(WebInput::touches.begin(),
                                 WebInput::touches.end(),
                                 [&touch](const auto& point) { return point.identifier == touch.identifier; });
        if (type == EMSCRIPTEN_EVENT_TOUCHSTART)
        {
            if (slot != WebInput::touches.end())
                continue;
            slot = std::find_if(WebInput::touches.begin(),
                                WebInput::touches.end(),
                                [](const auto& point) { return !point.identifier; });
            if (slot == WebInput::touches.end())
                continue;
            slot->identifier = touch.identifier;
        }
        else if (slot == WebInput::touches.end())
            continue;
        slot->position    = self.canvasPosition(touch.clientX, touch.clientY);
        const auto finger = static_cast<unsigned int>(slot - WebInput::touches.begin());
        if (type == EMSCRIPTEN_EVENT_TOUCHSTART)
            self.pushEvent(Event::TouchBegan{finger, slot->position});
        else if (type == EMSCRIPTEN_EVENT_TOUCHMOVE)
            self.pushEvent(Event::TouchMoved{finger, slot->position});
        else
        {
            slot->identifier.reset();
            self.pushEvent(Event::TouchEnded{finger, slot->position});
        }
    }
    return true;
}
void WindowImplWeb::cancelTouches()
{
    for (unsigned int finger = 0; finger < WebInput::touches.size(); ++finger)
    {
        auto& touch = WebInput::touches[finger];
        if (touch.identifier)
        {
            touch.identifier.reset();
            pushEvent(Event::TouchEnded{finger, touch.position});
        }
    }
}
bool WindowImplWeb::visibilityCallback(int, const EmscriptenVisibilityChangeEvent*, void* user)
{
    static_cast<WindowImplWeb*>(user)->updateFocus();
    return false;
}
void WindowImplWeb::processEvents()
{
    updateFocus();
    if (!m_lost && WebContext::isLost())
    {
        m_lost = true;
        cancelTouches();
        err() << "WebGL context lost; reload the page to restore graphics" << std::endl;
        pushEvent(Event::Closed{});
    }
    double width  = 0;
    double height = 0;
    emscripten_get_element_css_size(getNativeHandle(), &width, &height);
    const auto     ratio = emscripten_get_device_pixel_ratio();
    const Vector2u size{static_cast<unsigned int>(std::max(1.0, std::round(width * ratio))),
                        static_cast<unsigned int>(std::max(1.0, std::round(height * ratio)))};
    if (size != m_size)
    {
        m_size = size;
        emscripten_set_canvas_element_size(getNativeHandle(), static_cast<int>(size.x), static_cast<int>(size.y));
        pushEvent(Event::Resized{size});
    }
}
} // namespace sf::priv
