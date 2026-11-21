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

#include <SFML/Graphics/GraphicsDevice.hpp>
#include <SFML/Graphics/RHI/Access.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTargetBackend.hpp>

#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowImpl.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <limits>
#include <ostream>
#ifdef SFML_SYSTEM_ANDROID
#include <SFML/System/Android/Activity.hpp>
#endif

namespace sf::priv
{
namespace
{
std::uint64_t resourceId()
{
    static std::atomic<std::uint64_t> next{1};
    return next.fetch_add(1, std::memory_order_relaxed);
}
} // namespace
TextureResource::TextureResource() : m_cacheId(resourceId())
{
}
void TextureResource::changed()
{
    m_cacheId   = resourceId();
    m_hasMipmap = false;
}
std::array<float, 16> TextureResource::coordinateMatrix(CoordinateType type) const
{
    std::array matrix{1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f};
    if (!m_actualSize.x || !m_actualSize.y)
        return matrix;
    const auto width  = static_cast<float>(m_actualSize.x);
    const auto height = static_cast<float>(m_actualSize.y);
    matrix[0]         = (type == CoordinateType::Pixels ? 1.f : static_cast<float>(m_size.x)) / width;
    matrix[5]         = (type == CoordinateType::Pixels ? 1.f : static_cast<float>(m_size.y)) / height;
    if (m_pixelsFlipped)
    {
        matrix[5]  = -matrix[5];
        matrix[13] = static_cast<float>(m_size.y) / height;
    }
    return matrix;
}
void DeviceResource::bindTexture(const rhi::Texture*, CoordinateType)
{
    err() << "Texture::bind requires OpenGL" << std::endl;
}
void DeviceResource::bindBuffer(const rhi::Buffer*)
{
    err() << "VertexBuffer::bind requires OpenGL" << std::endl;
}
void DeviceResource::bindShader(const rhi::ShaderProgram*)
{
    err() << "Shader::bind requires OpenGL" << std::endl;
}
bool DeviceResource::acceptsSettings(const ContextSettings& settings) const
{
    return withGraphicsDevice(settings,
                              [&](const GraphicsDevice& value, const ContextSettings&)
                              { return value.device.get() == this; });
}
#ifdef SFML_SYSTEM_ANDROID
void GraphicsWindowSurface::setNativeWindow(WindowHandle handle)
{
    static_cast<SurfaceResource&>(*surface).setNativeWindow(handle);
}
namespace
{
// A standalone RHI surface participates in Android's native-window callbacks too.
class AndroidSurface final : public rhi::WindowSurface, public priv::WindowSurface
{
public:
    explicit AndroidSurface(std::unique_ptr<rhi::WindowSurface> value) : m_surface(std::move(value))
    {
        auto&                 activity = getActivity();
        const std::lock_guard lock(activity.mutex);
        activity.surface = this;
    }
    ~AndroidSurface() override
    {
        auto&                 activity = getActivity();
        const std::lock_guard lock(activity.mutex);
        if (activity.surface == this)
            activity.surface = nullptr;
    }
    [[nodiscard]] std::shared_ptr<rhi::RenderTarget> getRenderTarget() const override
    {
        return m_surface->getRenderTarget();
    }
    [[nodiscard]] const ContextSettings& getSettings() const override
    {
        return getRenderTarget()->settings();
    }
    bool setActive(bool active) override
    {
        return m_surface->setActive(active);
    }
    void resize(Vector2u size) override
    {
        m_surface->resize(size);
    }
    void display() override
    {
        m_surface->display();
    }
    void setVerticalSyncEnabled(bool value) override
    {
        m_surface->setVerticalSyncEnabled(value);
    }
    bool copyTo(rhi::Texture& destination, Vector2u offset) const override
    {
        return m_surface->copyTo(destination, offset);
    }
    void setNativeWindow(WindowHandle handle) override
    {
        static_cast<SurfaceResource&>(*m_surface).setNativeWindow(handle);
    }

private:
    std::unique_ptr<rhi::WindowSurface> m_surface;
};
} // namespace
#endif
void RenderTargetBackend::pushGLStates(const rhi::TargetState&)
{
    err() << "OpenGL state operations require an OpenGL render target" << std::endl;
}
void RenderTargetBackend::popGLStates(const rhi::TargetState& state)
{
    pushGLStates(state);
}
void RenderTargetBackend::resetGLStates(const rhi::TargetState& state)
{
    pushGLStates(state);
}

bool validateShaderOptions(const rhi::ShaderOptions& options)
{
    if (options.vertexBindings.empty() && !options.vertexAttributes.empty())
        return false;
    for (const auto& binding : options.vertexBindings)
        if (!binding.stride || binding.stride > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            return false;
    for (std::size_t index = 0; index < options.vertexAttributes.size(); ++index)
    {
        const auto& attribute = options.vertexAttributes[index];
        if (attribute.name.empty() || attribute.semantic.empty() ||
            attribute.binding >= options.vertexBindings.size() || !attribute.components || attribute.components > 4)
            return false;
        const auto stride = options.vertexBindings[attribute.binding].stride;
        if (attribute.offset > stride || attribute.components * sizeof(float) > stride - attribute.offset)
            return false;
        for (std::size_t previous = 0; previous < index; ++previous)
            if (options.vertexAttributes[previous].location == attribute.location)
                return false;
    }
    std::size_t stride = 0;
    for (const auto& output : options.transformOutputs)
    {
        if (output.name.empty() || output.semantic.empty() || !output.components || output.components > 4 || stride > 2048)
            return false;
        stride += output.components * sizeof(float);
    }
    return options.transformOutputs.empty() ||
           (!options.vertexBindings.empty() && stride == options.transformStride && stride <= 2048);
}
bool validateVertexBindings(const rhi::ShaderOptions&                    options,
                            const std::vector<rhi::VertexBufferBinding>& bindings,
                            std::size_t                                  first,
                            std::size_t                                  count,
                            std::size_t                                  instances)
{
    if (bindings.size() != options.vertexBindings.size() || first > std::numeric_limits<std::size_t>::max() - count)
        return false;
    for (std::size_t index = 0; index < bindings.size(); ++index)
    {
        const auto& binding = bindings[index];
        const auto& layout  = options.vertexBindings[index];
        if (!binding.buffer || !layout.stride || binding.offset > binding.buffer->getSize())
            return false;
        const auto elements = layout.stepRate ? (instances ? 1 + (instances - 1) / layout.stepRate : 0) : first + count;
        if (elements > (binding.buffer->getSize() - binding.offset) / layout.stride)
            return false;
    }
    return true;
}

ClearDraw prepareClear(std::optional<Color> color, std::optional<StencilValue> stencil)
{
    const auto value = color.value_or(Color::White);
    ClearDraw  result{{{{{-1.f, -1.f}, value},
                        {{1.f, -1.f}, value},
                        {{-1.f, 1.f}, value},
                        {{-1.f, 1.f}, value},
                        {{1.f, -1.f}, value},
                        {{1.f, 1.f}, value}}},
                      {}};
    result.settings.blendMode               = BlendNone;
    result.settings.stencilMode.stencilOnly = !color;
    if (stencil)
    {
        result.settings.stencilMode.stencilUpdateOperation = StencilUpdateOperation::Replace;
        result.settings.stencilMode.stencilReference       = StencilValue(stencil->value & 0xFFu);
    }
    return result;
}

DrawParameters prepareDraw(const rhi::TargetState& target, const rhi::DrawSettings& state, bool clearing)
{
    DrawParameters result{};
    const auto     model      = clearing ? Transform::Identity : state.transform;
    const auto     projection = clearing ? Transform::Identity : target.projection;
    std::copy_n((projection * model).getMatrix(), 16, result.transform.begin());
    std::copy_n(model.getMatrix(), 16, result.modelView.begin());
    std::copy_n(projection.getMatrix(), 16, result.projection.begin());
    result.textureTransform = {1.f, 1.f, 0.f, 0.f};
    result.targetSize       = {static_cast<float>(target.size.x),
                               static_cast<float>(target.size.y),
                         target.size.x ? 1.f / static_cast<float>(target.size.x) : 0.f,
                         target.size.y ? 1.f / static_cast<float>(target.size.y) : 0.f};
    if (state.texture)
    {
        const auto matrix       = state.texture->coordinateMatrix(state.coordinateType);
        result.textureTransform = {matrix[0], matrix[5], matrix[12], matrix[13]};
        if (state.shader && state.shader->getOptions().glslMatrixLayout && state.texture->glslFlipped())
        {
            result.textureTransform[1] = -result.textureTransform[1];
            result.textureTransform[3] = 1.f - result.textureTransform[3];
        }
    }
    return result;
}
rhi::DrawSettings prepareState(const RenderStates& state)
{
    return {state.blendMode,
            state.stencilMode,
            state.transform,
            state.coordinateType,
            state.texture ? RhiAccess::texture(*state.texture) : nullptr,
            state.shader ? RhiAccess::shader(*state.shader) : nullptr};
}
rhi::TargetState prepareTarget(const RenderTarget& target)
{
    const auto size    = target.getSize();
    const auto scissor = target.getScissor(target.getView()).findIntersection(IntRect({}, Vector2i(size)));
    return {size,
            target.getViewport(target.getView()),
            scissor.value_or(IntRect{}),
            target.getView().getTransform(),
            target.isSrgb()};
}
bool RhiAccess::copyWindow(const Window& window, rhi::Texture& texture, Vector2u offset)
{
    if (const auto* adapter = resourceCast<const GraphicsWindowSurface>(window.getSurface()))
        return adapter->surface->copyTo(texture, offset);
    // Plain sf::Window exposes an OpenGL context rather than an RHI surface.
    if (const auto device = rhi::getDevice())
        return static_cast<DeviceResource&>(*device).copyWindow(window, texture, offset);
    return false;
}
} // namespace sf::priv

namespace sf::rhi
{
std::shared_ptr<Device> getDevice()
{
    const auto* value = priv::getGraphicsDevice();
    return value ? value->device : nullptr;
}
std::unique_ptr<WindowSurface> Device::createWindowSurface(WindowBase& window, const ContextSettings& settings)
{
    const auto* owner = priv::RhiAccess::window(window);
    if (!owner)
        return {};
    std::unique_ptr<WindowSurface> surface;
    (void)priv::withGraphicsDevice(settings,
                                   [&](const priv::GraphicsDevice& device, const ContextSettings& effective)
                                   {
                                       if (device.device.get() != this)
                                           return false;
                                       surface = static_cast<priv::DeviceResource&>(*this)
                                                     .createSurface(*owner, VideoMode::getDesktopMode().bitsPerPixel, effective);
                                       return surface != nullptr;
                                   });
#ifdef SFML_SYSTEM_ANDROID
    if (surface)
        return std::make_unique<priv::AndroidSurface>(std::move(surface));
#endif
    return surface;
}
} // namespace sf::rhi
