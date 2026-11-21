# Migration from SFML 3.0 to SFML 3.1

> [!NOTE]
> For migrating from SFML 2, see the [Migration from SFML 2 to SFML 3](#migration-from-SFML-2-to-SFML-3) section below.

Welcome to SFML 3.1!  
This release brings major improvements to text layout, networking, audio device management, and more.

## SFML-ME graphics backends

Configure the SFML build with `SFML_BUILD_OPENGL_BACKEND` and
`SFML_BUILD_VULKAN_BACKEND`, and `SFML_BUILD_DIRECTX_BACKEND`. DirectX uses
Direct3D 11 and is enabled by default on Windows only. When the OpenGL backend is enabled,
`SFML_OPENGL_ES` selects GLES instead of desktop OpenGL. The Window and
Graphics targets propagate `SFML_RENDER_OPENGL`, `SFML_RENDER_OPENGLES`,
and `SFML_RENDER_VULKAN` for the backends present in that build. The Graphics
target additionally propagates `SFML_RENDER_DIRECTX` when DirectX is enabled.

Choose a backend before the first graphics resource or capability query
that uses a GPU. The following call requests Vulkan 1.3.

~~~cpp
if (!sf::setGraphicsBackend(sf::GraphicsBackend::Vulkan, sf::VulkanSettings{1, 3}))
    return 1;

sf::RenderWindow window(sf::VideoMode({800, 600}), "Vulkan");
if (!window.isOpen())
    return 1;
~~~

On Windows, automatic selection tries DirectX, then the compiled OpenGL
variant, then Vulkan. On macOS and iOS it tries Metal first. Other platforms
retain OpenGL/GLES before Vulkan.
The first successful initialization fixes the device for the process.
An explicit API version is a minimum requirement for that request.
If the driver supplies a lower version or an incompatible profile,
initialization fails. The built-in minimums are desktop OpenGL 2.0,
OpenGL ES 3.0, Vulkan 1.0, and Direct3D feature level 10_0.

Use `sf::setGraphicsBackend(sf::GraphicsBackend::DirectX, sf::DirectXSettings{11, 0})`
to require feature level 11_0 before creating resources. The supported feature
levels are 10_0, 10_1, 11_0, 11_1, 12_0 and 12_1. These are hardware capability
requirements, not API versions: even 12_0 and 12_1 use the Direct3D 11 API.
The device chooses the highest supported level satisfying the request.
Explicit requests never fall back to another backend or a lower feature level.
Windows retains its Windows 7 API baseline. WARP software rendering is used
only by the test harness, not by automatic selection in applications.

`DirectXSettings` is an additional alternative in `ContextSettings::backend`.
Recompile applications against the updated headers and libraries. Existing
rendering, shader loading and uniform function signatures are unchanged.
GL object handles return zero on DirectX, and GL-specific binding/state
operations are unavailable, as with Vulkan. `sf::Window` and `sf::Context`
continue to provide native OpenGL contexts; use `sf::RenderWindow` for DirectX.
Align single-pixel points and lines to pixel centers, for example
`{x + 0.5f, y + 0.5f}`. Pixels exactly on primitive edges follow the selected
API's rasterization rules and can differ between backends.

Move API settings into `ContextSettings::backend`. The depth, stencil,
MSAA, and sRGB fields still configure individual targets. For example,
`settings.backend = sf::OpenGLSettings{4, 1, sf::OpenGLProfile::Core}`
requests a Core context. `Window::getSettings()` reports the created
settings. Use `RenderWindow` for SFML Vulkan rendering or
`WindowBase::getNativeHandle()` for external Vulkan WSI.

Desktop Compatibility shaders keep the existing GLSL interface. Core and
GLES shaders use SFML's `sf_Vertex`, `sf_Color`, and `sf_MultiTexCoord0`
attributes, along with the `sf_ModelViewProjectionMatrix` and
`sf_TextureMatrix` uniforms. A standalone custom stage can pair with
the built-in stage using `sf_FrontColor` and `sf_TexCoord0` varyings.

With Vulkan, the existing `Shader` loaders accept binary SPIR-V for the
selected API version. Use `main` as the entry point. Vertex inputs are
position (location 0, `vec2`), color (location 1, `vec4`), and texture
coordinates (location 2, `vec2`). The default stages exchange color and
normalized texture coordinates at locations 0 and 1. Set 0 provides
the built-in draw uniform block at binding 0 (`mat4 transform` and
`vec4 textureTransform` in std140 layout, optionally followed by
`mat4 modelView` and `mat4 projection`, the separate model-view and
Vulkan clip-space projection matrices whose product is `transform`) and the
current texture at binding 1. Use set 1 for user uniform buffers and combined
2D samplers. Samplers that are not assigned through `setUniform` sample the
current texture, like GLSL samplers left at texture unit 0.
Keep SPIR-V variable names to bind values through `setUniform` and
`setUniformArray`.

With DirectX, the existing `Shader` loaders accept self-contained HLSL source
with `main` as the entry point, or precompiled DXBC bytecode (`.cso`). DXBC is
detected by its header rather than the filename and bypasses `D3DCompile`.
Source and bytecode can be mixed across stages. Retain reflection metadata
for uniforms, resources and stage signatures; DXIL is not supported.
GLSL is not translated. Source compilation uses Shader Model 4.0 at feature
level 10_0, 4.1 at 10_1, and 5.0 at 11_0 or higher. Precompiled bytecode must
target a shader model supported by the selected feature level.
Loading custom shaders requires `d3dcompiler_47.dll`; deploy the appropriate
Windows SDK redistributable with applications on systems that do not provide
it. DXBC loading uses that DLL for reflection only. Built-in shaders are
precompiled and do not require that DLL. For SPIR-V or DXBC in memory, pass a
`std::string_view` with an explicit byte length to preserve embedded zeros.

SPIR-V and DXBC are intermediate bytecode, not driver pipeline caches. With
Vulkan, use `sf::loadPipelineCacheFromMemory(data, size)` to merge a previously
exported driver cache into the device cache before drawing. Use
`sf::getPipelineCacheData()` after drawing or prewarming the required
shader/render-state combinations to obtain an optional byte vector for the
next run. These functions initialize the selected backend if necessary, so
call `sf::setGraphicsBackend` first. The cache covers all custom and built-in
shaders; SPIR-V is still required to create shader modules and pipelines.
The application owns compilation/prewarming policy, file I/O, integrity checks
and invalidation. Pass intact data from a previous export. Invalid or
incompatible version-one headers (vendor, device, cache UUID) are rejected;
they do not clear the existing cache. Driver/hardware changes can invalidate
caches, and cache hits do not guarantee that all driver work is skipped.
On other backends, import returns `false` and export returns `std::nullopt`.
Direct3D 11 driver caching is managed by the OS/driver when supported; SFML
cannot import or export that cache through the D3D11 API.

Vertex inputs are `float2 POSITION`, `float4 COLOR0`, and `float2 TEXCOORD0`.
The built-in stages exchange `float4 SV_POSITION`, `float4 COLOR0`, and
`float2 TEXCOORD0`, in that order. Declare this complete input signature when
using interpolated data with a built-in stage, even if some fields are unused;
Direct3D also requires the semantic register locations to match. A fragment
shader returns `SV_TARGET`. Missing vertex or
fragment stages are supplied by SFML, including geometry-only shader loads.
The following block is reserved at `b0`; user constant buffers start at `b1`.

~~~hlsl
cbuffer SFMLDraw : register(b0)
{
    column_major float4x4 sf_ModelViewProjectionMatrix;
    float4 sf_TextureTransform;
    column_major float4x4 sf_ModelViewMatrix;  // optional
    column_major float4x4 sf_ProjectionMatrix; // optional
};
~~~

The matrix maps SFML vertex positions to clip space. The two optional trailing
matrices, declared together, are its model-view (drawable transform) and
projection (view) factors, as in GLSL. Normalize texture
coordinates with `uv * sf_TextureTransform.xy + sf_TextureTransform.zw`.
Bind a `Texture2D` and its `SamplerState` at matching `tN`/`sN` registers
(`N < 16`); pass the texture variable's name to the existing `setUniform`
overloads. Textures that are not assigned through `setUniform`, such as
`sf_Texture`, sample the current drawing texture, like GLSL samplers left at
texture unit 0. Constants
are reflected by name across all stages, including array padding and row- or
column-major matrices. `sf::Glsl` value types remain the CPU-side uniform types.

### Metal on macOS and iOS

`SFML_BUILD_METAL_BACKEND` is enabled by default on macOS and iOS and
propagates `SFML_RENDER_METAL` from the Graphics target. OpenGL stays
available on Apple platforms through `SFML_BUILD_OPENGL_BACKEND`; either
backend may be disabled, but at least one of them is required. Automatic
selection tries Metal first, then OpenGL/GLES. OpenGL requests below the
built-in minimums (desktop OpenGL 2.0, OpenGL ES 3.0) are rejected.

Metal has no version setting: the available Metal features follow the
deployment target. SFML now requires macOS 13.0 or iOS 16.0
(`CMAKE_OSX_DEPLOYMENT_TARGET`, checked when configuring and when compiling
the Metal backend). Select Metal explicitly with
`sf::setGraphicsBackend(sf::GraphicsBackend::Metal)` or
`settings.backend = sf::MetalSettings{}`. As with DirectX and Vulkan, GL
handles return zero and GL state functions are unavailable; use
`sf::RenderWindow` to present. `setVerticalSyncEnabled` maps to
`CAMetalLayer.displaySyncEnabled` on macOS; iOS always synchronizes with the
display.

The existing `Shader` loaders accept MSL source text or a precompiled
metallib. A metallib is detected by its `MTLB` header rather than the
filename, and must be built for the running platform: macOS, iOS devices and
the iOS simulator need separate libraries. The entry point is the function
named `main0` of the matching stage (the name SPIRV-Cross generates), or the
only function of that stage in the library. Geometry shaders are not
supported. For a metallib in memory, pass a `std::string_view` with an explicit
byte length.

Vertex attributes 0, 1 and 2 are the `float2` position, `float4` color and
`float2` texture coordinates, read with `[[stage_in]]` from vertex buffer 30.
`buffer(0)` is reserved in both stages for the draw parameters, and user
buffers start at `buffer(1)`:

~~~cpp
struct SFMLDraw
{
    float4x4 sf_ModelViewProjectionMatrix;
    float4 sf_TextureTransform;
    float4x4 sf_ModelViewMatrix;  // optional
    float4x4 sf_ProjectionMatrix; // optional
};
~~~

The two optional trailing matrices, declared together, are the model-view
(drawable transform) and projection (view) factors of
`sf_ModelViewProjectionMatrix`, as in GLSL. Normalize texture coordinates with
`uv * sf_TextureTransform.xy + sf_TextureTransform.zw`. The built-in stages
exchange `float4 color [[user(sf_color)]]` and
`float2 texCoord [[user(sf_texcoord)]]`; use these names when pairing a custom
stage with a built-in one. A custom vertex shader used to draw points must
write a `[[point_size]]` output. Bind textures and their samplers at matching
`texture(N)`/`sampler(N)` indices (`N < 16`) and pass the texture parameter's
name to `setUniform`. Textures that are not assigned through `setUniform`,
such as `sf_Texture`, sample the current drawing texture, like GLSL samplers
left at texture unit 0.
Uniforms are the members of the structs bound at `buffer(1)` and above (or the
buffer parameter itself for a single value), set by member name with the usual
`setUniform` and `setUniformArray` overloads, including arrays, `bool`
vectors and `float3x3`/`float4x4` matrices.

With Vulkan, DirectX and Metal, normalized texture coordinates of a
`RenderTexture` grow downward like those of any other texture. OpenGL stores
render textures upside down, so there its normalized coordinates grow upward.
Sampling is unaffected, but shaders that offset coordinates asymmetrically
along v (for example a kernel that is not symmetric vertically) can produce
slightly different results between OpenGL and the other backends.

With Metal, `sf::getPipelineCacheData()` serializes an `MTLBinaryArchive` with
the render pipelines created since the device was initialized, and
`sf::loadPipelineCacheFromMemory(data, size)` adds a previously exported
archive that subsequent pipeline creation looks up first. Several archives can
be loaded. Archives are specific to the GPU and OS version; invalid data is
rejected without affecting the existing caches, and a stale archive simply
misses. The archive data is staged in the temporary directory while loaded.

The `stencil` example accepts `--backend directx`, `--backend metal`,
`--backend opengl`, or `--backend vulkan`; omitting the option uses the
automatic priority.
The `shader` example accepts `--backend opengl`, `--backend metal`,
`--backend directx`, or `--backend vulkan`; without the option it prefers
Metal when it is built and OpenGL otherwise. The other backends load
translations of the GLSL resources: MSL (`*.metal`), HLSL (`*.hlsl`) and
SPIR-V (`vulkan/*.spv`, compiled from the GLSL 4.50 sources next to them with
`glslang -V --target-env vulkan1.0`). Metal draws the billboards as point
sprites because it has no geometry shaders. The `island` example explicitly
selects OpenGL for its desktop GLSL resources.

## Dependencies

We've introduced a bunch of new dependencies to cover all the improvements:

- FetchContent / System Dependency
  - [HarfBuzz](https://github.com/harfbuzz/harfbuzz) for complex text layouts
  - [SheenBidi](https://github.com/Tehreer/SheenBidi) for correct bidirectional text rendering
  - [Mbed TLS](https://github.com/Mbed-TLS/mbedtls) for TLS support
  - [libssh2](https://github.com/libssh2/libssh2) for SFTP support
- Header-only
  - [cpp-unicodelib](https://github.com/yhirose/cpp-unicodelib) for proper Unicode support
  - [wepoll](https://github.com/piscisaureus/wepoll) for an improved SocketSelector
  - [qoi](https://github.com/phoboslab/qoi) to support the QOI image format

## Deprecations

Deprecated functions remain available and fully functional, though you may receive a compiler warning.
It is recommended to migrate to the alternatives described below, as they generally provide noticable improvements.

### Font Kerning

The `sf::Font::getKerning` overload that uses `std::uint32_t` to represent Unicode code points has been deprecated.
Use the overload that takes `char32_t` instead.
The behavior is identical - only the parameter type changes.

v3.0:
```cpp
float kerning = font.getKerning(static_cast<std::uint32_t>('A'),
                                static_cast<std::uint32_t>('V'),
                                30);
```

v3.1:
```cpp
float kerning = font.getKerning(U'A', U'V', 30);
```

### Character Positions in Text

`sf::Text::findCharacterPos(std::size_t index)` has been deprecated in favor of `getShapedGlyphs()`.

The old function returned the global-space position of a single character.
The new function returns the full list of `ShapedGlyph` objects produced during text shaping, giving you access to each glyph's local position, cluster ID, text direction, and baseline in addition to the underlying `sf::Glyph` data.

```cpp
struct ShapedGlyph
{
    Glyph         glyph;         // Glyph data
    Vector2f      position;      // Local position within the text
    std::uint32_t cluster;       // Cluster ID (multiple glyphs may share a cluster)
    TextDirection textDirection; // Direction of the surrounding text run
    float         baseline;      // Baseline Y position of the line this glyph is on
    std::size_t   vertexOffset;  // Starting offset into the vertex buffer
    std::size_t   vertexCount;   // Number of vertices for this glyph
};
```

To migrate, iterate over the shaped glyphs and use the `position` field.

> [!NOTE]
> Note that `getShapedGlyphs()` returns positions in local coordinates, while `findCharacterPos()` returned global coordinates.
> Apply the text's transform yourself if you need global coordinates.

v3.0:
```cpp
// Find the position of the 5th character (global space)
sf::Vector2f pos = text.findCharacterPos(4);
```

v3.1:
```cpp
// Find the position of the glyph belonging to the 5th character (local space)
const auto& glyphs = text.getShapedGlyphs();
if (glyphs.size() > 4)
{
    sf::Vector2f localPos = glyphs[4].position;
    // Convert to global space if needed:
    sf::Vector2f globalPos = text.getTransform().transformPoint(localPos);
}
```

Multiple Unicode code points can form a single grapheme cluster (for example, a base character combined with a combining accent).
When this happens, several consecutive entries in the glyph list will share the same `cluster` value.
If you are implementing a text cursor, treat all glyphs in the same cluster as a single unit.

### Real-Time Touch Input

`sf::Touch::isDown(unsigned int finger)` and both overloads of `sf::Touch::getPosition(unsigned int finger)` have been deprecated.
On Android these functions could crash when queried for a finger that is not currently down; the event-driven approach is always safe.

Use the `position` member of the relevant touch events instead:

| Deprecated function      | Replacement                                                                                      |
|--------------------------|--------------------------------------------------------------------------------------------------|
| `sf::Touch::isDown`      | `sf::Event::TouchBegan` / `sf::Event::TouchEnded`                                                |
| `sf::Touch::getPosition` | `position` field of `sf::Event::TouchBegan`, `sf::Event::TouchMoved`, or `sf::Event::TouchEnded` |

v3.0:
```cpp
if (sf::Touch::isDown(0))
{
    sf::Vector2i pos = sf::Touch::getPosition(0);
    // use pos
}
```

v3.1:
```cpp
// In your event loop:
window.handleEvents([&](const sf::Event::TouchBegan& touch)
{
    if (touch.finger == 0)
    {
        sf::Vector2i pos = touch.position;
        // use pos
    }
},
[&](const sf::Event::TouchMoved& touch)
{
    if (touch.finger == 0)
    {
        sf::Vector2i pos = touch.position;
        // use pos
    }
});
```

### FTP Client

`sf::Ftp` has been deprecated in favor of the new `sf::Sftp` class.
FTP transmits credentials and data in plain text, making SFTP the strongly preferred alternative for any new code.

### Hostname Resolution

`sf::IpAddress::resolve(std::string_view address)` has been deprecated.
It only resolves to IPv4 addresses and lacks control over the DNS servers or timeout.

Use `sf::Dns::resolve()` instead, which supports both IPv4 and IPv6 and gives you full control over the query.

v3.0:
```cpp
std::optional<sf::IpAddress> address = sf::IpAddress::resolve("www.sfml-dev.org");
if (address)
{
    // use *address
}
```

v3.1:
```cpp
std::optional<std::vector<sf::IpAddress>> addresses = sf::Dns::resolve("www.sfml-dev.org");
if (addresses && !addresses->empty())
{
    sf::IpAddress address = addresses->front();
    // use address
}
```

---

# Migration from SFML 2 to SFML 3

Welcome to SFML 3!
The SFML Team has put a lot of effort into delivering a library that is both familiar to existing users while also making significant improvements.
This document will walk you through how to upgrade your SFML 2 application to SFML 3.

One of the headline changes in SFML 3 is raising the C++ standard to C++17 thus bringing SFML into the world of modern C++!
This change has enabled a vast number of internal improvements as well as new opportunities for improving the API that will be discussed in this document.

## Compiler Requirements

Along with the upgrade from C++03 to C++17 you may need to upgrade your compiler.
C++17 support has been widespread in all major compiler implementations for years prior to SFML 3's release so in all likelihood the compiler you're already using will work.
In case you do need to upgrade, here are the minimum compiler versions.

| Compiler   | Version      |
| ---------- | ------------ |
| MSVC       | 16 (VS 2019) |
| GCC        | 9            |
| Clang      | 9            |
| AppleClang | 12           |

## CMake Targets

SFML 3 uses modern CMake convention for library targets which entails having a namespace in front of the target name.
These namespaces tell CMake "this is a target" whereas something like `sfml-graphics` might be a target or might be a precompiled library on disk like `libsfml-graphics.so`.
Fixing this ambiguity leads to more useful error messages when a given target can't be found due to, for example, forgetting a `find_package` call.
The component names used when calling `find_package` were also changed to capitalize the first letter.

| v2 Target       | v3 Target        |
| --------------- | ---------------- |
| `sfml-system`   | `SFML::System`   |
| `sfml-window`   | `SFML::Window`   |
| `sfml-graphics` | `SFML::Graphics` |
| `sfml-network`  | `SFML::Network`  |
| `sfml-audio`    | `SFML::Audio`    |
| `sfml-main`     | `SFML::Main`     |

| v2 Component | v3 Component |
| ------------ | ------------ |
| `system`     | `System`     |
| `window`     | `Window`     |
| `graphics`   | `Graphics`   |
| `network`    | `Network`    |
| `audio`      | `Audio`      |
| `main`       | `Main`       |


v2:
```cmake
find_package(SFML 2 REQUIRED COMPONENTS graphics audio network)
...
target_link_libraries(my_app PRIVATE sfml-graphics sfml-audio sfml-network)
```

v3:
```cmake
find_package(SFML 3 REQUIRED COMPONENTS Graphics Audio Network)
...
target_link_libraries(my_app PRIVATE SFML::Graphics SFML::Audio SFML::Network)
```

## Linux Dependencies

When using X11 as the backend on Linux, as opposed to DRM, `libxi-dev` is a newly required dependency.
This was introduced with the [raw mouse input support](https://www.sfml-dev.org/documentation/3.0.2/structsf_1_1Event_1_1MouseMovedRaw.html).

## `sf::Vector2<T>` Parameters

A common pattern in SFML 2 was to use pairs of scalar parameters to represent concepts like sizes or positions.
Take `sf::Transformable::setPosition(float, float)` for example.
The two parameters combine to represent a position in world space.

SFML 3 takes all of the APIs with pairs of parameters like `(float, float)` or `(unsigned int, unsigned int)` and converts them to their corresponding `sf::Vector2<T>` type like `sf::Vector2f` or `sf::Vector2u` to make the interface more expressive and composable.
This transition is often as simple as wrapping the two adjacent parameters with braces to construct the vector.

v2:
```cpp
sf::VideoMode videoMode(480, 640, 24);
sf::CircleShape circle(10);
circle.setPosition(10, 20);
sf::IntRect rect(250, 400, 50, 100);
```

v3:
```cpp
sf::VideoMode videoMode({480, 640}, 24);
sf::CircleShape circle(10);
circle.setPosition({10, 20});
sf::IntRect rect({250, 400}, {50, 100});
```

## Fixed Width Integers

SFML 2 contained various typedefs for fixed width integers.
Those are now replaced with the fixed width integers provided in the `<cstdint>` header from the standard library.

| v2           | v3              |
| ------------ | --------------- |
| `sf::Int8`   | `std::int8_t`   |
| `sf::Uint8`  | `std::uint8_t`  |
| `sf::Int16`  | `std::int16_t`  |
| `sf::Uint16` | `std::uint16_t` |
| `sf::Int32`  | `std::int32_t`  |
| `sf::Uint32` | `std::uint32_t` |
| `sf::Int64`  | `std::int64_t`  |
| `sf::Uint64` | `std::uint64_t` |

## `sf::Event`

SFML 3 uses `std::variant` under the hood to implement a totally new, type-safe API for events.
There are two main ways to use this new API.
Check out the new EventHandling example program to see these methods in practice.

### `sf::Event::getIf<T>`

The first option is based around `sf::Event::getIf<T>` and `sf::Event::is<T>`.
`getIf<T>` works by providing a template parameter which must be an event subtype.
Event subtypes are types like `sf::Event::Resized` or `sf::Event::MouseMoved`.
If the template argument matches the active event subtype then a pointer to the data is returned.
If that template argument is not the active event subtype then `nullptr` is returned.
`sf::Event::is<T>` more simply returns `true` if `T` matches the active event subtype.
`is<T>` is often used for subtypes like `sf::Event::Closed` which contain no data.
Here's what that looks like:

```cpp
while (window.isOpen())
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                window.close();
        }
    }

    // Remainder of main loop
}
```

Note how the API for getting events has changed slightly.
`sf::WindowBase::pollEvent` and `sf::WindowBase::waitEvent` now return a `std::optional<sf::Event>`.
These two functions _might_ return an event but they might not.
C++ lets you deduce the template parameter which is why you can write `const std::optional event` instead of `const std::optional<sf::Event> event`.
`const auto event` is another valid choice if you prefer a more terse expression.

### `sf::WindowBase::handleEvents`

The second option for processing events is via the new `sf::WindowBase::handleEvents` function.
This functions performs event visitation.
What this means is that you can provide callbacks which take different event subtypes as arguments.
Alternatively you may provide an object (or objects) with `operator()` implementations which handle the event subtypes you want to process.
Notably you do not have to provide callbacks for all possible event subtypes.
Depending on the current active event subtype, the corresponding callback is called.
Here's what that looks like:

```cpp
const auto onClose = [&window](const sf::Event::Closed&)
{
    window.close();
};

const auto onKeyPressed = [&window](const sf::Event::KeyPressed& keyPressed)
{
    if (keyPressed.scancode == sf::Keyboard::Scancode::Escape)
        window.close();
};

while (window.isOpen())
{
    window.handleEvents(onClose, onKeyPressed);

    // Remainder of main loop
}
```

## Window Styles and States

A new `sf::State` enumeration was added for specifying the state of the window which means whether the window is floating or fullscreen.
Here's a before-and-after showing how this affects constructing a window.

v2:
```cpp
sf::RenderWindow window(sf::VideoMode::getFullscreenModes().at(0), "Title", sf::Style::Fullscreen);
```

v3:
```cpp
sf::RenderWindow window(sf::VideoMode::getFullscreenModes().at(0), "Title", sf::State::Fullscreen);
```

## Scoped Enumerations

SFML 3 converts all old style unscoped enumerations to scoped enumerations.
This improves the type safety of the interface.
This means that the name of the enumeration is now part of the namespace required to access values of that enumeration.

For example, take the enumeration `sf::Keyboard::Key`.
`sf::Keyboard::A` becomes `sf::Keyboard::Key::A`.
The name of the enumeration now appears as a nested namespace when accessing one of the enumeration's values.

Here is a complete list of all enumerations which have undergone this change:

* `sf::BlendMode::Equation`
* `sf::BlendMode::Factor`
* `sf::Cursor::Type`
* `sf::Ftp::Response::Status`
* `sf::Ftp::TransferMode`
* `sf::Http::Request::Method`
* `sf::Http::Response::Status`
* `sf::Joystick::Axis`
* `sf::Keyboard::Key`
* `sf::Keyboard::Scan`
* `sf::Mouse::Button`
* `sf::Mouse::Wheel`
* `sf::PrimitiveType`
* `sf::Sensor::Type`
* `sf::Shader::Type`
* `sf::Socket::Status`
* `sf::Socket::Type`
* `sf::SoundSource::Status`
* `sf::VertexBuffer::Usage`

## `sf::Rect<T>`

`sf::Rect<T>` has been refactored from the four scalar values `top`, `left`, `width`, and `height` into two `sf::Vector2<T>`s named `position` and `size`.
This means that `sf::Rect<T>::getPosition()` and `sf::Rect<T>::getSize()` have been removed in favor of directly accessing the `position` and `size` data members.
The 4-parameter constructor was also removed in favor of the constructor which takes two `sf::Vector2<T>`s.

| v2        | v3            |
| --------- | ------------- |
| `.left`   | `.position.x` |
| `.top`    | `.position.y` |
| `.width`  | `.size.x`     |
| `.height` | `.size.y`     |

v2:
```cpp
sf::FloatRect rect(10, 20, 30, 40);
sf::Vector2f position = rect.getPosition();
sf::Vector2f size = rect.getSize();
```

v3:
```cpp
sf::FloatRect rect({10, 20}, {30, 40});
sf::Vector2f position = rect.position;
sf::Vectro2f size = rect.size;
```

The two overloads of `sf::Rect<T>::intersects` have been replaced with one unified function called `sf::Rect<T>::findIntersection` which returns a `std::optional<Rect<T>>`.
This optional contains the overlapping area if the rectangles overlap.
Otherwise the optional is empty.

```cpp
sf::IntRect rect1({0, 0}, {200, 200});
sf::IntRect rect2({100, 100}, {200, 200});
std::optional<sf::IntRect> intersection = rect1.findIntersection(rect2);
// position={100, 100} size={100, 100}
```

## `sf::Angle`

All angles are now represented with a strong type named `sf::Angle`.
This type provides two functions for creating angles called `sf::degrees(float)` and `sf::radians(float)` which construct an angle from either some value of degrees or radians.
Operators (`+`, `-`, etc.) are provided to perform mathematical operations with angles.
If you need access to the raw angle as a `float` then you can use either `sf::Angle::asDegrees()` or `sf::Angle::asRadians()`.

v2:
```cpp
sf::RectangleShape shape(sf::Vector2f(50, 50));
shape.setRotation(90);
std::cout << "Rotation: " << shape.getRotation() << '\n';
```

v3:
```cpp
sf::RectangleShape shape({50, 50});
shape.setRotation(sf::degrees(90));
std::cout << "Rotation: " << shape.getRotation().asDegrees() << '\n';
```

## Renamed Functions

A number of functions have new names but otherwise have not changed their semantics.

| v2                                | v3                                |
| --------------------------------- | --------------------------------- |
| `sf::Font::loadFromFile`          | `sf::Font::openFromFile`          |
| `sf::Socket::getHandle`           | `sf::Socket::getNativeHandle`     |
| `sf::WindowBase::getSystemHandle` | `sf::WindowBase::getNativeHandle` |
| `sf::Texture::create`             | `sf::Texture::resize`             |
| `sf::RenderTexture::create`       | `sf::RenderTexture::resize`       |
| `sf::Image::create`               | `sf::Image::resize`               |
| `sf::Sound::getLoop`              | `sf::Sound::isLooping`            |
| `sf::Sound::setLoop`              | `sf::Sound::setLooping`           |
| `sf::SoundStream::getLoop`        | `sf::SoundStream::isLooping`      |
| `sf::SoundStream::setLoop`        | `sf::SoundStream::setLooping`     |

## Removal of Default Constructors

The default constructors `sf::Sound::Sound()`, `sf::Text::Text()`, and `sf::Sprite::Sprite()` were removed.
They can be replaced by the corresponding constructors which accept a resource type.

| v2                     | v3                                         |
| ---------------------- | ------------------------------------------ |
| `sf::Sound::Sound()`   | `sf::Sound::Sound(const sf::SoundBuffer&)` |
| `sf::Text::Text()`     | `sf::Text::Text(const sf::Font&)`          |
| `sf::Sprite::Sprite()` | `sf::Sprite::Sprite(const sf::Texture&)`   |

Now that these classes are guaranteed to be holding a reference to their corresponding resource type, the functions used to access to those resources can return a reference instead of a pointer.
These functions are `sf::Sound::getBuffer()`, `sf::Text::getFont()`, and `sf::Sprite::getTexture()`.

v2:
```cpp
const sf::SoundBuffer soundBuffer("sound.flac");
sf::Sound sound;
sound.setBuffer(soundBuffer);
```

v3:
```cpp
const sf::SoundBuffer soundBuffer("sound.flac");
sf::Sound sound(soundBuffer);
```

## `std::optional` Usage

SFML 3 makes liberal use of `std::optional` to express when a given function may not return a value.
Some of these usages have already been mentioned like `sf::WindowBase::pollEvent`.
Here are some more places where SFML 3 makes use of `std::optional`.

* `sf::IpAddress` uses `std::optional` to express how resolving an address from a string may not yield a usable IP address.
* `sf::Image::saveToMemory` returns a `std::optional` because the `sf::Image` may be empty or the underlying implementation may fail.
* `sf::SoundFileReader::open` returns a `std::optional` because the stream being opened may not be valid.
* `sf::Music::onLoop` and `sf::SoundStream::onLoop` returns a `std::optional` because if the objects are not in a looping state then there is nothing to return.
* `sf::InputStream` uses `std::optional` in various places.
Instead of returning `-1` to signal an error, `std::nullopt` can be returned.

LearnCpp.com is a great place to learn more about using `std::optional`.
Read more about that [here](https://www.learncpp.com/cpp-tutorial/stdoptional/).

## New Constructors for Loading Resources

The following classes gained constructors that allow for loading/opening resources in a single expression.
Upon failure they throw an `sf::Exception`.

* `sf::InputSoundFile`
* `sf::OutputSoundFile`
* `sf::Music`
* `sf::SoundBuffer`
* `sf::Font`
* `sf::Image`
* `sf::RenderTexture`
* `sf::Shader`
* `sf::Texture`
* `sf::FileInputStream`
* `sf::Cursor`

SFML 3 still supports the SFML 2 style of error handling in addition to these new constructors.

v2:
```cpp
sf::SoundBuffer soundBuffer;
if (!soundBuffer.loadFromFile("sound.wav"))
{
    // Handle error
}
```

v3:
```cpp
sf::SoundBuffer soundBuffer;
if (!soundBuffer.loadFromFile("sound.wav"))
{
    // Handle error
}

// OR

const sf::SoundBuffer soundBuffer("sound.wav");
```

## `sf::Vector2<T>` and `sf::Vector3<T>` Utility Functions

`sf::Vector2<T>` and `sf::Vector3<T>` gained a number of new functions for performing common mathematic operations on vectors.

| `sf::Vector2<T>` Function       | Description                              |
| ------------------------------- | ---------------------------------------- |
| `Vector2(T, sf::Angle)`         | Construct from polar coordinates         |
| `length()`                      | Get length                               |
| `lengthSquared()`               | Get length squared                       |
| `normalized()`                  | Get vector normalized to unit circle     |
| `angleTo(sf::Vector2)`          | Get angle to another vector              |
| `angle()`                       | Get angle from X axis                    |
| `rotatedBy(sf::Angle)`          | Get vector rotated by a given angle      |
| `projectedOnto(sf::Vector2)`    | Get vector projected onto another vector |
| `perpendicular()`               | Get perpendicular vector                 |
| `dot(sf::Vector2)`              | Get dot product                          |
| `cross(sf::Vector2)`            | Get Z component of cross product         |
| `componentWiseMul(sf::Vector2)` | Get component-wise multiple              |
| `componentWiseDiv(sf::Vector2)` | Get component-wise divisor               |

| `sf::Vector3<T>` Function       | Description                           |
| ------------------------------- | ------------------------------------- |
| `length()`                      | Get length                            |
| `lengthSquared()`               | Get length squared                    |
| `normalized()`                  | Get vector normalized to unit circle  |
| `dot(sf::Vector3)`              | Get dot product                       |
| `cross(sf::Vector3)`            | Get cross product                     |
| `componentWiseMul(sf::Vector3)` | Get component-wise multiple           |
| `componentWiseDiv(sf::Vector3)` | Get component-wise divisor            |

## Threading Primitives

`sf::Lock`, `sf::Mutex`, `sf::Thread`, `sf::ThreadLocal`, and `sf::ThreadLocalPtr` were removed and replaced with their equivalents from the standard library.
The standard library provides multiple options for threads, locks, and mutexes among other threading primitives.

| v2                   | v3                                               |
| -------------------- | ------------------------------------------------ |
| `sf::Lock `          | `std::lock_guard` or `std::unique_lock`          |
| `sf::Mutex`          | `std::mutex` or `std::recursive_mutex`           |
| `sf::Thread`         | `std::thread` or `std::jthread` (requires C++20) |
| `sf::ThreadLocal`    | `thread_local`                                   |
| `sf::ThreadLocalPtr` | `thread_local`                                   |

## Sound Samples and Channel Map

SFML 3 introduces the concept of a Channel Map which defines the mapping of the position in sample frame to the sound channel.
For example, if you have a sound frame with six different samples for a 5.1 sound system, the Channel Map defines how each of those samples map to which speaker channel.

SFML 2 always assumed the order as specified by OpenAL.

```cpp
auto samples = std::vector<std::int16_t>();
// ...

auto channelMap = std::vector<sf::SoundChannel>{
    sf::SoundChannel::FrontLeft,
    sf::SoundChannel::FrontCenter,
    sf::SoundChannel::FrontRight,
    sf::SoundChannel::BackRight,
    sf::SoundChannel::BackLeft,
    sf::SoundChannel::LowFrequencyEffects
};
auto soundBuffer = sf::SoundBuffer(samples.data(), samples.size(), channelMap.size(), 44100, channelMap);
auto sound = sf::Sound(soundBuffer);
```

This a breaking change for the following APIs:

- `bool sf::SoundBuffer::loadFromSamples(...)`
- `bool sf::SoundBuffer::update(...)`
- `void sf::SoundStream::initialize(...)`
- `bool sf::OutputSoundFile::openFromFile(...)`
- `bool sf::SoundFileWriter::open(...)`

## Deprecated APIs

SFML 3 removes all of the deprecated APIs in SFML 2.

| Deprecated API                      | Replacement                        |
| ----------------------------------- | ---------------------------------- |
| `sf::Event::MouseWheelEvent`        | `sf::Event::MouseWheelScrolled`    |
| `sf::RenderWindow::capture`         | See 1                              |
| `sf::RenderTexture::create`         | `sf::RenderTexture::resize`        |
| `sf::Shader::setParameter`          | `sf::Shader::setUniform`           |
| `sf::Text::setColor`                | `sf::Text::setFillColor`           |
| `sf::Text::getColor`                | `sf::Text::getFillColor`           |
| `sf::PrimitiveType::LinesStrip`     | `sf::PrimitiveType::LineStrip`     |
| `sf::PrimitiveType::TrianglesStrip` | `sf::PrimitiveType::TriangleStrip` |
| `sf::PrimitiveType::TrianglesFan`   | `sf::PrimitiveType::TriangleFan`   |
| `sf::PrimitiveType::Quads`          | See 2                              |
| `sf::Keyboard::BackSlash`           | `sf::Keyboard::Key::Backslash`     |
| `sf::Keyboard::BackSpace`           | `sf::Keyboard::Key::Backspace`     |
| `sf::Keyboard::Dash`                | `sf::Keyboard::Key::Dash`          |
| `sf::Keyboard::Quote`               | `sf::Keyboard::Key::Hyphen`        |
| `sf::Keyboard::Return`              | `sf::Keyboard::Key::Enter`         |
| `sf::Keyboard::SemiColon`           | `sf::Keyboard::Key::Semicolon`     |
| `sf::Keyboard::Tilde`               | `sf::Keyboard::Key::Grave`         |

1. `sf::RenderWindow::capture` can be recreated by using an `sf::Texture` and its `sf::Texture::update(const Window&)` function to copy its contents into an `sf::Image` instead.
2. `sf::PrimitiveType::Quads` can be replaced by another primitive type.
This is not a drop-in replacement but rather will require refactoring your code to work with a new geometry.
One viable option is to use `sf::PrimitiveType::Triangles` where two adjacent triangles join to form what was previously one quad.

## Anti-Aliasing Renamed

SFML 3 capitalizes the `A` of `aliasing` for all the APIs.

* `sf::RenderTexture::getMaximumAntialiasingLevel()` becomes `sf::RenderTexture::getMaximumAntiAliasingLevel()`
* `sf::ContextSettings::antialiasingLevel` becomes `sf::ContextSettings::antiAliasingLevel`

## CoordinateType for RenderStates

The enum `sf::CoordinateType` was moved from the `sf::Texture` into its own dedicated enum class.

The `sf::RenderStates` class got a new member of type `sf::CoordinateType` to control how the texture coordinates will be interpreted.
By default SFML uses `sf::CoordinateType::Pixels`, while `sf::CoordinateType::Normalized` is the default for OpenGL.
Using `sf::CoordinateType::Normalized` with `sf::RenderStates` allows for using normalized textures with `sf::VertexArray` and `sf::VertexBuffer`.

Additionally, as SFML now supports Stencil Testing, there's an additional overload for the Stencil Mode.

The constructor for `sf::RenderStates` has changed.

v2:
```cpp
auto renderStates = sf::RenderStates(sf::BlendAlpha,
                                     transform,
                                     texture,
                                     nullptr);
```

v3:
```cpp
auto renderStates = sf::RenderStates(sf::BlendAlpha,
                                     sf::StencilMode(),
                                     transform,
                                     sf::CoordinateTye::Pixels,
                                     texture,
                                     nullptr);
```

## Other Minor Changes

SFML 3 includes various smaller changes that ought to be mentioned.

* Changed the parameter order of the `sf::Text` constructor, so that the provided font is always the first parameter
* Reverted to default value of CMake's `BUILD_SHARED_LIBS` which means SFML now builds static libraries by default
* Changed `sf::String` interface to use `std::u16string` and `std::u32string`
* Removed `sf::ContextSettings` constructor in favor of aggregate initialization
* Removed `sf::View::reset` in favor of assigning from a new `sf::View` object
* Removed `sf::Vertex` constructors in favor of aggregate initialization
* Renamed `sf::Mouse::Button::XButton1` and `sf::Mouse::Button::XButton2` enumerators to `sf::Mouse::Button::Extra1` and `sf::Mouse::Button::Extra2`
* Removed NonCopyable.hpp header in favor of using built-in language features for disabling copy operators
* Converted the following classes to namespaces: `sf::Clipboard`, `sf::Keyboard`, `sf::Joystick`, `sf::Listener`, `sf::Mouse`, `sf::Sensor`, `sf::Touch`, `sf::Vulkan`
* Removed `sf::SoundStream::setProcessingInterval` as miniaudio matches the internal processing rate to the underlying backend
