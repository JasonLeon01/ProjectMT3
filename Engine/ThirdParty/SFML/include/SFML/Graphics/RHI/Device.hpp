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

#include <SFML/Graphics/Export.hpp>

#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/CoordinateType.hpp>
#include <SFML/Graphics/GraphicsBackend.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/StencilMode.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace sf
{
class WindowBase;
class Texture;
class Shader;
class RenderTarget;
} // namespace sf

namespace sf::rhi
{
enum class BufferUsage
{
    Stream,
    Dynamic,
    Static
};
enum class UniformType : unsigned char
{
    Float,
    Vec2,
    Vec3,
    Vec4,
    Int,
    IVec2,
    IVec3,
    IVec4,
    Mat3,
    Mat4
};

struct VertexBinding
{
    std::size_t  stride{};
    unsigned int stepRate{};
};
struct VertexAttribute
{
    std::string  name;
    unsigned int location{};
    unsigned int binding{};
    unsigned int components{4};
    std::size_t  offset{};
    std::string  semantic{"TEXCOORD"};
    unsigned int semanticIndex{};
};
struct TransformOutput
{
    std::string  name;
    std::string  semantic{"TEXCOORD"};
    unsigned int semanticIndex{};
    unsigned int components{4};
};
struct UniformAlias
{
    std::string name;
    std::string translatedName;
};
struct SamplerCoordinateTransform
{
    std::string name;
    std::string uniformName;
};
struct ShaderOptions
{
    /// Native-backend sampling transforms generated for GLSL sampler uniforms.
    std::vector<SamplerCoordinateTransform> samplerCoordinateTransforms;
    std::vector<UniformAlias>               uniformAliases;
    /// Preserve GLSL render-texture coordinates and SPIRV-Cross matrix upload layout on native backends.
    bool                         glslMatrixLayout{};
    std::vector<VertexBinding>   vertexBindings;
    std::vector<VertexAttribute> vertexAttributes;
    std::vector<TransformOutput> transformOutputs;
    std::size_t                  transformStride{};
    unsigned int                 metalOutputBufferIndex{28};
    unsigned int                 metalIndirectBufferIndex{29};
};
struct UniformInfo
{
    std::string name;
    UniformType type{UniformType::Float};
    std::size_t count{1};
    bool        sampler{};
};
/// Empty fields mean the identity is unavailable; do not reuse a persistent device cache.
struct CacheIdentity
{
    std::string device;
    std::string driver;
};
enum class ReadbackStatus
{
    Pending,
    Ready,
    Failed
};
class SFML_GRAPHICS_API Readback
{
public:
    virtual ~Readback()                                                 = default;
    [[nodiscard]] virtual ReadbackStatus                   poll()       = 0;
    [[nodiscard]] virtual const std::vector<std::uint8_t>& data() const = 0;
};
class SFML_GRAPHICS_API Timer
{
public:
    virtual ~Timer()   = default;
    virtual void end() = 0;
    /// nullopt while pending, a negative value when completed but invalid.
    [[nodiscard]] virtual std::optional<double> elapsedMilliseconds() = 0;
};

/// RGBA8 2D texture. Uploads and readbacks use top-left image coordinates.
class SFML_GRAPHICS_API Texture
{
public:
    [[nodiscard]] virtual const void* implementationType() const
    {
        return nullptr;
    }
    virtual ~Texture()                                                                                     = default;
    [[nodiscard]] virtual Vector2u getSize() const                                                         = 0;
    [[nodiscard]] virtual bool     isSrgb() const                                                          = 0;
    [[nodiscard]] virtual bool     isSmooth() const                                                        = 0;
    [[nodiscard]] virtual bool     isRepeated() const                                                      = 0;
    [[nodiscard]] virtual bool     hasMipmap() const                                                       = 0;
    virtual void                   setSmooth(bool smooth)                                                  = 0;
    virtual void                   setRepeated(bool repeated)                                              = 0;
    [[nodiscard]] virtual bool     upload(const std::uint8_t* pixels, Vector2u size, Vector2u offset = {}) = 0;
    [[nodiscard]] virtual bool     copy(const Texture& source, Vector2u offset = {})                       = 0;
    [[nodiscard]] virtual Image    read() const                                                            = 0;
    [[nodiscard]] virtual bool     generateMipmap()                                                        = 0;
    [[nodiscard]] virtual bool     glslFlipped() const
    {
        return false;
    }
    [[nodiscard]] virtual bool uploadFloats(const float*, Vector2u, Vector2u = {})
    {
        return false;
    }
    virtual void                                invalidateMipmap()                          = 0;
    [[nodiscard]] virtual std::array<float, 16> coordinateMatrix(CoordinateType type) const = 0;
};

/// Byte-addressed vertex storage. Updates never resize the allocation.
class SFML_GRAPHICS_API Buffer
{
public:
    [[nodiscard]] virtual const void* implementationType() const
    {
        return nullptr;
    }
    virtual ~Buffer()                                                                                     = default;
    [[nodiscard]] virtual std::size_t getSize() const                                                     = 0;
    [[nodiscard]] virtual bool        upload(const void* data, std::size_t bytes, std::size_t offset = 0) = 0;
    [[nodiscard]] virtual bool        copy(const Buffer& source)                                          = 0;
    [[nodiscard]] virtual bool        read(void* data, std::size_t bytes, std::size_t offset = 0) const   = 0;
};

struct VertexBufferBinding
{
    std::shared_ptr<Buffer> buffer;
    std::size_t             offset{};
};

/// Sources retain each backend's existing format (GLSL, SPIR-V, HLSL or MSL/metallib).
class SFML_GRAPHICS_API ShaderProgram
{
public:
    [[nodiscard]] virtual const void* implementationType() const
    {
        return nullptr;
    }
    virtual ~ShaderProgram() = default;
    /// Copy binding state while retaining compiled code and pipelines. Unsupported backends return null.
    [[nodiscard]] virtual std::shared_ptr<ShaderProgram> clone() const
    {
        return {};
    }

