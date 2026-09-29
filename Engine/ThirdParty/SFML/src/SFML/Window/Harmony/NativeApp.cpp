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
#include <SFML/Window/Harmony/NativeAppImpl.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <arkui/native_key_event.h>
#include <arkui/ui_input_event.h>
#include <chrono>
#include <deviceinfo.h>
#include <native_window/external_window.h>
#include <optional>
#include <ostream>
#include <string_view>
#include <system_error>
#include <thread>
#include <utility>

#include <cmath>
#include <cstring>

#ifdef SFML_HARMONY_2IN1
#include <window_manager/oh_display_manager.h>
#include <window_manager/oh_window.h>
#endif


namespace
{
sf::Vector2i toPosition(float x, float y)
{
    return {static_cast<int>(std::lround(x)), static_cast<int>(std::lround(y))};
}


bool isSupportedDevice()
{
    const char* deviceType = OH_GetDeviceType();
    if (!deviceType)
        return false;

    const std::string_view type(deviceType);
#ifdef SFML_HARMONY_2IN1
    return type == "2in1";
#else
    // API 21 deviceinfo documents "default" as the phone value used by some
    // products, in addition to the explicit "phone" spelling.
    return type == "default" || type == "phone" || type == "tablet";
#endif
}


void queueEvent(sf::priv::Harmony::HostState& state, sf::Event event)
{
    state.events.push(event);
}


void clearTouches(sf::priv::Harmony::HostState& state, bool emitTouchEnd)
{
    if (emitTouchEnd)
    {
        for (const auto& [finger, position] : state.touches)
            queueEvent(state, sf::Event::TouchEnded{finger, position});
    }

    state.touches.clear();
}


void clearInputState(sf::priv::Harmony::HostState& state, bool emitTouchEnd)
{
    clearTouches(state, emitTouchEnd);
    state.mouseButtons = {};
    state.keys.clear();
    state.scancodes.clear();
    state.keyCodes.clear();
}


void applyWindowState(sf::priv::Harmony::HostState& state, const sf::priv::Harmony::WindowState& next)
{
    const bool wasFocused = state.focused;

    state.windowState         = next;
    state.windowState.focused = next.focused && next.visible && state.foreground && !state.destroyed;
    state.focused             = state.windowState.focused;
    state.fullscreen          = next.fullscreen;

    if (wasFocused == state.focused)
        return;

    if (state.focused)
    {
        queueEvent(state, sf::Event::FocusGained{});
    }
    else
    {
        clearInputState(state, true);
        queueEvent(state, sf::Event::FocusLost{});
    }
}


void applySuccessfulWindowCommand(sf::priv::Harmony::HostState& state, const sf::priv::Harmony::WindowCommand& command)
{
    using sf::priv::Harmony::WindowCommandType;

    switch (command.type)
    {
        case WindowCommandType::Configure:
            state.title                  = command.title;
            state.style                  = command.style;
            state.fullscreen             = command.fullscreen;
            state.keepScreenOn           = command.keepScreenOn;
            state.windowConfigurationSet = true;
            break;
        case WindowCommandType::Restore:
            state.fullscreen             = false;
            state.keepScreenOn           = false;
            state.windowConfigurationSet = false;
            break;
        case WindowCommandType::SetMinimumSize:
            state.minimumSize = command.sizeLimit;
            break;
        case WindowCommandType::SetMaximumSize:
            state.maximumSize = command.sizeLimit;
            break;
        case WindowCommandType::SetTitle:
            state.title = command.title;
            break;
        case WindowCommandType::SetIcon:
            break;
        case WindowCommandType::SetPosition:
        case WindowCommandType::SetSize:
        case WindowCommandType::SetVisible:
        case WindowCommandType::RequestFocus:
            break;
    }
}


void cancelWindowCommands(sf::priv::Harmony::HostState& state)
{
    if (state.pendingWindowCommands.empty())
        return;

    state.pendingWindowCommands.clear();
    state.windowCommandCondition.notify_all();
}


void maybeStartMain(sf::priv::Harmony::HostState& state)
{
    if (!state.mainEntry || state.mainStarted || !state.hostInitialized || !state.window || !state.size.x ||
        !state.size.y || state.destroyed || state.mainStartFailed)
        return;

    try
    {
        state.mainThread  = std::thread(state.mainEntry);
        state.mainStarted = true;
    } catch (const std::system_error& exception)
    {
        state.mainStartFailed = true;
        sf::err() << "Failed to start the Harmony application thread: " << exception.what() << std::endl;
        queueEvent(state, sf::Event::Closed{});
    }
}


enum class SurfaceAction
{
    Created,
    Changed,
    Destroyed
};


void applySurface(sf::priv::Harmony::HostState& state,
                  NativeWindow*                 nativeWindow,
                  sf::Vector2u                  size,
                  bool                          forceSurfaceChange = false)
{
    const bool hadSurface     = state.window != nullptr;
    const bool sizeChanged    = state.size != size;
    const bool surfaceChanged = state.window != nativeWindow || (forceSurfaceChange && (state.window || nativeWindow));
    const bool wasRenderable  = state.window && state.size.x && state.size.y;
    const bool isRenderable   = nativeWindow && size.x && size.y;
    const bool renderabilityChanged        = wasRenderable != isRenderable;
    [[maybe_unused]] const bool wasFocused = state.focused;

    if (!surfaceChanged && !sizeChanged)
        return;

    state.window = nativeWindow;
    state.size   = size;
    if (nativeWindow && size.x && size.y)
        state.windowState.clientSize = size;
    if (surfaceChanged || renderabilityChanged)
        ++state.surfaceGeneration;

    if (!nativeWindow && hadSurface)
    {
#ifdef SFML_HARMONY_MOBILE
        state.focused             = false;
        state.windowState.focused = false;
#endif
        clearInputState(state, true);
#ifdef SFML_HARMONY_MOBILE
        if (wasFocused)
            queueEvent(state, sf::Event::FocusLost{});
#endif
    }
    else if (nativeWindow)
    {
        // A direct replacement is logically a surface loss followed by a
        // restore even when the old destroy callback is delayed.
        if (surfaceChanged && hadSurface)
        {
#ifdef SFML_HARMONY_MOBILE
            state.focused             = false;
            state.windowState.focused = false;
#endif
            clearInputState(state, true);
#ifdef SFML_HARMONY_MOBILE
            if (wasFocused)
                queueEvent(state, sf::Event::FocusLost{});
#endif
        }

#ifdef SFML_HARMONY_MOBILE
        if (!hadSurface || surfaceChanged)
        {
            state.focused             = state.foreground;
            state.windowState.focused = state.focused;
            if (state.focused)
                queueEvent(state, sf::Event::FocusGained{});
        }
#endif

        if (!hadSurface || surfaceChanged || sizeChanged)
            queueEvent(state, sf::Event::Resized{size});
    }

    state.surfaceCondition.notify_all();
    maybeStartMain(state);
}


void updateSurface(OH_NativeXComponent* component, void* window, SurfaceAction action)
{
    auto&                  state = sf::priv::Harmony::getHostState();
    const std::unique_lock lock(state.mutex);

    if (state.destroyed || (state.component != component && state.registeringComponent != component) || !window)
        return;

    auto* const   callbackWindow = static_cast<NativeWindow*>(window);
    NativeWindow* nativeWindow   = callbackWindow;

    // RegisterCallback can synchronously replay a surface. Keep that surface
    // separate until every callback is installed and the replacement
    // component is committed. The old component can therefore remain active
    // during registration, and a delayed old-node destroy cannot erase the
    // replacement surface.
    if (state.registeringComponent == component && state.component != component)
    {
        if (action == SurfaceAction::Destroyed)
        {
            if (state.registeringWindow == callbackWindow)
            {
                state.registeringWindow = nullptr;
                state.registeringSize   = {};
            }
            return;
        }

        sf::Vector2u  size;
        std::uint64_t width  = 0;
        std::uint64_t height = 0;
        if (OH_NativeXComponent_GetXComponentSize(component, window, &width, &height) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS)
        {
            size.x = static_cast<unsigned int>(std::min<std::uint64_t>(width, UINT32_MAX));
            size.y = static_cast<unsigned int>(std::min<std::uint64_t>(height, UINT32_MAX));
        }

        if (action == SurfaceAction::Created || !state.registeringWindow || state.registeringWindow == callbackWindow)
        {
            state.registeringWindow = callbackWindow;
            state.registeringSize   = size;
        }
        return;
    }

    if (action == SurfaceAction::Destroyed)
    {
        state.retiredWindows.erase(callbackWindow);
        state.destroyedWindows.insert(callbackWindow);

        // A delayed destroy belonging to a replaced surface must not clear
        // the newer surface that is already live.
        if (state.window != callbackWindow)
            return;

        nativeWindow = nullptr;
    }
    else if (action == SurfaceAction::Changed)
    {
        // Change callbacks only describe the current lifecycle. If no surface
        // was observed yet, a change may recover a create callback that was
        // delivered while native registration was still completing.
        if ((state.window && state.window != callbackWindow) ||
            (!state.window && (state.retiredWindows.count(callbackWindow) || state.destroyedWindows.count(callbackWindow))))
            return;
    }
    else
    {
        // A new create is authoritative, including when it arrives before the
        // old surface's destroy. A create for an un-destroyed retired pointer
        // is a delayed duplicate and remains ignored.
        if (state.retiredWindows.count(callbackWindow))
            return;

        state.destroyedWindows.erase(callbackWindow);
        if (state.window && state.window != callbackWindow)
            state.retiredWindows.insert(state.window);
    }

    sf::Vector2u size;
    if (nativeWindow)
    {
        std::uint64_t width  = 0;
        std::uint64_t height = 0;
        if (OH_NativeXComponent_GetXComponentSize(component, window, &width, &height) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS)
        {
            size.x = static_cast<unsigned int>(std::min<std::uint64_t>(width, UINT32_MAX));
            size.y = static_cast<unsigned int>(std::min<std::uint64_t>(height, UINT32_MAX));
        }
    }

    applySurface(state, nativeWindow, size);
}


void onSurfaceCreated(OH_NativeXComponent* component, void* window)
{
    updateSurface(component, window, SurfaceAction::Created);
}


void onSurfaceChanged(OH_NativeXComponent* component, void* window)
{
    updateSurface(component, window, SurfaceAction::Changed);
}


void onSurfaceDestroyed(OH_NativeXComponent* component, void* window)
{
    updateSurface(component, window, SurfaceAction::Destroyed);
}


void onTouch(OH_NativeXComponent* component, void* window)
{
    OH_NativeXComponent_TouchEvent input{};
    if (OH_NativeXComponent_GetTouchEvent(component, window, &input) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS)
        return;

    auto&                 state = sf::priv::Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.component != component)
        return;

