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

#include <SFML/Graphics/Vulkan/VulkanProgram.hpp>
#include <SFML/Graphics/Vulkan/VulkanRHI.hpp>
#include <SFML/Graphics/Vulkan/VulkanRenderTarget.hpp>

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
namespace VulkanTargetImpl
{
struct DepthFormat
{
    VkFormat     format;
    unsigned int depth;
    unsigned int stencil;
};

DepthFormat chooseDepthFormat(const VulkanContext& ctx, const ContextSettings& settings)
{
    constexpr std::array formats{DepthFormat{VK_FORMAT_S8_UINT, 0, 8},
                                 DepthFormat{VK_FORMAT_D16_UNORM, 16, 0},
                                 DepthFormat{VK_FORMAT_X8_D24_UNORM_PACK32, 24, 0},
                                 DepthFormat{VK_FORMAT_D32_SFLOAT, 32, 0},
                                 DepthFormat{VK_FORMAT_D16_UNORM_S8_UINT, 16, 8},
                                 DepthFormat{VK_FORMAT_D24_UNORM_S8_UINT, 24, 8},
                                 DepthFormat{VK_FORMAT_D32_SFLOAT_S8_UINT, 32, 8}};
    if (!settings.depthBits && !settings.stencilBits)
        return {};

    DepthFormat  chosen{};
    unsigned int best = std::numeric_limits<unsigned int>::max();
    for (const auto& candidate : formats)
    {
        if ((settings.depthBits && !candidate.depth) || (settings.stencilBits && !candidate.stencil))
            continue;

        VkFormatProperties properties{};
        ctx.vk.GetPhysicalDeviceFormatProperties(ctx.physicalDevice, candidate.format, &properties);
        if (!(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
            continue;

        const auto distance = [](unsigned int actual, unsigned int requested)
        { return actual < requested ? 64u + std::min(requested - actual, 64u) : actual - requested; };
        const auto score = distance(candidate.depth, settings.depthBits) + distance(candidate.stencil, settings.stencilBits);
        if (score < best)
        {
            best   = score;
            chosen = candidate;
        }
    }
    return chosen;
}

VkSampleCountFlagBits chooseSamples(VkSampleCountFlags supported, unsigned int requested)
{
    for (const auto count :
         {VK_SAMPLE_COUNT_64_BIT,
          VK_SAMPLE_COUNT_32_BIT,
          VK_SAMPLE_COUNT_16_BIT,
          VK_SAMPLE_COUNT_8_BIT,
          VK_SAMPLE_COUNT_4_BIT,
          VK_SAMPLE_COUNT_2_BIT})
        if ((supported & static_cast<VkSampleCountFlags>(count)) && static_cast<unsigned int>(count) <= requested)
            return count;
    return VK_SAMPLE_COUNT_1_BIT;
}

VkSampleCountFlags supportedSamples(const VulkanContext& ctx, VkFormat format, VkImageUsageFlags usage)
{
    VkImageFormatProperties properties{};
    if (ctx.vk.GetPhysicalDeviceImageFormatProperties(ctx.physicalDevice,
                                                      format,
                                                      VK_IMAGE_TYPE_2D,
                                                      VK_IMAGE_TILING_OPTIMAL,
                                                      usage,
                                                      0,
                                                      &properties) != VK_SUCCESS)
        return 0;
    return properties.sampleCounts;
}

VkRect2D getScissor(const rhi::TargetState& target, Vector2u size)
{
    const auto rect  = target.scissor;
    const auto left  = std::clamp<std::int64_t>(rect.position.x, 0, size.x);
    const auto top   = std::clamp<std::int64_t>(rect.position.y, 0, size.y);
    const auto right = std::clamp<std::int64_t>(static_cast<std::int64_t>(rect.position.x) + rect.size.x, left, size.x);
    const auto bottom = std::clamp<std::int64_t>(static_cast<std::int64_t>(rect.position.y) + rect.size.y, top, size.y);
    return {{static_cast<std::int32_t>(left), static_cast<std::int32_t>(top)},
            {static_cast<std::uint32_t>(right - left), static_cast<std::uint32_t>(bottom - top)}};
}
} // namespace VulkanTargetImpl
} // namespace

struct VulkanRenderTarget::Attachments
{
    explicit Attachments(std::shared_ptr<VulkanContext> value) : context(std::move(value))
    {
    }

