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

#include <SFML/Graphics/DirectX/DirectXProgram.hpp>
#include <SFML/Graphics/DirectX/DirectXRHI.hpp>
#include <SFML/Graphics/DirectX/DirectXRenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <limits>
#include <map>
#include <ostream>
#include <utility>
#include <vector>

#include <cstring>

namespace sf::priv
{
namespace
{
namespace DirectXTargetImpl
{
D3D11_BLEND factor(BlendMode::Factor value, bool alpha)
{
    switch (value)
    {
        case BlendMode::Factor::Zero:
            return D3D11_BLEND_ZERO;
        case BlendMode::Factor::One:
            return D3D11_BLEND_ONE;
        case BlendMode::Factor::SrcColor:
            return alpha ? D3D11_BLEND_SRC_ALPHA : D3D11_BLEND_SRC_COLOR;
        case BlendMode::Factor::OneMinusSrcColor:
            return alpha ? D3D11_BLEND_INV_SRC_ALPHA : D3D11_BLEND_INV_SRC_COLOR;
        case BlendMode::Factor::DstColor:
            return alpha ? D3D11_BLEND_DEST_ALPHA : D3D11_BLEND_DEST_COLOR;
        case BlendMode::Factor::OneMinusDstColor:
            return alpha ? D3D11_BLEND_INV_DEST_ALPHA : D3D11_BLEND_INV_DEST_COLOR;
        case BlendMode::Factor::SrcAlpha:
            return D3D11_BLEND_SRC_ALPHA;
        case BlendMode::Factor::OneMinusSrcAlpha:
            return D3D11_BLEND_INV_SRC_ALPHA;
        case BlendMode::Factor::DstAlpha:
            return D3D11_BLEND_DEST_ALPHA;
        case BlendMode::Factor::OneMinusDstAlpha:
            return D3D11_BLEND_INV_DEST_ALPHA;
    }
    return D3D11_BLEND_ONE;
}
D3D11_BLEND_OP equation(BlendMode::Equation value)
{
    switch (value)
    {
        case BlendMode::Equation::Add:
            return D3D11_BLEND_OP_ADD;
        case BlendMode::Equation::Subtract:
            return D3D11_BLEND_OP_SUBTRACT;
        case BlendMode::Equation::ReverseSubtract:
            return D3D11_BLEND_OP_REV_SUBTRACT;
        case BlendMode::Equation::Min:
            return D3D11_BLEND_OP_MIN;
        case BlendMode::Equation::Max:
            return D3D11_BLEND_OP_MAX;
    }
    return D3D11_BLEND_OP_ADD;
}
D3D11_COMPARISON_FUNC comparison(StencilComparison value)
{
    switch (value)
    {
        case StencilComparison::Never:
            return D3D11_COMPARISON_NEVER;
        case StencilComparison::Less:
            return D3D11_COMPARISON_LESS;
        case StencilComparison::LessEqual:
            return D3D11_COMPARISON_LESS_EQUAL;
        case StencilComparison::Greater:
            return D3D11_COMPARISON_GREATER;
        case StencilComparison::GreaterEqual:
            return D3D11_COMPARISON_GREATER_EQUAL;
        case StencilComparison::Equal:
            return D3D11_COMPARISON_EQUAL;
        case StencilComparison::NotEqual:
            return D3D11_COMPARISON_NOT_EQUAL;
        case StencilComparison::Always:
            return D3D11_COMPARISON_ALWAYS;
    }
    return D3D11_COMPARISON_ALWAYS;
}
D3D11_STENCIL_OP stencilOperation(StencilUpdateOperation value)
{
    switch (value)
    {
        case StencilUpdateOperation::Keep:
            return D3D11_STENCIL_OP_KEEP;
        case StencilUpdateOperation::Zero:
            return D3D11_STENCIL_OP_ZERO;
        case StencilUpdateOperation::Replace:
            return D3D11_STENCIL_OP_REPLACE;
        case StencilUpdateOperation::Increment:
            return D3D11_STENCIL_OP_INCR_SAT;
        case StencilUpdateOperation::Decrement:
            return D3D11_STENCIL_OP_DECR_SAT;
        case StencilUpdateOperation::Invert:
            return D3D11_STENCIL_OP_INVERT;
    }
    return D3D11_STENCIL_OP_KEEP;
}
D3D11_PRIMITIVE_TOPOLOGY topology(PrimitiveType value)
{
    switch (value)
    {
        case PrimitiveType::Points:
            return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        case PrimitiveType::Lines:
            return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        case PrimitiveType::LineStrip:
            return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
        case PrimitiveType::TriangleStrip:
            return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        case PrimitiveType::Triangles:
        case PrimitiveType::TriangleFan:
            return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
    return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}
} // namespace DirectXTargetImpl
} // namespace

struct DirectXRenderTarget::Impl
{
    Impl(std::shared_ptr<DirectXDevice> value, const ContextSettings& configuration) :
        device(std::move(value)),
        requested(configuration)
    {
    }
    std::shared_ptr<DirectXDevice>                                         device;
    ContextSettings                                                        requested;
    ContextSettings                                                        settings;
    std::shared_ptr<DirectXImage>                                          image;
    ComPtr<ID3D11Texture2D>                                                multisample;
    ComPtr<ID3D11RenderTargetView>                                         color;
    ComPtr<ID3D11Texture2D>                                                depth;
    ComPtr<ID3D11DepthStencilView>                                         depthView;
    ComPtr<ID3D11RasterizerState>                                          rasterizer;
    ComPtr<ID3D11Buffer>                                                   stream;
    ComPtr<ID3D11Buffer>                                                   fanIndices;
    UINT                                                                   fanIndexCapacity{};
    UINT                                                                   streamCapacity{};
    bool                                                                   dirty{};
    std::map<std::array<unsigned int, 7>, ComPtr<ID3D11BlendState>>        blends;
    std::map<std::array<unsigned int, 3>, ComPtr<ID3D11DepthStencilState>> stencils;
};

DirectXRenderTarget::DirectXRenderTarget(std::shared_ptr<DirectXDevice> device, const ContextSettings& settings) :
    m_impl(std::make_unique<Impl>(std::move(device), settings))
{
}
DirectXRenderTarget::~DirectXRenderTarget() = default;

std::shared_ptr<DirectXRenderTarget> DirectXRenderTarget::create(std::shared_ptr<DirectXDevice> device,
                                                                 std::shared_ptr<DirectXImage>  image,
                                                                 const ContextSettings&         settings)
{
    auto result = std::shared_ptr<DirectXRenderTarget>(new DirectXRenderTarget(std::move(device), settings));
    return result->attach(std::move(image)) ? result : nullptr;
}

bool DirectXRenderTarget::attach(std::shared_ptr<DirectXImage> image)
{
    auto&      impl   = *m_impl;
    auto&      device = *impl.device;
    const auto guard  = device.lock();
    if (!image || device.isLost())
        return false;
    ContextSettings actual  = impl.requested;
    actual.backend          = device.settings();
    actual.sRgbCapable      = image->format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    DXGI_FORMAT depthFormat = DXGI_FORMAT_UNKNOWN;
    actual.depthBits = actual.stencilBits = 0;
    if (impl.requested.depthBits || impl.requested.stencilBits)
    {
        if (impl.requested.stencilBits)
        {
            depthFormat        = DXGI_FORMAT_D24_UNORM_S8_UINT;
            actual.depthBits   = 24;
            actual.stencilBits = 8;
        }
        else
        {
            depthFormat      = impl.requested.depthBits <= 16 ? DXGI_FORMAT_D16_UNORM : DXGI_FORMAT_D32_FLOAT;
            actual.depthBits = impl.requested.depthBits <= 16 ? 16u : 32u;
        }
    }
    const auto sampleCount   = device.samples(image->format, depthFormat, impl.requested.antiAliasingLevel);
    actual.antiAliasingLevel = sampleCount == 1 ? 0 : sampleCount;
    D3D11_TEXTURE2D_DESC description{};
    description.Width     = image->size.x;
    description.Height    = image->size.y;
    description.MipLevels = description.ArraySize = 1;
    description.SampleDesc.Count                  = sampleCount;
    description.Format                            = image->format;
    description.BindFlags                         = D3D11_BIND_RENDER_TARGET;
    ComPtr<ID3D11Texture2D>        multisample;
    ComPtr<ID3D11RenderTargetView> color;
    ComPtr<ID3D11Texture2D>        depth;
    ComPtr<ID3D11DepthStencilView> depthView;
    if (sampleCount > 1 &&
        (!device.check(device.device()->CreateTexture2D(&description, nullptr, multisample.put()),
                       "create MSAA attachment") ||
         !device.check(device.device()->CreateRenderTargetView(multisample.get(), nullptr, color.put()),
                       "create MSAA view")))
        return false;
    if (depthFormat != DXGI_FORMAT_UNKNOWN)
    {
        description.Format    = depthFormat;
        description.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        if (!device.check(device.device()->CreateTexture2D(&description, nullptr, depth.put()),
                          "create depth/stencil attachment") ||
            !device.check(device.device()->CreateDepthStencilView(depth.get(), nullptr, depthView.put()),
                          "create depth/stencil view"))
            return false;
        device.context()->ClearDepthStencilView(depthView.get(),
                                                D3D11_CLEAR_DEPTH |
                                                    (actual.stencilBits ? static_cast<UINT>(D3D11_CLEAR_STENCIL) : 0u),
                                                1.f,
                                                0);
    }
    D3D11_RASTERIZER_DESC rasterizer{};
    rasterizer.FillMode          = D3D11_FILL_SOLID;
    rasterizer.CullMode          = D3D11_CULL_NONE;
    rasterizer.DepthClipEnable   = TRUE;
    rasterizer.ScissorEnable     = TRUE;
    rasterizer.MultisampleEnable = sampleCount > 1;
    ComPtr<ID3D11RasterizerState> rasterizerState;
    if (!device.check(device.device()->CreateRasterizerState(&rasterizer, rasterizerState.put()), "create rasterizer"))
        return false;
    device.unbindTextures();
    impl.settings    = actual;
    impl.image       = std::move(image);
    impl.multisample = std::move(multisample);
    impl.color       = std::move(color);
    impl.depth       = std::move(depth);
    impl.depthView   = std::move(depthView);
    impl.rasterizer  = std::move(rasterizerState);
    impl.dirty       = false;
    return true;
}

bool DirectXRenderTarget::resize(Vector2u size)
{
    return attach(m_impl->device->makeTexture(size, m_impl->settings.sRgbCapable));
}
bool DirectXRenderTarget::setActive(bool)
{
    const auto guard = m_impl->device->lock();
    return !m_impl->device->isLost();
}
void DirectXRenderTarget::viewChanged()
{
}
const ContextSettings& DirectXRenderTarget::settings() const
{
    return m_impl->settings;
}
const std::shared_ptr<DirectXImage>& DirectXRenderTarget::image() const
{
    return m_impl->image;
}
unsigned int DirectXRenderTarget::getMaximumAntiAliasingLevel(const DirectXDevice& device)
{
    const auto guard = device.lock();
    const auto count = device.samples(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 32);
    return count == 1 ? 0 : count;
}

void DirectXRenderTarget::finish()
{
    colorWritten();
    const auto guard = m_impl->device->lock();
    if (m_impl->dirty && m_impl->multisample && !m_impl->device->isLost())
    {
        m_impl->device->unbindTextures();
        m_impl->device->context()
            ->ResolveSubresource(m_impl->image->texture.get(), 0, m_impl->multisample.get(), 0, m_impl->image->format);
    }
    m_impl->dirty = false;
}

void DirectXRenderTarget::clear(const rhi::TargetState& target, std::optional<Color> color, std::optional<StencilValue> stencil)
{
    if (!color && (!stencil || !m_impl->settings.stencilBits))
        return;
    const auto clear = prepareClear(color, stencil);
    // D3D11.0 clear calls ignore the scissor. A draw also preserves SFML's partial-clear semantics.
    drawVertices(target, clear.vertices.data(), clear.vertices.size(), PrimitiveType::Triangles, clear.settings, true);
}

void DirectXRenderTarget::draw(const rhi::TargetState&  target,
                               const Vertex*            vertices,
                               std::size_t              count,
                               PrimitiveType            type,
                               const rhi::DrawSettings& states)
{
    if (!vertices || !count)
        return;
    drawVertices(target, vertices, count, type, states, false);
}

void DirectXRenderTarget::drawVertices(
    const rhi::TargetState&  target,
    const Vertex*            vertices,
    std::size_t              count,
    PrimitiveType            type,
    const rhi::DrawSettings& states,
    bool                     clearing)
{
    if (count > std::numeric_limits<UINT>::max() / sizeof(Vertex))
        return;
    const auto guard  = m_impl->device->lock();
    auto&      device = *m_impl->device;
    if (device.isLost())
        return;
    const auto bytes = static_cast<UINT>(count * sizeof(Vertex));
    if (bytes > m_impl->streamCapacity)
    {
        D3D11_BUFFER_DESC description{};
        description.ByteWidth      = bytes;
        description.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        description.Usage          = D3D11_USAGE_DYNAMIC;
        description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        ComPtr<ID3D11Buffer> stream;
        if (!device.check(device.device()->CreateBuffer(&description, nullptr, stream.put()), "create vertex stream"))
            return;
        m_impl->stream         = std::move(stream);
        m_impl->streamCapacity = bytes;
    }
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (!device.check(device.context()->Map(m_impl->stream.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped),
                      "stream vertices"))
        return;
    std::memcpy(mapped.pData, vertices, bytes);
    device.context()->Unmap(m_impl->stream.get(), 0);
    drawBuffer(target, m_impl->stream.get(), 0, count, type, states, clearing);
}

void DirectXRenderTarget::draw(const rhi::TargetState&  target,
                               const rhi::Buffer&       buffer,
                               std::size_t              first,
                               std::size_t              count,
                               PrimitiveType            type,
                               const rhi::DrawSettings& states)
{
    if (first > buffer.getSize() / sizeof(Vertex) || count > buffer.getSize() / sizeof(Vertex) - first)
        return;
    if (nativeDirectXBuffer(buffer))
        drawBuffer(target, nativeDirectXBuffer(buffer)->buffer.get(), first, count, type, states);
}

bool DirectXRenderTarget::drawVertices(
    const rhi::TargetState&                      target,
    const std::vector<rhi::VertexBufferBinding>& buffers,
    std::size_t                                  first,
    std::size_t                                  count,
    std::size_t                                  instances,
    PrimitiveType                                type,
    const rhi::DrawSettings&                     states)
{
    if (!states.shader || !instances || instances > UINT_MAX)
        return false;
    auto shader = resourcePointerCast<DirectXProgram>(states.shader);
    if (!shader || !shader->bindVertexBuffers(*m_impl->device, buffers, first, count, instances))
        return false;
    drawBuffer(target, nullptr, first, count, type, states, false, &buffers, instances);
    return !m_impl->device->isLost();
}
bool DirectXRenderTarget::prewarm(const rhi::TargetState& target, const rhi::DrawSettings& states)
{
    drawBuffer(target, nullptr, 0, 1, PrimitiveType::Triangles, states, false, nullptr, 1, true);
    return !m_impl->device->isLost();
}

void DirectXRenderTarget::drawBuffer(
    const rhi::TargetState&                      target,
    ID3D11Buffer*                                buffer,
    std::size_t                                  first,
    std::size_t                                  count,
    PrimitiveType                                type,
    const rhi::DrawSettings&                     states,
    bool                                         clearing,
    const std::vector<rhi::VertexBufferBinding>* bindings,
    std::size_t                                  instances,
    bool                                         prewarmOnly)
{
    using namespace DirectXTargetImpl;
    auto&      impl   = *m_impl;
    auto&      device = *impl.device;
    const auto guard  = device.lock();
    if (device.isLost() || !count || first > std::numeric_limits<UINT>::max() ||
        count > std::numeric_limits<UINT>::max() - first)
        return;
    const auto size     = Vector2i(impl.image->size);
    const auto viewport = clearing ? IntRect({0, 0}, size) : target.viewport;
    const auto scissor  = target.scissor.findIntersection(IntRect({0, 0}, size));
    if (!scissor || viewport.size.x <= 0 || viewport.size.y <= 0)
        return;
    auto program = states.shader ? resourcePointerCast<DirectXProgram>(states.shader) : device.defaultProgram();
    if (!program)
        return;
    auto parameters = prepareDraw(target, states, clearing);
    device.unbindTextures();
    if (!program->bind(device, parameters, states.texture.get(), *impl.image))
        return;
    const auto&      blend = states.blendMode;
    const std::array blendKey{static_cast<unsigned int>(blend.colorSrcFactor),
                              static_cast<unsigned int>(blend.colorDstFactor),
                              static_cast<unsigned int>(blend.colorEquation),
                              static_cast<unsigned int>(blend.alphaSrcFactor),
                              static_cast<unsigned int>(blend.alphaDstFactor),
                              static_cast<unsigned int>(blend.alphaEquation),
                              static_cast<unsigned int>(states.stencilMode.stencilOnly)};
    auto&            blendState = impl.blends[blendKey];
    if (!blendState)
    {
        D3D11_BLEND_DESC description{};
        auto&            output      = description.RenderTarget[0];
        output.BlendEnable           = TRUE;
        output.SrcBlend              = factor(blend.colorSrcFactor, false);
        output.DestBlend             = factor(blend.colorDstFactor, false);
        output.BlendOp               = equation(blend.colorEquation);
        output.SrcBlendAlpha         = factor(blend.alphaSrcFactor, true);
        output.DestBlendAlpha        = factor(blend.alphaDstFactor, true);
        output.BlendOpAlpha          = equation(blend.alphaEquation);
        output.RenderTargetWriteMask = states.stencilMode.stencilOnly ? 0 : D3D11_COLOR_WRITE_ENABLE_ALL;
        if (!device.check(device.device()->CreateBlendState(&description, blendState.put()), "create blend state"))
            return;
    }
    const auto&      stencil = states.stencilMode;
    const std::array stencilKey{static_cast<unsigned int>(stencil.stencilComparison),
                                static_cast<unsigned int>(stencil.stencilUpdateOperation),
                                stencil.stencilMask.value & 0xFFu};
    auto&            stencilState = impl.stencils[stencilKey];
    if (!stencilState)
    {
        D3D11_DEPTH_STENCIL_DESC description{};
        description.DepthFunc               = D3D11_COMPARISON_ALWAYS;
        description.StencilEnable           = impl.settings.stencilBits != 0;
        description.StencilReadMask         = static_cast<UINT8>(stencilKey[2]);
        description.StencilWriteMask        = 0xFF;
        description.FrontFace.StencilFunc   = comparison(stencil.stencilComparison);
        description.FrontFace.StencilFailOp = description.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        description.FrontFace.StencilPassOp = stencilOperation(stencil.stencilUpdateOperation);
        description.BackFace                = description.FrontFace;
        if (!device.check(device.device()->CreateDepthStencilState(&description, stencilState.put()),
                          "create stencil state"))
            return;
    }
    auto* context    = device.context();
    auto* attachment = impl.color ? impl.color.get() : impl.image->attachment.get();
    context->OMSetRenderTargets(1, &attachment, impl.depthView.get());
    context->OMSetBlendState(blendState.get(), nullptr, ~0u);
    context->OMSetDepthStencilState(stencilState.get(), std::min(stencil.stencilReference.value, 255u));
    context->RSSetState(impl.rasterizer.get());
    const D3D11_VIEWPORT rectangle{static_cast<float>(viewport.position.x),
                                   static_cast<float>(viewport.position.y),
                                   static_cast<float>(viewport.size.x),
                                   static_cast<float>(viewport.size.y),
                                   0.f,
                                   1.f};
    const D3D11_RECT     clip{scissor->position.x,
                          scissor->position.y,
                          scissor->position.x + scissor->size.x,
                          scissor->position.y + scissor->size.y};
    context->RSSetViewports(1, &rectangle);
    context->RSSetScissorRects(1, &clip);
    if (prewarmOnly)
        return;
    if (bindings && !program->bindVertexBuffers(device, *bindings, first, count, instances))
        return;
    constexpr UINT stride = sizeof(Vertex);
    constexpr UINT offset = 0;
    if (!bindings)
        context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
    context->IASetPrimitiveTopology(topology(type));
    if (type == PrimitiveType::TriangleFan)
    {
        if (count < 3 || count - 2 > std::numeric_limits<UINT>::max() / (3 * sizeof(UINT)))
            return;
        const auto indexCount = static_cast<UINT>((count - 2) * 3);
        if (indexCount > impl.fanIndexCapacity)
        {
            std::vector<UINT> indices;
            indices.reserve(indexCount);
            for (UINT index = 1; index + 1 < count; ++index)
                indices.insert(indices.end(), {0, index, index + 1});
            D3D11_BUFFER_DESC description{};
            description.ByteWidth = indexCount * sizeof(UINT);
            description.BindFlags = D3D11_BIND_INDEX_BUFFER;
            description.Usage     = D3D11_USAGE_IMMUTABLE;
            const D3D11_SUBRESOURCE_DATA data{indices.data(), 0, 0};
            ComPtr<ID3D11Buffer>         indexBuffer;
            if (!device.check(device.device()->CreateBuffer(&description, &data, indexBuffer.put()),
                              "create triangle fan indices"))
                return;
            impl.fanIndices       = std::move(indexBuffer);
            impl.fanIndexCapacity = indexCount;
        }
        context->IASetIndexBuffer(impl.fanIndices.get(), DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexedInstanced(indexCount, static_cast<UINT>(instances), 0, static_cast<INT>(first), 0);
    }
    else
    {
        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
        context->DrawInstanced(static_cast<UINT>(count), static_cast<UINT>(instances), static_cast<UINT>(first), 0);
    }
    impl.dirty = true;
}
Vector2u DirectXRenderTarget::getSize() const
{
    return image()->size;
}
} // namespace sf::priv
