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
#include <SFML/Graphics/Vulkan/BuiltInShaders.hpp>
#include <SFML/Graphics/Vulkan/VulkanProgram.hpp>
#include <SFML/Graphics/Vulkan/VulkanRHI.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <limits>
#include <ostream>
#include <spirv_reflect.h>
#include <tuple>
#include <utility>

#include <cstring>

namespace sf::priv
{
namespace
{
namespace VulkanProgramImpl
{
struct Member
{
    std::string   name;
    std::uint32_t offset;
    std::uint32_t components;
    std::uint32_t columns;
    std::uint32_t matrixStride;
    std::uint32_t arrayLength;
    std::uint32_t arrayStride;
    bool          floating;
    bool          rowMajor;
};

struct Binding
{
    VkDescriptorSetLayoutBinding  layout{};
    std::string                   name;
    std::vector<Member>           members;
    std::vector<std::byte>        data;
    std::shared_ptr<rhi::Texture> texture;
    bool                          currentTexture{};
};

bool stencilFormat(VkFormat format)
{
    return format == VK_FORMAT_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
}

std::uint32_t maxSpirvVersion(std::uint32_t apiVersion)
{
    if (apiVersion >= VK_MAKE_VERSION(1, 3, 0))
        return 0x00010600;
    if (apiVersion >= VK_MAKE_VERSION(1, 2, 0))
        return 0x00010500;
    if (apiVersion >= VK_MAKE_VERSION(1, 1, 0))
        return 0x00010300;
    return 0x00010000;
}

template <std::size_t N>
std::string_view bytes(const std::array<std::uint32_t, N>& words)
{
    return {reinterpret_cast<const char*>(words.data()), sizeof(words)};
}

void reflectMembers(std::vector<Member>& members, const SpvReflectBlockVariable& block, const std::string& prefix)
{
    for (std::uint32_t i = 0; i < block.member_count; ++i)
    {
        const auto& value = block.members[i];
        if (!value.name)
            continue;
        const auto name = prefix + value.name;
        if (value.member_count && !value.array.dims_count)
        {
            reflectMembers(members, value, name + ".");
            continue;
        }

        const auto  flags   = value.type_description ? value.type_description->type_flags : 0;
        const auto& numeric = value.numeric;
        if (!(flags & (SPV_REFLECT_TYPE_FLAG_FLOAT | SPV_REFLECT_TYPE_FLAG_INT | SPV_REFLECT_TYPE_FLAG_BOOL)) ||
            (numeric.scalar.width != 32 && !(flags & SPV_REFLECT_TYPE_FLAG_BOOL)) || value.array.dims_count > 1)
            continue;

        Member member{name,
                      value.absolute_offset,
                      std::max({1u, numeric.vector.component_count, numeric.matrix.row_count}),
                      std::max(1u, numeric.matrix.column_count),
                      numeric.matrix.stride,
                      value.array.dims_count ? value.array.dims[0] : 1u,
                      value.array.stride,
                      (flags & SPV_REFLECT_TYPE_FLAG_FLOAT) != 0,
                      (value.decoration_flags & SPV_REFLECT_DECORATION_ROW_MAJOR) != 0};
        if (member.arrayLength)
            members.push_back(std::move(member));
    }
}

bool reflect(const VulkanContext&          context,
             const SpvReflectShaderModule& module,
             VkShaderStageFlagBits         stage,
             std::vector<Binding>&         bindings,
             bool                          transform,
             bool&                         storageOutput)
{
    const auto* entry = spvReflectGetEntryPoint(&module, "main");
    if (!entry || static_cast<VkShaderStageFlagBits>(entry->shader_stage) != stage)
        return false;

    std::uint32_t pushConstants{};
    if (spvReflectEnumerateEntryPointPushConstantBlocks(&module, "main", &pushConstants, nullptr) !=
            SPV_REFLECT_RESULT_SUCCESS ||
        pushConstants)
        return false;

    std::uint32_t count{};
    if (spvReflectEnumerateEntryPointDescriptorBindings(&module, "main", &count, nullptr) != SPV_REFLECT_RESULT_SUCCESS)
        return false;
    std::vector<SpvReflectDescriptorBinding*> descriptors(count);
    if (count && spvReflectEnumerateEntryPointDescriptorBindings(&module, "main", &count, descriptors.data()) !=
                     SPV_REFLECT_RESULT_SUCCESS)
        return false;

    for (const auto* descriptor : descriptors)
    {
        if (descriptor->set == 2 && descriptor->binding == 0 && descriptor->count == 1 && transform &&
            stage == VK_SHADER_STAGE_VERTEX_BIT && descriptor->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER)
        {
            storageOutput = true;
            continue;
        }
        const bool uniform = descriptor->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        const bool sampler = descriptor->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER &&
                             descriptor->image.dim == SpvDim2D && !descriptor->image.arrayed && !descriptor->image.ms;
        if (descriptor->count != 1 || (!uniform && !sampler) || descriptor->set > 1)
            return false;
        if (uniform && (!descriptor->block.padded_size ||
                        descriptor->block.padded_size > context.properties.limits.maxUniformBufferRange))
            return false;

        if (!descriptor->set)
        {
            if ((descriptor->binding == 0 && uniform && descriptor->block.padded_size <= sizeof(VulkanDrawParameters)) ||
                (descriptor->binding == 1 && sampler))
                continue;
            return false;
        }

        const auto type = uniform ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        auto       it   = std::find_if(bindings.begin(),
                               bindings.end(),
                               [&](const auto& binding) { return binding.layout.binding == descriptor->binding; });
        if (it == bindings.end())
        {
            Binding binding;
            binding.layout = {descriptor->binding, type, 1, static_cast<VkShaderStageFlags>(stage), nullptr};
            binding.name   = descriptor->name ? descriptor->name : "";
            // Like GLSL samplers left at unit 0, unassigned samplers sample the current texture
            binding.currentTexture = sampler;
            bindings.push_back(std::move(binding));
            it = std::prev(bindings.end());
        }
        else if (it->layout.descriptorType != type)
        {
            return false;
        }

        it->layout.stageFlags |= static_cast<VkShaderStageFlags>(stage);
        if (uniform)
        {
            it->data.resize(std::max(it->data.size(), static_cast<std::size_t>(descriptor->block.padded_size)));
            reflectMembers(it->members, descriptor->block, {});
        }
    }

    return true;
}

VkShaderModule makeModule(const VulkanContext&      context,
                          std::string_view          source,
                          VkShaderStageFlagBits     stage,
                          std::vector<Binding>&     bindings,
                          const rhi::ShaderOptions& options,
                          bool&                     feedback)
{
    const bool transform = !options.transformOutputs.empty();
    if (source.size() < 5 * sizeof(std::uint32_t) || source.size() % sizeof(std::uint32_t))
        return VK_NULL_HANDLE;

    std::vector<std::uint32_t> words(source.size() / sizeof(std::uint32_t));
    std::memcpy(words.data(), source.data(), source.size());
    if (words[0] != SpvMagicNumber || words[1] < 0x00010000 || words[1] > maxSpirvVersion(context.apiVersion))
        return VK_NULL_HANDLE;

    struct Reflection
    {
        SpvReflectShaderModule module{};
        ~Reflection()
        {
            spvReflectDestroyShaderModule(&module);
        }
    } reflection;
    bool storageOutput = false;
    if (spvReflectCreateShaderModule(source.size(), words.data(), &reflection.module) != SPV_REFLECT_RESULT_SUCCESS ||
        !reflect(context, reflection.module, stage, bindings, transform, storageOutput))
        return VK_NULL_HANDLE;
    const auto* entry = spvReflectGetEntryPoint(&reflection.module, "main");
    bool        xfb   = false;
    for (std::uint32_t i = 0; i < entry->execution_mode_count; ++i)
        xfb |= entry->execution_modes[i] == SpvExecutionModeXfb;
    if (transform != (xfb || storageOutput) || (xfb && storageOutput) ||
        (storageOutput && !context.features.vertexPipelineStoresAndAtomics))
        return VK_NULL_HANDLE;
    if (xfb)
    {
        const auto& limits = context.feedbackProperties;
        if (!context.bindFeedbackBuffers || options.transformStride > limits.maxTransformFeedbackBufferDataStride ||
            options.transformStride > limits.maxTransformFeedbackBufferDataSize ||
            options.transformStride > limits.maxTransformFeedbackStreamDataSize)
            return VK_NULL_HANDLE;
        // SPIRV-Reflect exposes Xfb execution mode but not buffer/stride decorations.
        bool hasStride = false;
        for (std::size_t index = 5; index < words.size();)
        {
            const auto length = words[index] >> 16;
            if (!length || length > words.size() - index)
                return VK_NULL_HANDLE;
            const auto op         = words[index] & 0xffffu;
            const auto decoration = op == SpvOpDecorate ? 2u : op == SpvOpMemberDecorate ? 3u : 0u;
            if (decoration && length > decoration + 1)
            {
                const auto kind  = words[index + decoration];
                const auto value = words[index + decoration + 1];
                if ((kind == SpvDecorationXfbBuffer && value != 0) ||
                    (kind == SpvDecorationXfbStride && value != options.transformStride))
                    return VK_NULL_HANDLE;
                hasStride |= kind == SpvDecorationXfbStride;
            }
            index += length;
        }
        if (!hasStride)
            return VK_NULL_HANDLE;
        feedback = true;
    }

    VkShaderModuleCreateInfo info{};
    info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = source.size();
    info.pCode    = words.data();
    VkShaderModule shader{};
    if (context.vk.CreateShaderModule(context.device, &info, nullptr, &shader) != VK_SUCCESS)
        return VK_NULL_HANDLE;

    return shader;
}

bool operator==(const VulkanPipelineState& lhs, const VulkanPipelineState& rhs)
{
    return std::tie(lhs.colorFormat,
                    lhs.depthFormat,
                    lhs.samples,
                    lhs.primitive,
                    lhs.blend,
                    lhs.comparison,
                    lhs.stencilOp,
                    lhs.stencilOnly,
                    lhs.present) ==
           std::tie(rhs.colorFormat,
                    rhs.depthFormat,
                    rhs.samples,
                    rhs.primitive,
                    rhs.blend,
                    rhs.comparison,
                    rhs.stencilOp,
                    rhs.stencilOnly,
                    rhs.present);
}

VkDescriptorSetLayout createLayout(const VulkanContext& context, const VkDescriptorSetLayoutBinding* bindings, std::uint32_t count)
{
    VkDescriptorSetLayoutCreateInfo info{};
    info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = count;
    info.pBindings    = bindings;
    VkDescriptorSetLayout layout{};
    if (context.vk.CreateDescriptorSetLayout(context.device, &info, nullptr, &layout) != VK_SUCCESS)
        return VK_NULL_HANDLE;
    return layout;
}
} // namespace VulkanProgramImpl
} // namespace

struct VulkanProgram::Impl
{
    explicit Impl(std::shared_ptr<VulkanContext> value) : context(std::move(value))
    {
    }

