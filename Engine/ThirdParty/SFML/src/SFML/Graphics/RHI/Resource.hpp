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

#include <SFML/Graphics/RHI/Device.hpp>

#include <SFML/Window/WindowSurface.hpp>

#include <atomic>
#include <type_traits>

namespace sf
{
class Window;
}

namespace sf::priv
{
template <typename Type>
const void* resourceType()
{
    static char tag{};
    return &tag;
}
template <typename Target, typename Source>
Target* resourceCast(Source* value)
{
    return value && value->implementationType() == resourceType<std::remove_cv_t<Target>>()
               ? static_cast<Target*>(value)
               : nullptr;
}
template <typename Target, typename Source>
std::shared_ptr<Target> resourcePointerCast(const std::shared_ptr<Source>& value)
{
    auto* pointer = resourceCast<Target>(value.get());
    return pointer ? std::shared_ptr<Target>(value, pointer) : nullptr;
}

class WindowImpl;


class TextureResource : public rhi::Texture
{
public:
    TextureResource();
    TextureResource(const TextureResource&)                  = delete;
    TextureResource&       operator=(const TextureResource&) = delete;
    [[nodiscard]] Vector2u getSize() const override
    {
        return m_size;
    }
    [[nodiscard]] bool isSrgb() const override
    {
        return m_sRgb;
    }
    [[nodiscard]] bool isSmooth() const override
    {
        return m_isSmooth;
    }
    [[nodiscard]] bool isRepeated() const override
    {
        return m_isRepeated;
    }
    [[nodiscard]] bool hasMipmap() const override
    {
        return m_hasMipmap;
    }
    void setSmooth(bool value) override
    {
        m_isSmooth = value;
    }
    void setRepeated(bool value) override
    {
        m_isRepeated = value;
    }
    void invalidateMipmap() override
    {
        m_hasMipmap = false;
    }
    [[nodiscard]] std::array<float, 16> coordinateMatrix(CoordinateType type) const override;
    [[nodiscard]] virtual unsigned int  nativeHandle() const
    {
        return 0;
    }
    [[nodiscard]] std::uint64_t revision() const
    {
        return m_cacheId;
    }
    [[nodiscard]] bool attachment() const
    {
        return m_fboAttachment;
    }
    void changed();
    void setGlslFlipped(bool value)
    {
        m_glslFlipped = value;
    }
    [[nodiscard]] bool glslFlipped() const override
    {
        return m_glslFlipped;
    }

protected:
    Vector2u      m_size;
    Vector2u      m_actualSize;
    bool          m_isSmooth{};
    bool          m_sRgb{};
    bool          m_isRepeated{};
    mutable bool  m_pixelsFlipped{};
    bool          m_fboAttachment{};
    bool          m_glslFlipped{};
    bool          m_hasMipmap{};
    std::uint64_t m_cacheId{};
};

class BufferResource : public rhi::Buffer
{
public:
    BufferResource()                                                    = default;
    BufferResource(const BufferResource&)                               = delete;
    BufferResource&                    operator=(const BufferResource&) = delete;
    [[nodiscard]] virtual unsigned int nativeHandle() const
    {
        return 0;
    }
};

class ProgramResource : public rhi::ShaderProgram
{
public:
    ProgramResource()                                                    = default;
    ProgramResource(const ProgramResource&)                              = delete;
    ProgramResource&                   operator=(const ProgramResource&) = delete;
    [[nodiscard]] virtual unsigned int nativeHandle() const
    {
        return 0;
    }
};

class DeviceResource : public rhi::Device
{
public:
    [[nodiscard]] bool                          acceptsSettings(const ContextSettings& settings) const;
    virtual std::unique_ptr<rhi::WindowSurface> createSurface(const WindowImpl&      owner,
                                                              unsigned int           bitsPerPixel,
                                                              const ContextSettings& settings) = 0;
    virtual bool                                copyWindow(const Window&, rhi::Texture&, Vector2u)
    {
        return false;
    }
    virtual void bindTexture(const rhi::Texture* texture, CoordinateType type);
    virtual void bindBuffer(const rhi::Buffer* buffer);
    virtual void bindShader(const rhi::ShaderProgram* shader);
};

class SurfaceResource : public rhi::WindowSurface
{
public:
#ifdef SFML_SYSTEM_ANDROID
    virtual void setNativeWindow(WindowHandle handle) = 0;
#endif
};

/// Keeps Window independent of Graphics; the Window module continues to own this adapter.
class GraphicsWindowSurface final : public WindowSurface
{
public:
    [[nodiscard]] const void* implementationType() const override
    {
        return resourceType<GraphicsWindowSurface>();
    }
    explicit GraphicsWindowSurface(std::unique_ptr<rhi::WindowSurface> value) : surface(std::move(value))
    {
    }
    [[nodiscard]] const ContextSettings& getSettings() const override
    {
        return surface->getRenderTarget()->settings();
    }
    bool setActive(bool active) override
    {
        return surface->setActive(active);
    }
    void display() override
    {
        surface->display();
    }
    void setVerticalSyncEnabled(bool enabled) override
    {
        surface->setVerticalSyncEnabled(enabled);
    }
#ifdef SFML_SYSTEM_ANDROID
    void setNativeWindow(WindowHandle handle) override;
#endif
    std::unique_ptr<rhi::WindowSurface> surface;
};

struct DrawParameters
{
    std::array<float, 16> transform;
    std::array<float, 4>  textureTransform;
    std::array<float, 16> modelView;
    std::array<float, 16> projection;
    std::array<float, 4>  targetSize;
};

struct ClearDraw
{
    std::array<Vertex, 6> vertices;
    rhi::DrawSettings     settings;
};

[[nodiscard]] bool validateShaderOptions(const rhi::ShaderOptions& options);
[[nodiscard]] bool validateVertexBindings(const rhi::ShaderOptions&                    options,
                                          const std::vector<rhi::VertexBufferBinding>& bindings,
                                          std::size_t                                  first,
                                          std::size_t                                  count,
                                          std::size_t                                  instances);

[[nodiscard]] ClearDraw prepareClear(std::optional<Color> color, std::optional<StencilValue> stencil);

[[nodiscard]] DrawParameters prepareDraw(const rhi::TargetState& target, const rhi::DrawSettings& state, bool clearing = false);
[[nodiscard]] std::unique_ptr<rhi::WindowSurface> createWindowSurface(rhi::Device&           device,
                                                                      WindowBase&            window,
                                                                      const ContextSettings& settings);
} // namespace sf::priv
