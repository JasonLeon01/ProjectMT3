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

#ifndef __OBJC__
#error "MetalObjects.hpp may only be included from Objective-C++ sources"
#endif

#include <SFML/Graphics/Metal/MetalDevice.hpp>
#include <SFML/Graphics/Metal/MetalProgram.hpp>

#import <Metal/Metal.h>
#include <array>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace sf::priv
{
struct MetalImage
{
    id<MTLTexture> texture;
    Vector2u       size;
};

struct MetalBuffer
{
    id<MTLBuffer> buffer;
    std::size_t   capacity{};
};

struct MetalTransient
{
    id<MTLBuffer> buffer;
    NSUInteger    offset{};
    void*         contents{};
};

struct MetalTransientPool
{
    std::mutex                 mutex;
    std::vector<id<MTLBuffer>> free;
};

struct MetalTimingRange
{
    std::vector<id<MTLCommandBuffer>> buffers;
    bool                              capturing{true};
};

// Color format, depth format, stencil format, sample count, 6 blend factors/equations, color write mask
using MetalPipelineKey = std::array<NSUInteger, 11>;

struct MetalDevice::Impl
{
    struct ImportedArchive
    {
        std::shared_ptr<const std::vector<std::uint8_t>> data;
        std::string                                      path;
    };

    id<MTLDevice>                                        device;
    id<MTLCommandQueue>                                  queue;
    id<MTLLibrary>                                       library;
    id<MTLCommandBuffer>                                 commands;
    id<MTLRenderCommandEncoder>                          renderEncoder;
    id<MTLBlitCommandEncoder>                            blitEncoder;
    const void*                                          renderOwner{};
    dispatch_semaphore_t                                 frames;
    std::array<id<MTLSamplerState>, 8>                   samplers;
    std::map<MTLPixelFormat, id<MTLRenderPipelineState>> presentPipelines;
    std::map<NSUInteger, id<MTLDepthStencilState>>       depthStencilStates;
    id<MTLBinaryArchive>                                 archive;
    NSMutableArray<id<MTLBinaryArchive>>*                archives;
    std::vector<ImportedArchive>                         importedArchives;
    std::shared_ptr<const std::vector<std::uint8_t>>     archiveData;
    bool                                                 archiveDirty{};
    bool                                                 archiveHasEntries{};
    std::shared_ptr<MetalTransientPool>                  pool;
    std::vector<id<MTLBuffer>>                           used;
    id<MTLBuffer>                                        chunk;
    NSUInteger                                           chunkOffset{};
    std::shared_ptr<MetalImage>                          white;
    std::shared_ptr<MetalProgram>                        program;
    std::vector<std::weak_ptr<MetalTimingRange>>         timingRanges;

    [[nodiscard]] id<MTLCommandBuffer>        commandBuffer();
    [[nodiscard]] id<MTLBlitCommandEncoder>   blit();
    [[nodiscard]] id<MTLRenderCommandEncoder> render(const void* owner, MTLRenderPassDescriptor* pass);
    [[nodiscard]] bool                        rendering(const void* owner) const;
    void                                      endEncoding();
    [[nodiscard]] bool                        submit(bool wait, bool paced = false);
    [[nodiscard]] MetalTransient              allocate(std::size_t bytes, std::size_t alignment);
    [[nodiscard]] id<MTLRenderPipelineState>  makePipeline(MTLRenderPipelineDescriptor* descriptor,
                                                           MTLPipelineOption            options = MTLPipelineOptionNone,
                                                           MTLAutoreleasedRenderPipelineReflection* reflection = nullptr);
    [[nodiscard]] id<MTLDepthStencilState>    depthStencil(NSUInteger key);
    [[nodiscard]] id<MTLRenderPipelineState>  presentPipeline(MTLPixelFormat format);
    [[nodiscard]] id<MTLSamplerState>         sampler(bool smooth, bool repeated, bool mipmapped) const;
};

struct MetalUniform
{
    std::string name;
    NSUInteger  offset{};
    MTLDataType type{MTLDataTypeNone};
    NSUInteger  elements{};
    NSUInteger  stride{};
};

struct MetalUniformBufferLayout
{
    NSUInteger                index{};
    NSUInteger                size{};
    std::vector<MetalUniform> uniforms;
};

struct MetalSamplerLayout
{
    std::string name;
    NSUInteger  index{};
};

struct MetalStageReflection
{
    bool                                  draw{};
    std::vector<MetalUniformBufferLayout> buffers;
    std::vector<MetalSamplerLayout>       textures;
    std::vector<NSUInteger>               samplers;
};

struct MetalProgramPipelines
{
    std::mutex                                             mutex;
    std::map<MetalPipelineKey, id<MTLRenderPipelineState>> values;
};

struct MetalProgramData
{
    rhi::ShaderOptions                     options;
    id<MTLLibrary>                         vertexLibrary;
    id<MTLLibrary>                         fragmentLibrary;
    id<MTLFunction>                        vertex;
    id<MTLFunction>                        fragment;
    MTLVertexDescriptor*                   layout;
    id<MTLRenderPipelineState>             capturePipeline;
    std::array<MetalStageReflection, 2>    stages;
    std::shared_ptr<MetalProgramPipelines> pipelines{std::make_shared<MetalProgramPipelines>()};
};

struct MetalUniformBuffer
{
    const MetalUniformBufferLayout* layout{};
    std::vector<std::byte>          data;
};

struct MetalSampler
{
    const MetalSamplerLayout*     layout{};
    std::shared_ptr<rhi::Texture> texture;
    bool                          current{true};
};

struct MetalStage
{
    const MetalStageReflection*     reflection{};
    std::vector<MetalUniformBuffer> buffers;
    std::vector<MetalSampler>       textures;
};

struct MetalProgram::Impl
{
    std::mutex                              mutex;
    std::shared_ptr<const MetalProgramData> data;
    std::array<MetalStage, 2>               stages;

    void initialize();
    [[nodiscard]] bool writeUniform(const std::string& name, ShaderUniformType type, const void* values, std::size_t count);
    [[nodiscard]] id<MTLRenderPipelineState> pipeline(MetalDevice::Impl& device, const MetalPipelineKey& key) const;
    [[nodiscard]] bool                       bind(MetalDevice::Impl&          device,
                                                  id<MTLRenderCommandEncoder> encoder,
                                                  const MetalDrawParameters&  parameters,
                                                  const rhi::Texture*         texture,
                                                  id<MTLTexture>              attachment);
};

inline constexpr NSUInteger metalVertexBufferIndex       = 30;
inline constexpr NSUInteger metalCustomVertexBufferIndex = 16;
inline constexpr NSUInteger metalInlineDataLimit         = 4096;

[[nodiscard]] std::string describe(NSError* error);
} // namespace sf::priv
