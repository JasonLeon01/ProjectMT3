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

#include <SFML/Graphics/Metal/MetalObjects.hpp>
#include <SFML/Graphics/Metal/MetalRHI.hpp>
#include <SFML/Graphics/Metal/MetalRenderTarget.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <limits>
#include <ostream>
#include <utility>

#include <cstring>

namespace sf::priv
{
namespace
{
namespace MetalTargetImpl
{
MTLBlendFactor factor(BlendMode::Factor value)
{
    switch (value)
    {
        case BlendMode::Factor::Zero:
            return MTLBlendFactorZero;
        case BlendMode::Factor::One:
            return MTLBlendFactorOne;
        case BlendMode::Factor::SrcColor:
            return MTLBlendFactorSourceColor;
        case BlendMode::Factor::OneMinusSrcColor:
            return MTLBlendFactorOneMinusSourceColor;
        case BlendMode::Factor::DstColor:
            return MTLBlendFactorDestinationColor;
        case BlendMode::Factor::OneMinusDstColor:
            return MTLBlendFactorOneMinusDestinationColor;
        case BlendMode::Factor::SrcAlpha:
            return MTLBlendFactorSourceAlpha;
        case BlendMode::Factor::OneMinusSrcAlpha:
            return MTLBlendFactorOneMinusSourceAlpha;
        case BlendMode::Factor::DstAlpha:
            return MTLBlendFactorDestinationAlpha;
        case BlendMode::Factor::OneMinusDstAlpha:
            return MTLBlendFactorOneMinusDestinationAlpha;
    }
    return MTLBlendFactorOne;
}

MTLBlendOperation equation(BlendMode::Equation value)
{
    switch (value)
    {
        case BlendMode::Equation::Add:
            return MTLBlendOperationAdd;
        case BlendMode::Equation::Subtract:
            return MTLBlendOperationSubtract;
        case BlendMode::Equation::ReverseSubtract:
            return MTLBlendOperationReverseSubtract;
        case BlendMode::Equation::Min:
            return MTLBlendOperationMin;
        case BlendMode::Equation::Max:
            return MTLBlendOperationMax;
    }
    return MTLBlendOperationAdd;
}

MTLCompareFunction comparison(StencilComparison value)
{
    switch (value)
    {
        case StencilComparison::Never:
            return MTLCompareFunctionNever;
        case StencilComparison::Less:
            return MTLCompareFunctionLess;
        case StencilComparison::LessEqual:
            return MTLCompareFunctionLessEqual;
        case StencilComparison::Greater:
            return MTLCompareFunctionGreater;
        case StencilComparison::GreaterEqual:
            return MTLCompareFunctionGreaterEqual;
        case StencilComparison::Equal:
            return MTLCompareFunctionEqual;
        case StencilComparison::NotEqual:
            return MTLCompareFunctionNotEqual;
        case StencilComparison::Always:
            return MTLCompareFunctionAlways;
    }
    return MTLCompareFunctionAlways;
}

MTLStencilOperation stencilOperation(StencilUpdateOperation value)
{
    switch (value)
    {
        case StencilUpdateOperation::Keep:
            return MTLStencilOperationKeep;
        case StencilUpdateOperation::Zero:
            return MTLStencilOperationZero;
        case StencilUpdateOperation::Replace:
            return MTLStencilOperationReplace;
        case StencilUpdateOperation::Increment:
            return MTLStencilOperationIncrementClamp;
        case StencilUpdateOperation::Decrement:
            return MTLStencilOperationDecrementClamp;
        case StencilUpdateOperation::Invert:
            return MTLStencilOperationInvert;
    }
    return MTLStencilOperationKeep;
}

MTLPrimitiveType primitive(PrimitiveType value)
{
    switch (value)
    {
        case PrimitiveType::Points:
            return MTLPrimitiveTypePoint;
        case PrimitiveType::Lines:
            return MTLPrimitiveTypeLine;
        case PrimitiveType::LineStrip:
            return MTLPrimitiveTypeLineStrip;
        case PrimitiveType::TriangleStrip:
            return MTLPrimitiveTypeTriangleStrip;
        case PrimitiveType::Triangles:
        case PrimitiveType::TriangleFan:
            return MTLPrimitiveTypeTriangle;
    }
    return MTLPrimitiveTypeTriangle;
}

MetalPipelineKey pipelineKey(MTLPixelFormat           color,
                             MTLPixelFormat           depth,
                             MTLPixelFormat           stencil,
                             NSUInteger               samples,
                             const rhi::DrawSettings& states)
{
    const auto& blend = states.blendMode;
    return {color,
            depth,
            stencil,
            samples,
            factor(blend.colorSrcFactor),
            factor(blend.colorDstFactor),
            equation(blend.colorEquation),
            factor(blend.alphaSrcFactor),
            factor(blend.alphaDstFactor),
            equation(blend.alphaEquation),
            states.stencilMode.stencilOnly ? MTLColorWriteMaskNone : MTLColorWriteMaskAll};
}

NSUInteger samples(id<MTLDevice> device, unsigned int requested)
{
    for (const NSUInteger count : {8u, 4u, 2u})
        if (requested >= count && [device supportsTextureSampleCount:count])
            return count;
    return 1;
}

id<MTLTexture> makeAttachment(id<MTLDevice> device, MTLPixelFormat format, Vector2u size, NSUInteger sampleCount)
{
    auto* const descriptor       = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:format
                                     width:size.x
                                    height:size.y
                                 mipmapped:NO];
    descriptor.textureType       = sampleCount > 1 ? MTLTextureType2DMultisample : MTLTextureType2D;
    descriptor.sampleCount       = sampleCount;
    descriptor.usage             = MTLTextureUsageRenderTarget;
    descriptor.storageMode       = MTLStorageModePrivate;
    const id<MTLTexture> texture = [device newTextureWithDescriptor:descriptor];
    if (!texture)
        err() << "Failed to create a Metal render target attachment" << std::endl;
    return texture;
}
} // namespace MetalTargetImpl
} // namespace

