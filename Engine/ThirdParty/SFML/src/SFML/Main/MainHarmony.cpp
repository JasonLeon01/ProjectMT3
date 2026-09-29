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

#include <SFML/Window/Harmony/InputMethodImpl.hpp>
#include <SFML/Window/Harmony/NativeApp.hpp>
#include <SFML/Window/Harmony/NativeAppImpl.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/Harmony/ResourceManagerImpl.hpp>
#include <SFML/System/String.hpp>

#include <SFML/Main/MainHarmony.hpp>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <array>
#include <chrono>
#include <exception>
#include <hilog/log.h>
#include <memory>
#include <mutex>
#include <ostream>
#include <rawfile/raw_file_manager.h>
#include <streambuf>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <cstring>


int sfmlMain();


__attribute__((weak)) int sfmlMain(int, char**)
{
    return sfmlMain();
}


__attribute__((weak)) int sfmlMain()
{
    return 0;
}


namespace
{
constexpr unsigned int harmonyLogDomain = 0x5346;
constexpr const char*  harmonyLogTag    = "SFML";


class HarmonyLogStream final : public std::streambuf
{
private:
    static std::string& pendingMessage()
    {
        // SFML can report errors from its render, audio and application
        // threads concurrently. Keep incomplete lines separate until HiLog
        // receives them as complete messages.
        thread_local std::string message;
        return message;
    }

    static void emit(std::string& message)
    {
        (void)OH_LOG_Print(LOG_APP, LOG_ERROR, harmonyLogDomain, harmonyLogTag, "%{public}s", message.c_str());
        message.clear();
    }

    static void append(const char* data, std::size_t size)
    {
        auto&       message = pendingMessage();
        std::size_t begin   = 0;

        while (begin < size)
        {
            const std::string_view remaining(data + begin, size - begin);
            const std::size_t      newline = remaining.find('\n');
            if (newline == std::string_view::npos)
            {
                message.append(remaining);
                return;
            }

            message.append(remaining.substr(0, newline));
            emit(message);
            begin += newline + 1;
        }
    }

    int_type overflow(int_type character) override
    {
        if (traits_type::eq_int_type(character, traits_type::eof()))
            return sync() == 0 ? traits_type::not_eof(character) : traits_type::eof();

        const char value = traits_type::to_char_type(character);
        append(&value, 1);
        return character;
    }

    std::streamsize xsputn(const char* data, std::streamsize size) override
    {
        if (size > 0)
            append(data, static_cast<std::size_t>(size));
        return size;
    }

    int sync() override
    {
        auto& message = pendingMessage();
        if (!message.empty())
            emit(message);
        return 0;
    }
};


class HarmonyErrorRedirect
{
public:
    HarmonyErrorRedirect() : m_previous(sf::err().rdbuf(&m_stream))
    {
    }

    ~HarmonyErrorRedirect()
    {
        // Do not override a stream buffer subsequently selected by the user.
        // If ours is still installed, restore SFML's original buffer before
        // this object is destroyed so later static destructors remain safe.
        if (sf::err().rdbuf() == &m_stream)
        {
            m_stream.pubsync();
            sf::err().rdbuf(m_previous);
        }
    }