    const auto finger   = static_cast<unsigned int>(std::max(input.id, 0));
    const auto position = toPosition(input.x, input.y);

    switch (input.type)
    {
        case OH_NATIVEXCOMPONENT_DOWN:
            state.touches[finger] = position;
            queueEvent(state, sf::Event::TouchBegan{finger, position});
            break;

        case OH_NATIVEXCOMPONENT_MOVE:
            for (std::uint32_t index = 0; index < input.numPoints; ++index)
            {
                const auto& point = input.touchPoints[index];
                const auto  id    = static_cast<unsigned int>(std::max(point.id, 0));
                const auto  next  = toPosition(point.x, point.y);
                const auto  found = state.touches.find(id);
                if (found == state.touches.end() || found->second != next)
                {
                    state.touches[id] = next;
                    queueEvent(state, sf::Event::TouchMoved{id, next});
                }
            }
            break;

        case OH_NATIVEXCOMPONENT_UP:
            state.touches.erase(finger);
            queueEvent(state, sf::Event::TouchEnded{finger, position});
            break;

        case OH_NATIVEXCOMPONENT_CANCEL:
            clearTouches(state, true);
            break;

        case OH_NATIVEXCOMPONENT_UNKNOWN:
            break;
    }
}


std::optional<sf::Mouse::Button> toMouseButton(OH_NativeXComponent_MouseEventButton button)
{
    switch (button)
    {
        case OH_NATIVEXCOMPONENT_LEFT_BUTTON:
            return sf::Mouse::Button::Left;
        case OH_NATIVEXCOMPONENT_RIGHT_BUTTON:
            return sf::Mouse::Button::Right;
        case OH_NATIVEXCOMPONENT_MIDDLE_BUTTON:
            return sf::Mouse::Button::Middle;
        case OH_NATIVEXCOMPONENT_BACK_BUTTON:
            return sf::Mouse::Button::Extra1;
        case OH_NATIVEXCOMPONENT_FORWARD_BUTTON:
            return sf::Mouse::Button::Extra2;
        default:
            return std::nullopt;
    }
}


