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

#include <SFML/Window/ContextSettings.hpp>

#include <SFML/System/Vector2.hpp>

#include <memory>
#include <mutex>
#include <optional>
#include <vector>
#include <vulkan_core.h>

#include <cstddef>
#include <cstdint>

namespace sf
{
class Image;
}

namespace sf::priv
{
class VulkanProgram;
struct VulkanDispatch
{
#define SFML_VK_INSTANCE(name) PFN_vk##name name{};
#define SFML_VK_DEVICE(name)   PFN_vk##name name{};
#include <SFML/Graphics/Vulkan/VulkanFunctions.inc>
#undef SFML_VK_DEVICE
#undef SFML_VK_INSTANCE
};

struct VulkanContext
{
    ~VulkanContext();

    VulkanDispatch                                 vk;
    VkInstance                                     instance{};
    VkPhysicalDevice                               physicalDevice{};
    VkDevice                                       device{};
    VkPipelineCache                                pipelineCache{};
    std::uint32_t                                  apiVersion{};
    VkPhysicalDeviceProperties                     properties{};
    VkPhysicalDeviceMemoryProperties               memory{};
    VkPhysicalDeviceFeatures                       features{};
    VkPhysicalDeviceTransformFeedbackPropertiesEXT feedbackProperties{};
    PFN_vkCmdBindTransformFeedbackBuffersEXT       bindFeedbackBuffers{};
    PFN_vkCmdBeginTransformFeedbackEXT             beginFeedback{};
    PFN_vkCmdEndTransformFeedbackEXT               endFeedback{};
    std::uint32_t                                  graphicsFamily{};
    std::vector<VkQueue>                           queues;
};

struct VulkanBuffer
{
    explicit VulkanBuffer(std::shared_ptr<VulkanContext> value);
    ~VulkanBuffer();

    VulkanBuffer(const VulkanBuffer&)            = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    void write(VkDeviceSize offset, const void* source, std::size_t bytes) const;
    void invalidate() const;

    std::shared_ptr<VulkanContext> context;
    VkBuffer                       buffer{};
    VkDeviceMemory                 memory{};
    VkDeviceSize                   capacity{};
    void*                          mapped{};
    bool                           coherent{};
};

struct VulkanImage
{
    explicit VulkanImage(std::shared_ptr<VulkanContext> value);
    ~VulkanImage();

    VulkanImage(const VulkanImage&)            = delete;
    VulkanImage& operator=(const VulkanImage&) = delete;

    std::shared_ptr<VulkanContext> context;
    VkImage                        image{};
    VkDeviceMemory                 memory{};
    VkImageView                    view{};
    VkImageView                    attachment{};
    VkFormat                       format{VK_FORMAT_UNDEFINED};
    VkImageLayout                  layout{VK_IMAGE_LAYOUT_UNDEFINED};
    VkImageAspectFlags             aspect{};
    Vector2u                       size;
    std::uint32_t                  levels{1};
};

class VulkanDevice
{
public:
    [[nodiscard]] static std::shared_ptr<VulkanDevice> create(const VulkanSettings& requested);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&)            = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    [[nodiscard]] std::unique_lock<std::recursive_mutex> lock() const;
    [[nodiscard]] const std::shared_ptr<VulkanContext>&  context() const;
    [[nodiscard]] bool                                   isLost() const;

    [[nodiscard]] bool                                     loadPipelineCache(const void* data, std::size_t size);
    [[nodiscard]] std::optional<std::vector<std::uint8_t>> getPipelineCacheData() const;

    // Record commands and change resources while holding the device lock.
    [[nodiscard]] VkCommandBuffer commands();
    [[nodiscard]] bool            flush(VkSemaphore wait = VK_NULL_HANDLE, VkSemaphore signal = VK_NULL_HANDLE);
    void                          retain(std::shared_ptr<void> object);

    [[nodiscard]] std::shared_ptr<VulkanBuffer> makeBuffer(std::size_t bytes) const
    {
        VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (context()->bindFeedbackBuffers)
            usage |= VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT;
        return makeBuffer(bytes, usage);
    }
    [[nodiscard]] std::shared_ptr<VulkanBuffer> makeBuffer(VkDeviceSize bytes, VkBufferUsageFlags usage) const;
    [[nodiscard]] std::shared_ptr<VulkanImage>  makeImage(
         Vector2u              size,
         VkFormat              format,
         VkImageUsageFlags     usage,
         VkImageAspectFlags    aspect,
         VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
         std::uint32_t         levels  = 1) const;
    [[nodiscard]] std::shared_ptr<VulkanImage> makeTexture(Vector2u size, bool sRgb) const;
    [[nodiscard]] bool                         supportsFloatTextures() const;
    [[nodiscard]] bool uploadFloats(const std::shared_ptr<VulkanImage>& image, const float* pixels, Vector2u size, Vector2u offset);
    void transition(VulkanImage& image, VkImageLayout layout);

    [[nodiscard]] bool upload(const std::shared_ptr<VulkanImage>& image, const std::uint8_t* pixels, Vector2u size, Vector2u offset);
    [[nodiscard]] bool      copy(const std::shared_ptr<VulkanImage>& source,
                                 const std::shared_ptr<VulkanImage>& destination,
                                 Vector2u                            offset);
    [[nodiscard]] Image     readImage(const std::shared_ptr<VulkanImage>& image);
    [[nodiscard]] bool      generateMipmap(const std::shared_ptr<VulkanImage>& image);
    [[nodiscard]] VkSampler sampler(bool smooth, bool repeated, bool mipmapped);

    struct Upload
    {
        VkBuffer     buffer{};
        VkDeviceSize offset{};
    };
    [[nodiscard]] Upload stage(const void* data, std::size_t size);
    [[nodiscard]] VkDescriptorSet allocateDescriptorSet(VkDescriptorSetLayout layout, std::uint32_t buffers, std::uint32_t images);
    [[nodiscard]] const std::shared_ptr<VulkanImage>&   whiteTexture();
    [[nodiscard]] const std::shared_ptr<VulkanProgram>& defaultProgram();

private:
    explicit VulkanDevice(std::shared_ptr<VulkanContext> value);
    [[nodiscard]] bool initialize();
    bool               uploadPixels(const std::shared_ptr<VulkanImage>& image,
                                    const void*                         pixels,
                                    Vector2u                            size,
                                    Vector2u                            offset,
                                    std::size_t                         bytesPerPixel);

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
} // namespace sf::priv
