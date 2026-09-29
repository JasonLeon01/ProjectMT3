# SFML OpenHarmony mobile diagnostics

This is a DevEco Studio Stage-model application for the SFML OpenHarmony/HarmonyOS mobile backend. It targets and declares compatibility with API 22 and packages `arm64-v8a` for current consumer devices. The declared device forms are phone and tablet; 2in1/PC is intentionally excluded. `x86_64` remains an optional cross-build for emulator/toolchain checks. The HarmonyOS Stage packager rejects `armeabi-v7a`, so that ABI can only be validated separately with the OpenHarmony NDK rather than included in this HAP.

## Requirements

- DevEco Studio with an OpenHarmony API 22 or newer native SDK.
- The SDK CMake toolchain, Clang 15, Ninja, and the shared C++ runtime installed by DevEco Studio.
- A signing configuration supplied by DevEco Studio when installing on a physical device.

Open this `examples/harmony` directory as the DevEco project. The native module builds SFML and its bundled dependencies as position-independent static libraries, then links them into the single NAPI module `libentry.so`. The HAP ABIs are selected in `entry/build-profile.json5`.

The native build passes `OHOS_COMPATIBLE_SDK_VERSION=22` explicitly. Keep this argument when integrating the MOBILE package so the compiler target remains versioned for API 22 availability checks.

The command-line equivalent, after DevEco has initialized the project, is:

```sh
JAVA_HOME=/Applications/DevEco-Studio.app/Contents/jbr/Contents/Home \
NODE_HOME=/Applications/DevEco-Studio.app/Contents/tools/node \
DEVECO_SDK_HOME=/Applications/DevEco-Studio.app/Contents/sdk \
  /Applications/DevEco-Studio.app/Contents/tools/hvigor/bin/hvigorw assembleHap \
    --mode module -p module=entry@default -p product=default
```

`DEVECO_SDK_HOME` must point at the SDK repository root shown above, not its `default` child; Hvigor discovers `default/sdk-pkg.json` from that root.

The exact task spelling can vary between Hvigor releases; DevEco Studio's **Build Hap(s)** action uses the project configuration directly.

The XComponent NAPI bridge waits for a positive Stage window ID before calling `initializeHost`. Its fourth argument supplies ArkTS callbacks for pointer visibility, the explicit SFML-to-InputKit system cursor mapping, custom RGBA cursors, and the virtual-keyboard fallback. The fallback keeps a committed-text shadow and submits UTF-16-safe delete/replace deltas through the native-IME-gated `submitTextEdit` transaction; it is inactive while the native OHInputMethod proxy is attached. This retry is required because XComponent `onLoad` can run before `getMainWindow()` resolves.

The consumer-device HarmonyOS HAP contains `arm64-v8a`. An `x86_64` cross-build remains an optional emulator/toolchain smoke test rather than a release requirement. DevEco Studio 6.0.1 rejects `armeabi-v7a` in a HarmonyOS product's `abiFilters`; SFML can still cross-compile that ABI outside the HAP when an OpenHarmony integration needs it.

## What the example checks

The application owns one full-surface XComponent and starts `sfmlMain` only after the Stage host and native surface are ready. Its generated diagnostics avoid checked-in binary test assets and exercise:

- EGL/OpenGL ES 2.0 context normalization, Shader ABI, CPU vertices, VBOs, blending, stencil, views, transforms,
  textures, and pixel readback. It deliberately disturbs tracked raw GL state and verifies that
  `pushGLStates`/`popGLStates` restores it. HiLog records null-safe `GL_VENDOR`, `GL_RENDERER`, `GL_VERSION`, and
  `GL_SHADING_LANGUAGE_VERSION` values once the first context is current.
- color-only, depth-only, stencil-only, and combined depth/stencil RenderTexture configurations, copy-to-image pixel
  checks (including depth ordering and stencil masking), and deterministic rejection of RenderTexture AA greater than
  zero.
- XComponent resize, zero-size, destroy/recreate transitions and post-recreate use of the Texture, Shader, and
  RenderTexture objects that existed before surface loss.
- touch began/moved/ended events, concurrent-touch maximum and deprecated polling compatibility; mouse movement, all
  five buttons, horizontal/vertical axes and polling; key code, scancode, modifiers, repeat, Unicode/IME commit and
  backspace; clipboard and system/custom/hidden cursor paths.
- all six SFML sensor types with availability, event counts, last event XYZ and polled XYZ; controller identification,
  button/axis capabilities, all eight axis values, button state, and connection/event counters.
- playback/capture device enumeration, default/current playback device and rate, playback notifications, generated
  44.1 kHz mono and 48 kHz stereo tones, selectable null/default route, and permission-gated recording sample/peak
  metering followed by playback.
- explicit and relative-fallback rawfile streams, Images, Textures, Shaders and a Font, a temporary sandbox-file round
  trip, IPv4/IPv6 TCP and UDP loopback, DNS, nonblocking sockets, `SocketSelector`, and a Vulkan instance/surface
  create-destroy smoke test.