void onMouse(OH_NativeXComponent* component, void* window)
{
    OH_NativeXComponent_MouseEvent input{};
    if (OH_NativeXComponent_GetMouseEvent(component, window, &input) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS)
        return;

    auto&                 state = sf::priv::Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.component != component)
        return;

    const auto position            = toPosition(input.x, input.y);
    state.mousePosition            = position;
    state.pointerLocationAvailable = true;

    const auto button = toMouseButton(input.button);
    switch (input.action)
    {
        case OH_NATIVEXCOMPONENT_MOUSE_PRESS:
            if (button)
            {
                state.mouseButtons[*button] = true;
                queueEvent(state, sf::Event::MouseButtonPressed{*button, position});
            }
            break;

        case OH_NATIVEXCOMPONENT_MOUSE_RELEASE:
            if (button)
            {
                state.mouseButtons[*button] = false;
                queueEvent(state, sf::Event::MouseButtonReleased{*button, position});
            }
            break;

        case OH_NATIVEXCOMPONENT_MOUSE_CANCEL:
            sf::priv::Harmony::releasePressedMouseButtons(state.mouseButtons, position, state.events);
            break;

        case OH_NATIVEXCOMPONENT_MOUSE_MOVE:
            queueEvent(state, sf::Event::MouseMoved{position});
            break;

        case OH_NATIVEXCOMPONENT_MOUSE_NONE:
            break;
    }
}


void onHover(OH_NativeXComponent* component, bool hover)
{
    auto&                 state = sf::priv::Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.component == component)
    {
        state.pointerLocationAvailable = true;
        queueEvent(state, hover ? sf::Event(sf::Event::MouseEntered{}) : sf::Event(sf::Event::MouseLeft{}));
    }
}


void onAxis(OH_NativeXComponent* component, ArkUI_UIInputEvent* input, ArkUI_UIInputEvent_Type type)
{
    if (!input || type != ARKUI_UIINPUTEVENT_TYPE_AXIS)
        return;

    const auto vertical   = static_cast<float>(OH_ArkUI_AxisEvent_GetVerticalAxisValue(input));
    const auto horizontal = static_cast<float>(OH_ArkUI_AxisEvent_GetHorizontalAxisValue(input));
    const auto position   = toPosition(OH_ArkUI_PointerEvent_GetX(input), OH_ArkUI_PointerEvent_GetY(input));

    auto&                 state = sf::priv::Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.component != component)
        return;

    state.mousePosition            = position;
    state.pointerLocationAvailable = true;
    if (vertical != 0.f)
        queueEvent(state, sf::Event::MouseWheelScrolled{sf::Mouse::Wheel::Vertical, -vertical, position});
    if (horizontal != 0.f)
        queueEvent(state, sf::Event::MouseWheelScrolled{sf::Mouse::Wheel::Horizontal, -horizontal, position});
}


void onFocus(OH_NativeXComponent* component, void*)
{
    auto&                 state = sf::priv::Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.component == component && state.foreground && !state.destroyed && !state.focused)
    {
        state.pointerLocationAvailable.reset();
        state.focused             = true;
        state.windowState.focused = true;
        queueEvent(state, sf::Event::FocusGained{});
    }
}


void onBlur(OH_NativeXComponent* component, void*)
{
    auto&                 state = sf::priv::Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.component == component)
    {
        clearInputState(state, true);
        if (state.focused)
        {
            state.focused             = false;
            state.windowState.focused = false;
            queueEvent(state, sf::Event::FocusLost{});
        }
    }
}


void processKey(OH_NativeXComponent* component, OH_NativeXComponent_KeyCode code, bool pressed, char32_t unicode)
{
    const auto key      = sf::priv::Harmony::keyCodeToKey(code);
    const auto scancode = sf::priv::Harmony::keyCodeToScancode(code);

    auto&                 state = sf::priv::Harmony::getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.component != component)
        return;

    const bool repeated = pressed && !state.keyCodes.insert(static_cast<std::int32_t>(code)).second;
    if (!pressed)
        state.keyCodes.erase(static_cast<std::int32_t>(code));

    if (key != sf::Keyboard::Key::Unknown)
    {
        if (pressed)
            state.keys.insert(key);
        else
            state.keys.erase(key);
    }
    if (scancode != sf::Keyboard::Scan::Unknown)
    {
        if (pressed)
            state.scancodes.insert(scancode);
        else
            state.scancodes.erase(scancode);
    }

    const bool alt     = state.keys.count(sf::Keyboard::Key::LAlt) || state.keys.count(sf::Keyboard::Key::RAlt);
    const bool control = state.keys.count(sf::Keyboard::Key::LControl) || state.keys.count(sf::Keyboard::Key::RControl);
    const bool shift   = state.keys.count(sf::Keyboard::Key::LShift) || state.keys.count(sf::Keyboard::Key::RShift);
    const bool system  = state.keys.count(sf::Keyboard::Key::LSystem) || state.keys.count(sf::Keyboard::Key::RSystem);

    const bool emitPressed = pressed && (!repeated || state.keyRepeatEnabled);
    if (emitPressed)
        queueEvent(state, sf::Event::KeyPressed{key, scancode, alt, control, shift, system});
    else if (!pressed)
        queueEvent(state, sf::Event::KeyReleased{key, scancode, alt, control, shift, system});

    if (emitPressed && unicode && unicode <= 0x10FFFF && (unicode < 0xD800 || unicode > 0xDFFF))
        queueEvent(state, sf::Event::TextEntered{unicode});
}