    std::shared_ptr<VulkanContext>                          context;
    std::vector<VkPipelineShaderStageCreateInfo>            stages;
    std::vector<VulkanProgramImpl::Binding>                 bindings;
    rhi::ShaderOptions                                      options;
    std::vector<VkVertexInputBindingDescription>            vertexBindings;
    std::vector<VkVertexInputAttributeDescription>          vertexAttributes;
    bool                                                    feedback{};
    VkRenderPass                                            transformPass{};
    VkFramebuffer                                           transformFramebuffer{};
    std::array<VkDescriptorSetLayout, 3>                    sets{};
    VkPipelineLayout                                        layout{};
    std::vector<std::pair<VulkanPipelineState, VkPipeline>> pipelines;
    std::mutex                                              uniformsMutex;
};

VulkanProgram::VulkanProgram(std::shared_ptr<VulkanContext> context) :
    m_impl(std::make_unique<Impl>(std::move(context)))
{
}

VulkanProgram::~VulkanProgram()
{
    const auto& ctx = *m_impl->context;
    for (const auto& pipeline : m_impl->pipelines)
        ctx.vk.DestroyPipeline(ctx.device, pipeline.second, nullptr);
    if (m_impl->transformFramebuffer)
        ctx.vk.DestroyFramebuffer(ctx.device, m_impl->transformFramebuffer, nullptr);
    if (m_impl->transformPass)
        ctx.vk.DestroyRenderPass(ctx.device, m_impl->transformPass, nullptr);
    if (m_impl->layout)
        ctx.vk.DestroyPipelineLayout(ctx.device, m_impl->layout, nullptr);
    for (const VkDescriptorSetLayout layout : m_impl->sets)
        if (layout)
            ctx.vk.DestroyDescriptorSetLayout(ctx.device, layout, nullptr);
    for (const auto& stage : m_impl->stages)
        ctx.vk.DestroyShaderModule(ctx.device, stage.module, nullptr);
}

std::shared_ptr<VulkanProgram> VulkanProgram::create(
    VulkanDevice&             device,
    std::string_view          vertex,
    std::string_view          geometry,
    std::string_view          fragment,
    const rhi::ShaderOptions& options)
{
    using namespace VulkanProgramImpl;
    const auto  lock      = device.lock();
    const auto& ctx       = *device.context();
    const bool  transform = !options.transformOutputs.empty();
    const auto& limits    = ctx.properties.limits;
    if (!validateShaderOptions(options) || options.vertexBindings.size() > limits.maxVertexInputBindings ||
        options.vertexAttributes.size() > limits.maxVertexInputAttributes ||
        (transform && ((!ctx.features.vertexPipelineStoresAndAtomics && !ctx.bindFeedbackBuffers) || vertex.empty() ||
                       !geometry.empty())) ||
        (!geometry.empty() && !ctx.features.geometryShader))
        return {};
    if (vertex.empty())
        vertex = bytes(VulkanShaders::vertex);
    if (transform)
        fragment = {};
    else if (fragment.empty())
        fragment = bytes(VulkanShaders::fragment);

    auto  result = std::shared_ptr<VulkanProgram>(new VulkanProgram(device.context()));
    auto& impl   = *result->m_impl;
    impl.options = options;
    constexpr std::array formats{VK_FORMAT_R32_SFLOAT,
                                 VK_FORMAT_R32G32_SFLOAT,
                                 VK_FORMAT_R32G32B32_SFLOAT,
                                 VK_FORMAT_R32G32B32A32_SFLOAT};
    for (const auto& layout : options.vertexBindings)
    {
        if (layout.stepRate > 1 || layout.stride > limits.maxVertexInputBindingStride)
            return {};
        impl.vertexBindings.push_back({static_cast<std::uint32_t>(impl.vertexBindings.size()),
                                       static_cast<std::uint32_t>(layout.stride),
                                       layout.stepRate ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX});
    }
    for (const auto& attribute : options.vertexAttributes)
    {
        if (attribute.location >= limits.maxVertexInputAttributes || attribute.offset > limits.maxVertexInputAttributeOffset)
            return {};
        impl.vertexAttributes.push_back(
            {attribute.location,
             attribute.binding,
             formats[attribute.components - 1],
             static_cast<std::uint32_t>(attribute.offset)});
    }
    for (const auto& [stage, source] : {std::pair{VK_SHADER_STAGE_VERTEX_BIT, vertex},
                                        std::pair{VK_SHADER_STAGE_GEOMETRY_BIT, geometry},
                                        std::pair{VK_SHADER_STAGE_FRAGMENT_BIT, fragment}})
    {
        if (source.empty())
            continue;
        const VkShaderModule module = makeModule(ctx, source, stage, impl.bindings, options, impl.feedback);
        if (!module)
        {
            err() << "Could not load Vulkan SPIR-V for the requested API version, stage, and SFML shader layout"
                  << std::endl;
            return {};
        }
        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage  = stage;
        stageInfo.module = module;
        stageInfo.pName  = "main";
        impl.stages.push_back(stageInfo);
    }

    const VkShaderStageFlags flags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT |
                                     (geometry.empty() ? 0u : static_cast<VkShaderStageFlags>(VK_SHADER_STAGE_GEOMETRY_BIT));
    const std::array common{VkDescriptorSetLayoutBinding{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, flags, nullptr},
                            VkDescriptorSetLayoutBinding{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, flags, nullptr}};
    impl.sets[0] = createLayout(ctx, common.data(), static_cast<std::uint32_t>(common.size()));
    if (!impl.sets[0])
        return {};

    if (!impl.bindings.empty() || (transform && !impl.feedback))
    {
        std::vector<VkDescriptorSetLayoutBinding> layout;
        layout.reserve(impl.bindings.size());
        for (const auto& binding : impl.bindings)
            layout.push_back(binding.layout);
        impl.sets[1] = createLayout(ctx, layout.data(), static_cast<std::uint32_t>(layout.size()));
        if (!impl.sets[1])
            return {};
    }

    if (transform)
    {
        if (!impl.feedback)
        {
            const VkDescriptorSetLayoutBinding output{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr};
            impl.sets[2] = createLayout(ctx, &output, 1);
            if (!impl.sets[2])
                return {};
        }
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        VkRenderPassCreateInfo pass{};
        pass.sType        = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        pass.subpassCount = 1;
        pass.pSubpasses   = &subpass;
        if (ctx.vk.CreateRenderPass(ctx.device, &pass, nullptr, &impl.transformPass) != VK_SUCCESS)
            return {};
        VkFramebufferCreateInfo framebuffer{};
        framebuffer.sType      = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer.renderPass = impl.transformPass;
        framebuffer.width = framebuffer.height = framebuffer.layers = 1;
        if (ctx.vk.CreateFramebuffer(ctx.device, &framebuffer, nullptr, &impl.transformFramebuffer) != VK_SUCCESS)
            return {};
    }

    VkPipelineLayoutCreateInfo pipeline{};
    pipeline.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline.setLayoutCount = impl.sets[2] ? 3u : impl.sets[1] ? 2u : 1u;
    pipeline.pSetLayouts    = impl.sets.data();
    if (ctx.vk.CreatePipelineLayout(ctx.device, &pipeline, nullptr, &impl.layout) != VK_SUCCESS)
        return {};
    return result;
}

const rhi::ShaderOptions& VulkanProgram::getOptions() const
{
    return m_impl->options;
}

std::string VulkanProgram::translatedName(std::string name) const
{
    for (const auto& alias : getOptions().uniformAliases)
        if (alias.name == name)
            return alias.translatedName;
    return name;
}

std::vector<rhi::UniformInfo> VulkanProgram::getActiveUniforms() const
{
    std::vector<rhi::UniformInfo> result;
    const auto                    append = [&](rhi::UniformInfo value)
    {
        for (const auto& alias : getOptions().uniformAliases)
            if (alias.translatedName == value.name)
                value.name = alias.name;
        for (const auto& transform : getOptions().samplerCoordinateTransforms)
            if (transform.uniformName == value.name)
                return;
        if (std::none_of(result.begin(), result.end(), [&](const auto& previous) { return previous.name == value.name; }))
            result.push_back(std::move(value));
    };
    for (const auto& binding : m_impl->bindings)
    {
        if (binding.layout.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            append({binding.name, rhi::UniformType::Int, 1, true});
        for (const auto& member : binding.members)
        {
            if (member.columns != 1 &&
                (member.columns != member.components || !member.floating || member.columns < 3 || member.columns > 4))
                continue;
            const auto type = member.columns == 1
                                  ? static_cast<rhi::UniformType>((member.floating ? 0u : 4u) + member.components - 1)
                              : member.columns == 3 ? rhi::UniformType::Mat3
                                                    : rhi::UniformType::Mat4;
            append({member.name, type, member.arrayLength, false});
        }
    }
    return result;
}

void VulkanProgram::setUniform(const std::string& name, ShaderUniformType type, const void* values, std::size_t count)
{
    const std::lock_guard lock(m_impl->uniformsMutex);
    if (!writeUniform(translatedName(name), type, values, count))
        err() << "Vulkan uniform not found or has incompatible type: " << name << std::endl;
}

bool VulkanProgram::writeUniform(const std::string& name, ShaderUniformType type, const void* values, std::size_t count)
{
    if (!values || !count)
        return false;
    const auto  index      = static_cast<unsigned int>(type);
    const bool  matrix     = type == ShaderUniformType::Mat3 || type == ShaderUniformType::Mat4;
    const auto  columns    = matrix ? (type == ShaderUniformType::Mat3 ? 3u : 4u) : 1u;
    const auto  components = matrix ? columns : (index % 4) + 1;
    const bool  floating   = index < 4 || matrix;
    const auto* data       = static_cast<const std::byte*>(values);

    bool found = false;
    for (auto& binding : m_impl->bindings)
    {
        for (const auto& member : binding.members)
        {
            if (member.name != name && (binding.name.empty() || binding.name + "." + member.name != name))
                continue;
            if (member.components != components || member.columns != columns || member.floating != floating ||
                count > member.arrayLength || (count > 1 && !member.arrayStride) || (matrix && !member.matrixStride))
                continue;

            const std::size_t end = member.offset + (count - 1) * member.arrayStride +
                                    (columns - 1) * member.matrixStride + components * sizeof(std::uint32_t);
            if (end > binding.data.size())
                continue;

            for (std::size_t element = 0; element < count; ++element)
            {
                auto* destination = binding.data.data() + member.offset + element * member.arrayStride;
                for (std::size_t column = 0; column < columns; ++column)
                {
                    for (std::size_t row = 0; row < components; ++row)
                    {
                        const auto sourceIndex = element * columns * components + column * components + row;
                        const auto offset = member.rowMajor ? row * member.matrixStride + column * sizeof(std::uint32_t)
                                                            : column * member.matrixStride + row * sizeof(std::uint32_t);
                        std::memcpy(destination + offset, data + sourceIndex * sizeof(std::uint32_t), sizeof(std::uint32_t));
                    }
                }
            }
            found = true;
        }
    }

    return found;
}

void VulkanProgram::setSampler(const std::string& name, std::shared_ptr<rhi::Texture> texture, bool current)
{
    const std::lock_guard lock(m_impl->uniformsMutex);
    const auto            samplerName = translatedName(name);
    bool                  found       = false;
    for (auto& binding : m_impl->bindings)
    {
        if (binding.layout.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER && binding.name == samplerName)
        {
            binding.texture        = texture;
            binding.currentTexture = current;
            found                  = true;
        }
    }

    if (!found && !(current && name == "sf_Texture"))
        err() << "Vulkan sampler not found: " << name << std::endl;
}

VkPipeline VulkanProgram::getPipeline(const VulkanPipelineState& state, VkRenderPass pass)
{
    using namespace VulkanProgramImpl;
    for (const auto& pipeline : m_impl->pipelines)
        if (pipeline.first == state)
            return pipeline.second;

    constexpr std::array topologies{VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
                                    VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
                                    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
                                    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
                                    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN};
    constexpr std::array factors{VK_BLEND_FACTOR_ZERO,
                                 VK_BLEND_FACTOR_ONE,
                                 VK_BLEND_FACTOR_SRC_COLOR,
                                 VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
                                 VK_BLEND_FACTOR_DST_COLOR,
                                 VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
                                 VK_BLEND_FACTOR_SRC_ALPHA,
                                 VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                                 VK_BLEND_FACTOR_DST_ALPHA,
                                 VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA};
    constexpr std::array
        equations{VK_BLEND_OP_ADD, VK_BLEND_OP_SUBTRACT, VK_BLEND_OP_REVERSE_SUBTRACT, VK_BLEND_OP_MIN, VK_BLEND_OP_MAX};
    constexpr std::array comparisons{VK_COMPARE_OP_NEVER,
                                     VK_COMPARE_OP_LESS,
                                     VK_COMPARE_OP_LESS_OR_EQUAL,
                                     VK_COMPARE_OP_GREATER,
                                     VK_COMPARE_OP_GREATER_OR_EQUAL,
                                     VK_COMPARE_OP_EQUAL,
                                     VK_COMPARE_OP_NOT_EQUAL,
                                     VK_COMPARE_OP_ALWAYS};
    constexpr std::array operations{VK_STENCIL_OP_KEEP,
                                    VK_STENCIL_OP_ZERO,
                                    VK_STENCIL_OP_REPLACE,
                                    VK_STENCIL_OP_INCREMENT_AND_CLAMP,
                                    VK_STENCIL_OP_DECREMENT_AND_CLAMP,
                                    VK_STENCIL_OP_INVERT};

    constexpr VkVertexInputBindingDescription binding{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
    constexpr std::array
        attributes{VkVertexInputAttributeDescription{0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position)},
                   VkVertexInputAttributeDescription{1, 0, VK_FORMAT_R8G8B8A8_UNORM, offsetof(Vertex, color)},
                   VkVertexInputAttributeDescription{2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoords)}};
    VkPipelineVertexInputStateCreateInfo vertex{};
    vertex.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex.vertexBindingDescriptionCount   = 1;
    vertex.pVertexBindingDescriptions      = &binding;
    vertex.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attributes.size());
    vertex.pVertexAttributeDescriptions    = attributes.data();

