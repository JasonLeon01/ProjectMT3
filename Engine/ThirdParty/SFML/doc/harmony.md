# OpenHarmony and HarmonyOS backend

This SFML 3.1.0-ME backend targets OpenHarmony/HarmonyOS API 22 on phone, tablet, and 2in1/PC. It is a distinct SFML platform and does not depend on the product's kernel identity: some OpenHarmony profiles use a Linux kernel, while HarmonyOS products may use another OHOS kernel. Platform-specific code must test `SFML_SYSTEM_HARMONY` before any Linux selection.

## Build contract

The DevEco OHOS CMake toolchain defines `__OHOS__`. Select the device form with `SFML_HARMONY_DEVICE_FORM=MOBILE` or `SFML_HARMONY_DEVICE_FORM=2IN1`. SFML then exposes:

- `SFML_SYSTEM_HARMONY` for the base platform;
- exactly one of `SFML_HARMONY_MOBILE` and `SFML_HARMONY_2IN1`;
- `SFML_OPENGL_ES` when the complete build uses the GLES client API.

MOBILE forces `SFML_OPENGL_ES=ON`. A fresh 2IN1 build defaults it to `OFF`, selecting desktop OpenGL; pass `-DSFML_OPENGL_ES=ON` to build a separate GLES binary. This is a whole-build contract covering SFML, the final NAPI DSO, public headers, EGL binding and link libraries. Do not define it for an individual source file or attempt to switch client APIs per window at runtime.

Use a fresh or form-specific CMake build directory when changing between MOBILE and 2IN1. SFML rejects an in-place form change so a cached forced MOBILE GLES value cannot silently leak into a 2IN1 build.

The native SDK minimum is API 22 for both MOBILE and 2IN1. Every Harmony build must pass an explicit `OHOS_COMPATIBLE_SDK_VERSION` of API 22 or newer so the DevEco toolchain emits a versioned native compiler target and enforces availability. The supplied MOBILE example targets and declares compatibility with API 22; the supplied PC example compiles and targets API 24 while retaining API 22 compatibility. The official DevEco toolchain selects and validates `OHOS_ARCH`, and SFML requires its `c++_shared` runtime. The backend accepts the toolchain's `arm64-v8a`, `armeabi-v7a`, and `x86_64` names, while both supplied HAP examples package `arm64-v8a`. Native code uses position-independent code because static SFML libraries and dependencies are normally folded into one HAP NAPI shared object.

A standalone install follows SFML's Android-style ABI layout: libraries and the package configuration are placed below `lib/<OHOS_ARCH>`. Point consumers at the exact package directory with `-DSFML_DIR=<install-prefix>/lib/<OHOS_ARCH>/cmake/SFML`; for example, an arm64 consumer uses `lib/arm64-v8a/cmake/SFML`. This keeps multiple installed ABIs separate while preserving relocatable exported targets.

Use `examples/harmony` for MOBILE diagnostics and `examples/harmony-2in1` for the API 24 MateBook Pro emulator project. The latter exposes separate `desktopGl` and `gles` module targets.

SFML does not pretend to be desktop Linux. POSIX implementation files are reused selectively for threads, monotonic time, the filesystem, dynamic loading, and BSD sockets. Bundled third-party libraries receive only the musl/POSIX feature definitions they require. ALSA, PulseAudio, JACK, X11, Wayland, GLX, UDev, and GLES1 are not backend dependencies. The 2IN1 desktop path is still `WindowStage + XComponent + OHNativeWindow + EGL`; it links Harmony's `libGLv4.so`, not a Linux GLX stack.

## Stage host and entry point

Include `SFML/Main.hpp` in the native application. On Harmony it maps the user's `main` to `sfmlMain`. Link `SFML::Main`; the source-side Stage glue shown in `examples/harmony/entry/src/main/cpp/napi_init.cpp` connects the NAPI module to SFML without adding Harmony-only public headers.

The exported Stage host functions are `initializeHost`, `onForeground`, `onBackground`, `onXComponentDestroy`, `onDestroy`, `submitText`, `submitKeyText`, and `submitTextEdit`. ArkTS calls `initializeHost(resourceManager, windowId, deviceType, callbacks)` only after the Stage window ID is greater than zero. The callback object must provide `setPointerVisible(boolean)`, `setSystemPointer(number)`, `setCustomPointer(Uint8Array, width, height, hotspotX, hotspotY)`, `setVirtualKeyboardVisible(boolean)`, `configureWindow(fullscreen, keepScreenOn)`, and `requestExit()`; a 2IN1 host must additionally provide `executeWindowCommand(command)`. Native worker threads reach these functions through NAPI thread-safe functions, and ArkTS marshals the operations to InputKit/ArkUI.

After an ordered 2IN1 command completes, ArkTS calls `completeWindowCommand(requestId, success, x, y, clientWidth, clientHeight, visible, focused, fullscreen)`. Unsolicited rect, focus, visibility and window-state changes use `updateWindowState(x, y, clientWidth, clientHeight, visible, focused, fullscreen)`. These exports are internal host glue rather than public SFML C++ API.