void onKey(OH_NativeXComponent* component, ArkUI_UIInputEvent* input, ArkUI_UIInputEvent_Type type)
{
    if (!input || type != ARKUI_UIINPUTEVENT_TYPE_KEY)
        return;
    if (OH_ArkUI_KeyEvent_GetKeySource(input) == ARKUI_KEY_SOURCE_TYPE_JOYSTICK)
        return;

    const auto action  = OH_ArkUI_KeyEvent_GetType(input);
    const bool pressed = action == ARKUI_KEY_EVENT_DOWN || action == ARKUI_KEY_EVENT_LONG_PRESS;
    if (!pressed && action != ARKUI_KEY_EVENT_UP)
        return;

    processKey(component,
               static_cast<OH_NativeXComponent_KeyCode>(OH_ArkUI_KeyEvent_GetKeyCode(input)),
               pressed,
               static_cast<char32_t>(OH_ArkUI_KeyEvent_GetUnicode(input)));
}


void onLegacyKey(OH_NativeXComponent* component, void*)
{
    OH_NativeXComponent_KeyEvent*       event{};
    OH_NativeXComponent_KeyAction       action{OH_NATIVEXCOMPONENT_KEY_ACTION_UNKNOWN};
    OH_NativeXComponent_KeyCode         code{KEY_UNKNOWN};
    OH_NativeXComponent_EventSourceType source{OH_NATIVEXCOMPONENT_SOURCE_TYPE_UNKNOWN};
    if (OH_NativeXComponent_GetKeyEvent(component, &event) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS || !event ||
        OH_NativeXComponent_GetKeyEventAction(event, &action) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS ||
        OH_NativeXComponent_GetKeyEventCode(event, &code) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS ||
        (action != OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN && action != OH_NATIVEXCOMPONENT_KEY_ACTION_UP))
        return;
    if (OH_NativeXComponent_GetKeyEventSourceType(event, &source) == OH_NATIVEXCOMPONENT_RESULT_SUCCESS &&
        source == OH_NATIVEXCOMPONENT_SOURCE_TYPE_JOYSTICK)
        return;

    // The legacy XComponent key callback is the API21-compatible fallback.
    // Committed Unicode still arrives through the native IME bridge; newer
    // runtimes that accept ARKUI_UIINPUTEVENT_TYPE_KEY use onKey above.
    processKey(component, code, action == OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN, 0);
}


OH_NativeXComponent_Callback surfaceCallbacks{onSurfaceCreated, onSurfaceChanged, onSurfaceDestroyed, onTouch};
OH_NativeXComponent_MouseEvent_Callback mouseCallbacks{onMouse, onHover};

} // namespace


namespace sf::priv::Harmony
{
HostState& getHostState()
{
    static HostState state;
    return state;
}


SurfaceSnapshot getSurfaceSnapshot()
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    return {state.window, state.size, state.surfaceGeneration};
}


std::deque<Event> drainEvents()
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    return state.events.drain();
}


void enqueueEvent(Event event)
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (!state.destroyed)
        queueEvent(state, event);
}


void submitTextEdit(std::size_t backwardDeletions, std::size_t forwardDeletions, std::u32string_view insertedText)
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.destroyed)
        return;

    const auto pushEditingKey = [&](Keyboard::Key key, Keyboard::Scancode scancode)
    {
        queueEvent(state, Event::KeyPressed{key, scancode, false, false, false, false});
        queueEvent(state, Event::KeyReleased{key, scancode, false, false, false, false});
    };

    for (std::size_t index = 0; index < backwardDeletions; ++index)
    {
        pushEditingKey(Keyboard::Key::Backspace, Keyboard::Scan::Backspace);
        queueEvent(state, Event::TextEntered{U'\b'});
    }
    for (std::size_t index = 0; index < forwardDeletions; ++index)
        pushEditingKey(Keyboard::Key::Delete, Keyboard::Scan::Delete);

    for (const char32_t unicode : insertedText)
    {
        if (unicode && unicode <= 0x10FFFF && (unicode < 0xD800 || unicode > 0xDFFF))
            queueEvent(state, Event::TextEntered{unicode});
    }
}


bool claimWindow()
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.windowClaimed || state.destroyed)
        return false;
    state.windowClaimed = true;
    return true;
}


void releaseWindow()
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    state.windowClaimed = false;
    cancelWindowCommands(state);
}