    HarmonyErrorRedirect(const HarmonyErrorRedirect&)            = delete;
    HarmonyErrorRedirect& operator=(const HarmonyErrorRedirect&) = delete;

private:
    mutable HarmonyLogStream m_stream;
    std::streambuf*          m_previous{};
};


void installHarmonyErrorStream()
{
    // Function-local static initialization is thread-safe and redirects only
    // once, so a later XComponent registration cannot replace a user buffer.
    static const HarmonyErrorRedirect redirect;
    (void)redirect;
}


std::mutex                                     resourceMutex;
NativeResourceManager*                         resourceManager{};
sf::priv::Harmony::ResourceManagerRegistration resourceManagerRegistration{};
bool                                           resourceShutdown{};


struct CustomPointerPayload
{
    std::vector<std::uint8_t> pixels;
    sf::Vector2u              size;
    sf::Vector2u              hotspot;
};


struct WindowConfigurationPayload
{
    bool fullscreen{};
    bool keepScreenOn{};
};


struct JsHostState
{
    std::mutex               mutex;
    napi_threadsafe_function pointerVisible{};
    napi_threadsafe_function systemPointer{};
    napi_threadsafe_function customPointer{};
    napi_threadsafe_function virtualKeyboard{};
    napi_threadsafe_function configureWindow{};
    napi_threadsafe_function executeWindowCommand{};
    napi_threadsafe_function requestExit{};
};


JsHostState jsHost;


void callFunction(napi_env env, napi_value function, std::size_t count, napi_value* arguments)
{
    if (!env || !function)
        return;

    napi_value receiver{};
    napi_value ignored{};
    napi_get_global(env, &receiver);
    napi_call_function(env, receiver, function, count, arguments, &ignored);
}


void callBoolean(napi_env env, napi_value function, void*, void* data)
{
    const std::unique_ptr<bool> value(static_cast<bool*>(data));
    if (!env || !function || !value)
        return;

    napi_value argument{};
    napi_get_boolean(env, *value, &argument);
    callFunction(env, function, 1, &argument);
}


void callUnsigned(napi_env env, napi_value function, void*, void* data)
{
    const std::unique_ptr<std::uint32_t> value(static_cast<std::uint32_t*>(data));
    if (!env || !function || !value)
        return;

    napi_value argument{};
    napi_create_uint32(env, *value, &argument);
    callFunction(env, function, 1, &argument);
}


void callVoid(napi_env env, napi_value function, void*, void*)
{
    callFunction(env, function, 0, nullptr);
}


void callCustomPointer(napi_env env, napi_value function, void*, void* data)
{
    const std::unique_ptr<CustomPointerPayload> value(static_cast<CustomPointerPayload*>(data));
    if (!env || !function || !value)
        return;

    void*      destination = nullptr;
    napi_value buffer{};
    napi_value pixels{};
    if (napi_create_arraybuffer(env, value->pixels.size(), &destination, &buffer) != napi_ok || !destination)
        return;
    std::memcpy(destination, value->pixels.data(), value->pixels.size());
    if (napi_create_typedarray(env, napi_uint8_array, value->pixels.size(), buffer, 0, &pixels) != napi_ok)
        return;

    std::array<napi_value, 5> arguments{};
    arguments[0] = pixels;
    napi_create_uint32(env, value->size.x, &arguments[1]);
    napi_create_uint32(env, value->size.y, &arguments[2]);
    napi_create_uint32(env, value->hotspot.x, &arguments[3]);
    napi_create_uint32(env, value->hotspot.y, &arguments[4]);
    callFunction(env, function, arguments.size(), arguments.data());
}


void callWindowConfiguration(napi_env env, napi_value function, void*, void* data)
{
    const std::unique_ptr<WindowConfigurationPayload> value(static_cast<WindowConfigurationPayload*>(data));
    if (!env || !function || !value)
        return;

    std::array<napi_value, 2> arguments{};
    napi_get_boolean(env, value->fullscreen, arguments.data());
    napi_get_boolean(env, value->keepScreenOn, arguments.data() + 1);
    callFunction(env, function, arguments.size(), arguments.data());
}


bool setNamedUint32(napi_env env, napi_value object, const char* name, std::uint32_t value)
{
    napi_value property{};
    return napi_create_uint32(env, value, &property) == napi_ok &&
           napi_set_named_property(env, object, name, property) == napi_ok;
}


bool setNamedInt32(napi_env env, napi_value object, const char* name, std::int32_t value)
{
    napi_value property{};
    return napi_create_int32(env, value, &property) == napi_ok &&
           napi_set_named_property(env, object, name, property) == napi_ok;
}


bool setNamedBoolean(napi_env env, napi_value object, const char* name, bool value)
{
    napi_value property{};
    return napi_get_boolean(env, value, &property) == napi_ok &&
           napi_set_named_property(env, object, name, property) == napi_ok;
}


bool setNamedBytes(napi_env env, napi_value object, const char* name, const std::vector<std::uint8_t>& bytes)
{
    void*      destination{};
    napi_value buffer{};
    napi_value array{};
    if (napi_create_arraybuffer(env, bytes.size(), &destination, &buffer) != napi_ok || (!bytes.empty() && !destination))
        return false;
    if (!bytes.empty())
        std::memcpy(destination, bytes.data(), bytes.size());
    return napi_create_typedarray(env, napi_uint8_array, bytes.size(), buffer, 0, &array) == napi_ok &&
           napi_set_named_property(env, object, name, array) == napi_ok;
}


void callWindowCommand(napi_env env, napi_value function, void*, void* data)
{
    const std::unique_ptr<sf::priv::Harmony::WindowCommand> command(static_cast<sf::priv::Harmony::WindowCommand*>(data));
    if (!command)
        return;

    const auto reject = [&]
    { (void)sf::priv::Harmony::completeWindowCommand(command->requestId, false, sf::priv::Harmony::getWindowState()); };
    if (!env || !function)
    {
        reject();
        return;
    }

    napi_value object{};
    napi_value title{};
    const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
                               command->deadline - std::chrono::steady_clock::now())
                               .count();
    if (remaining <= 0)
    {
        reject();
        return;
    }
    if (napi_create_object(env, &object) != napi_ok || !setNamedUint32(env, object, "requestId", command->requestId) ||
        !setNamedUint32(env, object, "timeoutMs", static_cast<std::uint32_t>(remaining)) ||
        !setNamedUint32(env, object, "type", static_cast<std::uint32_t>(command->type)) ||
        !setNamedInt32(env, object, "x", command->position.x) || !setNamedInt32(env, object, "y", command->position.y) ||
        !setNamedUint32(env, object, "width", command->size.x) || !setNamedUint32(env, object, "height", command->size.y) ||
        !setNamedBoolean(env, object, "hasSizeLimit", command->sizeLimit.has_value()) ||
        !setNamedUint32(env, object, "limitWidth", command->sizeLimit ? command->sizeLimit->x : 0) ||
        !setNamedUint32(env, object, "limitHeight", command->sizeLimit ? command->sizeLimit->y : 0) ||
        napi_create_string_utf8(env, command->title.data(), command->title.size(), &title) != napi_ok ||
        napi_set_named_property(env, object, "title", title) != napi_ok ||
        !setNamedUint32(env, object, "iconWidth", command->iconSize.x) ||
        !setNamedUint32(env, object, "iconHeight", command->iconSize.y) ||
        !setNamedBytes(env, object, "iconPixels", command->iconPixels) ||
        !setNamedUint32(env, object, "style", command->style) ||
        !setNamedBoolean(env, object, "fullscreen", command->fullscreen) ||
        !setNamedBoolean(env, object, "visible", command->visible) ||
        !setNamedBoolean(env, object, "keepScreenOn", command->keepScreenOn))
    {
        reject();
        return;
    }