    ~Attachments()
    {
        if (framebuffer)
            context->vk.DestroyFramebuffer(context->device, framebuffer, nullptr);
        if (pass)
            context->vk.DestroyRenderPass(context->device, pass, nullptr);
    }

    std::shared_ptr<VulkanContext> context;
    std::shared_ptr<VulkanImage>   color;
    std::shared_ptr<VulkanImage>   multisample;
    std::shared_ptr<VulkanImage>   depth;
    VkSampleCountFlagBits          samples{VK_SAMPLE_COUNT_1_BIT};
    VkRenderPass                   pass{};
    VkFramebuffer                  framebuffer{};
};

std::shared_ptr<VulkanRenderTarget> VulkanRenderTarget::create(std::shared_ptr<VulkanDevice> device,
                                                               std::shared_ptr<VulkanImage>  image,
                                                               const ContextSettings&        settings)
{
    if (!image || !image->attachment)
        return {};

    auto       target = std::shared_ptr<VulkanRenderTarget>(new VulkanRenderTarget(std::move(device), settings));
    const auto lock   = target->m_device->lock();
    if (!target->attach(std::move(image)))
    {
        err() << "Could not create Vulkan render target attachments" << std::endl;
        return {};
    }
    return target;
}

VulkanRenderTarget::VulkanRenderTarget(std::shared_ptr<VulkanDevice> device, const ContextSettings& settings) :
    m_device(std::move(device)),
    m_requested(settings),
    m_settings(settings)
{
}

VulkanRenderTarget::~VulkanRenderTarget() = default;

bool VulkanRenderTarget::attach(std::shared_ptr<VulkanImage> image)
{
    using namespace VulkanTargetImpl;
    const auto& ctx         = *m_device->context();
    auto        attachments = std::make_shared<Attachments>(m_device->context());
    attachments->color      = std::move(image);
    const auto size         = attachments->color->size;
    const auto colorFormat  = attachments->color->format;
    const auto depth        = chooseDepthFormat(ctx, m_requested);
    auto       supported    = supportedSamples(ctx, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    if (depth.format != VK_FORMAT_UNDEFINED)
        supported &= supportedSamples(ctx, depth.format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    const auto samples   = chooseSamples(supported, m_requested.antiAliasingLevel);
    attachments->samples = samples;

    if (samples != VK_SAMPLE_COUNT_1_BIT)
    {
        attachments->multisample = m_device->makeImage(size,
                                                       colorFormat,
                                                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                       VK_IMAGE_ASPECT_COLOR_BIT,
                                                       samples);
        if (!attachments->multisample)
            return false;
    }
    if (depth.format != VK_FORMAT_UNDEFINED)
    {
        const VkImageAspectFlags aspects = (depth.depth ? VK_IMAGE_ASPECT_DEPTH_BIT : 0) |
                                           (depth.stencil ? VK_IMAGE_ASPECT_STENCIL_BIT : 0);
        attachments->depth = m_device->makeImage(size, depth.format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, aspects, samples);
        if (!attachments->depth)
            return false;
    }

    std::array<VkAttachmentDescription, 3> descriptions{};
    std::array<VkImageView, 3>             views{};
    std::uint32_t                          count{};
    const auto                             add = [&](const VulkanImage&    target,
                         VkSampleCountFlagBits sampleCount,
                         VkImageLayout         layout,
                         VkAttachmentLoadOp    load,
                         VkAttachmentLoadOp    stencilLoad)
    {
        auto& description          = descriptions[count];
        description.format         = target.format;
        description.samples        = sampleCount;
        description.loadOp         = load;
        description.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        description.stencilLoadOp  = stencilLoad;
        description.stencilStoreOp = stencilLoad == VK_ATTACHMENT_LOAD_OP_LOAD ? VK_ATTACHMENT_STORE_OP_STORE
                                                                               : VK_ATTACHMENT_STORE_OP_DONT_CARE;
        description.initialLayout = description.finalLayout = layout;
        views[count]                                        = target.attachment;
        return VkAttachmentReference{count++, layout};
    };

    const auto&           colorImage = attachments->multisample ? attachments->multisample : attachments->color;
    const auto            colorRef   = add(*colorImage,
                              samples,
                              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                              VK_ATTACHMENT_LOAD_OP_LOAD,
                              VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    VkAttachmentReference resolveRef{};
    if (attachments->multisample)
        resolveRef = add(*attachments->color,
                         VK_SAMPLE_COUNT_1_BIT,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                         VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                         VK_ATTACHMENT_LOAD_OP_DONT_CARE);
    VkAttachmentReference depthRef{};
    if (attachments->depth)
        depthRef = add(*attachments->depth,
                       samples,
                       VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                       depth.depth ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                       depth.stencil ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_DONT_CARE);

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorRef;
    subpass.pResolveAttachments     = attachments->multisample ? &resolveRef : nullptr;
    subpass.pDepthStencilAttachment = attachments->depth ? &depthRef : nullptr;
    VkRenderPassCreateInfo pass{};
    pass.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    pass.attachmentCount = count;
    pass.pAttachments    = descriptions.data();
    pass.subpassCount    = 1;
    pass.pSubpasses      = &subpass;
    if (ctx.vk.CreateRenderPass(ctx.device, &pass, nullptr, &attachments->pass) != VK_SUCCESS)
        return false;

    VkFramebufferCreateInfo framebuffer{};
    framebuffer.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer.renderPass      = attachments->pass;
    framebuffer.attachmentCount = count;
    framebuffer.pAttachments    = views.data();
    framebuffer.width           = size.x;
    framebuffer.height          = size.y;
    framebuffer.layers          = 1;
    if (ctx.vk.CreateFramebuffer(ctx.device, &framebuffer, nullptr, &attachments->framebuffer) != VK_SUCCESS)
        return false;

    m_attachments                = std::move(attachments);
    m_settings.depthBits         = depth.depth;
    m_settings.stencilBits       = depth.stencil;
    m_settings.antiAliasingLevel = samples == VK_SAMPLE_COUNT_1_BIT ? 0u : static_cast<unsigned int>(samples);
    m_settings.sRgbCapable       = colorFormat == VK_FORMAT_R8G8B8A8_SRGB;
    return true;
}

bool VulkanRenderTarget::resize(Vector2u size)
{
    const auto lock = m_device->lock();
    if (size == m_attachments->color->size)
        return true;
    if (!size.x || !size.y)
        return false;
    auto replacement = m_device->makeTexture(size, m_settings.sRgbCapable);
    return replacement && attach(std::move(replacement));
}

const ContextSettings& VulkanRenderTarget::settings() const
{
    return m_settings;
}

const std::shared_ptr<VulkanImage>& VulkanRenderTarget::image() const
{
    return m_attachments->color;
}

unsigned int VulkanRenderTarget::getMaximumAntiAliasingLevel(const VulkanDevice& device)
{
    const auto supported = VulkanTargetImpl::supportedSamples(*device.context(),
                                                              VK_FORMAT_R8G8B8A8_UNORM,
                                                              VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    const auto samples   = VulkanTargetImpl::chooseSamples(supported, 64);
    return samples == VK_SAMPLE_COUNT_1_BIT ? 0u : static_cast<unsigned int>(samples);
}

bool VulkanRenderTarget::setActive(bool)
{
    const auto lock = m_device->lock();
    return m_attachments && !m_device->isLost();
}

void VulkanRenderTarget::viewChanged()
{
}

void VulkanRenderTarget::finish()
{
    colorWritten();
    const auto lock = m_device->lock();
    if (m_device->commands())
    {
        m_device->transition(*image(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        m_device->retain(image());
        (void)m_device->flush();
    }
}

VkCommandBuffer VulkanRenderTarget::beginPass()
{
    const VkCommandBuffer command = m_device->commands();
    if (!command)
        return VK_NULL_HANDLE;

    m_device->transition(*m_attachments->color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    if (m_attachments->multisample)
        m_device->transition(*m_attachments->multisample, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    if (m_attachments->depth)
        m_device->transition(*m_attachments->depth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    VkRenderPassBeginInfo pass{};
    pass.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass.renderPass        = m_attachments->pass;
    pass.framebuffer       = m_attachments->framebuffer;
    pass.renderArea.extent = {image()->size.x, image()->size.y};
    m_device->context()->vk.CmdBeginRenderPass(command, &pass, VK_SUBPASS_CONTENTS_INLINE);
    m_device->retain(m_attachments);
    return command;
}

void VulkanRenderTarget::clear(const rhi::TargetState& target, std::optional<Color> color, std::optional<StencilValue> stencil)
{
    const auto lock = m_device->lock();
    const auto area = VulkanTargetImpl::getScissor(target, image()->size);
    if (!area.extent.width || !area.extent.height)
        return;

    std::array<VkClearAttachment, 2> attachments{};
    std::uint32_t                    count{};
    if (color)
    {
        auto& attachment            = attachments[count++];
        attachment.aspectMask       = VK_IMAGE_ASPECT_COLOR_BIT;
        attachment.clearValue.color = {{color->r / 255.f, color->g / 255.f, color->b / 255.f, color->a / 255.f}};
    }
    if (stencil && m_settings.stencilBits)
    {
        auto& attachment                           = attachments[count++];
        attachment.aspectMask                      = VK_IMAGE_ASPECT_STENCIL_BIT;
        attachment.clearValue.depthStencil.stencil = stencil->value;
    }
    if (!count)
        return;

    if (const VkCommandBuffer command = beginPass())
    {
        const VkClearRect rect{area, 0, 1};
        const auto&       vk = m_device->context()->vk;
        vk.CmdClearAttachments(command, count, attachments.data(), 1, &rect);
        vk.CmdEndRenderPass(command);
    }
}

void VulkanRenderTarget::draw(const rhi::TargetState&  target,
                              const Vertex*            vertices,
                              std::size_t              count,
                              PrimitiveType            type,
                              const rhi::DrawSettings& states)
{
    if (!vertices || !count)
        return;
    constexpr auto maxVertices = std::min<std::size_t>(std::numeric_limits<std::uint32_t>::max(),
                                                       std::numeric_limits<std::size_t>::max() / sizeof(Vertex));
    if (count > maxVertices)
        return;

    const auto lock   = m_device->lock();
    const auto upload = m_device->stage(vertices, count * sizeof(Vertex));
    if (upload.buffer)
        draw(target, upload, 0, count, type, states);
}

void VulkanRenderTarget::draw(const rhi::TargetState&  target,
                              const rhi::Buffer&       buffer,
                              std::size_t              first,
                              std::size_t              count,
                              PrimitiveType            type,
                              const rhi::DrawSettings& states)
{
    if (first > buffer.getSize() / sizeof(Vertex) || count > buffer.getSize() / sizeof(Vertex) - first)
        return;
    if (!nativeVulkanBuffer(buffer) || first > std::numeric_limits<std::uint32_t>::max() ||
        count > std::numeric_limits<std::uint32_t>::max() - first)
        return;

    const auto lock = m_device->lock();
    m_device->retain(nativeVulkanBuffer(buffer));
    draw(target, VulkanDevice::Upload{nativeVulkanBuffer(buffer)->buffer, 0}, first, count, type, states);
}

bool VulkanRenderTarget::drawVertices(
    const rhi::TargetState&                      target,
    const std::vector<rhi::VertexBufferBinding>& inputs,
    std::size_t                                  first,
    std::size_t                                  count,
    std::size_t                                  instances,
    PrimitiveType                                type,
    const rhi::DrawSettings&                     states)
{
    constexpr auto maximum = std::numeric_limits<std::uint32_t>::max();
    if (!count || !instances || first > maximum || count > maximum - first || instances > maximum)
        return false;
    const auto lock = m_device->lock();
    return draw(target, {}, first, count, type, states, &inputs, instances);
}

VulkanPipelineState VulkanRenderTarget::pipelineState(PrimitiveType type, const rhi::DrawSettings& states) const
{
    return {image()->format,
            m_attachments->depth ? m_attachments->depth->format : VK_FORMAT_UNDEFINED,
            m_attachments->samples,
            type,
            states.blendMode,
            states.stencilMode.stencilComparison,
            states.stencilMode.stencilUpdateOperation,
            states.stencilMode.stencilOnly};
}

bool VulkanRenderTarget::prewarm(const rhi::TargetState&, const rhi::DrawSettings& states)
{
    const auto lock    = m_device->lock();
    const auto program = states.shader ? resourcePointerCast<VulkanProgram>(states.shader) : m_device->defaultProgram();
    return program && program->getOptions().transformOutputs.empty() &&
           program->getPipeline(pipelineState(PrimitiveType::Triangles, states), m_attachments->pass) &&
           program->getPipeline(pipelineState(PrimitiveType::TriangleStrip, states), m_attachments->pass);
}

bool VulkanRenderTarget::draw(
    const rhi::TargetState&                      target,
    VulkanDevice::Upload                         buffer,
    std::size_t                                  first,
    std::size_t                                  count,
    PrimitiveType                                type,
    const rhi::DrawSettings&                     states,
    const std::vector<rhi::VertexBufferBinding>* inputs,
    std::size_t                                  instances)
{
    const auto rect    = target.viewport;
    const auto scissor = VulkanTargetImpl::getScissor(target, image()->size);
    if (rect.size.x <= 0 || rect.size.y <= 0 || !scissor.extent.width || !scissor.extent.height)
        return false;

    const auto program = states.shader ? resourcePointerCast<VulkanProgram>(states.shader) : m_device->defaultProgram();
    if (!program || !program->getOptions().transformOutputs.empty() ||
        (!inputs && !program->getOptions().vertexBindings.empty()))
        return false;
    const VkPipeline pipeline = program->getPipeline(pipelineState(type, states), m_attachments->pass);
    if (!pipeline)
        return false;

    auto parameters = prepareDraw(target, states);
    // Translated GLSL applies the clip-space conversion to its final vertex position.
    if (!program->getOptions().glslMatrixLayout)
        for (std::size_t column = 0; column < 4; ++column)
        {
            parameters.transform[4 * column + 1] *= -1.f;
            parameters.projection[4 * column + 1] *= -1.f;
        }

    auto       sample      = VulkanProgram::sample(states.texture.get());
    const auto descriptors = program->prepareDescriptors(*m_device, parameters, sample, image().get());
    if (!descriptors.count)
        return false;

    if (inputs && !program->bindVertexBuffers(*m_device, m_device->commands(), *inputs, first, count, instances))
        return false;
    if (const VkCommandBuffer command = beginPass())
    {
        const VkViewport viewport{static_cast<float>(rect.position.x),
                                  static_cast<float>(rect.position.y),
                                  static_cast<float>(rect.size.x),
                                  static_cast<float>(rect.size.y),
                                  0.f,
                                  1.f};
        const auto&      vk = m_device->context()->vk;
        program->bind(command, pipeline, descriptors);
        vk.CmdSetViewport(command, 0, 1, &viewport);
        vk.CmdSetScissor(command, 0, 1, &scissor);
        vk.CmdSetStencilCompareMask(command, VK_STENCIL_FRONT_AND_BACK, states.stencilMode.stencilMask.value);
        vk.CmdSetStencilWriteMask(command, VK_STENCIL_FRONT_AND_BACK, ~0u);
        vk.CmdSetStencilReference(command, VK_STENCIL_FRONT_AND_BACK, states.stencilMode.stencilReference.value);
        if (!inputs)
            vk.CmdBindVertexBuffers(command, 0, 1, &buffer.buffer, &buffer.offset);
        vk.CmdDraw(command,
                   static_cast<std::uint32_t>(count),
                   static_cast<std::uint32_t>(instances),
                   static_cast<std::uint32_t>(first),
                   0);
        vk.CmdEndRenderPass(command);
        m_device->retain(program);
        return true;
    }
    return false;
}
Vector2u VulkanRenderTarget::getSize() const
{
    return image()->size;
}
} // namespace sf::priv