Because XComponent `onLoad` can precede asynchronous Stage-window discovery, hosts must retain the native context and retry initialization when the window ID arrives. The native XComponent is unwrapped from `OH_NATIVE_XCOMPONENT_OBJ`; its surface and input callbacks are registered internally. The application worker starts exactly once when both the host and a non-zero native surface are ready.

Only one surface-owning `sf::Window` or `sf::RenderWindow` is allowed in a process. A second native window fails deterministically. `sf::WindowHandle` is `void*` and denotes the current `OHNativeWindow*` belonging to the XComponent.

Surface loss destroys only the EGLSurface. The EGLContext and shared objects remain alive until application teardown; a later surface callback rebuilds the EGLSurface and queues resize/focus events. Duplicate and out-of-order Stage/XComponent lifecycle callbacks are idempotent.

## Graphics contract

Harmony MOBILE and a 2IN1 build configured with `SFML_OPENGL_ES=ON` use EGL with the OpenGL ES 2.0 API surface and ESSL 1.00. The native link dependencies are `libEGL.so` and `libGLESv2.so`; neither GLES1 nor GLES3 is linked. Requests above 2.0 in `sf::ContextSettings` are normalized to an ES2 client request, while `getSettings()` reports the context version actually returned by the driver. A backward-compatible ES 3.2 driver may therefore be reported as 3.2 even though SFML exposes only GLES2 headers, imports, and rendering paths. Harmony GLES shader sources without a `#version` directive are compiled explicitly as ESSL 1.00; an application-provided version directive remains unchanged. On the ESSL 1.00 compatibility path, SFML internally aliases a `sampler2D` uniform whose identifier is `texture` while preserving `texture` as the public `setUniform` name. This avoids a conflict in PC graphics translation layers that lower `texture2D(...)` to the newer built-in `texture(...)` function.

The default 2IN1 build links `libEGL.so` and `libGLv4.so`, binds `EGL_OPENGL_API`, and requires `EGL_OPENGL_BIT`. Requests below OpenGL 3.0 are promoted to 3.0 and requests above the officially supported 4.2 ceiling are clamped with a diagnostic. Profile flags are submitted for versions where EGL defines them. API 22's `OH_Graphics_QueryGL` is a required capability check: a false or missing result fails context creation and instructs the application to rebuild with `SFML_OPENGL_ES=ON`; a desktop binary never silently changes client API.

The API level and compile SDK do not by themselves guarantee that a particular PC emulator image ships the desktop-GL runtime. Some API 24 PC images omit `libGLv4.so`; on such an image the dynamic loader rejects the NAPI shared object before its native module initialization runs, so neither SFML nor `OH_Graphics_QueryGL` can diagnose or recover from the missing dependency. Validate the `desktopGl` build only with an image that provides both `libGLv4.so` and a working desktop-GL capability query. The separately built GLES target can validate the Stage host and GLES path on an image without desktop GL, but it is not a fallback for, or proof of, the desktop binary: a `desktopGl` binary never silently switches to GLES.

The desktop configuration prefers an EGL config supporting both window and pbuffer surfaces so the shared context can survive XComponent surface loss. If that combination is unavailable, `EGL_KHR_surfaceless_context` is required. Absence of both is a clear context-creation failure rather than a resource-loss hazard.

Raw GL code should resolve entry points with `sf::Context::getFunction` so calls use the same vendor dispatch selected by EGL. Harmony's desktop implementation does not guarantee legacy fixed-pipeline exports such as `glBegin`, even though generic SDK headers may declare them; applications must use the programmable pipeline.

OpenGL ES 3.2 is backward compatible with the GLES2 programmable pipeline, but it does not restore GLES1 fixed-function entry points such as `glMatrixMode`. Applications must use Shader-based rendering. SFML users are not given a raw GLES3 context in this first backend.

The inherited 3.1.0-ME Shader contract uses attributes `sf_Vertex`, `sf_Color`, and `sf_MultiTexCoord0`; matrices `sf_ModelViewMatrix`, `sf_ProjectionMatrix`, `sf_ModelViewProjectionMatrix`, and `sf_TextureMatrix`; varyings `sf_FrontColor` and `sf_TexCoord0`; and uniforms `sf_Texture` and `sf_TextureEnabled`. Harmony vertex stages use high precision. Attribute locations 0, 1, and 2 are used for position, color, and texture coordinates.

Window multisampling follows the available EGL configurations. RenderTexture supports color, depth, stencil, separate depth/stencil, FBO readback, and copy-to-image. Its multisampling path remains disabled for GLES but is capability-driven for 2IN1 desktop OpenGL. sRGB textures and window surfaces are enabled only when the required GL/EGL extensions are present; unsupported requests fall back to linear storage and report the actual setting.

## Mobile window behavior

