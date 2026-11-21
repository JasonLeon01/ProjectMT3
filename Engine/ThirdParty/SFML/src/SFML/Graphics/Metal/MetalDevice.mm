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

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Metal/BuiltInShaders.hpp>
#include <SFML/Graphics/Metal/MetalObjects.hpp>

#include <SFML/System/Err.hpp>

#include <TargetConditionals.h>
#include <algorithm>
#include <ostream>
#include <utility>

#include <cstring>

#if TARGET_OS_IPHONE
#if __IPHONE_OS_VERSION_MIN_REQUIRED < 160000
#error "The Metal backend requires an iOS deployment target of at least 16.0"
#endif
#elif __MAC_OS_X_VERSION_MIN_REQUIRED < 130000
#error "The Metal backend requires a macOS deployment target of at least 13.0"
#endif

namespace sf::priv
{
namespace
{
namespace MetalDeviceImpl
{
constexpr NSUInteger chunkSize        = 1 << 20;
constexpr long       framesInFlight   = 3;
constexpr auto       transientOptions = MTLResourceStorageModeShared | MTLResourceCPUCacheModeWriteCombined;

NSUInteger align(NSUInteger value, NSUInteger alignment)
{
    return (value + alignment - 1) / alignment * alignment;
}

std::string temporaryPath()
{
    NSString* const name = [NSString stringWithFormat:@"sfml-metal-%@.metallib", [[NSUUID UUID] UUIDString]];
    return [[NSTemporaryDirectory() stringByAppendingPathComponent:name] UTF8String];
}

NSURL* fileUrl(const std::string& path)
{
    return [NSURL fileURLWithPath:[NSString stringWithUTF8String:path.c_str()]];
}

void removeFile(const std::string& path)
{
    [[NSFileManager defaultManager] removeItemAtPath:[NSString stringWithUTF8String:path.c_str()] error:nil];
}
} // namespace MetalDeviceImpl
} // namespace

std::string describe(NSError* error)
{
    return error ? std::string([[error localizedDescription] UTF8String]) : std::string("unknown error");
}


////////////////////////////////////////////////////////////
id<MTLCommandBuffer> MetalDevice::Impl::commandBuffer()
{
    if (!commands)
        commands = [queue commandBuffer];
    return commands;
}

id<MTLBlitCommandEncoder> MetalDevice::Impl::blit()
{
    if (!blitEncoder)
    {
        endEncoding();
        blitEncoder = [commandBuffer() blitCommandEncoder];
    }
    return blitEncoder;
}

id<MTLRenderCommandEncoder> MetalDevice::Impl::render(const void* owner, MTLRenderPassDescriptor* pass)
{
    endEncoding();
    renderEncoder = [commandBuffer() renderCommandEncoderWithDescriptor:pass];
    renderOwner   = renderEncoder ? owner : nullptr;
    return renderEncoder;
}

bool MetalDevice::Impl::rendering(const void* owner) const
{
    return renderEncoder && renderOwner == owner;
}

void MetalDevice::Impl::endEncoding()
{
    if (renderEncoder)
    {
        [renderEncoder endEncoding];
        renderEncoder = nil;
        renderOwner   = nullptr;
    }
    if (blitEncoder)
    {
        [blitEncoder endEncoding];
        blitEncoder = nil;
    }
}

bool MetalDevice::Impl::submit(bool wait, bool paced)
{
    using namespace MetalDeviceImpl;
    endEncoding();
    if (!commands)
        return true;
    const id<MTLCommandBuffer> buffer = commands;
    timingRanges.erase(std::remove_if(timingRanges.begin(),
                                      timingRanges.end(),
                                      [&](const auto& weak)
                                      {
                                          const auto range = weak.lock();
                                          if (!range || !range->capturing)
                                              return true;
                                          range->buffers.push_back(buffer);
                                          return false;
                                      }),
                       timingRanges.end());
    commands                                                = nil;
    chunk                                                   = nil;
    chunkOffset                                             = 0;
    const std::vector<id<MTLBuffer>>          retired       = std::exchange(used, {});
    const std::shared_ptr<MetalTransientPool> transientPool = pool;
    const dispatch_semaphore_t                semaphore     = paced ? frames : nil;
    if (semaphore)
        dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    [buffer addCompletedHandler:^(id<MTLCommandBuffer> completed) {
        if (completed.status == MTLCommandBufferStatusError)
            err() << "Metal command buffer failed: " << describe(completed.error) << std::endl;
        {
            const std::lock_guard guard(transientPool->mutex);
            for (const id<MTLBuffer>& retiredBuffer : retired)
                if (retiredBuffer.length == chunkSize)
                    transientPool->free.push_back(retiredBuffer);
        }
        if (semaphore)
            dispatch_semaphore_signal(semaphore);
    }];
    [buffer commit];
    if (!wait)
        return true;
    [buffer waitUntilCompleted];
    return buffer.status == MTLCommandBufferStatusCompleted;
}

MetalTransient MetalDevice::Impl::allocate(std::size_t bytes, std::size_t alignment)
{
    using namespace MetalDeviceImpl;
    if (bytes > chunkSize)
    {
        const id<MTLBuffer> buffer = [device newBufferWithLength:bytes options:transientOptions];
        if (!buffer)
        {
            err() << "Failed to allocate " << bytes << " bytes of Metal staging memory" << std::endl;
            return {};
        }
        used.push_back(buffer);
        return {buffer, 0, buffer.contents};
    }
    NSUInteger offset = align(chunkOffset, alignment);
    if (!chunk || offset + bytes > chunkSize)
    {
        chunk = nil;
        {
            const std::lock_guard guard(pool->mutex);
            if (!pool->free.empty())
            {
                chunk = pool->free.back();
                pool->free.pop_back();
            }
        }
        if (!chunk)
            chunk = [device newBufferWithLength:chunkSize options:transientOptions];
        if (!chunk)
        {
            err() << "Failed to allocate Metal staging memory" << std::endl;
            return {};
        }
        used.push_back(chunk);
        offset = 0;
    }
    chunkOffset = offset + bytes;
    return {chunk, offset, static_cast<std::byte*>(chunk.contents) + offset};
}

id<MTLRenderPipelineState> MetalDevice::Impl::makePipeline(MTLRenderPipelineDescriptor*             descriptor,
                                                           MTLPipelineOption                        options,
                                                           MTLAutoreleasedRenderPipelineReflection* reflection)
{
    const auto archiveOptions = static_cast<MTLPipelineOption>(options | MTLPipelineOptionFailOnBinaryArchiveMiss);
    descriptor.binaryArchives = @[archive];
    NSError*                   archiveError = nil;
    id<MTLRenderPipelineState> state        = [device
        newRenderPipelineStateWithDescriptor:descriptor
                                     options:archiveOptions
                                  reflection:reflection
                                       error:&archiveError];
    if (state)
        return state;

    descriptor.binaryArchives = archives;
    if (archives.count > 1)
    {
        NSError* secondaryError = nil;
        state                   = [device newRenderPipelineStateWithDescriptor:descriptor
                                                     options:archiveOptions
                                                  reflection:reflection
                                                       error:&secondaryError];
        if (secondaryError)
            archiveError = secondaryError;
    }
    if (!state)
    {
        // Keep the archive-only error separate. Ordinary creation must validate the descriptor
        // before treating the archive-only failure as a cache miss.
        NSError* creationError = nil;
        state                  = [device newRenderPipelineStateWithDescriptor:descriptor
                                                     options:options
                                                  reflection:reflection
                                                       error:&creationError];
        if (!state)
        {
            err() << "Failed to create Metal render pipeline: " << describe(creationError)
                  << "; binary archive lookup failed: " << describe(archiveError) << std::endl;
            return nil;
        }
    }

    archiveDirty      = true;
    archiveHasEntries = true;
    NSError* error    = nil;
    if (![archive addRenderPipelineFunctionsWithDescriptor:descriptor error:&error])
    {
        err() << "Failed to record Metal render pipeline in the pipeline cache: " << describe(error) << std::endl;
        return nil;
    }
    return state;
}

id<MTLDepthStencilState> MetalDevice::Impl::depthStencil(NSUInteger key)
{
    auto& state = depthStencilStates[key];
    if (state)
        return state;
    auto* const descriptor = [MTLDepthStencilDescriptor new];
    // Bit 0: stencil enabled, bits 1-4: comparison, bits 5-8: update operation, bits 9-16: read mask
    if (key & 1u)
    {
        auto* const stencil               = [MTLStencilDescriptor new];
        stencil.stencilCompareFunction    = static_cast<MTLCompareFunction>((key >> 1) & 0xFu);
        stencil.stencilFailureOperation   = MTLStencilOperationKeep;
        stencil.depthFailureOperation     = MTLStencilOperationKeep;
        stencil.depthStencilPassOperation = static_cast<MTLStencilOperation>((key >> 5) & 0xFu);
        stencil.readMask                  = static_cast<std::uint32_t>((key >> 9) & 0xFFu);
        stencil.writeMask                 = 0xFF;
        descriptor.frontFaceStencil       = stencil;
        descriptor.backFaceStencil        = stencil;
    }
    descriptor.depthCompareFunction = MTLCompareFunctionAlways;
    descriptor.depthWriteEnabled    = NO;
    state                           = [device newDepthStencilStateWithDescriptor:descriptor];
    if (!state)
        err() << "Failed to create Metal depth/stencil state" << std::endl;
    return state;
}

id<MTLRenderPipelineState> MetalDevice::Impl::presentPipeline(MTLPixelFormat format)
{
    auto& state = presentPipelines[format];
    if (state)
        return state;
    auto* const descriptor                     = [MTLRenderPipelineDescriptor new];
    descriptor.vertexFunction                  = [library newFunctionWithName:@(MetalBuiltIn::presentVertex.data())];
    descriptor.fragmentFunction                = [library newFunctionWithName:@(MetalBuiltIn::presentFragment.data())];
    descriptor.colorAttachments[0].pixelFormat = format;
    state                                      = makePipeline(descriptor);
    return state;
}

id<MTLSamplerState> MetalDevice::Impl::sampler(bool smooth, bool repeated, bool mipmapped) const
{
    return samplers[static_cast<std::size_t>(smooth) | (static_cast<std::size_t>(repeated) << 1) |
                    (static_cast<std::size_t>(mipmapped) << 2)];
}


////////////////////////////////////////////////////////////
MetalDevice::MetalDevice() : m_impl(std::make_unique<Impl>())
{
}

MetalDevice::~MetalDevice()
{
    const auto guard = lock();
    m_impl->program.reset();
    m_impl->white.reset();
    (void)m_impl->submit(true);
    m_impl->archives = nil;
    m_impl->archive  = nil;
    for (const auto& imported : m_impl->importedArchives)
        MetalDeviceImpl::removeFile(imported.path);
    m_impl->importedArchives.clear();
    m_impl->archiveData.reset();
}

std::shared_ptr<MetalDevice> MetalDevice::create()
{
    @autoreleasepool
    {
        auto  result = std::shared_ptr<MetalDevice>(new MetalDevice);
        auto& impl   = *result->m_impl;
        impl.device  = MTLCreateSystemDefaultDevice();
        if (!impl.device)
        {
            err() << "No Metal device is available" << std::endl;
            return nullptr;
        }
        impl.queue = [impl.device newCommandQueue];
        if (!impl.queue)
        {
            err() << "Failed to create the Metal command queue" << std::endl;
            return nullptr;
        }
        NSError* error = nil;
        impl.library   = [impl.device newLibraryWithSource:@(MetalBuiltIn::library.data()) options:nil error:&error];
        if (!impl.library)
        {
            err() << "Failed to compile the built-in Metal shaders: " << describe(error) << std::endl;
            return nullptr;
        }
        impl.archive = [impl.device newBinaryArchiveWithDescriptor:[MTLBinaryArchiveDescriptor new] error:&error];
        if (!impl.archive)
        {
            err() << "Failed to create the Metal pipeline cache: " << describe(error) << std::endl;
            return nullptr;
        }
        impl.archives = [NSMutableArray arrayWithObject:impl.archive];
        impl.frames   = dispatch_semaphore_create(MetalDeviceImpl::framesInFlight);
        impl.pool     = std::make_shared<MetalTransientPool>();
        for (std::size_t index = 0; index < impl.samplers.size(); ++index)
        {
            const bool  smooth      = index & 1u;
            const bool  repeated    = index & 2u;
            const bool  mipmapped   = index & 4u;
            auto* const descriptor  = [MTLSamplerDescriptor new];
            descriptor.minFilter    = smooth ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
            descriptor.magFilter    = descriptor.minFilter;
            descriptor.mipFilter    = mipmapped ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNotMipmapped;
            descriptor.sAddressMode = repeated ? MTLSamplerAddressModeRepeat : MTLSamplerAddressModeClampToEdge;
            descriptor.tAddressMode = descriptor.sAddressMode;
            descriptor.lodMaxClamp  = mipmapped ? FLT_MAX : 0.f;
            impl.samplers[index]    = [impl.device newSamplerStateWithDescriptor:descriptor];
            if (!impl.samplers[index])
            {
                err() << "Failed to create a Metal sampler" << std::endl;
                return nullptr;
            }
        }
        return result;
    }
}

std::unique_lock<std::recursive_mutex> MetalDevice::lock() const
{
    return std::unique_lock(m_mutex);
}

MetalDevice::Impl& MetalDevice::impl() const
{
    return *m_impl;
}

unsigned int MetalDevice::maximumTextureSize() const
{
    return 16384;
}

unsigned int MetalDevice::maximumSamples() const
{
    for (const NSUInteger count : {8u, 4u, 2u})
        if ([m_impl->device supportsTextureSampleCount:count])
            return static_cast<unsigned int>(count);
    return 1;
}

std::shared_ptr<MetalImage> MetalDevice::makeTexture(Vector2u size, bool sRgb, bool mipmapped, bool floating)
{
    const auto guard = lock();
    if (!size.x || !size.y || size.x > maximumTextureSize() || size.y > maximumTextureSize())
        return nullptr;
    @autoreleasepool
    {
        auto&       impl       = *m_impl;
        auto* const descriptor = [MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat:floating ? MTLPixelFormatRGBA32Float
                                               : sRgb   ? MTLPixelFormatRGBA8Unorm_sRGB
                                                        : MTLPixelFormatRGBA8Unorm
                                         width:size.x
                                        height:size.y
                                     mipmapped:mipmapped ? YES : NO];
        descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget | MTLTextureUsagePixelFormatView;
        descriptor.storageMode = MTLStorageModePrivate;
        auto image             = std::make_shared<MetalImage>();
        image->texture         = [impl.device newTextureWithDescriptor:descriptor];
        image->size            = size;
        if (!image->texture)
        {
            err() << "Failed to create a " << size.x << "x" << size.y << " Metal texture" << std::endl;
            return nullptr;
        }
        // Private textures start with undefined contents, other backends start out transparent black
        auto* const pass                     = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture     = image->texture;
        pass.colorAttachments[0].loadAction  = MTLLoadActionClear;
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        pass.colorAttachments[0].clearColor  = MTLClearColorMake(0, 0, 0, 0);
        if (!impl.render(nullptr, pass))
            return nullptr;
        impl.endEncoding();
        return image;
    }
}

std::shared_ptr<MetalBuffer> MetalDevice::makeBuffer(std::size_t bytes)
{
    const auto guard  = lock();
    auto       result = std::make_shared<MetalBuffer>();
    result->buffer    = [m_impl->device newBufferWithLength:std::max<std::size_t>(bytes, 4)
                                                 options:MTLResourceStorageModePrivate];
    result->capacity  = bytes;
    if (!result->buffer)
    {
        err() << "Failed to create a Metal buffer of " << bytes << " bytes" << std::endl;
        return nullptr;
    }
    return result;
}

bool MetalDevice::upload(const MetalImage& image, const std::uint8_t* pixels, Vector2u size, Vector2u offset)
{
    return image.texture.pixelFormat != MTLPixelFormatRGBA32Float && uploadPixels(image, pixels, size, offset, 4);
}

bool MetalDevice::uploadFloats(const MetalImage& image, const float* pixels, Vector2u size, Vector2u offset)
{
    return image.texture.pixelFormat == MTLPixelFormatRGBA32Float &&
           uploadPixels(image, pixels, size, offset, 4 * sizeof(float));
}

bool MetalDevice::uploadPixels(const MetalImage& image, const void* pixels, Vector2u size, Vector2u offset, std::size_t pixelSize)
{
    const auto guard = lock();
    if (!pixels || offset.x > image.size.x || offset.y > image.size.y || size.x > image.size.x - offset.x ||
        size.y > image.size.y - offset.y)
        return false;
    if (!size.x || !size.y)
        return true;
    @autoreleasepool
    {
        const std::size_t sourceRowBytes = static_cast<std::size_t>(size.x) * pixelSize;
        const std::size_t rowBytes       = (sourceRowBytes + 255) & ~std::size_t{255};
        const std::size_t bytes          = rowBytes * size.y;
        const auto        staging        = m_impl->allocate(bytes, 256);
        if (!staging.buffer)
            return false;
        for (unsigned int row = 0; row < size.y; ++row)
            std::memcpy(static_cast<std::byte*>(staging.contents) + row * rowBytes,
                        static_cast<const std::byte*>(pixels) + row * sourceRowBytes,
                        sourceRowBytes);
        const id<MTLBlitCommandEncoder> encoder = m_impl->blit();
        if (!encoder)
            return false;
        [encoder copyFromBuffer:staging.buffer
                   sourceOffset:staging.offset
              sourceBytesPerRow:rowBytes
            sourceBytesPerImage:bytes
                     sourceSize:MTLSizeMake(size.x, size.y, 1)
                      toTexture:image.texture
               destinationSlice:0
               destinationLevel:0
              destinationOrigin:MTLOriginMake(offset.x, offset.y, 0)];
        return true;
    }
}

bool MetalDevice::copy(const MetalImage& source, const MetalImage& destination, Vector2u offset)
{
    const auto guard = lock();
    if (offset.x > destination.size.x || offset.y > destination.size.y ||
        source.size.x > destination.size.x - offset.x || source.size.y > destination.size.y - offset.y)
        return false;
    if (source.texture == destination.texture)
        return true;
    @autoreleasepool
    {
        id<MTLTexture> view = source.texture;
        if (view.pixelFormat != destination.texture.pixelFormat)
            view = [source.texture newTextureViewWithPixelFormat:destination.texture.pixelFormat];
        [m_impl->blit()
              copyFromTexture:view
                  sourceSlice:0
                  sourceLevel:0
                 sourceOrigin:MTLOriginMake(0, 0, 0)
                   sourceSize:MTLSizeMake(source.size.x, source.size.y, 1)
                    toTexture:destination.texture
             destinationSlice:0
             destinationLevel:0
            destinationOrigin:MTLOriginMake(offset.x, offset.y, 0)];
        return true;
    }
}

Image MetalDevice::readImage(const MetalImage& image)
{
    if (image.texture.pixelFormat == MTLPixelFormatRGBA32Float)
        return {};
    const auto guard = lock();
    @autoreleasepool
    {
        const std::size_t   rowBytes = static_cast<std::size_t>(image.size.x) * 4;
        const std::size_t   bytes    = rowBytes * image.size.y;
        const id<MTLBuffer> readback = [m_impl->device newBufferWithLength:bytes options:MTLResourceStorageModeShared];
        if (!readback)
        {
            err() << "Failed to allocate Metal readback memory" << std::endl;
            return {};
        }
        [m_impl->blit() copyFromTexture:image.texture
                            sourceSlice:0
                            sourceLevel:0
                           sourceOrigin:MTLOriginMake(0, 0, 0)
                             sourceSize:MTLSizeMake(image.size.x, image.size.y, 1)
                               toBuffer:readback
                      destinationOffset:0
                 destinationBytesPerRow:rowBytes
               destinationBytesPerImage:bytes];
        if (!m_impl->submit(true))
            return {};
        return {image.size, static_cast<const std::uint8_t*>(readback.contents)};
    }
}

bool MetalDevice::generateMipmap(const MetalImage& image)
{
    const auto guard = lock();
    if (image.texture.mipmapLevelCount < 2)
        return false;
    @autoreleasepool
    {
        [m_impl->blit() generateMipmapsForTexture:image.texture];
        return true;
    }
}

bool MetalDevice::write(const MetalBuffer& buffer, const void* data, std::size_t offset, std::size_t bytes)
{
    const auto guard = lock();
    if (!data || offset > buffer.capacity || bytes > buffer.capacity - offset)
        return false;
    if (!bytes)
        return true;
    @autoreleasepool
    {
        const auto staging = m_impl->allocate(bytes, 16);
        if (!staging.buffer)
            return false;
        std::memcpy(staging.contents, data, bytes);
        [m_impl->blit() copyFromBuffer:staging.buffer
                          sourceOffset:staging.offset
                              toBuffer:buffer.buffer
                     destinationOffset:offset
                                  size:bytes];
        return true;
    }
}

bool MetalDevice::copy(const MetalBuffer& source, const MetalBuffer& destination, std::size_t bytes)
{
    const auto guard = lock();
    if (bytes > source.capacity || bytes > destination.capacity)
        return false;
    if (!bytes || source.buffer == destination.buffer)
        return true;
    @autoreleasepool
    {
        [m_impl->blit() copyFromBuffer:source.buffer
                          sourceOffset:0
                              toBuffer:destination.buffer
                     destinationOffset:0
                                  size:bytes];
        return true;
    }
}

const std::shared_ptr<MetalImage>& MetalDevice::whiteTexture()
{
    const auto guard = lock();
    if (!m_impl->white)
    {
        auto                                  white = makeTexture({1, 1}, false, false);
        constexpr std::array<std::uint8_t, 4> pixel{255, 255, 255, 255};
        if (white && upload(*white, pixel.data(), {1, 1}, {}))
            m_impl->white = std::move(white);
    }
    return m_impl->white;
}

const std::shared_ptr<MetalProgram>& MetalDevice::defaultProgram()
{
    const auto guard = lock();
    if (!m_impl->program)
        m_impl->program = MetalProgram::create(*this);
    return m_impl->program;
}

bool MetalDevice::submit(bool wait)
{
    const auto guard = lock();
    @autoreleasepool
    {
        return m_impl->submit(wait);
    }
}

bool MetalDevice::loadPipelineCache(const void* data, std::size_t size)
{
    using namespace MetalDeviceImpl;
    const auto guard = lock();
    if (!data || !size)
        return false;
    const auto* bytes   = static_cast<const std::uint8_t*>(data);
    const auto  matches = [&](const std::shared_ptr<const std::vector<std::uint8_t>>& existing)
    { return existing && existing->size() == size && std::equal(existing->begin(), existing->end(), bytes); };
    if (matches(m_impl->archiveData) ||
        std::any_of(m_impl->importedArchives.begin(),
                    m_impl->importedArchives.end(),
                    [&](const Impl::ImportedArchive& imported) { return matches(imported.data); }))
        return true;
    @autoreleasepool
    {
        Impl::ImportedArchive imported{std::make_shared<const std::vector<std::uint8_t>>(bytes, bytes + size),
                                       temporaryPath()};
        m_impl->importedArchives.reserve(m_impl->importedArchives.size() + 1);
        const auto& path = imported.path;
        if (![[NSData dataWithBytes:data length:size] writeToURL:fileUrl(path) atomically:NO])
        {
            err() << "Failed to stage the Metal pipeline cache in " << path << std::endl;
            removeFile(path);
            return false;
        }
        auto* const descriptor             = [MTLBinaryArchiveDescriptor new];
        descriptor.url                     = fileUrl(path);
        NSError*                   error   = nil;
        const id<MTLBinaryArchive> archive = [m_impl->device newBinaryArchiveWithDescriptor:descriptor error:&error];
        if (!archive)
        {
            err() << "Failed to load the Metal pipeline cache: " << describe(error) << std::endl;
            removeFile(path);
            return false;
        }
        // Adopt the first import while the initial archive is empty; later imports remain lookup sources.
        // Pipelines found only in those sources are added incrementally to the primary archive when used.
        if (m_impl->importedArchives.empty() && !m_impl->archiveHasEntries)
        {
            [m_impl->archives removeObjectIdenticalTo:m_impl->archive];
            m_impl->archive           = archive;
            m_impl->archiveData       = imported.data;
            m_impl->archiveDirty      = false;
            m_impl->archiveHasEntries = true;
        }
        [m_impl->archives addObject:archive];
        // Imported archives may read their files lazily, so retain both the source bytes and file until teardown.
        m_impl->importedArchives.push_back(std::move(imported));
        return true;
    }
}

std::optional<std::vector<std::uint8_t>> MetalDevice::getPipelineCacheData()
{
    using namespace MetalDeviceImpl;
    const auto guard = lock();
    if (!m_impl->archiveDirty && m_impl->archiveData)
        return *m_impl->archiveData;
    @autoreleasepool
    {
        const auto path  = temporaryPath();
        NSError*   error = nil;
        if (![m_impl->archive serializeToURL:fileUrl(path) error:&error])
        {
            err() << "Failed to serialize the Metal pipeline cache: " << describe(error) << std::endl;
            removeFile(path);
            return std::nullopt;
        }
        NSData* const contents = [NSData dataWithContentsOfURL:fileUrl(path)];
        removeFile(path);
        if (!contents || !contents.length)
        {
            err() << "Failed to read the serialized Metal pipeline cache" << std::endl;
            return std::nullopt;
        }
        const auto* bytes    = static_cast<const std::uint8_t*>(contents.bytes);
        m_impl->archiveData  = std::make_shared<const std::vector<std::uint8_t>>(bytes, bytes + contents.length);
        m_impl->archiveDirty = false;
        return *m_impl->archiveData;
    }
}

} // namespace sf::priv