    napi_value receiver{};
    napi_value ignored{};
    if (napi_get_global(env, &receiver) != napi_ok ||
        napi_call_function(env, receiver, function, 1, &object, &ignored) != napi_ok)
        reject();
}


bool createThreadsafeFunction(napi_env                         env,
                              napi_value                       object,
                              const char*                      property,
                              napi_threadsafe_function_call_js callback,
                              napi_threadsafe_function&        result)
{
    napi_value     function{};
    napi_value     name{};
    napi_valuetype type = napi_undefined;
    if (napi_get_named_property(env, object, property, &function) != napi_ok ||
        napi_typeof(env, function, &type) != napi_ok || type != napi_function ||
        napi_create_string_utf8(env, property, NAPI_AUTO_LENGTH, &name) != napi_ok)
        return false;

    return napi_create_threadsafe_function(env, function, nullptr, name, 0, 1, nullptr, nullptr, nullptr, callback, &result) ==
           napi_ok;
}


bool createOptionalThreadsafeFunction(napi_env                         env,
                                      napi_value                       object,
                                      const char*                      property,
                                      napi_threadsafe_function_call_js callback,
                                      napi_threadsafe_function&        result)
{
    bool exists{};
    if (napi_has_named_property(env, object, property, &exists) != napi_ok)
        return false;
    if (!exists)
        return true;
    return createThreadsafeFunction(env, object, property, callback, result);
}


void releaseJsHostCallbacks()
{
    sf::priv::Harmony::setHostCallbacks({});

    std::array<napi_threadsafe_function, 7> functions{};
    {
        const std::lock_guard lock(jsHost.mutex);
        functions = {std::exchange(jsHost.pointerVisible, nullptr),
                     std::exchange(jsHost.systemPointer, nullptr),
                     std::exchange(jsHost.customPointer, nullptr),
                     std::exchange(jsHost.virtualKeyboard, nullptr),
                     std::exchange(jsHost.configureWindow, nullptr),
                     std::exchange(jsHost.executeWindowCommand, nullptr),
                     std::exchange(jsHost.requestExit, nullptr)};
    }

    for (auto* const function : functions)
    {
        if (function)
            napi_release_threadsafe_function(function, napi_tsfn_abort);
    }
}


