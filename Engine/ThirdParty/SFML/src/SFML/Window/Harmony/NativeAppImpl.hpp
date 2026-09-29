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

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Harmony/CancelableInitialization.hpp>
#include <SFML/Window/Harmony/EventQueue.hpp>
#include <SFML/Window/Harmony/NativeApp.hpp>

#include <SFML/System/EnumArray.hpp>

#include <ace/xcomponent/native_interface_xcomponent.h>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>


struct NativeWindow;


namespace sf::priv::Harmony
{
using HostInitializationToken = CancelableInitialization::Token;


struct SurfaceSnapshot
{
    NativeWindow* window{};
    Vector2u      size;
    std::uint64_t generation{};
};


struct PendingWindowCommand
{
    WindowCommand command;
    bool          completed{};
    bool          success{};
};


struct HostState
{
    std::mutex              mutex;
    std::condition_variable surfaceCondition;
    std::condition_variable windowCommandCondition;

    OH_NativeXComponent* component{};
    OH_NativeXComponent* registeringComponent{};
    NativeWindow*        registeringWindow{};
    Vector2u             registeringSize;
    NativeWindow*        window{};
    Vector2u             size;
    std::uint64_t        surfaceGeneration{};

    // A replacement surface can be created before the destroy callback for
    // its predecessor. Retired windows remain ignored until that matching
    // destroy arrives; a later create can then legitimately reuse the pointer.
    std::unordered_set<NativeWindow*> retiredWindows;
    std::unordered_set<NativeWindow*> destroyedWindows;

    EventQueue events;

    std::unordered_map<unsigned int, Vector2i>         touches;
    Vector2i                                           mousePosition;
    EnumArray<Mouse::Button, bool, Mouse::ButtonCount> mouseButtons{};
    std::unordered_set<Keyboard::Key>                  keys;
    std::unordered_set<Keyboard::Scancode>             scancodes;
    std::unordered_set<std::int32_t>                   keyCodes;

    HostCallbacks callbacks;

    std::int32_t                                            windowId{};
    WindowState                                             windowState;
    std::uint32_t                                           nextWindowRequestId{1};
    std::unordered_map<std::uint32_t, PendingWindowCommand> pendingWindowCommands;
    std::optional<Vector2u>                                 minimumSize;
    std::optional<Vector2u>                                 maximumSize;
    std::string                                             title;
    std::uint32_t                                           style{};

    void (*mainEntry)(){};
    void (*shutdownCallback)(){};
    std::thread              mainThread;
    bool                     mainStarted{};
    bool                     mainStartFailed{};
    bool                     mainFinished{};
    bool                     hostInitialized{};
    CancelableInitialization hostInitialization;
    bool                     foreground{true};
    bool                     focused{};
    std::optional<bool>      pointerLocationAvailable;
    bool                     destroyed{};
    bool                     windowClaimed{};
    bool                     keyRepeatEnabled{true};
    bool                     legacyKeyCallback{};
    bool                     windowConfigurationSet{};
    bool                     fullscreen{};
    bool                     keepScreenOn{};
};


HostState& getHostState();

SurfaceSnapshot   getSurfaceSnapshot();
std::deque<Event> drainEvents();
void              enqueueEvent(Event event);
void submitTextEdit(std::size_t backwardDeletions, std::size_t forwardDeletions, std::u32string_view insertedText);

bool claimWindow();
void releaseWindow();

[[nodiscard]] bool        requestWindowCommand(WindowCommand command);
[[nodiscard]] bool        isWindowCommandPending(std::uint32_t requestId);
[[nodiscard]] WindowState getWindowState();

void                                  setMainEntry(void (*entry)());
void                                  setShutdownCallback(void (*callback)());
[[nodiscard]] HostInitializationToken beginHostInitialization();
[[nodiscard]] bool                    authorizeInputMethodAttach(HostInitializationToken token);
[[nodiscard]] bool                    commitHostInitialization(HostInitializationToken token);
void                                  abortHostInitialization(HostInitializationToken token);
void                                  cancelHostInitialization();
void                                  notifyMainFinished();

Keyboard::Key      keyCodeToKey(OH_NativeXComponent_KeyCode code);
Keyboard::Scancode keyCodeToScancode(OH_NativeXComponent_KeyCode code);

} // namespace sf::priv::Harmony