bool requestWindowCommand(WindowCommand command)
{
    constexpr auto commandTimeout = std::chrono::seconds(3);
    auto&          state          = getHostState();
    HostCallbacks  callbacks;
    std::uint32_t  requestId{};
    {
        const std::lock_guard lock(state.mutex);
        if (state.destroyed || !state.windowClaimed || !state.callbacks.executeWindowCommand)
            return false;

        // Zero is reserved for host notifications that do not acknowledge a
        // native request. Skip live identifiers if the counter wrapped.
        do
        {
            requestId = state.nextWindowRequestId++;
            if (state.nextWindowRequestId == 0)
                state.nextWindowRequestId = 1;
        } while (requestId == 0 || state.pendingWindowCommands.count(requestId));

        command.requestId = requestId;
        command.deadline  = std::chrono::steady_clock::now() + commandTimeout;
        state.pendingWindowCommands.emplace(requestId, PendingWindowCommand{command});
        callbacks = state.callbacks;
    }

    // Queue ArkTS work without holding HostState. The application thread then
    // sleeps with the mutex released, allowing the NAPI acknowledgement and
    // lifecycle cancellation paths to make progress.
    if (!callbacks.executeWindowCommand(command, callbacks.userData))
    {
        const std::lock_guard lock(state.mutex);
        state.pendingWindowCommands.erase(requestId);
        state.windowCommandCondition.notify_all();
        err() << "Failed to queue Harmony window command " << static_cast<std::uint32_t>(command.type) << " (request "
              << requestId << ')' << std::endl;
        return false;
    }

    std::unique_lock lock(state.mutex);
    const bool       acknowledged = state.windowCommandCondition
                                  .wait_until(lock,
                                              command.deadline,
                                              [&]
                                              {
                                                  const auto found = state.pendingWindowCommands.find(requestId);
                                                  return state.destroyed || found == state.pendingWindowCommands.end() ||
                                                         found->second.completed;
                                              });
    const auto found = state.pendingWindowCommands.find(requestId);
    if (!acknowledged || found == state.pendingWindowCommands.end())
    {
        state.pendingWindowCommands.erase(requestId);
        if (!state.destroyed && acknowledged)
            err() << "Harmony window command " << requestId << " was canceled" << std::endl;
        else if (!state.destroyed)
            err() << "Timed out waiting 3 seconds for Harmony window command " << requestId << std::endl;
        return false;
    }

    const bool success = found->second.success;
    state.pendingWindowCommands.erase(found);
    if (!success)
        err() << "Harmony host rejected window command " << requestId << std::endl;
    return success;
}


bool isWindowCommandPending(std::uint32_t requestId)
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    const auto            found = state.pendingWindowCommands.find(requestId);
    return !state.destroyed && found != state.pendingWindowCommands.end() && !found->second.completed;
}


WindowState getWindowState()
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    WindowState           result = state.windowState;
    if ((!result.clientSize.x || !result.clientSize.y) && state.size.x && state.size.y)
        result.clientSize = state.size;
    return result;
}


void setMainEntry(void (*entry)())
{
    auto&                  state = getHostState();
    const std::unique_lock lock(state.mutex);
    state.mainEntry = entry;
    maybeStartMain(state);
}


void setShutdownCallback(void (*callback)())
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    state.shutdownCallback = callback;
}


HostInitializationToken beginHostInitialization()
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.destroyed)
        return {};
    return state.hostInitialization.begin();
}


bool authorizeInputMethodAttach(HostInitializationToken token)
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    return !state.destroyed && state.hostInitialization.authorize(token);
}


bool commitHostInitialization(HostInitializationToken token)
{
    auto&                  state = getHostState();
    const std::unique_lock lock(state.mutex);
    if (state.destroyed || !state.hostInitialization.commit(token))
        return false;

    state.hostInitialized = true;
    maybeStartMain(state);
    return true;
}


void abortHostInitialization(HostInitializationToken token)
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    state.hostInitialization.abort(token);
}


void cancelHostInitialization()
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    state.hostInitialization.shutdown();
    state.hostInitialized = false;
}


void notifyMainFinished()
{
    auto& state        = getHostState();
    void (*shutdown)() = nullptr;
    {
        const std::lock_guard lock(state.mutex);
        state.mainFinished = true;
        // A normally returning main thread releases its own handle. Host
        // destruction may already have detached it to keep ArkUI nonblocking.
        if (state.mainThread.joinable() && state.mainThread.get_id() == std::this_thread::get_id())
        {
            try
            {
                state.mainThread.detach();
            } catch (const std::system_error& exception)
            {
                err() << "Failed to release the completed Harmony application thread: " << exception.what() << std::endl;
            }
        }
        if (state.destroyed)
            shutdown = std::exchange(state.shutdownCallback, nullptr);
    }
    if (shutdown)
        shutdown();
}


Keyboard::Key keyCodeToKey(OH_NativeXComponent_KeyCode code)
{
    if (code >= KEY_A && code <= KEY_Z)
        return static_cast<Keyboard::Key>(static_cast<int>(Keyboard::Key::A) + (code - KEY_A));
    if (code >= KEY_0 && code <= KEY_9)
        return static_cast<Keyboard::Key>(static_cast<int>(Keyboard::Key::Num0) + (code - KEY_0));
    if (code >= KEY_NUMPAD_0 && code <= KEY_NUMPAD_9)
        return static_cast<Keyboard::Key>(static_cast<int>(Keyboard::Key::Numpad0) + (code - KEY_NUMPAD_0));
    if (code >= KEY_F1 && code <= KEY_F12)
        return static_cast<Keyboard::Key>(static_cast<int>(Keyboard::Key::F1) + (code - KEY_F1));

    switch (code)
    {
        case KEY_ESCAPE:
            return Keyboard::Key::Escape;
        case KEY_CTRL_LEFT:
            return Keyboard::Key::LControl;
        case KEY_CTRL_RIGHT:
            return Keyboard::Key::RControl;
        case KEY_SHIFT_LEFT:
            return Keyboard::Key::LShift;
        case KEY_SHIFT_RIGHT:
            return Keyboard::Key::RShift;
        case KEY_ALT_LEFT:
            return Keyboard::Key::LAlt;
        case KEY_ALT_RIGHT:
            return Keyboard::Key::RAlt;
        case KEY_META_LEFT:
            return Keyboard::Key::LSystem;
        case KEY_META_RIGHT:
            return Keyboard::Key::RSystem;
        case KEY_MENU:
            return Keyboard::Key::Menu;
        case KEY_LEFT_BRACKET:
            return Keyboard::Key::LBracket;
        case KEY_RIGHT_BRACKET:
            return Keyboard::Key::RBracket;
        case KEY_SEMICOLON:
            return Keyboard::Key::Semicolon;
        case KEY_COMMA:
            return Keyboard::Key::Comma;
        case KEY_PERIOD:
            return Keyboard::Key::Period;
        case KEY_APOSTROPHE:
            return Keyboard::Key::Apostrophe;
        case KEY_SLASH:
            return Keyboard::Key::Slash;
        case KEY_BACKSLASH:
            return Keyboard::Key::Backslash;
        case KEY_GRAVE:
            return Keyboard::Key::Grave;
        case KEY_EQUALS:
            return Keyboard::Key::Equal;
        case KEY_MINUS:
            return Keyboard::Key::Hyphen;
        case KEY_SPACE:
            return Keyboard::Key::Space;
        case KEY_ENTER:
        case KEY_NUMPAD_ENTER:
            return Keyboard::Key::Enter;
        case KEY_DEL:
            return Keyboard::Key::Backspace;
        case KEY_TAB:
            return Keyboard::Key::Tab;
        case KEY_PAGE_UP:
            return Keyboard::Key::PageUp;
        case KEY_PAGE_DOWN:
            return Keyboard::Key::PageDown;
        case KEY_MOVE_END:
            return Keyboard::Key::End;
        case KEY_MOVE_HOME:
            return Keyboard::Key::Home;
        case KEY_INSERT:
            return Keyboard::Key::Insert;
        case KEY_FORWARD_DEL:
            return Keyboard::Key::Delete;
        case KEY_NUMPAD_ADD:
            return Keyboard::Key::Add;
        case KEY_NUMPAD_SUBTRACT:
            return Keyboard::Key::Subtract;
        case KEY_NUMPAD_MULTIPLY:
            return Keyboard::Key::Multiply;
        case KEY_NUMPAD_DIVIDE:
            return Keyboard::Key::Divide;
        case KEY_DPAD_LEFT:
            return Keyboard::Key::Left;
        case KEY_DPAD_RIGHT:
            return Keyboard::Key::Right;
        case KEY_DPAD_UP:
            return Keyboard::Key::Up;
        case KEY_DPAD_DOWN:
            return Keyboard::Key::Down;
        case KEY_BREAK:
            return Keyboard::Key::Pause;
        default:
            return Keyboard::Key::Unknown;
    }
}


