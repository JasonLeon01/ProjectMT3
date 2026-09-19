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
#include <SFML/Window/VulkanImpl.hpp>

#include <dlfcn.h>
#include <mutex>
#include <native_window/external_window.h>
#include <string_view>
#include <vector>

#include <cstdint>

#define VK_USE_PLATFORM_OHOS
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>


namespace
{
class NativeWindowReference
{
public:
    explicit NativeWindowReference(OHNativeWindow* window) : m_window(window)
    {
    }

    ~NativeWindowReference()
    {
        (void)OH_NativeWindow_NativeObjectUnreference(m_window);
    }

    NativeWindowReference(const NativeWindowReference&)            = delete;
    NativeWindowReference& operator=(const NativeWindowReference&) = delete;

private:
    OHNativeWindow* m_window;
};


struct VulkanLibrary
{
    ~VulkanLibrary()
    {
        if (handle)
            dlclose(handle);
    }

    bool load()
    {
        if (handle)
            return true;

        handle = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
        if (!handle)
            return false;

        getInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(handle, "vkGetInstanceProcAddr"));
        enumerateExtensions = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
            dlsym(handle, "vkEnumerateInstanceExtensionProperties"));
        if (!getInstanceProcAddr || !enumerateExtensions)
        {
            dlclose(handle);
            handle = nullptr;
            return false;
        }
        return true;
    }

    void*                                      handle{};
    PFN_vkGetInstanceProcAddr                  getInstanceProcAddr{};
    PFN_vkEnumerateInstanceExtensionProperties enumerateExtensions{};
};

VulkanLibrary library;
} // namespace


namespace sf::priv::VulkanImpl
{
bool isAvailable(bool requireGraphics)
{
    static const bool computeAvailable  = library.load();
    static const bool graphicsAvailable = []
    {
        if (!computeAvailable)
            return false;

        std::uint32_t count = 0;
        if (library.enumerateExtensions(nullptr, &count, nullptr) != VK_SUCCESS)
            return false;

        std::vector<VkExtensionProperties> extensions(count);
        if (library.enumerateExtensions(nullptr, &count, extensions.data()) != VK_SUCCESS)
            return false;

        bool surface = false;
        bool ohos    = false;
        for (const auto& extension : extensions)
        {
            const std::string_view name(extension.extensionName);
            surface |= name == VK_KHR_SURFACE_EXTENSION_NAME;
            ohos |= name == VK_OHOS_SURFACE_EXTENSION_NAME;
        }
        return surface && ohos;
    }();

    return requireGraphics ? graphicsAvailable : computeAvailable;
}


VulkanFunctionPointer getFunction(const char* name)
{
    if (!isAvailable(false))
        return nullptr;
    return reinterpret_cast<VulkanFunctionPointer>(dlsym(library.handle, name));
}


const std::vector<const char*>& getGraphicsRequiredInstanceExtensions()
{
    static const std::vector<const char*> extensions{VK_KHR_SURFACE_EXTENSION_NAME, VK_OHOS_SURFACE_EXTENSION_NAME};
    return extensions;
}


bool createVulkanSurface(const VkInstance&            instance,
                         WindowHandle                 windowHandle,
                         VkSurfaceKHR&                surface,
                         const VkAllocationCallbacks* allocator)
{
    if (!isAvailable() || !windowHandle)
        return false;

    const auto createSurface = reinterpret_cast<PFN_vkCreateSurfaceOHOS>(
        library.getInstanceProcAddr(instance, "vkCreateSurfaceOHOS"));
    if (!createSurface)
        return false;

    OHNativeWindow* nativeWindow{};
    {
        // The XComponent may replace or destroy its native window on the UI
        // thread. Validate the public handle and retain the exact live object
        // while vkCreateSurfaceOHOS consumes it.
        auto&                 host = sf::priv::Harmony::getHostState();
        const std::lock_guard lock(host.mutex);
        if (host.window != windowHandle || !host.size.x || !host.size.y)
            return false;

        nativeWindow = host.window;
        if (OH_NativeWindow_NativeObjectReference(nativeWindow) != 0)
            return false;
    }
    const NativeWindowReference reference(nativeWindow);

    VkSurfaceCreateInfoOHOS createInfo{};
    createInfo.sType  = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_OHOS;
    createInfo.window = nativeWindow;
    return createSurface(instance, &createInfo, allocator, &surface) == VK_SUCCESS;
}

} // namespace sf::priv::VulkanImpl
