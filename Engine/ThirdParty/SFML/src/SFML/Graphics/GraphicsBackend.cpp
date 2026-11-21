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

#include <SFML/Graphics/GraphicsDevice.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <array>
#include <mutex>
#include <optional>
#include <ostream>
#include <type_traits>
#include <utility>

namespace sf
{
namespace
{
namespace GraphicsBackendImpl
{
constexpr std::array drivers{
#ifdef SFML_SYSTEM_WINDOWS
#ifdef SFML_RENDER_DIRECTX
    GraphicsBackend::DirectX,
#endif
#elif defined(SFML_SYSTEM_MACOS) || defined(SFML_SYSTEM_IOS)
#ifdef SFML_RENDER_METAL
    GraphicsBackend::Metal,
#endif
#endif
#ifdef SFML_RENDER_OPENGL
    GraphicsBackend::OpenGL,
#endif
#ifdef SFML_RENDER_OPENGLES
    GraphicsBackend::OpenGLES,
#endif
#ifdef SFML_RENDER_VULKAN
    GraphicsBackend::Vulkan,
#endif
    GraphicsBackend::Automatic};

using Settings = ContextSettings::BackendSettings;

struct Registry
{
    std::recursive_mutex                mutex;
    GraphicsBackend                     requested{GraphicsBackend::Automatic};
    Settings                            settings;
    std::optional<priv::GraphicsDevice> device;
    bool                                committed{};
};

Registry& registry()
{
    static Registry value;
    return value;
}

const char* name(GraphicsBackend backend)
{
    switch (backend)
    {
        case GraphicsBackend::OpenGL:
            return "OpenGL";
        case GraphicsBackend::OpenGLES:
            return "OpenGL ES";
        case GraphicsBackend::Vulkan:
            return "Vulkan";
        case GraphicsBackend::DirectX:
            return "DirectX";
        case GraphicsBackend::Metal:
            return "Metal";
        case GraphicsBackend::Automatic:
            return "Automatic";
    }
    return "Unknown";
}

GraphicsBackend backendOf(const Settings& settings)
{
    if (std::holds_alternative<OpenGLSettings>(settings))
        return GraphicsBackend::OpenGL;
    if (std::holds_alternative<OpenGLESSettings>(settings))
        return GraphicsBackend::OpenGLES;
    if (std::holds_alternative<VulkanSettings>(settings))
        return GraphicsBackend::Vulkan;
    if (std::holds_alternative<DirectXSettings>(settings))
        return GraphicsBackend::DirectX;
    if (std::holds_alternative<MetalSettings>(settings))
        return GraphicsBackend::Metal;
    return GraphicsBackend::Automatic;
}

std::pair<unsigned int, unsigned int> versionOf(const Settings& settings)
{
    return std::visit(
        [](const auto& value) -> std::pair<unsigned int, unsigned int>
        {
            using Type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<Type, std::monostate> || std::is_same_v<Type, MetalSettings>)
                return {};
            else
                return {value.majorVersion, value.minorVersion};
        },
        settings);
}

Settings defaults(GraphicsBackend backend)
{
    switch (backend)
    {
        case GraphicsBackend::OpenGL:
            return OpenGLSettings{};
        case GraphicsBackend::OpenGLES:
            return OpenGLESSettings{};
        case GraphicsBackend::Vulkan:
            return VulkanSettings{};
        case GraphicsBackend::DirectX:
            return DirectXSettings{};
        case GraphicsBackend::Metal:
            return MetalSettings{};
        default:
            return {};
    }
}

bool valid(GraphicsBackend backend, const Settings& settings)
{
    if (const auto* directX = std::get_if<DirectXSettings>(&settings))
        return backend == GraphicsBackend::DirectX && directX->majorVersion >= 10 && directX->majorVersion <= 12 &&
               directX->minorVersion <= 1;
    return std::holds_alternative<std::monostate>(settings) ||
           (backendOf(settings) == backend && versionOf(settings) >= versionOf(defaults(backend)));
}

bool compatibleProfiles(const Settings& first, const Settings& second)
{
    const auto* lhs = std::get_if<OpenGLSettings>(&first);
    const auto* rhs = std::get_if<OpenGLSettings>(&second);
    return !lhs || !rhs || lhs->profile == OpenGLProfile::Automatic || rhs->profile == OpenGLProfile::Automatic ||
           lhs->profile == rhs->profile;
}

bool supports(const priv::GraphicsDevice& device, GraphicsBackend backend, const Settings& requested)
{
    if (backend != GraphicsBackend::Automatic && backend != device.backend)
        return false;

    if (std::holds_alternative<std::monostate>(requested))
        return true;

    const auto* requestGl = std::get_if<OpenGLSettings>(&requested);
    const auto* actualGl  = std::get_if<OpenGLSettings>(&device.settings);
    return backendOf(requested) == device.backend && versionOf(device.settings) >= versionOf(requested) &&
           compatibleProfiles(requested, device.settings) && (!requestGl || !requestGl->debug || actualGl->debug);
}

void reportFailure(GraphicsBackend backend, const Settings& settings)
{
    const auto [major, minor] = versionOf(settings);
    err() << "Could not initialize or use graphics backend " << name(backend);
    if (major)
        err() << ' ' << major << '.' << minor;
    err() << std::endl;
}

bool isBuilt(GraphicsBackend backend)
{
    return std::find(drivers.begin(), drivers.end(), backend) != drivers.end();
}

Settings mergeRequirements(const Settings& startup, const Settings& target)
{
    Settings merged = versionOf(startup) >= versionOf(target) ? startup : target;
    if (auto* gl = std::get_if<OpenGLSettings>(&merged))
    {
        for (const auto* requirements : {&startup, &target})
        {
            if (const auto* requested = std::get_if<OpenGLSettings>(requirements))
            {
                if (requested->profile != OpenGLProfile::Automatic)
                    gl->profile = requested->profile;
                gl->debug |= requested->debug;
            }
        }
    }
    return merged;
}
} // namespace GraphicsBackendImpl
} // namespace