bool installJsHostCallbacks(napi_env env, napi_value object)
{
    releaseJsHostCallbacks();

    napi_threadsafe_function pointerVisible{};
    napi_threadsafe_function systemPointer{};
    napi_threadsafe_function customPointer{};
    napi_threadsafe_function virtualKeyboard{};
    napi_threadsafe_function configureWindow{};
    napi_threadsafe_function executeWindowCommand{};
    napi_threadsafe_function requestExit{};

    if (!createThreadsafeFunction(env, object, "setPointerVisible", callBoolean, pointerVisible) ||
        !createThreadsafeFunction(env, object, "setSystemPointer", callUnsigned, systemPointer) ||
        !createThreadsafeFunction(env, object, "setCustomPointer", callCustomPointer, customPointer) ||
        !createThreadsafeFunction(env, object, "setVirtualKeyboardVisible", callBoolean, virtualKeyboard) ||
        !createThreadsafeFunction(env, object, "configureWindow", callWindowConfiguration, configureWindow) ||
        !createOptionalThreadsafeFunction(env, object, "executeWindowCommand", callWindowCommand, executeWindowCommand) ||
        !createThreadsafeFunction(env, object, "requestExit", callVoid, requestExit))
    {
        for (auto* const function :
             {pointerVisible, systemPointer, customPointer, virtualKeyboard, configureWindow, executeWindowCommand, requestExit})
        {
            if (function)
                napi_release_threadsafe_function(function, napi_tsfn_abort);
        }
        return false;
    }

    {
        const std::lock_guard lock(jsHost.mutex);
        jsHost.pointerVisible       = pointerVisible;
        jsHost.systemPointer        = systemPointer;
        jsHost.customPointer        = customPointer;
        jsHost.virtualKeyboard      = virtualKeyboard;
        jsHost.configureWindow      = configureWindow;
        jsHost.executeWindowCommand = executeWindowCommand;
        jsHost.requestExit          = requestExit;
    }

    sf::priv::Harmony::HostCallbacks callbacks;
    callbacks.setPointerVisible = [](bool visible, void*)
    {
        const std::lock_guard lock(jsHost.mutex);
        auto*                 value = new bool(visible);
        if (!jsHost.pointerVisible ||
            napi_call_threadsafe_function(jsHost.pointerVisible, value, napi_tsfn_nonblocking) != napi_ok)
            delete value;
    };
    callbacks.setSystemPointer = [](unsigned int type, void*)
    {
        const std::lock_guard lock(jsHost.mutex);
        auto*                 value = new std::uint32_t(type);
        if (!jsHost.systemPointer ||
            napi_call_threadsafe_function(jsHost.systemPointer, value, napi_tsfn_nonblocking) != napi_ok)
            delete value;
    };
    callbacks.setCustomPointer = [](const std::uint8_t* pixels, sf::Vector2u size, sf::Vector2u hotspot, void*)
    {
        if (!pixels || !size.x || !size.y)
            return;

        auto value = std::make_unique<CustomPointerPayload>();
        value->pixels.assign(pixels, pixels + static_cast<std::size_t>(size.x) * size.y * 4u);
        value->size    = size;
        value->hotspot = hotspot;

        const std::lock_guard lock(jsHost.mutex);
        if (jsHost.customPointer &&
            napi_call_threadsafe_function(jsHost.customPointer, value.get(), napi_tsfn_nonblocking) == napi_ok)
            (void)value.release(); // NOLINT(bugprone-unused-return-value) - The queued callback owns the payload.
    };
    callbacks.setVirtualKeyboardVisible = [](bool visible, void*)
    {
        const std::lock_guard lock(jsHost.mutex);
        auto*                 value = new bool(visible);
        if (!jsHost.virtualKeyboard ||
            napi_call_threadsafe_function(jsHost.virtualKeyboard, value, napi_tsfn_nonblocking) != napi_ok)
            delete value;
    };
    callbacks.configureWindow = [](bool fullscreen, bool keepScreenOn, void*)
    {
        auto value          = std::make_unique<WindowConfigurationPayload>();
        value->fullscreen   = fullscreen;
        value->keepScreenOn = keepScreenOn;

        const std::lock_guard lock(jsHost.mutex);
        if (jsHost.configureWindow &&
            napi_call_threadsafe_function(jsHost.configureWindow, value.get(), napi_tsfn_nonblocking) == napi_ok)
            (void)value.release(); // NOLINT(bugprone-unused-return-value) - The queued callback owns the payload.
    };
    if (executeWindowCommand)
    {
        callbacks.executeWindowCommand = [](const sf::priv::Harmony::WindowCommand& command, void*)
        {
            auto value = std::make_unique<sf::priv::Harmony::WindowCommand>(command);

            const std::lock_guard lock(jsHost.mutex);
            if (jsHost.executeWindowCommand &&
                napi_call_threadsafe_function(jsHost.executeWindowCommand, value.get(), napi_tsfn_nonblocking) == napi_ok)
            {
                (void)value.release(); // NOLINT(bugprone-unused-return-value) - The queued callback owns the payload.
                return true;
            }
            return false;
        };
    }
    callbacks.requestExit = [](void*)
    {
        const std::lock_guard lock(jsHost.mutex);
        if (jsHost.requestExit)
            (void)napi_call_threadsafe_function(jsHost.requestExit, nullptr, napi_tsfn_nonblocking);
    };
    sf::priv::Harmony::setHostCallbacks(callbacks);
    return true;
}


