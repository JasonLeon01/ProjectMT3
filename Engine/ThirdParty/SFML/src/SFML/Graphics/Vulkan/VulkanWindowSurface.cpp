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

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Vulkan/VulkanProgram.hpp>
#include <SFML/Graphics/Vulkan/VulkanRenderTarget.hpp>
#include <SFML/Graphics/Vulkan/VulkanWindowSurface.hpp>

#include <SFML/Window/VulkanImpl.hpp>
#ifdef SFML_SYSTEM_HARMONY
#include <SFML/Window/Harmony/NativeAppImpl.hpp>
#endif

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <array>
#include <limits>
#include <ostream>
#include <utility>

namespace sf::priv
{
namespace
{
namespace VulkanWindowImpl
{
template <typename T, typename Query>
std::vector<T> surfaceValues(Query query)
{
    std::uint32_t count{};
    if (query(&count, static_cast<T*>(nullptr)) != VK_SUCCESS)
        return {};

    std::vector<T> result(count);
    if (!count || query(&count, result.data()) != VK_SUCCESS)
        return {};
    result.resize(count);
    return result;
}

VkSurfaceFormatKHR chooseFormat(const std::vector<VkSurfaceFormatKHR>& formats, bool sRgb)
{
    const auto rgba = sRgb ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
    const auto bgra = sRgb ? VK_FORMAT_B8G8R8A8_SRGB : VK_FORMAT_B8G8R8A8_UNORM;
    for (const auto& format : formats)
        if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && (format.format == rgba || format.format == bgra))
            return format;

    if (formats.front().format == VK_FORMAT_UNDEFINED)
        return {bgra, formats.front().colorSpace};

    return formats.front();
}
} // namespace VulkanWindowImpl
} // namespace

struct VulkanWindowSurface::Impl
{
    struct Swapchain
    {
        struct Frame
        {
            VkImageView   view{};
            VkFramebuffer framebuffer{};
            VkSemaphore   ready{};
        };

        explicit Swapchain(std::shared_ptr<VulkanContext> value) : context(std::move(value))
        {
        }

        ~Swapchain()
        {
            const auto& vk = context->vk;
            if (presentQueue)
                vk.QueueWaitIdle(presentQueue);
            for (const auto& frame : frames)
            {
                if (frame.ready)
                    vk.DestroySemaphore(context->device, frame.ready, nullptr);
                if (frame.framebuffer)
                    vk.DestroyFramebuffer(context->device, frame.framebuffer, nullptr);
                if (frame.view)
                    vk.DestroyImageView(context->device, frame.view, nullptr);
            }
            if (acquired)
                vk.DestroySemaphore(context->device, acquired, nullptr);
            if (pass)
                vk.DestroyRenderPass(context->device, pass, nullptr);
            if (handle)
                vk.DestroySwapchainKHR(context->device, handle, nullptr);
        }

        std::shared_ptr<VulkanContext> context;
        VkQueue                        presentQueue{};
        VkSwapchainKHR                 handle{};
        VkRenderPass                   pass{};
        VkSemaphore                    acquired{};
        VkFormat                       format{VK_FORMAT_UNDEFINED};
        VkExtent2D                     extent{};
        std::vector<Frame>             frames;
    };

    Impl(std::shared_ptr<VulkanDevice> value, WindowHandle native, Vector2u initialSize) :
        device(std::move(value)),
        handle(native),
        size(initialSize)
    {
    }

    std::shared_ptr<VulkanDevice>       device;
    std::shared_ptr<VulkanRenderTarget> target;
    WindowHandle                        handle;
#ifdef SFML_SYSTEM_HARMONY
    std::uint64_t surfaceGeneration{Harmony::getSurfaceSnapshot().generation};
#endif
    Vector2u                   size;
    VkSurfaceKHR               surface{};
    std::uint32_t              presentFamily{};
    std::unique_ptr<Swapchain> swapchain;
    bool                       vsync{};
    bool                       dirty{true};
};

std::unique_ptr<VulkanWindowSurface> VulkanWindowSurface::create(
    std::shared_ptr<VulkanDevice> device,
    WindowHandle                  handle,
    Vector2u                      size,
    const ContextSettings&        settings)
{
    if (!handle || !size.x || !size.y)
        return {};

    auto       surface = std::unique_ptr<VulkanWindowSurface>(new VulkanWindowSurface(std::move(device), handle, size));
    const auto lock    = surface->m_impl->device->lock();
    if (!surface->createSurface())
        return {};

    auto texture            = surface->m_impl->device->makeTexture(size, settings.sRgbCapable);
    surface->m_impl->target = VulkanRenderTarget::create(surface->m_impl->device, std::move(texture), settings);
    if (!surface->m_impl->target || !surface->createSwapchain())
    {
        err() << "Could not create the Vulkan window's swapchain and render target" << std::endl;
        return {};
    }

    return surface;
}