bool setGraphicsBackend(GraphicsBackend backend, const ContextSettings::BackendSettings& settings)
{
    using namespace GraphicsBackendImpl;
    auto&                  state = registry();
    const std::scoped_lock lock(state.mutex);

    if (!isBuilt(backend) || !valid(backend, settings) || (state.device && !supports(*state.device, backend, settings)))
    {
        reportFailure(backend, settings);
        return false;
    }

    if (!state.device)
    {
        state.requested = backend;
        state.settings  = settings;
    }

    return true;
}

GraphicsBackend getGraphicsBackend()
{
    auto&                  state = GraphicsBackendImpl::registry();
    const std::scoped_lock lock(state.mutex);
    return state.committed ? state.device->backend : GraphicsBackend::Automatic;
}

bool loadPipelineCacheFromMemory(const void* data, std::size_t size)
{
    if (!data || !size)
        return false;
    const auto device = rhi::getDevice();
    return device && device->loadPipelineCache(data, size);
}

std::optional<std::vector<std::uint8_t>> getPipelineCacheData()
{
    const auto device = rhi::getDevice();
    return device ? device->getPipelineCacheData() : std::nullopt;
}

namespace priv
{
bool withGraphicsDevice(const ContextSettings&                                                    settings,
                        const std::function<bool(const GraphicsDevice&, const ContextSettings&)>& operation)
{
    using namespace GraphicsBackendImpl;
    auto&            state = registry();
    std::unique_lock lock(state.mutex);

    const auto targetBackend = backendOf(settings.backend);
    if (!valid(targetBackend, settings.backend) ||
        (state.requested != GraphicsBackend::Automatic && targetBackend != GraphicsBackend::Automatic &&
         state.requested != targetBackend) ||
        !compatibleProfiles(state.settings, settings.backend))
    {
        reportFailure(targetBackend, settings.backend);
        return false;
    }

    auto effective = settings;
    if (state.device)
    {
        if (!supports(*state.device, targetBackend, settings.backend))
        {
            const auto [major, minor] = versionOf(state.device->settings);
            reportFailure(targetBackend, settings.backend);
            err() << "Active graphics device is " << name(state.device->backend) << ' ' << major << '.' << minor
                  << std::endl;
            return false;
        }

        const auto& device = *state.device;
        effective.backend  = device.settings;
        if (state.committed)
            lock.unlock();
        return operation(device, effective);
    }

    const auto requested = targetBackend == GraphicsBackend::Automatic ? state.requested : targetBackend;
    for (const auto driver : drivers)
    {
        if (driver == GraphicsBackend::Automatic || (requested != GraphicsBackend::Automatic && driver != requested))
            continue;

        auto requirements = mergeRequirements(state.settings, settings.backend);
        if (std::holds_alternative<std::monostate>(requirements))
            requirements = defaults(driver);

        auto initialization    = ContextSettings{};
        initialization.backend = requirements;
        std::shared_ptr<rhi::Device> implementation;
        switch (driver)
        {
#ifdef SFML_RENDER_DIRECTX
            case GraphicsBackend::DirectX:
                implementation = createDirectXRhi(std::get<DirectXSettings>(requirements));
                break;
#endif
#ifdef SFML_RENDER_METAL
            case GraphicsBackend::Metal:
                implementation = createMetalRhi();
                break;
#endif
#ifdef SFML_RENDER_VULKAN
            case GraphicsBackend::Vulkan:
                implementation = createVulkanRhi(std::get<VulkanSettings>(requirements));
                break;
#endif
#ifdef SFML_RENDER_GL
            case GraphicsBackend::OpenGL:
            case GraphicsBackend::OpenGLES:
                implementation = createOpenGLRhi(initialization);
                break;
#endif
            default:
                break;
        }
        if (!implementation)
        {
            reportFailure(driver, requirements);
            continue;
        }
        GraphicsDevice device{driver, implementation->settings(), std::move(implementation)};

        effective.backend = device.settings;
        state.device      = std::move(device);
        try
        {
            if (operation(*state.device, effective))
            {
                state.committed = true;
                return true;
            }
        } catch (...)
        {
            state.device.reset();
            throw;
        }
        state.device.reset();
        reportFailure(driver, requirements);
    }

    if (requested != GraphicsBackend::Automatic && !isBuilt(requested))
        reportFailure(requested, settings.backend);
    return false;
}

const GraphicsDevice* getGraphicsDevice()
{
    const GraphicsDevice* device = nullptr;
    (void)withGraphicsDevice({},
                             [&](const GraphicsDevice& value, const ContextSettings&)
                             {
                                 device = &value;
                                 return true;
                             });
    return device;
}
} // namespace priv
} // namespace sf
