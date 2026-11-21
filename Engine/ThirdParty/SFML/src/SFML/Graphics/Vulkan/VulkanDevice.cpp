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

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Vulkan/VulkanDevice.hpp>
#include <SFML/Graphics/Vulkan/VulkanProgram.hpp>

#include <SFML/Window/Vulkan.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <array>
#include <limits>
#include <optional>
#include <ostream>
#include <string_view>
#include <utility>

#include <cstring>

namespace sf::priv
{
namespace
{
namespace VulkanDeviceImpl
{
constexpr auto uploadUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

template <typename T, typename Query>
std::vector<T> enumerate(Query query)
{
    std::uint32_t count{};
    if (query(&count, static_cast<T*>(nullptr)) != VK_SUCCESS || !count)
        return {};

    std::vector<T> values(count);
    if (query(&count, values.data()) != VK_SUCCESS)
        return {};
    values.resize(count);
    return values;
}

std::optional<std::uint32_t> memoryType(const VulkanContext&  context,
                                        std::uint32_t         allowed,
                                        VkMemoryPropertyFlags required,
                                        VkMemoryPropertyFlags preferred)
{
    std::optional<std::uint32_t> fallback;
    for (std::uint32_t index = 0; index < context.memory.memoryTypeCount; ++index)
    {
        const auto flags = context.memory.memoryTypes[index].propertyFlags;
        if (!(allowed & (1u << index)) || (flags & required) != required)
            continue;
        if ((flags & preferred) == preferred)
            return index;
        if (!fallback)
            fallback = index;
    }
    return fallback;
}

VkImageView makeView(const VulkanContext& context, const VulkanImage& image, std::uint32_t levels)
{
    VkImageViewCreateInfo info{};
    info.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image            = image.image;
    info.viewType         = VK_IMAGE_VIEW_TYPE_2D;
    info.format           = image.format;
    info.subresourceRange = {image.aspect, 0, levels, 0, 1};
    VkImageView result{};
    if (context.vk.CreateImageView(context.device, &info, nullptr, &result) != VK_SUCCESS)
        return VK_NULL_HANDLE;
    return result;
}

struct Access
{
    VkPipelineStageFlags stage;
    VkAccessFlags        flags;
};

Access accessFor(VkImageLayout layout)
{
    switch (layout)
    {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT};
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return {VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT};
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return {VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_ACCESS_SHADER_READ_BIT};
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return {VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT};
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return {VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT};
        default:
            return {VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0};
    }
}

void barrier(const VulkanContext& context,
             VkCommandBuffer      command,
             const VulkanImage&   image,
             VkImageLayout        from,
             VkImageLayout        to,
             std::uint32_t        baseLevel,
             std::uint32_t        levels)
{
    const auto           src = accessFor(from);
    const auto           dst = accessFor(to);
    VkImageMemoryBarrier info{};
    info.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    info.srcAccessMask       = src.flags;
    info.dstAccessMask       = dst.flags;
    info.oldLayout           = from;
    info.newLayout           = to;
    info.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    info.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    info.image               = image.image;
    info.subresourceRange    = {image.aspect, baseLevel, levels, 0, 1};
    context.vk.CmdPipelineBarrier(command, src.stage, dst.stage, 0, 0, nullptr, 0, nullptr, 1, &info);
}
} // namespace VulkanDeviceImpl
} // namespace

struct VulkanDevice::Impl
{
    struct UploadBlock
    {
        std::shared_ptr<VulkanBuffer> buffer;
        VkDeviceSize                  used{};
    };

    explicit Impl(std::shared_ptr<VulkanContext> value) : context(std::move(value))
    {
    }

    std::shared_ptr<VulkanContext>     context;
    mutable std::recursive_mutex       mutex;
    VkCommandPool                      commandPool{};
    VkCommandBuffer                    commandBuffer{};
    VkFence                            fence{};
    bool                               recording{};
    bool                               lost{};
    std::vector<std::shared_ptr<void>> retained;
    std::vector<UploadBlock>           uploads;
    struct DescriptorPool
    {
        VkDescriptorPool handle{};
        std::uint32_t    sets{};
        std::uint32_t    buffers{};
        std::uint32_t    images{};
    };
    std::vector<DescriptorPool>    descriptorPools;
    std::size_t                    currentDescriptorPool{};
    std::array<VkSampler, 8>       samplers{};
    std::shared_ptr<VulkanImage>   white;
    std::shared_ptr<VulkanProgram> program;
};

VulkanContext::~VulkanContext()
{
    if (pipelineCache)
        vk.DestroyPipelineCache(device, pipelineCache, nullptr);
    if (device && vk.DestroyDevice)
        vk.DestroyDevice(device, nullptr);
    if (instance && vk.DestroyInstance)
        vk.DestroyInstance(instance, nullptr);
}