struct MetalRenderTarget::Impl
{
    Impl(std::shared_ptr<MetalDevice> value, const ContextSettings& configuration) :
        device(std::move(value)),
        requested(configuration)
    {
    }

    std::shared_ptr<MetalDevice> device;
    ContextSettings              requested;
    ContextSettings              settings;
    std::shared_ptr<MetalImage>  image;
    id<MTLTexture>               multisample;
    id<MTLTexture>               depth;
    MTLPixelFormat               depthFormat{MTLPixelFormatInvalid};
    MTLPixelFormat               stencilFormat{MTLPixelFormatInvalid};
    NSUInteger                   sampleCount{1};
    bool                         initialized{};

    MTLRenderPassDescriptor* pass()
    {
        auto* const descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        auto* const color      = descriptor.colorAttachments[0];
        if (multisample)
        {
            color.texture        = multisample;
            color.resolveTexture = image->texture;
            color.loadAction     = initialized ? MTLLoadActionLoad : MTLLoadActionClear;
            color.clearColor     = MTLClearColorMake(0, 0, 0, 0);
            color.storeAction    = MTLStoreActionStoreAndMultisampleResolve;
        }
        else
        {
            color.texture     = image->texture;
            color.loadAction  = MTLLoadActionLoad;
            color.storeAction = MTLStoreActionStore;
        }
        if (depth)
        {
            descriptor.depthAttachment.texture     = depth;
            descriptor.depthAttachment.loadAction  = initialized ? MTLLoadActionLoad : MTLLoadActionClear;
            descriptor.depthAttachment.clearDepth  = 1.0;
            descriptor.depthAttachment.storeAction = MTLStoreActionStore;
            if (stencilFormat != MTLPixelFormatInvalid)
            {
                descriptor.stencilAttachment.texture      = depth;
                descriptor.stencilAttachment.loadAction   = initialized ? MTLLoadActionLoad : MTLLoadActionClear;
                descriptor.stencilAttachment.clearStencil = 0;
                descriptor.stencilAttachment.storeAction  = MTLStoreActionStore;
            }
        }
        initialized = true;
        return descriptor;
    }
};

MetalRenderTarget::MetalRenderTarget(std::shared_ptr<MetalDevice> device, const ContextSettings& settings) :
    m_impl(std::make_unique<Impl>(std::move(device), settings))
{
}

MetalRenderTarget::~MetalRenderTarget()
{
    const auto guard = m_impl->device->lock();
    if (m_impl->device->impl().rendering(this))
        m_impl->device->impl().endEncoding();
}