void releaseHostResources()
{
    sf::priv::Harmony::cancelHostInitialization();

    sf::priv::Harmony::ResourceManagerRegistration registration{};
    {
        const std::lock_guard lock(resourceMutex);
        resourceShutdown = true;
        resourceManager  = nullptr;
        registration     = std::exchange(resourceManagerRegistration, {});
    }

    // Platform teardown remains outside every lifecycle mutex. In-flight IME
    // Attach observes its canceled generation and detaches itself on return.
    sf::priv::Harmony::shutdownInputMethod();
    releaseJsHostCallbacks();

    // The final stream lease can release the native manager synchronously.
    // Keep platform destruction outside the host-state mutex.
    if (registration)
        (void)sf::priv::Harmony::unregisterResourceManager(registration);
}


void rollbackHostInitialization(sf::priv::Harmony::HostInitializationToken     hostToken,
                                sf::priv::Harmony::ResourceManagerRegistration resourceRegistration,
                                bool                                           shutdownInputMethod)
{
    if (shutdownInputMethod)
        sf::priv::Harmony::shutdownInputMethod();
    releaseJsHostCallbacks();

    sf::priv::Harmony::ResourceManagerRegistration retiredRegistration{};
    {
        const std::lock_guard lock(resourceMutex);
        if (resourceRegistration && resourceManagerRegistration == resourceRegistration)
        {
            resourceManager     = nullptr;
            retiredRegistration = std::exchange(resourceManagerRegistration, {});
        }
    }

    if (retiredRegistration)
        (void)sf::priv::Harmony::unregisterResourceManager(retiredRegistration);
    sf::priv::Harmony::abortHostInitialization(hostToken);
}


napi_value makeBoolean(napi_env env, bool value)
{
    napi_value result{};
    napi_get_boolean(env, value, &result);
    return result;
}


bool readWindowState(napi_env env, const napi_value* arguments, sf::priv::Harmony::WindowState& state)
{
    std::int32_t  x{};
    std::int32_t  y{};
    std::uint32_t width{};
    std::uint32_t height{};
    if (napi_get_value_int32(env, arguments[0], &x) != napi_ok || napi_get_value_int32(env, arguments[1], &y) != napi_ok ||
        napi_get_value_uint32(env, arguments[2], &width) != napi_ok ||
        napi_get_value_uint32(env, arguments[3], &height) != napi_ok ||
        napi_get_value_bool(env, arguments[4], &state.visible) != napi_ok ||
        napi_get_value_bool(env, arguments[5], &state.focused) != napi_ok ||
        napi_get_value_bool(env, arguments[6], &state.fullscreen) != napi_ok)
        return false;

    state.position   = {x, y};
    state.clientSize = {width, height};
    return true;
}


void runMain()
{
    std::array<char, 5>  name{'s', 'f', 'm', 'l', '\0'};
    std::array<char*, 2> arguments{name.data(), nullptr};
    try
    {
        (void)sfmlMain(1, arguments.data());
    } catch (const std::exception& exception)
    {
        sf::err() << "Unhandled exception from sfmlMain: " << exception.what() << std::endl;
    } catch (...)
    {
        sf::err() << "Unhandled non-standard exception from sfmlMain" << std::endl;
    }

    sf::priv::Harmony::HostCallbacks callbacks;
    bool                             destroyed = false;
    {
        auto&                 host = sf::priv::Harmony::getHostState();
        const std::lock_guard lock(host.mutex);
        callbacks = host.callbacks;
        destroyed = host.destroyed;
    }
    if (!destroyed && callbacks.requestExit)
        callbacks.requestExit(callbacks.userData);

    sf::priv::Harmony::notifyMainFinished();
}