    [[nodiscard]] virtual std::vector<UniformInfo> getActiveUniforms() const
    {
        return {};
    }
    [[nodiscard]] virtual const ShaderOptions& getOptions() const
    {
        static const ShaderOptions options;
        return options;
    }
    virtual void setUniform(const std::string& name, UniformType type, const void* values, std::size_t count) = 0;
    virtual void setSampler(const std::string& name, std::shared_ptr<Texture> texture, bool current = false)  = 0;
};

/// Pixel viewport/scissor use a top-left origin; projection uses SFML clip coordinates.
struct TargetState
{
    Vector2u  size;
    IntRect   viewport;
    IntRect   scissor;
    Transform projection;
    bool      sRgb{};
};

struct DrawSettings
{
    BlendMode                      blendMode{BlendAlpha};
    StencilMode                    stencilMode;
    Transform                      transform;
    CoordinateType                 coordinateType{CoordinateType::Pixels};
    std::shared_ptr<Texture>       texture;
    std::shared_ptr<ShaderProgram> shader;
};

/// Commands execute in order. Implementations retain submitted GPU resources as needed.
/// A context must not be recorded concurrently from different threads.
class SFML_GRAPHICS_API CommandContext
{
public:
    virtual ~CommandContext() = default;
    [[nodiscard]] virtual bool drawVertices(
        const TargetState&,
        const std::vector<VertexBufferBinding>&,
        std::size_t,
        std::size_t,
        std::size_t,
        PrimitiveType,
        const DrawSettings& = {})
    {
        return false;
    }
    [[nodiscard]] virtual bool prewarm(const TargetState&, const DrawSettings&)
    {
        return false;
    }

