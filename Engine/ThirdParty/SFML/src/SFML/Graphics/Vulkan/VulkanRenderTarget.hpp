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

#include <SFML/Graphics/RenderTargetBackend.hpp>
#include <SFML/Graphics/Vulkan/VulkanDevice.hpp>

namespace sf::priv
{
struct VulkanPipelineState;
class VulkanRenderTarget final : public RenderTargetBackend
{
public:
    [[nodiscard]] static std::shared_ptr<VulkanRenderTarget> create(std::shared_ptr<VulkanDevice> device,
                                                                    std::shared_ptr<VulkanImage>  image,
                                                                    const ContextSettings&        settings);
    ~VulkanRenderTarget() override;

    [[nodiscard]] Vector2u getSize() const override;
    void clear(const rhi::TargetState& target, std::optional<Color> color, std::optional<StencilValue> stencil) override;
    void               draw(const rhi::TargetState&  target,
                            const Vertex*            vertices,
                            std::size_t              count,
                            PrimitiveType            type,
                            const rhi::DrawSettings& states) override;
    void               draw(const rhi::TargetState&  target,
                            const rhi::Buffer&       buffer,
                            std::size_t              first,
                            std::size_t              count,
                            PrimitiveType            type,
                            const rhi::DrawSettings& states) override;
    [[nodiscard]] bool drawVertices(
        const rhi::TargetState&                      target,
        const std::vector<rhi::VertexBufferBinding>& inputs,
        std::size_t                                  first,
        std::size_t                                  count,
        std::size_t                                  instances,
        PrimitiveType                                type,
        const rhi::DrawSettings&                     states) override;
    [[nodiscard]] bool prewarm(const rhi::TargetState& target, const rhi::DrawSettings& states) override;
    bool               setActive(bool active) override;
    void               viewChanged() override;

    [[nodiscard]] bool                                resize(Vector2u size);
    void                                              finish() override;
    [[nodiscard]] const ContextSettings&              settings() const override;
    [[nodiscard]] const std::shared_ptr<VulkanImage>& image() const;
    [[nodiscard]] static unsigned int                 getMaximumAntiAliasingLevel(const VulkanDevice& device);

private:
    struct Attachments;
    explicit VulkanRenderTarget(std::shared_ptr<VulkanDevice> device, const ContextSettings& settings);
    [[nodiscard]] bool                attach(std::shared_ptr<VulkanImage> image);
    [[nodiscard]] VkCommandBuffer     beginPass();
    [[nodiscard]] VulkanPipelineState pipelineState(PrimitiveType type, const rhi::DrawSettings& states) const;
    bool                              draw(const rhi::TargetState&                      target,
                                           VulkanDevice::Upload                         buffer,
                                           std::size_t                                  first,
                                           std::size_t                                  count,
                                           PrimitiveType                                type,
                                           const rhi::DrawSettings&                     states,
                                           const std::vector<rhi::VertexBufferBinding>* inputs    = nullptr,
                                           std::size_t                                  instances = 1);

    std::shared_ptr<VulkanDevice> m_device;
    ContextSettings               m_requested;
    ContextSettings               m_settings;
    std::shared_ptr<Attachments>  m_attachments;
};
} // namespace sf::priv
