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
#include <SFML/Graphics/Metal/MetalWindowSurface.hpp>

#include <SFML/System/Err.hpp>

#import <QuartzCore/CAMetalLayer.h>
#include <TargetConditionals.h>
#include <algorithm>
#include <ostream>

namespace sf::priv
{
namespace
{
CAMetalLayer* toLayer(void* layer)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    return (__bridge CAMetalLayer*)layer;
#pragma GCC diagnostic pop
}

Vector2u drawableSize(Vector2u size)
{
    return {std::max(size.x, 1u), std::max(size.y, 1u)};
}
} // namespace

struct MetalWindowSurface::Impl
{
    std::shared_ptr<MetalDevice>       device;
    std::shared_ptr<MetalRenderTarget> target;
    CAMetalLayer*                      layer;
    Vector2u                           size;
};

MetalWindowSurface::MetalWindowSurface() : m_impl(std::make_unique<Impl>())
{
}

MetalWindowSurface::~MetalWindowSurface()
{
    const auto guard = m_impl->device->lock();
    m_impl->target.reset();
    (void)m_impl->device->submit(true);
}

std::unique_ptr<MetalWindowSurface> MetalWindowSurface::create(
    const std::shared_ptr<MetalDevice>& device,
    void*                               layer,
    Vector2u                            size,
    const ContextSettings&              settings)
{
    if (!device || !layer)
    {
        err() << "Failed to create the Metal window surface: no Metal layer" << std::endl;
        return nullptr;
    }
    const auto guard = device->lock();
    @autoreleasepool
    {
        auto surface = std::unique_ptr<MetalWindowSurface>(new MetalWindowSurface); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
        auto& impl  = *surface->m_impl;
        impl.device = device;
        impl.layer  = toLayer(layer);
        impl.size   = drawableSize(size);
        impl.target = MetalRenderTarget::create(device, device->makeTexture(impl.size, settings.sRgbCapable, false), settings);
        if (!impl.target)
            return nullptr;
        impl.layer.device = device->impl().device;
        impl.layer.pixelFormat = impl.target->settings().sRgbCapable ? MTLPixelFormatBGRA8Unorm_sRGB : MTLPixelFormatBGRA8Unorm;
        impl.layer.framebufferOnly = YES;
        impl.layer.opaque          = YES;
        impl.layer.drawableSize    = CGSizeMake(impl.size.x, impl.size.y);
        if (!device->impl().presentPipeline(impl.layer.pixelFormat))
            return nullptr;
        return surface;
    }
}

const ContextSettings& MetalWindowSurface::getSettings() const
{
    return m_impl->target->settings();
}

bool MetalWindowSurface::setActive(bool)
{
    return true;
}

void MetalWindowSurface::display()
{
    auto&      impl  = *m_impl;
    const auto guard = impl.device->lock();
    @autoreleasepool
    {
        auto& context = impl.device->impl();
        impl.target->finish();
        const id<CAMetalDrawable> drawable = [impl.layer nextDrawable];
        if (!drawable)
        {
            (void)context.submit(false);
            return;
        }
        auto* const pass                          = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture          = drawable.texture;
        pass.colorAttachments[0].loadAction       = MTLLoadActionDontCare;
        pass.colorAttachments[0].storeAction      = MTLStoreActionStore;
        const id<MTLRenderCommandEncoder> encoder = context.render(this, pass);
        if (!encoder)
        {
            err() << "Failed to begin the Metal present pass" << std::endl;
            (void)context.submit(false);
            return;
        }
        [encoder setRenderPipelineState:context.presentPipeline(impl.layer.pixelFormat)];
        [encoder setFragmentTexture:impl.target->image()->texture atIndex:0];
        [encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
        context.endEncoding();
        [context.commandBuffer() presentDrawable:drawable];
        (void)context.submit(false, true);
    }
}

void MetalWindowSurface::setVerticalSyncEnabled(bool enabled)
{
#if TARGET_OS_IPHONE
    // iOS always synchronizes presentation with the display
    (void)enabled;
#else
    m_impl->layer.displaySyncEnabled = enabled ? YES : NO;
#endif
}

void MetalWindowSurface::resize(Vector2u size)
{
    auto&      impl     = *m_impl;
    const auto guard    = impl.device->lock();
    const auto drawable = drawableSize(size);
    if (drawable == impl.size)
        return;
    if (!impl.target->resize(drawable))
    {
        err() << "Failed to resize the Metal window surface" << std::endl;
        return;
    }
    impl.size               = drawable;
    impl.layer.drawableSize = CGSizeMake(drawable.x, drawable.y);
}

const std::shared_ptr<MetalRenderTarget>& MetalWindowSurface::renderTarget() const
{
    return m_impl->target;
}

bool MetalWindowSurface::copyTo(const std::shared_ptr<MetalImage>& destination, Vector2u offset) const
{
    const auto guard = m_impl->device->lock();
    if (!destination)
        return false;
    m_impl->target->finish();
    return m_impl->device->copy(*m_impl->target->image(), *destination, offset);
}
} // namespace sf::priv