VulkanWindowSurface::VulkanWindowSurface(std::shared_ptr<VulkanDevice> device, WindowHandle handle, Vector2u size) :
    m_impl(std::make_unique<Impl>(std::move(device), handle, size))
{
}

VulkanWindowSurface::~VulkanWindowSurface()
{
    const auto lock = m_impl->device->lock();
    destroySurface();
}

bool VulkanWindowSurface::createSurface()
{
    const auto& ctx = *m_impl->device->context();
    if (!m_impl->handle || !VulkanImpl::createVulkanSurface(ctx.instance, m_impl->handle, m_impl->surface, nullptr))
        return false;

    const auto supported = [&](std::uint32_t family)
    {
        VkBool32 present{};
        return ctx.queues[family] &&
               ctx.vk.GetPhysicalDeviceSurfaceSupportKHR(ctx.physicalDevice, family, m_impl->surface, &present) ==
                   VK_SUCCESS &&
               present;
    };
    if (supported(ctx.graphicsFamily))
    {
        m_impl->presentFamily = ctx.graphicsFamily;
        return true;
    }

    for (std::uint32_t family = 0; family < ctx.queues.size(); ++family)
    {
        if (supported(family))
        {
            m_impl->presentFamily = family;
            return true;
        }
    }

    err() << "The active Vulkan device cannot present to this window" << std::endl;
    destroySurface();
    return false;
}

void VulkanWindowSurface::destroySurface()
{
    const auto& ctx = *m_impl->device->context();
    m_impl->swapchain.reset();
    if (m_impl->surface)
    {
        ctx.vk.DestroySurfaceKHR(ctx.instance, m_impl->surface, nullptr);
        m_impl->surface = VK_NULL_HANDLE;
    }
    m_impl->dirty = true;
}