    if (!m_impl->vertexBindings.empty() && !state.present)
    {
        vertex.vertexBindingDescriptionCount   = static_cast<std::uint32_t>(m_impl->vertexBindings.size());
        vertex.pVertexBindingDescriptions      = m_impl->vertexBindings.data();
        vertex.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(m_impl->vertexAttributes.size());
        vertex.pVertexAttributeDescriptions    = m_impl->vertexAttributes.data();
    }
    VkPipelineInputAssemblyStateCreateInfo assembly{};
    assembly.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly.topology = topologies[static_cast<std::size_t>(state.primitive)];

    VkPipelineViewportStateCreateInfo viewport{};
    viewport.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport.viewportCount = 1;
    viewport.scissorCount  = 1;

    VkPipelineRasterizationStateCreateInfo raster{};
    raster.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster.polygonMode             = VK_POLYGON_MODE_FILL;
    raster.cullMode                = VK_CULL_MODE_NONE;
    raster.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    raster.rasterizerDiscardEnable = !m_impl->options.transformOutputs.empty();
    raster.lineWidth               = 1.f;

    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = state.samples;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType             = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.stencilTestEnable = stencilFormat(state.depthFormat) &&
                                     (state.comparison != StencilMode{}.stencilComparison ||
                                      state.stencilOp != StencilUpdateOperation::Keep);
    depthStencil.front.failOp      = VK_STENCIL_OP_KEEP;
    depthStencil.front.passOp      = operations[static_cast<std::size_t>(state.stencilOp)];
    depthStencil.front.depthFailOp = depthStencil.front.passOp;
    depthStencil.front.compareOp   = comparisons[static_cast<std::size_t>(state.comparison)];
    depthStencil.back              = depthStencil.front;