- an explicitly triggered, optional external TLS certificate suite. One worker tests `sha256.badssl.com`,
  `wrong.host.badssl.com`, `self-signed.badssl.com`, and `expired.badssl.com` in sequence on port 443. Every socket is
  switched to nonblocking mode before `setupTlsClient(targetHostname, true)`. The positive host passes only after a
  verified handshake; the hostname-mismatch, self-signed, and expired hosts pass only when TLS rejects them after TCP
  connects. DNS/TCP failure or a five-second handshake timeout is inconclusive rather than a false security pass.
  Network work never runs on the render thread, and the worker is cancellable and joined during orderly exit.

No binary font or sound test file is checked in. A tiny public-domain printable-ASCII BDF bitmap font exercises `Font`
through an explicit HAP `rawfile:/` path and keeps the diagnostics panel readable if no known HarmonyOS system font can
be opened; a system font is preferred when available. The audio path uses generated PCM loaded into `SoundBuffer` at
both required formats. Text, image, texture, shader and font checks therefore exercise the HAP resource-manager path
directly.

The small ArkTS status overlay only reports whether the native host initialized. Detailed results appear inside the SFML window and in HiLog under the `SFML-Harmony` tag.

The native status panel has six pages. Press `P`, or tap the upper-right diagnostic area, to advance. Touch controls use
normalized current-surface pixels, and bottom strips are divided into equal-width targets; they therefore remain usable
in portrait, landscape, split-screen, and letterboxed aspect ratios. The matching keyboard shortcuts remain useful for
keyboard coverage.

Controls:

- Touch/click moves the animated shape and plays a generated tone.
- `Space` plays the tone.
- `A` pauses or resumes the generated tone.
- `C` performs a clipboard round trip. On page 3, the second bottom quarter does the same.
- `I` shows the IME; Return commits and hides it. On page 3, tap the leftmost bottom quarter. Committed Unicode text and
  Unicode/IME/backspace counters remain visible on that page.
- `K` switches between system and custom RGBA cursors; `V` toggles pointer visibility.
- `S` enables or disables every available sensor. Page 2 also accepts a bottom-area tap.
- `T` starts or reruns the optional four-host TLS certificate suite. Page 1 also accepts a bottom-area tap. A leading
  `+` means the host must be accepted; `-` means its bad certificate must be rejected.
- `R` starts/stops microphone recording and plays the captured buffer; `M` switches between 44.1 kHz mono and 48 kHz
  stereo. `N` selects the null playback route and `D` restores the default route. Page 6 divides its bottom strip into
  **record | format | null | default | tone** touch targets.
- `Escape` closes the native window.
- Tap the small top-center **surface ×100** button, or press `F10` when ArkUI receives that key, to start the formal
  100-cycle XComponent surface stress. Each removal and re-addition advances only from the corresponding ArkUI
  `onDestroy`/`onLoad` callback, with a callback timeout rather than a batch of state flips. The button is hidden during
  the run. Its transparent full-width layout wrapper passes all hits outside the 24-vp button to XComponent and sits
  above the native HUD; transient status is visible only while the surface is absent. `SFML-Harmony` HiLog lines retain
  the `completed/100` and `failed` result.

## Physical phone checklist

Use DevEco Studio's Log view filtered to the `SFML-Harmony` tag, and do not mark a row as passed merely because its API
exists. Record the on-screen counter/value change or its corresponding HiLog line.

1. Launch in portrait and record device model, HarmonyOS/OpenHarmony API level, and the four GL identity lines. On page
   1, require GLES 2.0, `GLerr 0`, and `ok` for raw resources/font, sandbox, TCP4+6, UDP4+6, local DNS, Shader, VBO,
   raw-GL push/pop, RenderTexture and Vulkan where the device advertises Vulkan. An ES2 `EXT_sRGB` context must report
   `ES2 EXT rejected (expected)` for sRGB mipmaps; an ES3 core context must report `ES3 core generated`. External DNS/TLS
   is optional and not a basic pass
   condition. With working Internet access, press `T` or tap page 1's bottom area and keep interacting with the scene
   while the four rows advance through `resolving`, `connecting`, and `handshaking`. The required final suite is:
   `valid-chain = PASS (verified peer)`, while wrong-host, self-signed, and expired each equal
   `PASS (certificate rejected)`. `FAIL (UNSAFE: invalid certificate accepted)` is a security failure. A DNS/TCP/timeout
   row is explicitly inconclusive and must not be recorded as either a positive or negative certificate pass. The
   entire external suite remains optional for the offline baseline.
