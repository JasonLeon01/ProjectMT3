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

#include <SFML/Window/VulkanImpl.hpp>

#include <dlfcn.h>

#define VK_USE_PLATFORM_ANDROID_KHR
#define VK_NO_PROTOTYPES
#include <algorithm>
#include <string_view>
#include <vulkan.h>

namespace sf::priv::VulkanImpl
{
namespace
{
void* getVulkanAndroidLoader()
{
    static void* const loader = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    return loader;
}
} // namespace

VulkanFunctionPointer getFunction(const char* name)
{
    auto* const loader = getVulkanAndroidLoader();
    return loader ? reinterpret_cast<VulkanFunctionPointer>(dlsym(loader, name)) : nullptr;
}

const std::vector<const char*>& getGraphicsRequiredInstanceExtensions()
{
    static const std::vector<const char*> extensions{VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_ANDROID_SURFACE_EXTENSION_NAME};
    return extensions;
}

bool isAvailable(bool requireGraphics)
{
    const auto getProc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(getFunction("vkGetInstanceProcAddr"));
    if (!getProc || !requireGraphics)
        return getProc != nullptr;

    static const bool supported = [getProc]
    {
        const auto enumerate = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
            getProc(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties"));
        std::uint32_t count{};
        if (!enumerate || enumerate(nullptr, &count, nullptr) != VK_SUCCESS)
            return false;
        std::vector<VkExtensionProperties> extensions(count);
        if (enumerate(nullptr, &count, extensions.data()) != VK_SUCCESS)
            return false;

        for (const auto* required : getGraphicsRequiredInstanceExtensions())
            if (std::none_of(extensions.begin(),
                             extensions.end(),
                             [&](const auto& ext) { return std::string_view(ext.extensionName) == required; }))
                return false;

        return true;
    }();

    return supported;
}

bool createVulkanSurface(const VkInstance& instance, WindowHandle window, VkSurfaceKHR& surface, const VkAllocationCallbacks* allocator)
{
    const auto getProc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(getFunction("vkGetInstanceProcAddr"));
    if (!getProc || !window)
        return false;

    const auto create = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(getProc(instance, "vkCreateAndroidSurfaceKHR"));
    if (!create)
        return false;

    VkAndroidSurfaceCreateInfoKHR info{};
    info.sType  = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    info.window = static_cast<ANativeWindow*>(window);
    return create(instance, &info, allocator, &surface) == VK_SUCCESS;
}
} // namespace sf::priv::VulkanImpl