    VkPipelineColorBlendAttachmentState blendAttachment{};
    blendAttachment.blendEnable         = state.blend != BlendNone;
    blendAttachment.srcColorBlendFactor = factors[static_cast<std::size_t>(state.blend.colorSrcFactor)];
    blendAttachment.dstColorBlendFactor = factors[static_cast<std::size_t>(state.blend.colorDstFactor)];
    blendAttachment.colorBlendOp        = equations[static_cast<std::size_t>(state.blend.colorEquation)];
    blendAttachment.srcAlphaBlendFactor = factors[static_cast<std::size_t>(state.blend.alphaSrcFactor)];
    blendAttachment.dstAlphaBlendFactor = factors[static_cast<std::size_t>(state.blend.alphaDstFactor)];
    blendAttachment.alphaBlendOp        = equations[static_cast<std::size_t>(state.blend.alphaEquation)];
    blendAttachment.colorWriteMask      = state.stencilOnly ? 0
                                                            : VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                             VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo blend{};
    blend.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend.attachmentCount = 1;
    blend.pAttachments    = &blendAttachment;

    constexpr std::array             dynamicStates{VK_DYNAMIC_STATE_VIEWPORT,
                                       VK_DYNAMIC_STATE_SCISSOR,
                                       VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
                                       VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
                                       VK_DYNAMIC_STATE_STENCIL_REFERENCE};
    VkPipelineDynamicStateCreateInfo dynamic{};
    dynamic.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size());
    dynamic.pDynamicStates    = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipeline{};
    pipeline.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.stageCount          = static_cast<std::uint32_t>(m_impl->stages.size());
    pipeline.pStages             = m_impl->stages.data();
    pipeline.pVertexInputState   = &vertex;
    pipeline.pInputAssemblyState = &assembly;
    pipeline.pViewportState      = &viewport;
    pipeline.pRasterizationState = &raster;
    pipeline.pMultisampleState   = &multisample;
    pipeline.pDepthStencilState  = &depthStencil;
    pipeline.pColorBlendState    = &blend;
    pipeline.pDynamicState       = &dynamic;
    pipeline.layout              = m_impl->layout;
    pipeline.renderPass          = pass;

    const auto& ctx = *m_impl->context;
    VkPipeline  result{};
    if (ctx.vk.CreateGraphicsPipelines(ctx.device, ctx.pipelineCache, 1, &pipeline, nullptr, &result) != VK_SUCCESS)
    {
        err() << "Failed to create Vulkan graphics pipeline" << std::endl;
        return VK_NULL_HANDLE;
    }

    m_impl->pipelines.emplace_back(state, result);
    return result;
}

