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
#include <SFML/Graphics/Metal/MetalDevice.hpp>
#include <SFML/Graphics/RHI/Resource.hpp>
#include <SFML/Graphics/ShaderUniformType.hpp>

#include <array>
#include <string>
#include <string_view>

#include <cstddef>

namespace sf::priv
{
using MetalDrawParameters = DrawParameters;

// Shaders may declare only the leading sf_ModelViewProjectionMatrix and sf_TextureTransform members
inline constexpr std::size_t metalDrawPrefixSize = offsetof(MetalDrawParameters, modelView);

class MetalProgram : public ProgramResource
{
public:
    struct Impl;

    [[nodiscard]] static std::shared_ptr<MetalProgram> create(MetalDevice&     device,
                                                              std::string_view vertex   = {},
                                                              std::string_view fragment = {});
    [[nodiscard]] static std::shared_ptr<MetalProgram> create(
        MetalDevice&     device,
        std::string_view vertex,
        std::string_view geometry,
        std::string_view fragment)
    {
        return geometry.empty() ? create(device, vertex, fragment) : nullptr;
    }
    [[nodiscard]] static std::shared_ptr<MetalProgram> create(
        MetalDevice&              device,
        std::string_view          vertex,
        std::string_view          geometry,
        std::string_view          fragment,
        const rhi::ShaderOptions& options);
    [[nodiscard]] const void* implementationType() const override
    {
        return resourceType<MetalProgram>();
    }
    [[nodiscard]] std::shared_ptr<rhi::ShaderProgram> clone() const override;
    [[nodiscard]] const rhi::ShaderOptions&           getOptions() const override;
    [[nodiscard]] std::vector<rhi::UniformInfo>       getActiveUniforms() const override;
    [[nodiscard]] bool                                transformVertices(MetalDevice&                                 device,
                                                                        const std::vector<rhi::VertexBufferBinding>& bindings,
                                                                        std::size_t                                  count,
                                                                        const rhi::Buffer&                           output);
    ~MetalProgram() override;
    MetalProgram(const MetalProgram&)            = delete;
    MetalProgram& operator=(const MetalProgram&) = delete;

    void setUniform(const std::string& name, ShaderUniformType type, const void* values, std::size_t count) override;
    void setSampler(const std::string& name, std::shared_ptr<rhi::Texture> texture, bool current) override;
    [[nodiscard]] Impl& impl() const;

private:
    MetalProgram();
    std::unique_ptr<Impl> m_impl;
};
} // namespace sf::priv