Keyboard::Scancode keyCodeToScancode(OH_NativeXComponent_KeyCode code)
{
    if (code >= KEY_A && code <= KEY_Z)
        return static_cast<Keyboard::Scancode>(static_cast<int>(Keyboard::Scan::A) + (code - KEY_A));

    switch (code)
    {
        case KEY_1:
            return Keyboard::Scan::Num1;
        case KEY_2:
            return Keyboard::Scan::Num2;
        case KEY_3:
            return Keyboard::Scan::Num3;
        case KEY_4:
            return Keyboard::Scan::Num4;
        case KEY_5:
            return Keyboard::Scan::Num5;
        case KEY_6:
            return Keyboard::Scan::Num6;
        case KEY_7:
            return Keyboard::Scan::Num7;
        case KEY_8:
            return Keyboard::Scan::Num8;
        case KEY_9:
            return Keyboard::Scan::Num9;
        case KEY_0:
            return Keyboard::Scan::Num0;
        case KEY_ENTER:
            return Keyboard::Scan::Enter;
        case KEY_ESCAPE:
            return Keyboard::Scan::Escape;
        case KEY_DEL:
            return Keyboard::Scan::Backspace;
        case KEY_TAB:
            return Keyboard::Scan::Tab;
        case KEY_SPACE:
            return Keyboard::Scan::Space;
        case KEY_MINUS:
            return Keyboard::Scan::Hyphen;
        case KEY_EQUALS:
            return Keyboard::Scan::Equal;
        case KEY_LEFT_BRACKET:
            return Keyboard::Scan::LBracket;
        case KEY_RIGHT_BRACKET:
            return Keyboard::Scan::RBracket;
        case KEY_BACKSLASH:
            return Keyboard::Scan::Backslash;
        case KEY_SEMICOLON:
            return Keyboard::Scan::Semicolon;
        case KEY_APOSTROPHE:
            return Keyboard::Scan::Apostrophe;
        case KEY_GRAVE:
            return Keyboard::Scan::Grave;
        case KEY_COMMA:
            return Keyboard::Scan::Comma;
        case KEY_PERIOD:
            return Keyboard::Scan::Period;
        case KEY_SLASH:
            return Keyboard::Scan::Slash;
        case KEY_CAPS_LOCK:
            return Keyboard::Scan::CapsLock;
        case KEY_SYSRQ:
            return Keyboard::Scan::PrintScreen;
        case KEY_SCROLL_LOCK:
            return Keyboard::Scan::ScrollLock;
        case KEY_BREAK:
            return Keyboard::Scan::Pause;
        case KEY_INSERT:
            return Keyboard::Scan::Insert;
        case KEY_MOVE_HOME:
            return Keyboard::Scan::Home;
        case KEY_PAGE_UP:
            return Keyboard::Scan::PageUp;
        case KEY_FORWARD_DEL:
            return Keyboard::Scan::Delete;
        case KEY_MOVE_END:
            return Keyboard::Scan::End;
        case KEY_PAGE_DOWN:
            return Keyboard::Scan::PageDown;
        case KEY_DPAD_RIGHT:
            return Keyboard::Scan::Right;
        case KEY_DPAD_LEFT:
            return Keyboard::Scan::Left;
        case KEY_DPAD_DOWN:
            return Keyboard::Scan::Down;
        case KEY_DPAD_UP:
            return Keyboard::Scan::Up;
        case KEY_NUM_LOCK:
            return Keyboard::Scan::NumLock;
        case KEY_NUMPAD_DIVIDE:
            return Keyboard::Scan::NumpadDivide;
        case KEY_NUMPAD_MULTIPLY:
            return Keyboard::Scan::NumpadMultiply;
        case KEY_NUMPAD_SUBTRACT:
            return Keyboard::Scan::NumpadMinus;
        case KEY_NUMPAD_ADD:
            return Keyboard::Scan::NumpadPlus;
        case KEY_NUMPAD_EQUALS:
            return Keyboard::Scan::NumpadEqual;
        case KEY_NUMPAD_ENTER:
            return Keyboard::Scan::NumpadEnter;
        case KEY_NUMPAD_DOT:
            return Keyboard::Scan::NumpadDecimal;
        case KEY_NUMPAD_1:
            return Keyboard::Scan::Numpad1;
        case KEY_NUMPAD_2:
            return Keyboard::Scan::Numpad2;
        case KEY_NUMPAD_3:
            return Keyboard::Scan::Numpad3;
        case KEY_NUMPAD_4:
            return Keyboard::Scan::Numpad4;
        case KEY_NUMPAD_5:
            return Keyboard::Scan::Numpad5;
        case KEY_NUMPAD_6:
            return Keyboard::Scan::Numpad6;
        case KEY_NUMPAD_7:
            return Keyboard::Scan::Numpad7;
        case KEY_NUMPAD_8:
            return Keyboard::Scan::Numpad8;
        case KEY_NUMPAD_9:
            return Keyboard::Scan::Numpad9;
        case KEY_NUMPAD_0:
            return Keyboard::Scan::Numpad0;
        case KEY_MENU:
            return Keyboard::Scan::Menu;
        case KEY_CTRL_LEFT:
            return Keyboard::Scan::LControl;
        case KEY_SHIFT_LEFT:
            return Keyboard::Scan::LShift;
        case KEY_ALT_LEFT:
            return Keyboard::Scan::LAlt;
        case KEY_META_LEFT:
            return Keyboard::Scan::LSystem;
        case KEY_CTRL_RIGHT:
            return Keyboard::Scan::RControl;
        case KEY_SHIFT_RIGHT:
            return Keyboard::Scan::RShift;
        case KEY_ALT_RIGHT:
            return Keyboard::Scan::RAlt;
        case KEY_META_RIGHT:
            return Keyboard::Scan::RSystem;
        case KEY_BACK:
            return Keyboard::Scan::Back;
        case KEY_FORWARD:
            return Keyboard::Scan::Forward;
        case KEY_REFRESH:
            return Keyboard::Scan::Refresh;
        case KEY_MEDIA_PLAY_PAUSE:
            return Keyboard::Scan::MediaPlayPause;
        case KEY_MEDIA_STOP:
            return Keyboard::Scan::MediaStop;
        case KEY_MEDIA_NEXT:
            return Keyboard::Scan::MediaNextTrack;
        case KEY_MEDIA_PREVIOUS:
            return Keyboard::Scan::MediaPreviousTrack;
        case KEY_VOLUME_MUTE:
            return Keyboard::Scan::VolumeMute;
        case KEY_VOLUME_UP:
            return Keyboard::Scan::VolumeUp;
        case KEY_VOLUME_DOWN:
            return Keyboard::Scan::VolumeDown;
        default:
            if (code >= KEY_F1 && code <= KEY_F12)
                return static_cast<Keyboard::Scancode>(static_cast<int>(Keyboard::Scan::F1) + (code - KEY_F1));
            return Keyboard::Scan::Unknown;
    }
}

} // namespace sf::priv::Harmony