VulkanBuffer::VulkanBuffer(std::shared_ptr<VulkanContext> value) : context(std::move(value))
{
}

VulkanBuffer::~VulkanBuffer()
{
    const auto& vk = context->vk;
    if (mapped)
        vk.UnmapMemory(context->device, memory);
    if (buffer)
        vk.DestroyBuffer(context->device, buffer, nullptr);
    if (memory)
        vk.FreeMemory(context->device, memory, nullptr);
}

void VulkanBuffer::write(VkDeviceSize offset, const void* source, std::size_t bytes) const
{
    std::memcpy(static_cast<std::byte*>(mapped) + offset, source, bytes);
    if (!coherent)
    {
        VkMappedMemoryRange range{};
        range.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range.memory = memory;
        range.size   = VK_WHOLE_SIZE;
        context->vk.FlushMappedMemoryRanges(context->device, 1, &range);
    }
}

void VulkanBuffer::invalidate() const
{
    if (!coherent)
    {
        VkMappedMemoryRange range{};
        range.sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range.memory = memory;
        range.size   = VK_WHOLE_SIZE;
        context->vk.InvalidateMappedMemoryRanges(context->device, 1, &range);
    }
}

VulkanImage::VulkanImage(std::shared_ptr<VulkanContext> value) : context(std::move(value))
{
}

VulkanImage::~VulkanImage()
{
    const auto& vk = context->vk;
    if (attachment && attachment != view)
        vk.DestroyImageView(context->device, attachment, nullptr);
    if (view)
        vk.DestroyImageView(context->device, view, nullptr);
    if (image)
        vk.DestroyImage(context->device, image, nullptr);
    if (memory)
        vk.FreeMemory(context->device, memory, nullptr);
}