std::shared_ptr<MetalRenderTarget> MetalRenderTarget::create(std::shared_ptr<MetalDevice>       device,
                                                             const std::shared_ptr<MetalImage>& image,
                                                             const ContextSettings&             settings)
{
    auto result = std::shared_ptr<MetalRenderTarget>(new MetalRenderTarget(std::move(device), settings));
    return result->attach(image) ? result : nullptr;
}

bool MetalRenderTarget::attach(const std::shared_ptr<MetalImage>& image)
{
    using namespace MetalTargetImpl;
    auto&      impl   = *m_impl;
    const auto guard  = impl.device->lock();
    auto&      device = impl.device->impl();
    if (!image)
        return false;
    @autoreleasepool
    {
        ContextSettings actual = impl.requested;
        actual.backend         = MetalSettings{};
        actual.sRgbCapable     = image->texture.pixelFormat == MTLPixelFormatRGBA8Unorm_sRGB;
        actual.depthBits = actual.stencilBits = 0;
        MTLPixelFormat depthFormat            = MTLPixelFormatInvalid;
        MTLPixelFormat stencilFormat          = MTLPixelFormatInvalid;
        if (impl.requested.stencilBits)
        {
            depthFormat = stencilFormat = MTLPixelFormatDepth32Float_Stencil8;
            actual.depthBits            = 32;
            actual.stencilBits          = 8;
        }
        else if (impl.requested.depthBits)
        {
            depthFormat      = impl.requested.depthBits <= 16 ? MTLPixelFormatDepth16Unorm : MTLPixelFormatDepth32Float;
            actual.depthBits = impl.requested.depthBits <= 16 ? 16u : 32u;
        }
        const auto sampleCount   = samples(device.device, impl.requested.antiAliasingLevel);
        actual.antiAliasingLevel = sampleCount == 1 ? 0u : static_cast<unsigned int>(sampleCount);
        id<MTLTexture> multisample;
        id<MTLTexture> depth;
        if (sampleCount > 1 &&
            !(multisample = makeAttachment(device.device, image->texture.pixelFormat, image->size, sampleCount)))
            return false;
        if (depthFormat != MTLPixelFormatInvalid &&
            !(depth = makeAttachment(device.device, depthFormat, image->size, sampleCount)))
            return false;
        if (device.rendering(this))
            device.endEncoding();
        impl.settings      = actual;
        impl.image         = image;
        impl.multisample   = multisample;
        impl.depth         = depth;
        impl.depthFormat   = depthFormat;
        impl.stencilFormat = stencilFormat;
        impl.sampleCount   = sampleCount;
        impl.initialized   = false;
        return true;
    }
}

bool MetalRenderTarget::resize(Vector2u size)
{
    const auto guard     = m_impl->device->lock();
    const bool mipmapped = m_impl->image->texture.mipmapLevelCount > 1;
    return attach(m_impl->device->makeTexture(size, m_impl->settings.sRgbCapable, mipmapped));
}

bool MetalRenderTarget::setActive(bool)
{
    return true;
}

void MetalRenderTarget::viewChanged()
{
}

const ContextSettings& MetalRenderTarget::settings() const
{
    return m_impl->settings;
}

const std::shared_ptr<MetalImage>& MetalRenderTarget::image() const
{
    return m_impl->image;
}

unsigned int MetalRenderTarget::getMaximumAntiAliasingLevel(const MetalDevice& device)
{
    const auto count = device.maximumSamples();
    return count == 1 ? 0 : count;
}

void MetalRenderTarget::finish()
{
    colorWritten();
    const auto guard = m_impl->device->lock();
    if (m_impl->device->impl().rendering(this))
        m_impl->device->impl().endEncoding();
}

void MetalRenderTarget::clear(const rhi::TargetState& target, std::optional<Color> color, std::optional<StencilValue> stencil)
{
    if (!color && (!stencil || !m_impl->settings.stencilBits))
        return;
    const auto clear = prepareClear(color, stencil);
    // A draw rather than a load action, so that clearing honors the scissor like the other backends
    (void)drawVertices(target, clear.vertices.data(), nullptr, 0, clear.vertices.size(), PrimitiveType::Triangles, clear.settings, true);
}