2. On page 2, note which of the six sensors report available. Move the phone forward/back, left/right and up/down for
   accelerometer/user acceleration; tilt it for gravity; rotate around all three axes for gyroscope and orientation;
   make a figure-eight motion for the magnetometer. Values and event counts must change. Orientation XYZ is explicitly
   in radians. Press `S` or tap the bottom, confirm available-sensor event counts stop, then enable them again.
3. On page 3, use one finger for began/moved/ended, then hold two or more fingers, move each independently, and lift them
   one at a time. `active` and `poll` must agree while held and `max` must reach the number used. Tap the bottom-left
   control, enter Chinese plus an emoji, use backspace, and press Return; the exact committed text and counters must be
   visible. Exercise the clipboard and cursor bottom targets too.
4. Connect a Bluetooth or USB-C mouse. Move it, press left/right/middle/back/forward where available, use the vertical
   wheel, and use a tilt wheel or trackpad horizontal gesture. Confirm five per-button polling/event fields and both
   wheel counters. Connect an external keyboard, press letters/navigation/function keys, combinations with
   Shift/Ctrl/Alt/System, and hold a key to generate repeats; verify code, scancode, modifier and down/up/repeat fields.
5. Connect a Bluetooth or USB gamepad and open page 4. Verify its name/product, reported button count and axis
   capabilities. Exercise both sticks (`X/Y`, `Z/R`), both triggers (`U/V`), D-pad (`PovX/PovY`), every available button,
   then disconnect/reconnect it. The eight polled values and event/connect counters must respond.
6. On page 5, require all four RenderTexture configurations, depth ordering, stencil-mask pixels, copy/pixel and AA
   rejection to show `ok`. Rotate portrait to landscape and back, send the app to the background and foreground, then
   lock/unlock the screen. Repeat the lifecycle
   cycle at least ten times. `lost/recreate` and persistence-check counts must advance together and persistence failures
   must stay zero. A zero-size count is recorded if ArkUI exposes that intermediate state; not all devices do. Tap
   **surface ×100** (or press `F10` if ArkTS receives it) once and require the ArkTS surface stress summary and HiLog to reach
   `surface stress 100/100 failed 0`; do not count a run that times out waiting for either real lifecycle callback.
7. On page 6, note playback/capture device counts, default/current route and sample rate. Play the tone, switch to 48 kHz
   stereo with `M` or the second bottom target and play again, then return to 44.1 kHz mono. For each format, start
   recording, speak and clap for several seconds, stop, require sample count greater than zero and peak greater than
   zero, and hear the recorded buffer. Deny microphone permission once and confirm recording becomes unavailable
   without terminating the app; grant it and retry after relaunch if required by the OS.
8. While a tone plays, select the null route (`N`/third target): output should become silent without a crash. Restore the
   default route (`D`/fourth target). Connect/disconnect a wired, USB, or Bluetooth output and background/foreground the
   app; verify started/stopped/rerouted/interruption notification counts and the last notification supported by that
   device. Re-run clipboard once with pasteboard permission denied and confirm only that diagnostic fails safely.

Save a final screenshot of every page and the filtered HiLog output. Items absent in the hardware, such as a fifth
mouse button, horizontal wheel, magnetometer, Vulkan, or a particular audio route, should be recorded as unavailable or
not exercised rather than reported as passed.

## Tablet follow-up

After the phone checklist passes, install the same API-21-built HAP on a tablet. Repeat pages 1, 2, 5, and 6, then focus
on portrait/landscape transitions, split/full-screen transitions that the system permits, high-resolution rendering,
three-or-more-finger input, an external mouse/keyboard, cursor shape/visibility, and gamepad routing. Repeat at least ten
surface lifecycle cycles, then tap **surface ×100** for the callback-paced 100-cycle surface stress. Record tablet model, OS API,
GPU/driver strings, tested orientations, maximum simultaneous
touches, and connected peripherals alongside the phone results.

On Harmony, `VideoMode::getDesktopMode()` and the single entry returned by `getFullscreenModes()` reflect the current
XComponent surface at each query. Neither API fabricates the opposite orientation, and an early pre-surface query cannot
permanently cache the temporary 1×1 fallback. A later query also leaves every mode array returned earlier unchanged.

The HAP declares `INTERNET`, `MICROPHONE`, `READ_PASTEBOARD`, `ACCELEROMETER`, and `GYROSCOPE`. The Stage host explicitly requests the user-grant microphone and pasteboard permissions; accelerometer and gyroscope are system-grant permissions and do not require a runtime dialog. A denied or unavailable permission makes only the corresponding diagnostic unavailable and does not terminate the application.

## Scope

This example covers the phone/tablet form only. Use the sibling [`examples/harmony-2in1`](../harmony-2in1/README.md) project for 2in1/PC builds with desktop OpenGL or GLES. Multiple owning native windows, exclusive display-mode switching, GLES1 fixed-function emulation, raw GLES3 contexts, and GLES RenderTexture multisampling remain unsupported. See [`doc/harmony.md`](../../doc/harmony.md) for the complete platform contract.