napi_value initializeHost(napi_env env, napi_callback_info info)
{
    std::array<napi_value, 4> arguments{};
    std::size_t               count = arguments.size();
    if (napi_get_cb_info(env, info, &count, arguments.data(), nullptr, nullptr) != napi_ok || count < 4)
        return makeBoolean(env, false);

    std::int32_t windowId = 0;
    if (napi_get_value_int32(env, arguments[1], &windowId) != napi_ok || windowId <= 0)
    {
        sf::err() << "Harmony initializeHost requires a valid Stage windowId" << std::endl;
        return makeBoolean(env, false);
    }

    const auto hostToken = sf::priv::Harmony::beginHostInitialization();
    if (!hostToken)
    {
        sf::err() << "Harmony host initialization is already running or the host was destroyed" << std::endl;
        return makeBoolean(env, false);
    }

    if (!installJsHostCallbacks(env, arguments[3]))
    {
        sf::priv::Harmony::abortHostInitialization(hostToken);
        sf::err() << "Harmony initializeHost requires pointer and keyboard host callbacks" << std::endl;
        return makeBoolean(env, false);
    }

    NativeResourceManager* next = OH_ResourceManager_InitNativeResourceManager(env, arguments[0]);
    if (!next)
    {
        rollbackHostInitialization(hostToken, {}, false);
        sf::err() << "Failed to initialize the Harmony native resource manager" << std::endl;
        return makeBoolean(env, false);
    }

    sf::priv::Harmony::ResourceManagerRegistration initializationRegistration{};
    bool                                           releaseNext{};
    bool                                           failed{};
    bool                                           rejectedAfterShutdown{};
    {
        const std::lock_guard lock(resourceMutex);
        if (resourceShutdown)
        {
            releaseNext           = true;
            failed                = true;
            rejectedAfterShutdown = true;
        }
        else if (resourceManager)
        {
            // initializeHost is idempotent for the lifetime of the one native
            // SFML main thread.  Replacing the manager while streams are open
            // would invalidate them, so retain the first successful handle.
            releaseNext = true;
        }
        else
        {
            const auto registration = sf::priv::Harmony::registerOwnedResourceManager(next);
            if (!registration)
            {
                releaseNext = true;
                failed      = true;
            }
            else
            {
                resourceManager             = next;
                resourceManagerRegistration = registration;
                initializationRegistration  = registration;
            }
        }
    }

    if (releaseNext)
        OH_ResourceManager_ReleaseNativeResourceManager(next);

    if (failed)
    {
        rollbackHostInitialization(hostToken, initializationRegistration, false);
        if (rejectedAfterShutdown)
            sf::err() << "Cannot initialize the Harmony resource manager after host shutdown" << std::endl;
        else
            sf::err() << "Failed to retain the Harmony native resource manager" << std::endl;
        return makeBoolean(env, false);
    }

    // This is the Attach linearization point. If destruction has already
    // invalidated the transaction, no platform IME function is called.
    if (!sf::priv::Harmony::authorizeInputMethodAttach(hostToken))
    {
        rollbackHostInitialization(hostToken, initializationRegistration, true);
        sf::err() << "Harmony host initialization was canceled before IME attach" << std::endl;
        return makeBoolean(env, false);
    }

    // Attaching the platform IME does not hold the host lifecycle mutex. Its
    // generation is canceled if destroy wins while Attach is in progress.
    const bool inputMethodAvailable = sf::priv::Harmony::initializeInputMethod(windowId);

    // Publish the Stage ID before commit can start sfmlMain. The 2-in-1
    // backend uses it for native property queries and cursor confinement.
    {
        auto&                 host = sf::priv::Harmony::getHostState();
        const std::lock_guard lock(host.mutex);
        if (!host.destroyed)
            host.windowId = windowId;
    }

    if (!sf::priv::Harmony::commitHostInitialization(hostToken))
    {
        rollbackHostInitialization(hostToken, initializationRegistration, true);
        sf::err() << "Harmony host initialization was canceled by destruction" << std::endl;
        return makeBoolean(env, false);
    }

    // deviceType remains a stable host ABI field. The authoritative device
    // form is also checked with OH_GetDeviceType when XComponent registers.
    if (!inputMethodAvailable)
        sf::err() << "Harmony IME is unavailable; hardware keyboard input remains active" << std::endl;

    return makeBoolean(env, true);
}


napi_value onForeground(napi_env env, napi_callback_info)
{
    sf::priv::Harmony::notifyForeground();
    return makeBoolean(env, true);
}