void MetalRenderTarget::draw(const rhi::TargetState&  target,
                             const Vertex*            vertices,
                             std::size_t              count,
                             PrimitiveType            type,
                             const rhi::DrawSettings& states)
{
    if (!vertices || !count)
        return;
    (void)drawVertices(target, vertices, nullptr, 0, count, type, states, false);
}

void MetalRenderTarget::draw(const rhi::TargetState&  target,
                             const rhi::Buffer&       buffer,
                             std::size_t              first,
                             std::size_t              count,
                             PrimitiveType            type,
                             const rhi::DrawSettings& states)
{
    if (first > buffer.getSize() / sizeof(Vertex) || count > buffer.getSize() / sizeof(Vertex) - first)
        return;
    if (nativeMetalBuffer(buffer))
        (void)drawVertices(target, nullptr, &buffer, first, count, type, states, false);
}

bool MetalRenderTarget::drawVertices(
    const rhi::TargetState&                      target,
    const std::vector<rhi::VertexBufferBinding>& bindings,
    std::size_t                                  first,
    std::size_t                                  count,
    std::size_t                                  instances,
    PrimitiveType                                type,
    const rhi::DrawSettings&                     states)
{
    const auto program = states.shader ? resourcePointerCast<MetalProgram>(states.shader) : m_impl->device->defaultProgram();
    if (!program || !instances || !program->getOptions().transformOutputs.empty())
        return false;
    const auto&        options = program->getOptions();
    rhi::ShaderOptions standard;
    if (options.vertexBindings.empty())
        standard.vertexBindings = {{sizeof(Vertex), 0}};
    if (!validateVertexBindings(options.vertexBindings.empty() ? standard : options, bindings, first, count, instances))
        return false;
    for (const auto& binding : bindings)
    {
        const auto resource = resourcePointerCast<MetalBufferResource>(binding.buffer);
        if (!resource || resource->device != m_impl->device)
            return false;
    }
    return drawVertices(target, nullptr, nullptr, first, count, type, states, false, &bindings, instances);
}

bool MetalRenderTarget::prewarm(const rhi::TargetState&, const rhi::DrawSettings& states)
{
    const auto guard = m_impl->device->lock();
    const auto program = states.shader ? resourcePointerCast<MetalProgram>(states.shader) : m_impl->device->defaultProgram();
    if (!program || !program->getOptions().transformOutputs.empty())
        return false;
    const auto key = MetalTargetImpl::pipelineKey(m_impl->image->texture.pixelFormat,
                                                  m_impl->depthFormat,
                                                  m_impl->stencilFormat,
                                                  m_impl->sampleCount,
                                                  states);
    return program->impl().pipeline(m_impl->device->impl(), key) != nil;
}