namespace sf::priv::Harmony
{
bool registerNativeXComponent(void* componentPointer)
{
    auto* component = static_cast<OH_NativeXComponent*>(componentPointer);
    auto& state     = getHostState();
    if (!component)
    {
        err() << "Cannot register a null Harmony Native XComponent" << std::endl;
        return false;
    }
    if (!isSupportedDevice())
    {
#ifdef SFML_HARMONY_2IN1
        err() << "SFML Harmony 2-in-1 requires device type 2in1 (device type: "
#else
        err() << "SFML Harmony mobile supports phone and tablet devices only (device type: "
#endif
              << (OH_GetDeviceType() ? OH_GetDeviceType() : "unknown") << ')' << std::endl;
        return false;
    }

    {
        const std::lock_guard lock(state.mutex);
        if (state.component == component)
            return true;
        if (state.destroyed)
        {
            err() << "Cannot register a Harmony Native XComponent after the host was destroyed" << std::endl;
            return false;
        }
        if (state.registeringComponent)
        {
            err() << "Another Harmony Native XComponent registration is already in progress" << std::endl;
            return false;
        }
        state.registeringComponent = component;
        state.registeringWindow    = nullptr;
        state.registeringSize      = {};
    }

    const auto uiKeyResult = OH_NativeXComponent_RegisterUIInputEventCallback(component, onKey, ARKUI_UIINPUTEVENT_TYPE_KEY);
    const bool legacyKeyCallback = uiKeyResult != 0;
    const auto keyResult         = uiKeyResult == 0 ? uiKeyResult
                                                    : OH_NativeXComponent_RegisterKeyEventCallback(component, onLegacyKey);
    const std::array<std::int32_t, 6>
        results{OH_NativeXComponent_RegisterCallback(component, &surfaceCallbacks),
                OH_NativeXComponent_RegisterMouseEventCallback(component, &mouseCallbacks),
                OH_NativeXComponent_RegisterFocusEventCallback(component, onFocus),
                OH_NativeXComponent_RegisterBlurEventCallback(component, onBlur),
                OH_NativeXComponent_RegisterUIInputEventCallback(component, onAxis, ARKUI_UIINPUTEVENT_TYPE_AXIS),
                keyResult};
    const bool registered = std::all_of(results.begin(), results.end(), [](std::int32_t result) { return result == 0; });
    if (!registered)
    {
        const std::lock_guard lock(state.mutex);
        if (state.registeringComponent == component)
        {
            state.registeringComponent = nullptr;
            state.registeringWindow    = nullptr;
            state.registeringSize      = {};
        }
        err() << "Failed to register one or more Harmony Native XComponent callbacks" << std::endl;
        return false;
    }

    const std::unique_lock lock(state.mutex);
    if (state.registeringComponent != component)
        return false;

    const bool  replacingComponent = state.component && state.component != component;
    auto* const replacementWindow  = state.registeringWindow;
    const auto  replacementSize    = state.registeringSize;

    state.registeringComponent = nullptr;
    state.registeringWindow    = nullptr;
    state.registeringSize      = {};
    state.component            = component;
    state.legacyKeyCallback    = legacyKeyCallback;
    state.pointerLocationAvailable.reset();
    state.retiredWindows.clear();
    state.destroyedWindows.clear();
    applySurface(state, replacementWindow, replacementSize, replacingComponent);
    return true;
}


void unregisterNativeXComponent(void* componentPointer)
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.registeringComponent == componentPointer)
    {
        state.registeringComponent = nullptr;
        state.registeringWindow    = nullptr;
        state.registeringSize      = {};
    }
    if (state.component == componentPointer)
    {
        state.component         = nullptr;
        state.legacyKeyCallback = false;
        state.pointerLocationAvailable.reset();
        state.retiredWindows.clear();
        state.destroyedWindows.clear();
        applySurface(state, nullptr, {});
    }
}