std::shared_ptr<VulkanDevice> VulkanDevice::create(const VulkanSettings& requested)
{
    using namespace VulkanDeviceImpl;
    if (requested.majorVersion > 0x7fu || requested.minorVersion > 0x3ffu || !Vulkan::isAvailable())
        return {};

    const auto getProc = reinterpret_cast<PFN_vkGetInstanceProcAddr>(Vulkan::getFunction("vkGetInstanceProcAddr"));
    if (!getProc)
        return {};

    const auto enumerateVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(
        getProc(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"));
    std::uint32_t supported = VK_API_VERSION_1_0;
    if (enumerateVersion && enumerateVersion(&supported) != VK_SUCCESS)
        return {};

    const auto requestedVersion = VK_MAKE_VERSION(requested.majorVersion, requested.minorVersion, 0);
    if (supported < requestedVersion)
    {
        err() << "Vulkan loader supports up to " << VK_VERSION_MAJOR(supported) << '.' << VK_VERSION_MINOR(supported)
              << std::endl;
        return {};
    }

    const auto createInstance = reinterpret_cast<PFN_vkCreateInstance>(getProc(VK_NULL_HANDLE, "vkCreateInstance"));
    if (!createInstance)
        return {};

    auto context        = std::make_shared<VulkanContext>();
    context->apiVersion = requestedVersion;
    VkApplicationInfo application{};
    application.sType       = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application.pEngineName = "SFML";
    application.apiVersion  = requestedVersion;

    auto extensions  = Vulkan::getGraphicsRequiredInstanceExtensions();
    bool properties2 = requestedVersion >= VK_API_VERSION_1_1;
    if (!properties2)
    {
        const auto enumerateExtensions = reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
            getProc(VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties"));
        if (enumerateExtensions)
        {
            const auto available = enumerate<VkExtensionProperties>(
                [&](auto* count, auto* values) { return enumerateExtensions(nullptr, count, values); });
            properties2 = std::any_of(available.begin(),
                                      available.end(),
                                      [](const auto& ext) {
                                          return std::string_view(ext.extensionName) ==
                                                 VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
                                      });
            if (properties2)
                extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        }
    }
    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo        = &application;
    instanceInfo.enabledExtensionCount   = static_cast<std::uint32_t>(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();
    if (createInstance(&instanceInfo, nullptr, &context->instance) != VK_SUCCESS)
        return {};

    auto& vk     = context->vk;
    bool  loaded = true;
#define SFML_VK_INSTANCE(name)                                                        \
    vk.name = reinterpret_cast<PFN_vk##name>(getProc(context->instance, "vk" #name)); \
    loaded  = loaded && vk.name;
#define SFML_VK_DEVICE(name)
#include <SFML/Graphics/Vulkan/VulkanFunctions.inc>
#undef SFML_VK_DEVICE
#undef SFML_VK_INSTANCE
    if (!loaded)
        return {};

    const auto physicalDevices = enumerate<VkPhysicalDevice>(
        [&](auto* count, auto* values) { return vk.EnumeratePhysicalDevices(context->instance, count, values); });

    std::vector<VkQueueFamilyProperties> chosenFamilies;
    std::uint32_t                        highestVersion{};
    int                                  bestScore         = -1;
    bool                                 feedbackExtension = false;
    for (const VkPhysicalDevice physical : physicalDevices)
    {
        VkPhysicalDeviceProperties properties{};
        vk.GetPhysicalDeviceProperties(physical, &properties);
        highestVersion = std::max(highestVersion, properties.apiVersion);
        if (properties.apiVersion < requestedVersion)
            continue;

        const auto deviceExtensions = enumerate<VkExtensionProperties>(
            [&](auto* count, auto* values)
            { return vk.EnumerateDeviceExtensionProperties(physical, nullptr, count, values); });
        if (std::none_of(deviceExtensions.begin(),
                         deviceExtensions.end(),
                         [](const auto& extension)
                         { return std::string_view(extension.extensionName) == VK_KHR_SWAPCHAIN_EXTENSION_NAME; }))
            continue;

        VkFormatProperties format{};
        vk.GetPhysicalDeviceFormatProperties(physical, VK_FORMAT_R8G8B8A8_UNORM, &format);
        constexpr VkFormatFeatureFlags required = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
                                                  VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT |
                                                  VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
        if ((format.optimalTilingFeatures & required) != required)
            continue;

        std::uint32_t count{};
        vk.GetPhysicalDeviceQueueFamilyProperties(physical, &count, nullptr);
        std::vector<VkQueueFamilyProperties> families(count);
        vk.GetPhysicalDeviceQueueFamilyProperties(physical, &count, families.data());
        const auto family = std::find_if(families.begin(),
                                         families.end(),
                                         [](const auto& item)
                                         { return item.queueCount && (item.queueFlags & VK_QUEUE_GRAPHICS_BIT); });
        if (family == families.end())
            continue;

        const int score = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 2
                          : properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
                              ? 1
                              : 0;
        if (score > bestScore)
        {
            bestScore               = score;
            context->physicalDevice = physical;
            context->properties     = properties;
            context->graphicsFamily = static_cast<std::uint32_t>(family - families.begin());
            chosenFamilies          = std::move(families);
            feedbackExtension       = std::any_of(deviceExtensions.begin(),
                                            deviceExtensions.end(),
                                            [](const auto& ext) {
                                                return std::string_view(ext.extensionName) ==
                                                       VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME;
                                            });
        }
    }

    if (!context->physicalDevice)
    {
        err() << "No suitable Vulkan device. Highest available API version is " << VK_VERSION_MAJOR(highestVersion)
              << '.' << VK_VERSION_MINOR(highestVersion) << std::endl;
        return {};
    }

    vk.GetPhysicalDeviceMemoryProperties(context->physicalDevice, &context->memory);
    VkPhysicalDeviceFeatures available{};
    vk.GetPhysicalDeviceFeatures(context->physicalDevice, &available);
    context->features.geometryShader                 = available.geometryShader;
    context->features.vertexPipelineStoresAndAtomics = available.vertexPipelineStoresAndAtomics;

    const float                          priority = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    for (std::uint32_t family = 0; family < chosenFamilies.size(); ++family)
    {
        if (!chosenFamilies[family].queueCount)
            continue;
        VkDeviceQueueCreateInfo queue{};
        queue.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue.queueFamilyIndex = family;
        queue.queueCount       = 1;
        queue.pQueuePriorities = &priority;
        queueInfos.push_back(queue);
    }

    VkPhysicalDeviceTransformFeedbackFeaturesEXT feedback{};
    feedback.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_FEATURES_EXT;
    std::vector<const char*> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    if (feedbackExtension && properties2)
    {
        const bool core        = requestedVersion >= VK_API_VERSION_1_1;
        const auto getFeatures = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(
            getProc(context->instance, core ? "vkGetPhysicalDeviceFeatures2" : "vkGetPhysicalDeviceFeatures2KHR"));
        const auto getProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
            getProc(context->instance, core ? "vkGetPhysicalDeviceProperties2" : "vkGetPhysicalDeviceProperties2KHR"));
        if (getFeatures && getProperties)
        {
            VkPhysicalDeviceFeatures2 features{};
            features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            features.pNext = &feedback;
            getFeatures(context->physicalDevice, &features);
            if (feedback.transformFeedback)
            {
                feedback.geometryStreams = VK_FALSE;
                auto& limits             = context->feedbackProperties;
                limits.sType             = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TRANSFORM_FEEDBACK_PROPERTIES_EXT;
                VkPhysicalDeviceProperties2 properties{};
                properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
                properties.pNext = &limits;
                getProperties(context->physicalDevice, &properties);
                deviceExtensions.push_back(VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME);
            }
        }
    }
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount    = static_cast<std::uint32_t>(queueInfos.size());
    deviceInfo.pQueueCreateInfos       = queueInfos.data();
    deviceInfo.pNext                   = feedback.transformFeedback ? &feedback : nullptr;
    deviceInfo.enabledExtensionCount   = static_cast<std::uint32_t>(deviceExtensions.size());
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceInfo.pEnabledFeatures        = &context->features;
    if (vk.CreateDevice(context->physicalDevice, &deviceInfo, nullptr, &context->device) != VK_SUCCESS)
        return {};

    loaded = true;
#define SFML_VK_INSTANCE(name)
#define SFML_VK_DEVICE(name)                                                                     \
    vk.name = reinterpret_cast<PFN_vk##name>(vk.GetDeviceProcAddr(context->device, "vk" #name)); \
    loaded  = loaded && vk.name;
#include <SFML/Graphics/Vulkan/VulkanFunctions.inc>
#undef SFML_VK_DEVICE
#undef SFML_VK_INSTANCE
    if (!loaded)
        return {};

    if (feedback.transformFeedback)
    {
        context->bindFeedbackBuffers = reinterpret_cast<PFN_vkCmdBindTransformFeedbackBuffersEXT>(
            vk.GetDeviceProcAddr(context->device, "vkCmdBindTransformFeedbackBuffersEXT"));
        context->beginFeedback = reinterpret_cast<PFN_vkCmdBeginTransformFeedbackEXT>(
            vk.GetDeviceProcAddr(context->device, "vkCmdBeginTransformFeedbackEXT"));
        context->endFeedback = reinterpret_cast<PFN_vkCmdEndTransformFeedbackEXT>(
            vk.GetDeviceProcAddr(context->device, "vkCmdEndTransformFeedbackEXT"));
        if (!context->bindFeedbackBuffers || !context->beginFeedback || !context->endFeedback)
            return {};
    }
    context->queues.resize(chosenFamilies.size());
    for (const auto& queue : queueInfos)
        vk.GetDeviceQueue(context->device, queue.queueFamilyIndex, 0, &context->queues[queue.queueFamilyIndex]);

    auto result = std::shared_ptr<VulkanDevice>(new VulkanDevice(std::move(context)));
    return result->initialize() ? std::move(result) : nullptr;
}

VulkanDevice::VulkanDevice(std::shared_ptr<VulkanContext> value) : m_impl(std::make_unique<Impl>(std::move(value)))
{
}

bool VulkanDevice::initialize()
{
    VkPipelineCacheCreateInfo cache{};
    cache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (context()->vk.CreatePipelineCache(context()->device, &cache, nullptr, &context()->pipelineCache) != VK_SUCCESS)
        return false;

    const auto&             ctx = *context();
    VkCommandPoolCreateInfo pool{};
    pool.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool.queueFamilyIndex = ctx.graphicsFamily;
    pool.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    if (ctx.vk.CreateCommandPool(ctx.device, &pool, nullptr, &m_impl->commandPool) != VK_SUCCESS)
        return false;

    VkCommandBufferAllocateInfo command{};
    command.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command.commandPool        = m_impl->commandPool;
    command.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command.commandBufferCount = 1;
    if (ctx.vk.AllocateCommandBuffers(ctx.device, &command, &m_impl->commandBuffer) != VK_SUCCESS)
        return false;

    VkFenceCreateInfo fence{};
    fence.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    return ctx.vk.CreateFence(ctx.device, &fence, nullptr, &m_impl->fence) == VK_SUCCESS;
}

VulkanDevice::~VulkanDevice()
{
    const auto& ctx = *context();
    if (m_impl->fence && !m_impl->lost)
        (void)flush();

    ctx.vk.DeviceWaitIdle(ctx.device);
    for (const auto& pool : m_impl->descriptorPools)
        ctx.vk.DestroyDescriptorPool(ctx.device, pool.handle, nullptr);
    for (const VkSampler sampler : m_impl->samplers)
        if (sampler)
            ctx.vk.DestroySampler(ctx.device, sampler, nullptr);
    if (m_impl->fence)
        ctx.vk.DestroyFence(ctx.device, m_impl->fence, nullptr);
    if (m_impl->commandPool)
        ctx.vk.DestroyCommandPool(ctx.device, m_impl->commandPool, nullptr);
}

std::unique_lock<std::recursive_mutex> VulkanDevice::lock() const
{
    return std::unique_lock(m_impl->mutex);
}

const std::shared_ptr<VulkanContext>& VulkanDevice::context() const
{
    return m_impl->context;
}

bool VulkanDevice::isLost() const
{
    return m_impl->lost;
}

bool VulkanDevice::loadPipelineCache(const void* data, std::size_t size)
{
    const auto            guard      = lock();
    const auto&           ctx        = *context();
    constexpr std::size_t headerSize = 4 * sizeof(std::uint32_t) + VK_UUID_SIZE;
    if (m_impl->lost || !data || size < headerSize)
        return false;

    // Vulkan's version-one header is tightly packed and always little-endian.
    const auto* bytes = static_cast<const std::uint8_t*>(data);
    const auto  word  = [bytes](std::size_t offset)
    {
        return std::uint32_t{bytes[offset]} | (std::uint32_t{bytes[offset + 1]} << 8) |
               (std::uint32_t{bytes[offset + 2]} << 16) | (std::uint32_t{bytes[offset + 3]} << 24);
    };
    if (word(0) != headerSize || word(4) != VK_PIPELINE_CACHE_HEADER_VERSION_ONE || word(8) != ctx.properties.vendorID ||
        word(12) != ctx.properties.deviceID || std::memcmp(bytes + 16, ctx.properties.pipelineCacheUUID, VK_UUID_SIZE) != 0)
        return false;

    VkPipelineCacheCreateInfo info{};
    info.sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    info.initialDataSize = size;
    info.pInitialData    = data;
    VkPipelineCache imported{};
    if (ctx.vk.CreatePipelineCache(ctx.device, &info, nullptr, &imported) != VK_SUCCESS)
        return false;
    const auto result = ctx.vk.MergePipelineCaches(ctx.device, ctx.pipelineCache, 1, &imported);
    ctx.vk.DestroyPipelineCache(ctx.device, imported, nullptr);
    return result == VK_SUCCESS;
}

std::optional<std::vector<std::uint8_t>> VulkanDevice::getPipelineCacheData() const
{
    const auto guard = lock();
    if (m_impl->lost)
        return std::nullopt;
    const auto& ctx = *context();
    std::size_t size{};
    if (ctx.vk.GetPipelineCacheData(ctx.device, ctx.pipelineCache, &size, nullptr) != VK_SUCCESS)
        return std::nullopt;
    std::vector<std::uint8_t> data(size);
    if (ctx.vk.GetPipelineCacheData(ctx.device, ctx.pipelineCache, &size, data.data()) != VK_SUCCESS)
        return std::nullopt;
    data.resize(size);
    return data;
}

VkCommandBuffer VulkanDevice::commands()
{
    if (m_impl->lost)
        return VK_NULL_HANDLE;
    if (!m_impl->recording)
    {
        const auto&              ctx = *context();
        VkCommandBufferBeginInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if (ctx.vk.BeginCommandBuffer(m_impl->commandBuffer, &info) != VK_SUCCESS)
        {
            m_impl->lost = true;
            err() << "Failed to begin Vulkan commands" << std::endl;
            return VK_NULL_HANDLE;
        }
        m_impl->recording = true;
    }
    return m_impl->commandBuffer;
}

bool VulkanDevice::flush(VkSemaphore wait, VkSemaphore signal)
{
    if (m_impl->lost)
        return false;
    if (!m_impl->recording)
        return true;

    const auto&                ctx       = *context();
    const auto&                vk        = ctx.vk;
    const VkFence              fence     = m_impl->fence;
    const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo               submit{};
    submit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.waitSemaphoreCount   = wait ? 1u : 0u;
    submit.pWaitSemaphores      = &wait;
    submit.pWaitDstStageMask    = &waitStage;
    submit.commandBufferCount   = 1;
    submit.pCommandBuffers      = &m_impl->commandBuffer;
    submit.signalSemaphoreCount = signal ? 1u : 0u;
    submit.pSignalSemaphores    = &signal;

    if (vk.EndCommandBuffer(m_impl->commandBuffer) != VK_SUCCESS || vk.ResetFences(ctx.device, 1, &fence) != VK_SUCCESS ||
        vk.QueueSubmit(ctx.queues[ctx.graphicsFamily], 1, &submit, fence) != VK_SUCCESS ||
        vk.WaitForFences(ctx.device, 1, &fence, VK_TRUE, std::numeric_limits<std::uint64_t>::max()) != VK_SUCCESS ||
        vk.ResetCommandPool(ctx.device, m_impl->commandPool, 0) != VK_SUCCESS)
    {
        m_impl->lost = true;
        err() << "Failed to submit or complete Vulkan commands" << std::endl;
        return false;
    }

    m_impl->recording = false;
    for (auto& pool : m_impl->descriptorPools)
    {
        vk.ResetDescriptorPool(ctx.device, pool.handle, 0);
        pool.sets = pool.buffers = pool.images = 0;
    }
    m_impl->currentDescriptorPool = 0;
    m_impl->retained.clear();
    for (auto& upload : m_impl->uploads)
        upload.used = 0;
    return true;
}

void VulkanDevice::retain(std::shared_ptr<void> object)
{
    m_impl->retained.push_back(std::move(object));
}

std::shared_ptr<VulkanBuffer> VulkanDevice::makeBuffer(VkDeviceSize bytes, VkBufferUsageFlags usage) const
{
    const auto& ctx    = *context();
    auto        result = std::make_shared<VulkanBuffer>(context());
    result->capacity   = std::max<VkDeviceSize>(1, bytes);

    VkBufferCreateInfo info{};
    info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size        = result->capacity;
    info.usage       = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (ctx.vk.CreateBuffer(ctx.device, &info, nullptr, &result->buffer) != VK_SUCCESS)
        return {};

    VkMemoryRequirements requirements{};
    ctx.vk.GetBufferMemoryRequirements(ctx.device, result->buffer, &requirements);
    const auto type = VulkanDeviceImpl::memoryType(ctx,
                                                   requirements.memoryTypeBits,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (!type)
        return {};
    result->coherent = (ctx.memory.memoryTypes[*type].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;

    VkMemoryAllocateInfo allocation{};
    allocation.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocation.allocationSize  = requirements.size;
    allocation.memoryTypeIndex = *type;
    if (ctx.vk.AllocateMemory(ctx.device, &allocation, nullptr, &result->memory) != VK_SUCCESS ||
        ctx.vk.BindBufferMemory(ctx.device, result->buffer, result->memory, 0) != VK_SUCCESS ||
        ctx.vk.MapMemory(ctx.device, result->memory, 0, VK_WHOLE_SIZE, 0, &result->mapped) != VK_SUCCESS)
        return {};

    return result;
}

std::shared_ptr<VulkanImage> VulkanDevice::makeImage(
    Vector2u              size,
    VkFormat              format,
    VkImageUsageFlags     usage,
    VkImageAspectFlags    aspect,
    VkSampleCountFlagBits samples,
    std::uint32_t         levels) const
{
    const auto& ctx    = *context();
    auto        result = std::make_shared<VulkanImage>(context());
    result->size       = size;
    result->format     = format;
    result->aspect     = aspect;
    result->levels     = levels;

    VkImageCreateInfo info{};
    info.sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType   = VK_IMAGE_TYPE_2D;
    info.format      = format;
    info.extent      = {size.x, size.y, 1};
    info.mipLevels   = levels;
    info.arrayLayers = 1;
    info.samples     = samples;
    info.tiling      = VK_IMAGE_TILING_OPTIMAL;
    info.usage       = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (ctx.vk.CreateImage(ctx.device, &info, nullptr, &result->image) != VK_SUCCESS)
        return {};

    VkMemoryRequirements requirements{};
    ctx.vk.GetImageMemoryRequirements(ctx.device, result->image, &requirements);
    const auto type = VulkanDeviceImpl::memoryType(ctx, requirements.memoryTypeBits, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (!type)
        return {};

    VkMemoryAllocateInfo allocation{};
    allocation.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocation.allocationSize  = requirements.size;
    allocation.memoryTypeIndex = *type;
    if (ctx.vk.AllocateMemory(ctx.device, &allocation, nullptr, &result->memory) != VK_SUCCESS ||
        ctx.vk.BindImageMemory(ctx.device, result->image, result->memory, 0) != VK_SUCCESS)
        return {};

    result->view = VulkanDeviceImpl::makeView(ctx, *result, levels);
    if (!result->view)
        return {};

    if (usage & (VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT))
    {
        result->attachment = levels == 1 ? result->view : VulkanDeviceImpl::makeView(ctx, *result, 1);
        if (!result->attachment)
            return {};
    }
    return result;
}

std::shared_ptr<VulkanImage> VulkanDevice::makeTexture(Vector2u size, bool sRgb) const
{
    std::uint32_t levels = 1;
    for (auto side = std::max(size.x, size.y); side > 1; side /= 2)
        ++levels;

    constexpr VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    return makeImage(size,
                     sRgb ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM,
                     usage,
                     VK_IMAGE_ASPECT_COLOR_BIT,
                     VK_SAMPLE_COUNT_1_BIT,
                     levels);
}

void VulkanDevice::transition(VulkanImage& image, VkImageLayout layout)
{
    // Attachment accesses may need a memory dependency even when the layout is unchanged.
    if (image.layout == layout && layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        return;

    if (const VkCommandBuffer command = commands())
    {
        VulkanDeviceImpl::barrier(*context(), command, image, image.layout, layout, 0, image.levels);
        image.layout = layout;
    }
}

bool VulkanDevice::upload(const std::shared_ptr<VulkanImage>& image, const std::uint8_t* pixels, Vector2u size, Vector2u offset)
{
    return image->format != VK_FORMAT_R32G32B32A32_SFLOAT && uploadPixels(image, pixels, size, offset, 4);
}

bool VulkanDevice::supportsFloatTextures() const
{
    VkImageFormatProperties properties{};
    return context()->vk.GetPhysicalDeviceImageFormatProperties(context()->physicalDevice,
                                                                VK_FORMAT_R32G32B32A32_SFLOAT,
                                                                VK_IMAGE_TYPE_2D,
                                                                VK_IMAGE_TILING_OPTIMAL,
                                                                VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                                                0,
                                                                &properties) == VK_SUCCESS;
}

bool VulkanDevice::uploadFloats(const std::shared_ptr<VulkanImage>& image, const float* pixels, Vector2u size, Vector2u offset)
{
    return image->format == VK_FORMAT_R32G32B32A32_SFLOAT && uploadPixels(image, pixels, size, offset, 4 * sizeof(float));
}

bool VulkanDevice::uploadPixels(const std::shared_ptr<VulkanImage>& image,
                                const void*                         pixels,
                                Vector2u                            size,
                                Vector2u                            offset,
                                std::size_t                         bytesPerPixel)
{
    if (!pixels || !size.x || !size.y || size.x > std::numeric_limits<std::size_t>::max() / bytesPerPixel / size.y)
        return false;
    const VkCommandBuffer command = commands();
    if (!command)
        return false;
    const auto buffer = stage(pixels, static_cast<std::size_t>(size.x) * size.y * bytesPerPixel);
    if (!buffer.buffer)
        return false;

    transition(*image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkBufferImageCopy region{};
    region.bufferOffset     = buffer.offset;
    region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.imageOffset      = {static_cast<std::int32_t>(offset.x), static_cast<std::int32_t>(offset.y), 0};
    region.imageExtent      = {size.x, size.y, 1};
    context()->vk.CmdCopyBufferToImage(command, buffer.buffer, image->image, image->layout, 1, &region);
    transition(*image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    retain(image);
    return true;
}

bool VulkanDevice::copy(const std::shared_ptr<VulkanImage>& source,
                        const std::shared_ptr<VulkanImage>& destination,
                        Vector2u                            offset)
{
    if ((source->format == VK_FORMAT_R32G32B32A32_SFLOAT) != (destination->format == VK_FORMAT_R32G32B32A32_SFLOAT))
        return false;
    if (source == destination)
        return true;
    const VkCommandBuffer command = commands();
    if (!command)
        return false;

    transition(*source, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    transition(*destination, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkImageCopy region{};
    region.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.dstSubresource = region.srcSubresource;
    region.dstOffset      = {static_cast<std::int32_t>(offset.x), static_cast<std::int32_t>(offset.y), 0};
    region.extent         = {source->size.x, source->size.y, 1};
    context()->vk.CmdCopyImage(command, source->image, source->layout, destination->image, destination->layout, 1, &region);
    transition(*source, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    transition(*destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    retain(source);
    retain(destination);
    return true;
}

Image VulkanDevice::readImage(const std::shared_ptr<VulkanImage>& image)
{
    if (image->format == VK_FORMAT_R32G32B32A32_SFLOAT)
        return {};
    const auto            buffer  = makeBuffer(static_cast<VkDeviceSize>(image->size.x) * image->size.y * 4,
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    const VkCommandBuffer command = commands();
    if (!buffer || !command)
        return {};

    transition(*image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    VkBufferImageCopy region{};
    region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    region.imageExtent      = {image->size.x, image->size.y, 1};
    context()->vk.CmdCopyImageToBuffer(command, image->image, image->layout, buffer->buffer, 1, &region);
    transition(*image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    retain(image);
    retain(buffer);
    if (!flush())
        return {};

    buffer->invalidate();
    return {image->size, static_cast<const std::uint8_t*>(buffer->mapped)};
}

bool VulkanDevice::generateMipmap(const std::shared_ptr<VulkanImage>& image)
{
    if (image->format == VK_FORMAT_R32G32B32A32_SFLOAT)
        return false;
    using namespace VulkanDeviceImpl;
    const auto&        ctx = *context();
    VkFormatProperties properties{};
    ctx.vk.GetPhysicalDeviceFormatProperties(ctx.physicalDevice, image->format, &properties);
    constexpr VkFormatFeatureFlags blit = VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT;
    if ((properties.optimalTilingFeatures & blit) != blit)
        return false;

    const VkCommandBuffer command = commands();
    if (!command)
        return false;
    transition(*image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    auto width  = static_cast<std::int32_t>(image->size.x);
    auto height = static_cast<std::int32_t>(image->size.y);
    for (std::uint32_t level = 1; level < image->levels; ++level)
    {
        barrier(ctx, command, *image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, level - 1, 1);
        VkImageBlit region{};
        region.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, level - 1, 0, 1};
        region.srcOffsets[1]  = {width, height, 1};
        width                 = std::max(1, width / 2);
        height                = std::max(1, height / 2);
        region.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, level, 0, 1};
        region.dstOffsets[1]  = {width, height, 1};
        const auto filter     = (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)
                                    ? VK_FILTER_LINEAR
                                    : VK_FILTER_NEAREST;
        ctx.vk.CmdBlitImage(command,
                            image->image,
                            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                            image->image,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            1,
                            &region,
                            filter);
    }

    if (image->levels > 1)
        barrier(ctx,
                command,
                *image,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                0,
                image->levels - 1);
    barrier(ctx,
            command,
            *image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            image->levels - 1,
            1);
    image->layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    retain(image);
    return true;
}

VkSampler VulkanDevice::sampler(bool smooth, bool repeated, bool mipmapped)
{
    const auto index = (smooth ? 1u : 0u) | (repeated ? 2u : 0u) | (mipmapped ? 4u : 0u);
    auto&      value = m_impl->samplers[index];
    if (!value)
    {
        const auto&         ctx = *context();
        VkSamplerCreateInfo info{};
        info.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.magFilter = info.minFilter = smooth ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
        info.mipmapMode                 = smooth ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
        info.addressModeU = info.addressModeV = info.addressModeW = repeated ? VK_SAMPLER_ADDRESS_MODE_REPEAT
                                                                             : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        info.maxLod                                               = mipmapped ? VK_LOD_CLAMP_NONE : 0.f;
        if (ctx.vk.CreateSampler(ctx.device, &info, nullptr, &value) != VK_SUCCESS)
            return VK_NULL_HANDLE;
    }
    return value;
}

VulkanDevice::Upload VulkanDevice::stage(const void* data, std::size_t size)
{
    const auto& limits    = context()->properties.limits;
    const auto  alignment = std::max<VkDeviceSize>(
        {VkDeviceSize{16}, limits.minUniformBufferOffsetAlignment, limits.optimalBufferCopyOffsetAlignment});
    for (auto& block : m_impl->uploads)
    {
        const auto offset = (block.used + alignment - 1) & ~(alignment - 1);
        if (offset <= block.buffer->capacity && size <= block.buffer->capacity - offset)
        {
            block.buffer->write(offset, data, size);
            block.used = offset + size;
            return {block.buffer->buffer, offset};
        }
    }

    const auto buffer = makeBuffer(std::max<VkDeviceSize>(256 * 1024, size), VulkanDeviceImpl::uploadUsage);
    if (!buffer)
        return {};
    buffer->write(0, data, size);
    m_impl->uploads.push_back({buffer, size});
    return {buffer->buffer, 0};
}

VkDescriptorSet VulkanDevice::allocateDescriptorSet(VkDescriptorSetLayout layout, std::uint32_t buffers, std::uint32_t images)
{
    constexpr std::uint32_t setsPerPool        = 256;
    constexpr std::uint32_t descriptorsPerType = 1024;
    if (buffers > descriptorsPerType || images > descriptorsPerType)
        return VK_NULL_HANDLE;

    const auto& ctx = *context();
    for (;;)
    {
        const auto index = m_impl->currentDescriptorPool;
        if (index == m_impl->descriptorPools.size())
        {
            const std::array sizes{VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorsPerType},
                                   VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorsPerType},
                                   VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, setsPerPool}};
            VkDescriptorPoolCreateInfo info{};
            info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            info.maxSets       = setsPerPool;
            info.poolSizeCount = static_cast<std::uint32_t>(sizes.size());
            info.pPoolSizes    = sizes.data();
            Impl::DescriptorPool pool;
            if (ctx.vk.CreateDescriptorPool(ctx.device, &info, nullptr, &pool.handle) != VK_SUCCESS)
                return VK_NULL_HANDLE;
            m_impl->descriptorPools.push_back(pool);
        }

        auto& pool = m_impl->descriptorPools[index];
        if (pool.sets == setsPerPool || buffers > descriptorsPerType - pool.buffers ||
            images > descriptorsPerType - pool.images)
        {
            ++m_impl->currentDescriptorPool;
            continue;
        }

        VkDescriptorSetAllocateInfo allocation{};
        allocation.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocation.descriptorPool     = pool.handle;
        allocation.descriptorSetCount = 1;
        allocation.pSetLayouts        = &layout;
        VkDescriptorSet set{};
        if (ctx.vk.AllocateDescriptorSets(ctx.device, &allocation, &set) != VK_SUCCESS)
            return VK_NULL_HANDLE;
        ++pool.sets;
        pool.buffers += buffers;
        pool.images += images;
        return set;
    }
}

const std::shared_ptr<VulkanImage>& VulkanDevice::whiteTexture()
{
    if (!m_impl->white)
    {
        constexpr std::array<std::uint8_t, 4> white{255, 255, 255, 255};
        m_impl->white = makeTexture({1, 1}, false);
        if (m_impl->white && !upload(m_impl->white, white.data(), {1, 1}, {}))
            m_impl->white.reset();
    }
    return m_impl->white;
}
const std::shared_ptr<VulkanProgram>& VulkanDevice::defaultProgram()
{
    if (!m_impl->program)
        m_impl->program = VulkanProgram::create(*this);
    return m_impl->program;
}
} // namespace sf::priv