bool VulkanWindowSurface::createSwapchain()
{
    using namespace VulkanWindowImpl;
    auto&                    state = *m_impl;
    const auto&              ctx   = *state.device->context();
    const auto&              vk    = ctx.vk;
    VkSurfaceCapabilitiesKHR capabilities{};
    const auto status = vk.GetPhysicalDeviceSurfaceCapabilitiesKHR(ctx.physicalDevice, state.surface, &capabilities);
    if (status == VK_ERROR_SURFACE_LOST_KHR)
        destroySurface();
    if (status != VK_SUCCESS || !(capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT))
        return false;

    auto extent = capabilities.currentExtent;
    if (extent.width == std::numeric_limits<std::uint32_t>::max())
    {
        extent.width = std::clamp(state.size.x, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(state.size.y, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
    if (!extent.width || !extent.height)
        return false;

    const auto formats = surfaceValues<VkSurfaceFormatKHR>(
        [&](auto* count, auto* values)
        { return vk.GetPhysicalDeviceSurfaceFormatsKHR(ctx.physicalDevice, state.surface, count, values); });
    if (formats.empty())
        return false;
    const auto format = chooseFormat(formats, state.target->settings().sRgbCapable);

    auto mode = VK_PRESENT_MODE_FIFO_KHR;
    if (!state.vsync)
    {
        const auto modes = surfaceValues<VkPresentModeKHR>(
            [&](auto* count, auto* values)
            { return vk.GetPhysicalDeviceSurfacePresentModesKHR(ctx.physicalDevice, state.surface, count, values); });
        if (std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != modes.end())
            mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        else if (std::find(modes.begin(), modes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != modes.end())
            mode = VK_PRESENT_MODE_MAILBOX_KHR;
    }

    auto alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    for (const auto candidate :
         {VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
          VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
          VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
          VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR})
    {
        if (capabilities.supportedCompositeAlpha & static_cast<VkCompositeAlphaFlagsKHR>(candidate))
        {
            alpha = candidate;
            break;
        }
    }

    auto imageCount = std::max(2u, capabilities.minImageCount);
    if (capabilities.maxImageCount)
        imageCount = std::min(imageCount, capabilities.maxImageCount);

    const std::array families{ctx.graphicsFamily, state.presentFamily};
    const bool       concurrent = families[0] != families[1];
    auto             swapchain  = std::make_unique<Impl::Swapchain>(state.device->context());
    swapchain->format           = format.format;
    swapchain->extent           = extent;

    VkSwapchainCreateInfoKHR info{};
    info.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface               = state.surface;
    info.minImageCount         = imageCount;
    info.imageFormat           = format.format;
    info.imageColorSpace       = format.colorSpace;
    info.imageExtent           = extent;
    info.imageArrayLayers      = 1;
    info.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.imageSharingMode      = concurrent ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = concurrent ? 2u : 0u;
    info.pQueueFamilyIndices   = families.data();
#ifdef SFML_SYSTEM_HARMONY
    // XComponent sizes and SFML input coordinates already follow the Stage orientation.
    info.preTransform = (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
                            ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                            : capabilities.currentTransform;
#else
    info.preTransform = capabilities.currentTransform;
#endif
    info.compositeAlpha = alpha;
    info.presentMode    = mode;
    info.clipped        = VK_TRUE;

    // Destroy the old swapchain after its presentation queue has completed.
    state.swapchain.reset();
    if (vk.CreateSwapchainKHR(ctx.device, &info, nullptr, &swapchain->handle) != VK_SUCCESS)
        return false;

    VkAttachmentDescription attachment{};
    attachment.format         = format.format;
    attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    const VkAttachmentReference reference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkSubpassDescription        subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &reference;
    VkSubpassDependency dependency{};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    VkRenderPassCreateInfo pass{};
    pass.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    pass.attachmentCount = 1;
    pass.pAttachments    = &attachment;
    pass.subpassCount    = 1;
    pass.pSubpasses      = &subpass;
    pass.dependencyCount = 1;
    pass.pDependencies   = &dependency;
    if (vk.CreateRenderPass(ctx.device, &pass, nullptr, &swapchain->pass) != VK_SUCCESS)
        return false;

    const auto images = surfaceValues<VkImage>(
        [&](auto* count, auto* values) { return vk.GetSwapchainImagesKHR(ctx.device, swapchain->handle, count, values); });
    if (images.empty())
        return false;

    VkSemaphoreCreateInfo semaphore{};
    semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vk.CreateSemaphore(ctx.device, &semaphore, nullptr, &swapchain->acquired) != VK_SUCCESS)
        return false;

    swapchain->frames.resize(images.size());
    for (std::size_t index = 0; index < images.size(); ++index)
    {
        auto&                 frame = swapchain->frames[index];
        VkImageViewCreateInfo view{};
        view.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view.image            = images[index];
        view.viewType         = VK_IMAGE_VIEW_TYPE_2D;
        view.format           = format.format;
        view.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        if (vk.CreateImageView(ctx.device, &view, nullptr, &frame.view) != VK_SUCCESS)
            return false;

        VkFramebufferCreateInfo framebuffer{};
        framebuffer.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer.renderPass      = swapchain->pass;
        framebuffer.attachmentCount = 1;
        framebuffer.pAttachments    = &frame.view;
        framebuffer.width           = extent.width;
        framebuffer.height          = extent.height;
        framebuffer.layers          = 1;
        if (vk.CreateFramebuffer(ctx.device, &framebuffer, nullptr, &frame.framebuffer) != VK_SUCCESS ||
            vk.CreateSemaphore(ctx.device, &semaphore, nullptr, &frame.ready) != VK_SUCCESS)
            return false;
    }

    swapchain->presentQueue = ctx.queues[state.presentFamily];
    state.swapchain         = std::move(swapchain);
    state.dirty             = false;
    return true;
}

const ContextSettings& VulkanWindowSurface::getSettings() const
{
    return m_impl->target->settings();
}

bool VulkanWindowSurface::setActive(bool)
{
    const auto lock = m_impl->device->lock();
    return m_impl->target && !m_impl->device->isLost();
}

void VulkanWindowSurface::setVerticalSyncEnabled(bool enabled)
{
    const auto lock = m_impl->device->lock();
    if (m_impl->vsync != enabled)
    {
        m_impl->vsync = enabled;
        m_impl->dirty = true;
    }
}

void VulkanWindowSurface::resize(Vector2u size)
{
    const auto lock = m_impl->device->lock();
    if (size != m_impl->size)
    {
        m_impl->size  = size;
        m_impl->dirty = true;
        if (size.x && size.y)
            (void)m_impl->target->resize(size);
    }
}

#ifdef SFML_SYSTEM_ANDROID
void VulkanWindowSurface::setNativeWindow(WindowHandle handle)
{
    const auto lock = m_impl->device->lock();
    destroySurface();
    m_impl->handle = handle;
}
#endif

const std::shared_ptr<VulkanRenderTarget>& VulkanWindowSurface::renderTarget() const
{
    return m_impl->target;
}

bool VulkanWindowSurface::copyTo(const std::shared_ptr<VulkanImage>& destination, Vector2u offset) const
{
    const auto  lock   = m_impl->device->lock();
    const auto& source = m_impl->target->image();
    if (offset.x > destination->size.x || offset.y > destination->size.y ||
        source->size.x > destination->size.x - offset.x || source->size.y > destination->size.y - offset.y)
        return false;
    return m_impl->device->copy(source, destination, offset);
}

void VulkanWindowSurface::display()
{
    auto&      state  = *m_impl;
    auto&      device = *state.device;
    const auto lock   = device.lock();
#ifdef SFML_SYSTEM_HARMONY
    const auto snapshot = Harmony::getSurfaceSnapshot();
    if (state.surfaceGeneration != snapshot.generation)
    {
        destroySurface();
        state.handle            = snapshot.window;
        state.surfaceGeneration = snapshot.generation;
    }
#endif
    if (device.isLost() || !state.handle || !state.size.x || !state.size.y)
    {
        (void)device.flush();
        return;
    }

    if ((!state.surface && !createSurface()) || (state.dirty && !createSwapchain()))
    {
        (void)device.flush();
        return;
    }

    const auto& ctx       = *device.context();
    const auto& vk        = ctx.vk;
    const auto& swapchain = *state.swapchain;
    const auto  program   = device.defaultProgram();
    if (!program)
        return;

    VulkanPipelineState pipelineState{};
    pipelineState.colorFormat = swapchain.format;
    pipelineState.primitive   = PrimitiveType::TriangleStrip;
    pipelineState.blend       = BlendNone;
    pipelineState.present     = true;
    const VkPipeline pipeline = program->getPipeline(pipelineState, swapchain.pass);
    if (!pipeline)
        return;

    VulkanDrawParameters parameters{};
    parameters.transform        = {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f};
    parameters.textureTransform = {1.f, 1.f, 0.f, 0.f};
    const auto descriptors      = program->prepareDescriptors(device, parameters, {state.target->image()}, nullptr);
    constexpr std::array  quad{Vertex{{-1.f, -1.f}, Color::White, {0.f, 0.f}},
                              Vertex{{1.f, -1.f}, Color::White, {1.f, 0.f}},
                              Vertex{{-1.f, 1.f}, Color::White, {0.f, 1.f}},
                              Vertex{{1.f, 1.f}, Color::White, {1.f, 1.f}}};
    const auto            upload  = device.stage(quad.data(), sizeof(quad));
    const VkCommandBuffer command = device.commands();
    if (!descriptors.count || !upload.buffer || !command)
        return;

    const VkSwapchainKHR handle = swapchain.handle;
    std::uint32_t        index{};
    const auto acquire = vk.AcquireNextImageKHR(ctx.device, handle, 50'000'000, swapchain.acquired, VK_NULL_HANDLE, &index);
    if (acquire != VK_SUCCESS && acquire != VK_SUBOPTIMAL_KHR)
    {
        (void)device.flush();
        if (acquire == VK_ERROR_SURFACE_LOST_KHR)
            destroySurface();
        else if (acquire == VK_ERROR_OUT_OF_DATE_KHR)
            state.dirty = true;
        return;
    }

    const auto&           frame = swapchain.frames[index];
    VkRenderPassBeginInfo pass{};
    pass.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass.renderPass        = swapchain.pass;
    pass.framebuffer       = frame.framebuffer;
    pass.renderArea.extent = swapchain.extent;
    vk.CmdBeginRenderPass(command, &pass, VK_SUBPASS_CONTENTS_INLINE);
    program->bind(command, pipeline, descriptors);
    const VkViewport
        viewport{0.f, 0.f, static_cast<float>(swapchain.extent.width), static_cast<float>(swapchain.extent.height), 0.f, 1.f};
    const VkRect2D scissor{{0, 0}, swapchain.extent};
    vk.CmdSetViewport(command, 0, 1, &viewport);
    vk.CmdSetScissor(command, 0, 1, &scissor);
    vk.CmdSetStencilCompareMask(command, VK_STENCIL_FRONT_AND_BACK, ~0u);
    vk.CmdSetStencilWriteMask(command, VK_STENCIL_FRONT_AND_BACK, ~0u);
    vk.CmdSetStencilReference(command, VK_STENCIL_FRONT_AND_BACK, 0);
    vk.CmdBindVertexBuffers(command, 0, 1, &upload.buffer, &upload.offset);
    vk.CmdDraw(command, static_cast<std::uint32_t>(quad.size()), 1, 0, 0);
    vk.CmdEndRenderPass(command);
    if (!device.flush(swapchain.acquired, frame.ready))
        return;

    VkPresentInfoKHR present{};
    present.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores    = &frame.ready;
    present.swapchainCount     = 1;
    present.pSwapchains        = &handle;
    present.pImageIndices      = &index;
    const auto result          = vk.QueuePresentKHR(swapchain.presentQueue, &present);
    if (result == VK_ERROR_SURFACE_LOST_KHR)
        destroySurface();
    else if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || acquire == VK_SUBOPTIMAL_KHR)
        state.dirty = true;
}
} // namespace sf::priv
