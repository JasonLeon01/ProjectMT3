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
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RHI/Resource.hpp>
#include <SFML/Graphics/ShaderUniformType.hpp>
#include <SFML/Graphics/StencilMode.hpp>
#include <SFML/Graphics/Vulkan/VulkanDevice.hpp>

#include <array>
#include <string>
#include <string_view>

namespace sf::priv
{
using VulkanDrawParameters = DrawParameters;

struct VulkanSample
{
    std::shared_ptr<VulkanImage> image;
    bool                         smooth{};
    bool                         repeated{};
    bool                         mipmapped{};
    bool                         glslFlipped{};
};

struct VulkanPipelineState
{
    VkFormat               colorFormat{};
    VkFormat               depthFormat{};
    VkSampleCountFlagBits  samples{VK_SAMPLE_COUNT_1_BIT};
    PrimitiveType          primitive{PrimitiveType::Triangles};
    BlendMode              blend;
    StencilComparison      comparison{StencilMode{}.stencilComparison};
    StencilUpdateOperation stencilOp{StencilUpdateOperation::Keep};
    bool                   stencilOnly{};
    bool                   present{};
};

class VulkanProgram : public ProgramResource
{
public:
    [[nodiscard]] const void* implementationType() const override
    {
        return resourceType<VulkanProgram>();
    }
    struct Descriptors
    {
        std::array<VkDescriptorSet, 3> sets{};
        std::uint32_t                  count{};
    };

    [[nodiscard]] static std::shared_ptr<VulkanProgram> create(
        VulkanDevice&             device,
        std::string_view          vertex   = {},
        std::string_view          geometry = {},
        std::string_view          fragment = {},
        const rhi::ShaderOptions& options  = {});
    ~VulkanProgram() override;

    VulkanProgram(const VulkanProgram&)            = delete;
    VulkanProgram& operator=(const VulkanProgram&) = delete;

    [[nodiscard]] const rhi::ShaderOptions&     getOptions() const override;
    [[nodiscard]] std::vector<rhi::UniformInfo> getActiveUniforms() const override;
    bool                                        bindVertexBuffers(VulkanDevice&                                device,
                                                                  VkCommandBuffer                              command,
                                                                  const std::vector<rhi::VertexBufferBinding>& inputs,
                                                                  std::size_t                                  first,
                                                                  std::size_t                                  count,
                                                                  std::size_t                                  instances) const;
    bool                                        transformVertices(VulkanDevice&                                device,
                                                                  const std::vector<rhi::VertexBufferBinding>& inputs,
                                                                  std::size_t                                  count,
                                                                  const rhi::Buffer&                           output);
    void setUniform(const std::string& name, ShaderUniformType type, const void* values, std::size_t count) override;
    void setSampler(const std::string& name, std::shared_ptr<rhi::Texture> texture, bool current) override;

    [[nodiscard]] VkPipeline  getPipeline(const VulkanPipelineState& state, VkRenderPass pass);
    [[nodiscard]] Descriptors prepareDescriptors(
        VulkanDevice&               device,
        const VulkanDrawParameters& parameters,
        const VulkanSample&         texture,
        const VulkanImage*          attachment,
        const VulkanBuffer*         output      = nullptr,
        VkDeviceSize                outputBytes = 0);
    void bind(VkCommandBuffer commands, VkPipeline pipeline, const Descriptors& descriptors) const;
    [[nodiscard]] static VulkanSample sample(const rhi::Texture* texture);

private:
    bool writeUniform(const std::string& name, ShaderUniformType type, const void* values, std::size_t count);
    [[nodiscard]] std::string translatedName(std::string name) const;
    explicit VulkanProgram(std::shared_ptr<VulkanContext> context);

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
} // namespace sf::priv
