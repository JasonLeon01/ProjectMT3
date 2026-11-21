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

#include <SFML/Graphics/DirectX/DirectXDevice.hpp>
#include <SFML/Graphics/RenderTargetBackend.hpp>

namespace sf::priv
{
class DirectXRenderTarget final : public RenderTargetBackend
{
public:
    [[nodiscard]] static std::shared_ptr<DirectXRenderTarget> create(std::shared_ptr<DirectXDevice> device,
                                                                     std::shared_ptr<DirectXImage>  image,
                                                                     const ContextSettings&         settings);
    ~DirectXRenderTarget() override;

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
        const std::vector<rhi::VertexBufferBinding>& buffers,
        std::size_t                                  first,
        std::size_t                                  count,
        std::size_t                                  instances,
        PrimitiveType                                type,
        const rhi::DrawSettings&                     states) override;
    [[nodiscard]] bool prewarm(const rhi::TargetState& target, const rhi::DrawSettings& states) override;
    bool               setActive(bool active) override;
    void               viewChanged() override;
    [[nodiscard]] bool resize(Vector2u size);
    void               finish() override;
    [[nodiscard]] const ContextSettings&               settings() const override;
    [[nodiscard]] const std::shared_ptr<DirectXImage>& image() const;
    [[nodiscard]] static unsigned int                  getMaximumAntiAliasingLevel(const DirectXDevice& device);

private:
    DirectXRenderTarget(std::shared_ptr<DirectXDevice> device, const ContextSettings& settings);
    [[nodiscard]] bool attach(std::shared_ptr<DirectXImage> image);
    void               drawVertices(const rhi::TargetState&  target,
                                    const Vertex*            vertices,
                                    std::size_t              count,
                                    PrimitiveType            type,
                                    const rhi::DrawSettings& states,
                                    bool                     clearing);
    void               drawBuffer(const rhi::TargetState&                      target,
                                  ID3D11Buffer*                                buffer,
                                  std::size_t                                  first,
                                  std::size_t                                  count,
                                  PrimitiveType                                type,
                                  const rhi::DrawSettings&                     states,
                                  bool                                         clearing    = false,
                                  const std::vector<rhi::VertexBufferBinding>* bindings    = nullptr,
                                  std::size_t                                  instances   = 1,
                                  bool                                         prewarmOnly = false);
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
} // namespace sf::priv