napi_value completeWindowCommand(napi_env env, napi_callback_info info)
{
    std::array<napi_value, 9> arguments{};
    std::size_t               count = arguments.size();
    if (napi_get_cb_info(env, info, &count, arguments.data(), nullptr, nullptr) != napi_ok || count != arguments.size())
        return makeBoolean(env, false);

    std::uint32_t                  requestId{};
    bool                           success{};
    sf::priv::Harmony::WindowState state;
    if (napi_get_value_uint32(env, arguments[0], &requestId) != napi_ok || requestId == 0 ||
        napi_get_value_bool(env, arguments[1], &success) != napi_ok || !readWindowState(env, arguments.data() + 2, state))
        return makeBoolean(env, false);

    return makeBoolean(env, sf::priv::Harmony::completeWindowCommand(requestId, success, state));
}


napi_value isWindowCommandPending(napi_env env, napi_callback_info info)
{
    napi_value    argument{};
    std::size_t   count = 1;
    std::uint32_t requestId{};
    if (napi_get_cb_info(env, info, &count, &argument, nullptr, nullptr) != napi_ok || count != 1 ||
        napi_get_value_uint32(env, argument, &requestId) != napi_ok || requestId == 0)
        return makeBoolean(env, false);

    return makeBoolean(env, sf::priv::Harmony::isWindowCommandPending(requestId));
}


napi_value updateWindowState(napi_env env, napi_callback_info info)
{
    std::array<napi_value, 7>      arguments{};
    std::size_t                    count = arguments.size();
    sf::priv::Harmony::WindowState state;
    if (napi_get_cb_info(env, info, &count, arguments.data(), nullptr, nullptr) != napi_ok ||
        count != arguments.size() || !readWindowState(env, arguments.data(), state))
        return makeBoolean(env, false);

    sf::priv::Harmony::updateWindowState(state);
    return makeBoolean(env, true);
}


napi_value onBackground(napi_env env, napi_callback_info)
{
    sf::priv::Harmony::notifyBackground();
    return makeBoolean(env, true);
}


napi_value onXComponentDestroy(napi_env env, napi_callback_info info)
{
    std::size_t count = 0;
    void*       component{};
    if (napi_get_cb_info(env, info, &count, nullptr, nullptr, &component) != napi_ok || !component)
        return makeBoolean(env, false);

    // This callback carries the exact component pointer captured when its
    // NAPI exports were created. A delayed destroy from an old ArkUI node can
    // therefore never unregister a newer replacement component.
    sf::priv::Harmony::unregisterNativeXComponent(component);
    return makeBoolean(env, true);
}


napi_value onDestroy(napi_env env, napi_callback_info)
{
    // HostState invalidates any prepare/attach token before platform cleanup.
    // An Attach already in progress is not waited on; it observes cancellation
    // before publishing its proxy and tears itself down on return.
    sf::priv::Harmony::notifyDestroy();

    {
        // The manager lease itself remains alive until releaseHostResources,
        // allowing a running sfmlMain to consume Closed and exit without
        // blocking ArkUI. New Host transactions are irreversibly rejected.
        const std::lock_guard lock(resourceMutex);
        resourceShutdown = true;
    }

    sf::priv::Harmony::shutdownInputMethod();
    releaseJsHostCallbacks();
    return makeBoolean(env, true);
}


napi_value submitText(napi_env env, napi_callback_info info)
{
    napi_value  argument{};
    std::size_t count = 1;
    if (napi_get_cb_info(env, info, &count, &argument, nullptr, nullptr) != napi_ok || count != 1)
        return makeBoolean(env, false);

    std::size_t size = 0;
    if (napi_get_value_string_utf8(env, argument, nullptr, 0, &size) != napi_ok)
        return makeBoolean(env, false);

    std::vector<char> utf8(size + 1);
    if (napi_get_value_string_utf8(env, argument, utf8.data(), utf8.size(), &size) != napi_ok)
        return makeBoolean(env, false);

    const auto text = sf::String::fromUtf8(utf8.data(), utf8.data() + size);
    for (const char32_t codepoint : text)
        sf::priv::Harmony::submitText(codepoint);
    return makeBoolean(env, true);
}


napi_value submitKeyText(napi_env env, napi_callback_info info)
{
    {
        auto&                 host = sf::priv::Harmony::getHostState();
        const std::lock_guard lock(host.mutex);
        // Newer runtimes deliver Unicode in the native ArkUI key callback.
        // The ArkTS keyText bridge is only needed for the API21-compatible
        // legacy XComponent callback, otherwise it would duplicate text.
        if (!host.legacyKeyCallback || host.destroyed)
            return makeBoolean(env, true);
    }

    return submitText(env, info);
}


