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
#include <SFML/Graphics/RHI/Resource.hpp>

#include <unordered_map>
namespace sf::priv
{
class OpenGLProgram final : public ProgramResource
{
public:
    [[nodiscard]] const void* implementationType() const override
    {
        return resourceType<OpenGLProgram>();
    }
    OpenGLProgram(std::shared_ptr<void> context, bool programmable) :
        m_context(std::move(context)),
        m_programmable(programmable)
    {
    }
    ~OpenGLProgram() override;
    bool                                    compile(std::string_view          vertex,
                                                    std::string_view          geometry,
                                                    std::string_view          fragment,
                                                    const rhi::ShaderOptions& options = {});
    bool                                    loadDefault();
    [[nodiscard]] const rhi::ShaderOptions& getOptions() const override
    {
        return m_options;
    }
    [[nodiscard]] std::vector<rhi::UniformInfo> getActiveUniforms() const override;
    [[nodiscard]] bool                          bindVertexBuffers(const std::vector<rhi::VertexBufferBinding>& buffers,
                                                                  std::size_t                                  first,
                                                                  std::size_t                                  count,
                                                                  std::size_t                                  instances) const;
    [[nodiscard]] bool                          transformVertices(const std::vector<rhi::VertexBufferBinding>& buffers,
                                                                  std::size_t                                  count,
                                                                  const rhi::Buffer&                           output);

    void setUniform(const std::string& name, rhi::UniformType type, const void* values, std::size_t count) override;
    void setSampler(const std::string& name, std::shared_ptr<rhi::Texture> texture, bool current) override;
    [[nodiscard]] unsigned int nativeHandle() const override
    {
        return m_shaderProgram;
    }
    static void bind(const rhi::ShaderProgram* value);
    static bool geometryAvailable();

private:
    rhi::ShaderOptions m_options;
    struct UniformBinder;
    int                                                    getUniformLocation(const std::string& name);
    void                                                   bindTextures() const;
    std::shared_ptr<void>                                  m_context;
    bool                                                   m_programmable;
    unsigned int                                           m_shaderProgram{};
    int                                                    m_currentTexture{-1};
    std::unordered_map<int, std::shared_ptr<rhi::Texture>> m_textures;
    std::unordered_map<std::string, int>                   m_uniforms;
};
void bindOpenGLTexture(const rhi::Texture* texture, CoordinateType type, bool programmable);
} // namespace sf::priv