    virtual void clear(const TargetState& state, std::optional<Color> color, std::optional<StencilValue> stencil = {}) = 0;
    virtual void draw(const TargetState&  target,
                      const Vertex*       vertices,
                      std::size_t         count,
                      PrimitiveType       type,
                      const DrawSettings& state = {}) = 0;
    virtual void draw(const TargetState&  target,
                      const Buffer&       buffer,
                      std::size_t         first,
                      std::size_t         count,
                      PrimitiveType       type,
                      const DrawSettings& state = {}) = 0;
};

class SFML_GRAPHICS_API RenderTarget
{
public:
    virtual ~RenderTarget()                                             = default;
    [[nodiscard]] virtual Vector2u               getSize() const        = 0;
    [[nodiscard]] virtual const ContextSettings& settings() const       = 0;
    [[nodiscard]] virtual CommandContext&        commands()             = 0;
    [[nodiscard]] virtual bool                   setActive(bool active) = 0;
    /// Resolve color attachments and make subsequent sampling/readback see completed draws.
    virtual void finish() = 0;
};

/// Release this surface and its render-target references before closing,
/// recreating or destroying the associated WindowBase.
/// One surface per native window. Call resize after processing a resize event.
class SFML_GRAPHICS_API WindowSurface
{
public:
    virtual ~WindowSurface()                                                                 = default;
    [[nodiscard]] virtual std::shared_ptr<RenderTarget> getRenderTarget() const              = 0;
    [[nodiscard]] virtual bool                          setActive(bool active)               = 0;
    virtual void                                        resize(Vector2u size)                = 0;
    virtual void                                        display()                            = 0;
    virtual void                                        setVerticalSyncEnabled(bool enabled) = 0;
    [[nodiscard]] virtual bool                          copyTo(Texture& destination, Vector2u offset = {}) const = 0;
};

struct Capabilities
{
    unsigned int maximumTextureSize{};
    unsigned int maximumAntiAliasingLevel{};
    bool         shaders{};
    bool         geometryShaders{};
    bool         vertexTransforms{};
    bool         instancedDrawing{};
    bool         floatTextures{};
    bool         asynchronousReadback{};
    bool         timers{};
};

class SFML_GRAPHICS_API Device
{
public:
    virtual ~Device() = default;
    [[nodiscard]] virtual CacheIdentity getCacheIdentity() const
    {
        return {};
    }
    [[nodiscard]] virtual std::shared_ptr<Texture> createFloatTexture(Vector2u, const float* = nullptr)
    {
        return {};
    }
    [[nodiscard]] virtual std::shared_ptr<ShaderProgram> createShader(
        std::string_view     vertex,
        std::string_view     geometry,
        std::string_view     fragment,
        const ShaderOptions& options)
    {
        return options.vertexBindings.empty() && options.transformOutputs.empty()
                   ? createShader(vertex, geometry, fragment)
                   : nullptr;
    }
    [[nodiscard]] virtual bool transformVertices(const std::shared_ptr<ShaderProgram>&,
                                                 const std::vector<VertexBufferBinding>&,
                                                 std::size_t,
                                                 const std::shared_ptr<Buffer>&)
    {
        return false;
    }
    [[nodiscard]] virtual std::shared_ptr<Readback> readTextureAsync(const std::shared_ptr<Texture>&, IntRect)
    {
        return {};
    }
    [[nodiscard]] virtual std::shared_ptr<Timer> beginTimer()
    {
        return {};
    }
    [[nodiscard]] bool prewarm(RenderTarget& target, const DrawSettings& state = {})
    {
        const auto size = target.getSize();
        return target.commands()
            .prewarm({size, {{0, 0}, Vector2i(size)}, {{0, 0}, Vector2i(size)}, Transform::Identity, false}, state);
    }

    [[nodiscard]] virtual GraphicsBackend                  backend() const                                 = 0;
    [[nodiscard]] virtual ContextSettings::BackendSettings settings() const                                = 0;
    [[nodiscard]] virtual Capabilities                     capabilities() const                            = 0;
    [[nodiscard]] virtual std::shared_ptr<Texture>         createTexture(Vector2u size, bool sRgb = false) = 0;
    [[nodiscard]] virtual std::shared_ptr<Buffer> createBuffer(std::size_t bytes, BufferUsage usage = BufferUsage::Stream) = 0;
    [[nodiscard]] virtual std::shared_ptr<ShaderProgram> createShader(std::string_view vertex,
                                                                      std::string_view geometry,
                                                                      std::string_view fragment)                  = 0;
    [[nodiscard]] virtual std::shared_ptr<RenderTarget>  createRenderTarget(std::shared_ptr<Texture> color,
                                                                            const ContextSettings&   settings = {}) = 0;
    [[nodiscard]] std::unique_ptr<WindowSurface> createWindowSurface(WindowBase& window, const ContextSettings& settings = {});
    [[nodiscard]] virtual bool submit(bool wait = false)                                  = 0;
    [[nodiscard]] virtual bool loadPipelineCache(const void* data, std::size_t size)      = 0;
    [[nodiscard]] virtual std::optional<std::vector<std::uint8_t>> getPipelineCacheData() = 0;
};

/// References retain resources; reacquire a target after resize/recreation.
[[nodiscard]] SFML_GRAPHICS_API std::shared_ptr<Texture> getTexture(const sf::Texture& texture);
[[nodiscard]] SFML_GRAPHICS_API std::shared_ptr<ShaderProgram> getShader(const sf::Shader& shader);
SFML_GRAPHICS_API void          setShader(sf::Shader& shader, std::shared_ptr<ShaderProgram> program);
[[nodiscard]] SFML_GRAPHICS_API std::shared_ptr<RenderTarget> getRenderTarget(sf::RenderTarget& target);
[[nodiscard]] SFML_GRAPHICS_API TargetState                   getTargetState(const sf::RenderTarget& target);

/// Acquire SFML's process-wide device, selected with setGraphicsBackend before first use.
/// A successful acquisition commits the backend, as does creating an SFML GPU resource.
[[nodiscard]] SFML_GRAPHICS_API std::shared_ptr<Device> getDevice();
} // namespace sf::rhi