napi_value submitTextEdit(napi_env env, napi_callback_info info)
{
    constexpr std::uint32_t maxEditLength = 8 * 1024;

    std::array<napi_value, 3> arguments{};
    std::size_t               count = arguments.size();
    if (napi_get_cb_info(env, info, &count, arguments.data(), nullptr, nullptr) != napi_ok || count != arguments.size())
        return makeBoolean(env, false);

    std::uint32_t backwardDeletions{};
    std::uint32_t forwardDeletions{};
    if (napi_get_value_uint32(env, arguments[0], &backwardDeletions) != napi_ok ||
        napi_get_value_uint32(env, arguments[1], &forwardDeletions) != napi_ok || backwardDeletions > maxEditLength ||
        forwardDeletions > maxEditLength)
        return makeBoolean(env, false);

    std::size_t size = 0;
    if (napi_get_value_string_utf8(env, arguments[2], nullptr, 0, &size) != napi_ok || size > maxEditLength * 4)
        return makeBoolean(env, false);

    std::vector<char> utf8(size + 1);
    if (napi_get_value_string_utf8(env, arguments[2], utf8.data(), utf8.size(), &size) != napi_ok)
        return makeBoolean(env, false);

    const auto text  = sf::String::fromUtf8(utf8.data(), utf8.data() + size);
    const auto utf32 = text.toUtf32();
    if (utf32.size() > maxEditLength)
        return makeBoolean(env, false);

    // The native proxy and this compatibility bridge are mutually exclusive.
    // A true result also covers the no-op edit; false means native IME owns
    // the event stream and ArkTS must not duplicate it.
    return makeBoolean(env, sf::priv::Harmony::submitFallbackTextEdit(backwardDeletions, forwardDeletions, utf32));
}

} // namespace


namespace sf::priv::Harmony
{
napi_value initializeNativeApp(napi_env env, napi_value exports)
{
    installHarmonyErrorStream();

    OH_NativeXComponent* registeredComponent = nullptr;
    napi_value           xcomponentValue{};
    if (napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &xcomponentValue) == napi_ok)
    {
        OH_NativeXComponent* component = nullptr;
        if (napi_unwrap(env, xcomponentValue, reinterpret_cast<void**>(&component)) == napi_ok && component)
        {
            if (registerNativeXComponent(component))
                registeredComponent = component;
            else
                sf::err() << "SFML Harmony XComponent initialization failed; registration remains retryable" << std::endl;
        }
        else
        {
            sf::err() << "Failed to unwrap the SFML Harmony Native XComponent" << std::endl;
        }
    }
    else
    {
        sf::err() << "The SFML Harmony NAPI module did not receive a Native XComponent" << std::endl;
    }

    const std::array properties{
        napi_property_descriptor{"initializeHost", nullptr, ::initializeHost, nullptr, nullptr, nullptr, napi_default, nullptr},
        napi_property_descriptor{"onForeground", nullptr, ::onForeground, nullptr, nullptr, nullptr, napi_default, nullptr},
        napi_property_descriptor{"onBackground", nullptr, ::onBackground, nullptr, nullptr, nullptr, napi_default, nullptr},
        napi_property_descriptor{"completeWindowCommand", nullptr, ::completeWindowCommand, nullptr, nullptr, nullptr, napi_default, nullptr},
        napi_property_descriptor{"isWindowCommandPending", nullptr, ::isWindowCommandPending, nullptr, nullptr, nullptr, napi_default, nullptr},
        napi_property_descriptor{"updateWindowState", nullptr, ::updateWindowState, nullptr, nullptr, nullptr, napi_default, nullptr},
        napi_property_descriptor{"onXComponentDestroy", nullptr, ::onXComponentDestroy, nullptr, nullptr, nullptr, napi_default, registeredComponent},
        napi_property_descriptor{"onDestroy", nullptr, ::onDestroy, nullptr, nullptr, nullptr, napi_default, nullptr},
        napi_property_descriptor{"submitText", nullptr, ::submitText, nullptr, nullptr, nullptr, napi_default, nullptr},
        napi_property_descriptor{"submitKeyText", nullptr, ::submitKeyText, nullptr, nullptr, nullptr, napi_default, nullptr},
        napi_property_descriptor{"submitTextEdit", nullptr, ::submitTextEdit, nullptr, nullptr, nullptr, napi_default, nullptr}};

    if (napi_define_properties(env, exports, properties.size(), properties.data()) != napi_ok)
        sf::err() << "Failed to define the SFML Harmony NAPI exports" << std::endl;

    sf::priv::Harmony::setShutdownCallback(releaseHostResources);
    sf::priv::Harmony::setMainEntry(runMain);
    return exports;
}

} // namespace sf::priv::Harmony