VulkanSample VulkanProgram::sample(const rhi::Texture* texture)
{
    return texture ? VulkanSample{nativeVulkanImage(texture),
                                  texture->isSmooth(),
                                  texture->isRepeated(),
                                  texture->hasMipmap(),
                                  texture->glslFlipped()}
                   : VulkanSample{};
}

VulkanProgram::Descriptors VulkanProgram::prepareDescriptors(
    VulkanDevice&               device,
    const VulkanDrawParameters& parameters,
    const VulkanSample&         texture,
    const VulkanImage*          attachment,
    const VulkanBuffer*         output,
    VkDeviceSize                outputBytes)
{
    const std::lock_guard lock(m_impl->uniformsMutex);
    const int             textureEnabled = texture.image ? 1 : 0;
    (void)writeUniform(translatedName("sf_TextureEnabled"), ShaderUniformType::Int, &textureEnabled, 1);
    for (const auto& coordinate : getOptions().samplerCoordinateTransforms)
    {
        const auto samplerName = translatedName(coordinate.name);
        bool       flipped     = texture.glslFlipped;
        for (const auto& binding : m_impl->bindings)
            if (binding.layout.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER &&
                binding.name == samplerName && !binding.currentTexture)
                flipped = binding.texture && binding.texture->glslFlipped();
        const std::array<float, 4> transform{1.f, flipped ? -1.f : 1.f, 0.f, flipped ? 1.f : 0.f};
        (void)writeUniform(translatedName(coordinate.uniformName), ShaderUniformType::Vec4, transform.data(), 1);
    }
    Descriptors result;
    if ((output != nullptr) != (m_impl->sets[2] != VK_NULL_HANDLE))
        return {};
    result.count   = output ? 3u : m_impl->sets[1] ? 2u : 1u;
    result.sets[0] = device.allocateDescriptorSet(m_impl->sets[0], 1, 1);
    if (!result.sets[0])
        return {};
    if (result.count >= 2)
    {
        const auto buffers = static_cast<std::uint32_t>(
            std::count_if(m_impl->bindings.begin(),
                          m_impl->bindings.end(),
                          [](const auto& binding)
                          { return binding.layout.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }));
        const auto images = static_cast<std::uint32_t>(m_impl->bindings.size()) - buffers;
        result.sets[1]    = device.allocateDescriptorSet(m_impl->sets[1], buffers, images);
        if (!result.sets[1])
            return {};
    }

    std::vector<VkDescriptorBufferInfo> buffers;
    std::vector<VkDescriptorImageInfo>  images;
    std::vector<VkWriteDescriptorSet>   writes;
    const auto                          maximumWrites = m_impl->bindings.size() + 2;
    buffers.reserve(maximumWrites);
    images.reserve(maximumWrites);
    writes.reserve(maximumWrites);

    const auto writeBuffer = [&](VkDescriptorSet set, std::uint32_t binding, const void* data, std::size_t size)
    {
        const auto upload = device.stage(data, size);
        if (!upload.buffer)
            return false;

        buffers.push_back({upload.buffer, upload.offset, size});
        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet          = set;
        write.dstBinding      = binding;
        write.descriptorCount = 1;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo     = &buffers.back();
        writes.push_back(write);
        return true;
    };

    const auto writeImage = [&](VkDescriptorSet set, std::uint32_t binding, VulkanSample sample)
    {
        if (!sample.image)
            sample.image = device.whiteTexture();
        if (!sample.image || sample.image.get() == attachment)
            return false;

        const VkSampler sampler = device.sampler(sample.smooth, sample.repeated, sample.mipmapped);
        if (!sampler)
            return false;
        device.transition(*sample.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        device.retain(sample.image);
        images.push_back({sampler, sample.image->view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet          = set;
        write.dstBinding      = binding;
        write.descriptorCount = 1;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo      = &images.back();
        writes.push_back(write);
        return true;
    };

    if (!writeBuffer(result.sets[0], 0, &parameters, sizeof(parameters)) || !writeImage(result.sets[0], 1, texture))
        return {};

    for (const auto& binding : m_impl->bindings)
    {
        if (binding.layout.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        {
            if (!writeBuffer(result.sets[1], binding.layout.binding, binding.data.data(), binding.data.size()))
                return {};
        }
        else if (!writeImage(result.sets[1],
                             binding.layout.binding,
                             binding.currentTexture ? texture : sample(binding.texture.get())))
        {
            return {};
        }
    }

    VkDescriptorBufferInfo storage{};
    if (output)
    {
        result.sets[2] = device.allocateDescriptorSet(m_impl->sets[2], 0, 0);
        if (!result.sets[2])
            return {};
        storage = {output->buffer, 0, outputBytes};
        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet          = result.sets[2];
        write.descriptorCount = 1;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write.pBufferInfo     = &storage;
        writes.push_back(write);
    }
    const auto& ctx = *m_impl->context;
    ctx.vk.UpdateDescriptorSets(ctx.device, static_cast<std::uint32_t>(writes.size()), writes.data(), 0, nullptr);
    return result;
}

bool VulkanProgram::bindVertexBuffers(
    VulkanDevice&                                device,
    VkCommandBuffer                              command,
    const std::vector<rhi::VertexBufferBinding>& inputs,
    std::size_t                                  first,
    std::size_t                                  count,
    std::size_t                                  instances) const
{
    rhi::ShaderOptions standard;
    standard.vertexBindings.push_back({sizeof(Vertex), 0});
    if (!command || m_impl->context != device.context() ||
        !validateVertexBindings(getOptions().vertexBindings.empty() ? standard : getOptions(), inputs, first, count, instances))
        return false;
    std::vector<VkBuffer>     buffers;
    std::vector<VkDeviceSize> offsets;
    for (const auto& input : inputs)
    {
        const auto buffer = nativeVulkanBuffer(*input.buffer);
        if (!buffer || buffer->context != m_impl->context || input.offset >= buffer->capacity)
            return false;
        buffers.push_back(buffer->buffer);
        offsets.push_back(input.offset);
        device.retain(buffer);
    }
    device.context()
        ->vk.CmdBindVertexBuffers(command, 0, static_cast<std::uint32_t>(buffers.size()), buffers.data(), offsets.data());
    return true;
}

bool VulkanProgram::transformVertices(VulkanDevice&                                device,
                                      const std::vector<rhi::VertexBufferBinding>& inputs,
                                      std::size_t                                  count,
                                      const rhi::Buffer&                           output)
{
    const auto buffer = nativeVulkanBuffer(output);
    const auto limit  = m_impl->feedback ? m_impl->context->feedbackProperties.maxTransformFeedbackBufferSize
                                         : m_impl->context->properties.limits.maxStorageBufferRange;
    if (!m_impl->transformPass || !count || count > std::numeric_limits<std::uint32_t>::max() || !buffer ||
        buffer->context != m_impl->context || device.context() != m_impl->context ||
        count > std::min<VkDeviceSize>(output.getSize(), limit) / getOptions().transformStride ||
        std::any_of(inputs.begin(), inputs.end(), [&](const auto& input) { return input.buffer.get() == &output; }))
        return false;
    VulkanPipelineState state;
    state.primitive           = PrimitiveType::Points;
    const VkPipeline pipeline = getPipeline(state, m_impl->transformPass);
    if (!pipeline)
        return false;
    const VkCommandBuffer command = device.commands();
    if (!bindVertexBuffers(device, command, inputs, 0, count, 1))
        return false;
    const VkDeviceSize bytes = count * getOptions().transformStride;
    const auto descriptors = prepareDescriptors(device, {}, {}, nullptr, m_impl->feedback ? nullptr : buffer.get(), bytes);
    if (!descriptors.count)
        return false;
    const auto&                vk         = device.context()->vk;
    const VkPipelineStageFlags writeStage = m_impl->feedback ? VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT
                                                             : VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    const VkAccessFlags writeAccess = m_impl->feedback ? VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT : VK_ACCESS_SHADER_WRITE_BIT;
    // A ping-pong destination can still be an input to an earlier draw.
    VkMemoryBarrier before{};
    before.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    before.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | writeAccess;
    before.dstAccessMask = writeAccess;
    vk.CmdPipelineBarrier(command, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | writeStage, writeStage, 0, 1, &before, 0, nullptr, 0, nullptr);
    VkRenderPassBeginInfo pass{};
    pass.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass.renderPass        = m_impl->transformPass;
    pass.framebuffer       = m_impl->transformFramebuffer;
    pass.renderArea.extent = {1, 1};
    vk.CmdBeginRenderPass(command, &pass, VK_SUBPASS_CONTENTS_INLINE);
    bind(command, pipeline, descriptors);
    if (m_impl->feedback)
    {
        constexpr VkDeviceSize offset = 0;
        m_impl->context->bindFeedbackBuffers(command, 0, 1, &buffer->buffer, &offset, &bytes);
        m_impl->context->beginFeedback(command, 0, 0, nullptr, nullptr);
    }
    vk.CmdDraw(command, static_cast<std::uint32_t>(count), 1, 0, 0);
    if (m_impl->feedback)
        m_impl->context->endFeedback(command, 0, 0, nullptr, nullptr);
    vk.CmdEndRenderPass(command);
    VkMemoryBarrier after{};
    after.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    after.srcAccessMask = writeAccess;
    after.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_HOST_READ_BIT;
    vk.CmdPipelineBarrier(command,
                          writeStage,
                          VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_HOST_BIT,
                          0,
                          1,
                          &after,
                          0,
                          nullptr,
                          0,
                          nullptr);
    device.retain(buffer);
    return true;
}

void VulkanProgram::bind(VkCommandBuffer commands, VkPipeline pipeline, const Descriptors& descriptors) const
{
    const auto& vk = m_impl->context->vk;
    vk.CmdBindPipeline(commands, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vk.CmdBindDescriptorSets(commands,
                             VK_PIPELINE_BIND_POINT_GRAPHICS,
                             m_impl->layout,
                             0,
                             descriptors.count,
                             descriptors.sets.data(),
                             0,
                             nullptr);
}
} // namespace sf::priv