bool MetalRenderTarget::drawVertices(
    const rhi::TargetState&                      target,
    const Vertex*                                vertices,
    const rhi::Buffer*                           buffer,
    std::size_t                                  first,
    std::size_t                                  count,
    PrimitiveType                                type,
    const rhi::DrawSettings&                     states,
    bool                                         clearing,
    const std::vector<rhi::VertexBufferBinding>* bindings,
    std::size_t                                  instances)
{
    using namespace MetalTargetImpl;
    auto&      impl  = *m_impl;
    const auto guard = impl.device->lock();
    if (!count || count > std::numeric_limits<std::uint32_t>::max() / sizeof(Vertex) ||
        first > std::numeric_limits<std::uint32_t>::max() - count)
        return false;
    @autoreleasepool
    {
        auto&      device   = impl.device->impl();
        const auto size     = Vector2i(impl.image->size);
        const auto viewport = clearing ? IntRect({0, 0}, size) : target.viewport;
        const auto scissor  = target.scissor.findIntersection(IntRect({0, 0}, size));
        if (!scissor || viewport.size.x <= 0 || viewport.size.y <= 0)
            return false;
        const auto program = states.shader ? resourcePointerCast<MetalProgram>(states.shader)
                                           : impl.device->defaultProgram();
        if (!program)
            return false;
        auto                             parameters = prepareDraw(target, states, clearing);
        const MetalPipelineKey           key        = MetalTargetImpl::pipelineKey(impl.image->texture.pixelFormat,
                                                                  impl.depthFormat,
                                                                  impl.stencilFormat,
                                                                  impl.sampleCount,
                                                                  states);
        const id<MTLRenderPipelineState> pipeline   = program->impl().pipeline(device, key);
        if (!pipeline)
            return false;
        const auto&                    stencil      = states.stencilMode;
        const auto                     stencilKey   = impl.settings.stencilBits
                                                          ? NSUInteger{1} | (NSUInteger{comparison(stencil.stencilComparison)} << 1) |
                                          (NSUInteger{stencilOperation(stencil.stencilUpdateOperation)} << 5) |
                                          (NSUInteger{stencil.stencilMask.value & 0xFFu} << 9)
                                                          : NSUInteger{0};
        const id<MTLDepthStencilState> depthStencil = device.depthStencil(stencilKey);
        if (!depthStencil)
            return false;
        if (!device.rendering(this) && !device.render(this, impl.pass()))
        {
            err() << "Failed to begin a Metal render pass" << std::endl;
            return false;
        }
        const id<MTLRenderCommandEncoder> encoder = device.renderEncoder;
        [encoder setRenderPipelineState:pipeline];
        [encoder setDepthStencilState:depthStencil];
        [encoder setStencilReferenceValue:std::min(stencil.stencilReference.value, 255u)];
        [encoder setViewport:MTLViewport{static_cast<double>(viewport.position.x),
                                         static_cast<double>(viewport.position.y),
                                         static_cast<double>(viewport.size.x),
                                         static_cast<double>(viewport.size.y),
                                         0.0,
                                         1.0}];
        [encoder setScissorRect:MTLScissorRect{static_cast<NSUInteger>(scissor->position.x),
                                               static_cast<NSUInteger>(scissor->position.y),
                                               static_cast<NSUInteger>(scissor->size.x),
                                               static_cast<NSUInteger>(scissor->size.y)}];
        if (!program->impl().bind(device, encoder, parameters, states.texture.get(), impl.image->texture))
            return false;
        if (bindings)
        {
            const auto& options = program->getOptions();
            for (std::size_t index = 0; index < bindings->size(); ++index)
            {
                const auto native = nativeMetalBuffer(*(*bindings)[index].buffer);
                [encoder setVertexBuffer:native->buffer
                                  offset:(*bindings)[index].offset
                                 atIndex:options.vertexBindings.empty() ? metalVertexBufferIndex
                                                                        : metalCustomVertexBufferIndex + index];
            }
        }
        else if (buffer)
        {
            [encoder setVertexBuffer:nativeMetalBuffer(*buffer)->buffer offset:0 atIndex:metalVertexBufferIndex];
        }
        else
        {
            const auto bytes = count * sizeof(Vertex);
            if (bytes <= metalInlineDataLimit)
            {
                [encoder setVertexBytes:vertices length:bytes atIndex:metalVertexBufferIndex];
            }
            else
            {
                const auto stream = device.allocate(bytes, 16);
                if (!stream.buffer)
                    return false;
                std::memcpy(stream.contents, vertices, bytes);
                [encoder setVertexBuffer:stream.buffer offset:stream.offset atIndex:metalVertexBufferIndex];
            }
        }
        if (type == PrimitiveType::TriangleFan)
        {
            if (count < 3)
                return false;
            const auto indexCount = (count - 2) * 3;
            const auto indices    = device.allocate(indexCount * sizeof(std::uint32_t), 4);
            if (!indices.buffer)
                return false;
            auto*      output = static_cast<std::uint32_t*>(indices.contents);
            const auto base   = static_cast<std::uint32_t>(first);
            for (std::uint32_t index = 1; index + 1 < count; ++index)
            {
                *output++ = base;
                *output++ = base + index;
                *output++ = base + index + 1;
            }
            [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                indexCount:indexCount
                                 indexType:MTLIndexTypeUInt32
                               indexBuffer:indices.buffer
                         indexBufferOffset:indices.offset
                             instanceCount:instances];
        }
        else
        {
            [encoder drawPrimitives:primitive(type) vertexStart:first vertexCount:count instanceCount:instances];
        }
    }
    colorWritten();
    return true;
}

Vector2u MetalRenderTarget::getSize() const
{
    return image()->size;
}
} // namespace sf::priv