The XComponent determines the window size. Setting the title, position, size, minimum/maximum size, or mouse position is a no-op. Fullscreen means occupying the primary Stage surface. `VideoMode::getDesktopMode()` reads the current surface size, while the single entry returned by `VideoMode::getFullscreenModes()` reflects the current surface at each query. Harmony does not synthesize a second, rotated mode, an early pre-surface query does not permanently cache the temporary 1x1 fallback, and later queries do not mutate mode arrays returned earlier. Additional windows are unsupported.

## 2in1 window behavior

A 2IN1 host still owns one Stage main window and one full-size XComponent; only one owning `sf::Window` or `sf::RenderWindow` is supported. The Ability manifest must include `supportWindowMode: ["fullscreen", "floating"]`. SFML maps titlebar, resize, close and borderless styles to the Stage window decoration and title-button controls, and maps client size constraints through the measured difference between the outer window rect and XComponent drawable rect.

The API 24 Stage switch that permits title-bar dragging also permits the system's double-click maximize gesture. The platform has no move-only title-bar switch, so a decorated SFML window without `Style::Resize` hides its maximize button and disables drag-resizing but may still be maximized by that system gesture.

The PC HAP's `AppScope/app.json5` must add `appEnvironments: [{ "name": "NEED_OPENGL", "value": "1" }]`. This enables the platform desktop-GL dispatch used by the default 2IN1 build; the GLES variant can retain the same manifest because its client API is still selected explicitly by the native build and `eglBindAPI`.

Position, size, limits, title, icon, visibility and focus operations are marshalled to ArkTS as ordered host commands. Each command carries a request ID and receives a completion callback; native setters wait for at most three seconds without holding the lifecycle mutex. A timeout, platform rejection, or teardown cancels the command and leaves getters at their last confirmed value. ArkTS hosts must submit global-window rect and state changes back to native code so requested and system-clamped geometry cannot diverge silently.

Harmony's public application API does not expose an exclusive video-mode switch. SFML therefore maps `State::Fullscreen` to borderless immersive maximize, preserves the previous floating rect, and recovers it when returning to windowed operation. A position requested while maximized/fullscreen is deferred until the window is floating. Hiding a window minimizes it and showing it restores the prior state.

`setMouseCursorGrabbed` uses `OH_WindowManager_LockCursor` and requires the normal `ohos.permission.LOCK_WINDOW_CURSOR` permission. The requested lock is reapplied after focus returns because the system releases it on focus loss. Global mouse polling uses InputKit and window-relative polling subtracts the confirmed global window origin. If InputKit reports that no pointer is available, SFML falls back to the most recent XComponent-local position and suspends global queries until focus or pointer input returns, avoiding a failed service query on every frame. Programmatic pointer warping remains unsupported.

`VideoMode::getDesktopMode()` queries the primary display through Native Display Manager. `getFullscreenModes()` returns the current primary-display mode because the application API does not expose an enumerated mode-switch list.

Input events cross a thread-safe queue. The backend maps touch and multitouch, pointer/mouse buttons and axes, keyboard, focus, and committed Unicode text. IME composition remains platform-owned. The native OHInputMethod editor is authoritative when attachment succeeds. Only when it is unavailable does the ArkTS TextInput fallback compare its previous committed value with each change and call `submitTextEdit(backwardDeletions, forwardDeletions, insertedText)`. Native attachment and fallback submission are mutually excluded, and each fallback delete/replace transaction enters the event queue under one lock, so composition updates do not leak and committed text is not duplicated. Clipboard uses the platform pasteboard APIs. Pointer visibility, system styles, custom RGBA cursors, and virtual keyboard requests use host callbacks because they must be marshalled to ArkUI/InputKit. Programmatic pointer-position injection is unsupported.

Sensors use the native sensor service. Game controllers use the API 21 game-controller service when available. Vulkan graphics uses `VK_OHOS_surface` with the same `OHNativeWindow*`.

## Audio, networking, and resources

Miniaudio remains SFML's decoder, mixer, and spatial-audio engine but is built with device I/O and desktop Linux backends disabled. OHAudio render callbacks pull PCM from the no-device engine. Harmony's media/game output API follows the system-selected route and does not allow an application to select arbitrary output hardware, so `PlaybackDevice::getAvailableDevices()` exposes that current route as its selectable entry and route changes are reported through the normal notification callback. Recording uses an OHAudio capturer; its device list contains only API 21 media-input descriptors accepted by `SoundRecorder::setDevice()`. The realtime capture callback only transfers data to a preallocated ring buffer and a worker invokes `onProcessSamples` outside the callback.

Networking uses BSD sockets for IPv4/IPv6, DNS, TCP, UDP, nonblocking sockets, and selectors. TLS and SSH use the bundled MbedTLS and libssh2 builds.

`rawfile:/path` explicitly opens a HAP rawfile through the resource manager. Other paths first address the application sandbox. If opening a relative sandbox path fails, SFML retries it as a rawfile. Rawfile access fails with a clear diagnostic until ArkTS has registered a native resource manager.

## Deferred scope

Multiple owning native windows, exclusive display-mode switching, programmatic pointer warping, GLES1 emulation, raw GLES3 contexts, and RenderTexture ES2 multisampling remain unsupported.