void notifyForeground()
{
    auto& state = getHostState();
#ifdef SFML_HARMONY_MOBILE
    HostCallbacks callbacks;
    bool          replayWindowConfiguration = false;
    bool          fullscreen{};
    bool          keepScreenOn{};
#endif
    {
        const std::lock_guard lock(state.mutex);
        if (state.destroyed)
            return;
        state.foreground = true;
#ifdef SFML_HARMONY_MOBILE
        if (state.window && !state.focused)
        {
            state.focused             = true;
            state.windowState.focused = true;
            queueEvent(state, Event::FocusGained{});
        }
#endif

#ifdef SFML_HARMONY_MOBILE
        callbacks                 = state.callbacks;
        replayWindowConfiguration = state.windowConfigurationSet;
        fullscreen                = state.fullscreen;
        keepScreenOn              = state.keepScreenOn;
#endif
    }

#ifdef SFML_HARMONY_MOBILE
    // The system may restore its bars and screen-on policy while another
    // ability owns the foreground. Replay the mobile window policy whenever
    // this Stage returns, without invoking ArkUI while holding HostState.
    if (replayWindowConfiguration && callbacks.configureWindow)
        callbacks.configureWindow(fullscreen, keepScreenOn, callbacks.userData);
#endif
}


void notifyBackground()
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.destroyed)
        return;
    state.foreground = false;
    clearInputState(state, true);
    if (state.focused)
    {
        state.focused             = false;
        state.windowState.focused = false;
        queueEvent(state, Event::FocusLost{});
    }
}


void notifyDestroy()
{
    auto& state        = getHostState();
    void (*shutdown)() = nullptr;
    {
        const std::lock_guard lock(state.mutex);
        if (!state.destroyed)
            queueEvent(state, Event::Closed{});
        const bool hadSurface = state.window != nullptr;
        state.destroyed       = true;
        state.hostInitialized = false;
        state.hostInitialization.shutdown();
        state.foreground          = false;
        state.focused             = false;
        state.windowState.focused = false;
        clearInputState(state, true);
        state.component              = nullptr;
        state.registeringComponent   = nullptr;
        state.registeringWindow      = nullptr;
        state.registeringSize        = {};
        state.window                 = nullptr;
        state.windowId               = 0;
        state.legacyKeyCallback      = false;
        state.windowConfigurationSet = false;
        state.fullscreen             = false;
        state.keepScreenOn           = false;
        state.size                   = {};
        state.windowState            = {};
        state.minimumSize.reset();
        state.maximumSize.reset();
        state.title.clear();
        state.style = 0;
        cancelWindowCommands(state);
        state.retiredWindows.clear();
        state.destroyedWindows.clear();
        if (hadSurface)
            ++state.surfaceGeneration;
        state.surfaceCondition.notify_all();

        // A running sfmlMain owns streams that may still use the resource
        // manager.  Keep host resources alive until it has handled Closed and
        // returned; if it never started, shutdown can complete immediately.
        if (!state.mainStarted || state.mainFinished)
            shutdown = std::exchange(state.shutdownCallback, nullptr);

        if (state.mainThread.joinable())
        {
            try
            {
                state.mainThread.detach();
            } catch (const std::system_error& exception)
            {
                err() << "Failed to detach the Harmony application thread during host destruction: " << exception.what()
                      << std::endl;
            }
        }
    }

    // Stage lifecycle callbacks run on ArkUI's thread and must never wait for
    // arbitrary user code in sfmlMain. A detached application thread releases
    // host resources from notifyMainFinished() after consuming Closed.
    if (shutdown)
        shutdown();
}


void submitText(char32_t unicode)
{
    if (!unicode || unicode > 0x10FFFF || (unicode >= 0xD800 && unicode <= 0xDFFF))
        return;

    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.destroyed)
        return;
    queueEvent(state, Event::TextEntered{unicode});
}


void setHostCallbacks(const HostCallbacks& callbacks)
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.callbacks.executeWindowCommand && (state.callbacks.executeWindowCommand != callbacks.executeWindowCommand ||
                                                 state.callbacks.userData != callbacks.userData))
    {
        cancelWindowCommands(state);
    }
    state.callbacks = callbacks;
}


bool completeWindowCommand(std::uint32_t requestId, bool success, const WindowState& nextState)
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (state.destroyed)
        return false;

    const auto found = state.pendingWindowCommands.find(requestId);
    if (found == state.pendingWindowCommands.end() || found->second.completed)
        return false;

    found->second.success   = success;
    found->second.completed = true;
    if (success)
    {
        applyWindowState(state, nextState);
        applySuccessfulWindowCommand(state, found->second.command);
    }
    state.windowCommandCondition.notify_all();
    return true;
}


void updateWindowState(const WindowState& nextState)
{
    auto&                 state = getHostState();
    const std::lock_guard lock(state.mutex);
    if (!state.destroyed)
        applyWindowState(state, nextState);
}

} // namespace sf::priv::Harmony
