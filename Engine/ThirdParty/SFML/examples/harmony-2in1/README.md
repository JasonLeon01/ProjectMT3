# SFML HarmonyOS 2in1 diagnostics

This standalone DevEco Studio Stage-model project exercises SFML on HarmonyOS 2in1/PC devices. It compiles with the HarmonyOS 6.1.1 API 24 SDK, declares HarmonyOS 6.0.2/API 22 as its compatible floor, packages only `arm64-v8a`, and can be deployed to the MateBook Pro 2in1 emulator. DevEco's version map assigns API 22 to `6.0.2(22)`; `6.1.0` corresponds to API 23 and cannot be paired with `(22)`.

The application declares only the `2in1` device type. Its AppScope enables `NEED_OPENGL=1`, the EntryAbility supports both full-screen and floating window modes, and the HAP requests `ohos.permission.LOCK_WINDOW_CURSOR` for relative/captured pointer diagnostics.

## Build targets

| Product and module target | Native selection | Expected renderer |
| --- | --- | --- |
| `desktopGl` | `SFML_HARMONY_DEVICE_FORM=2IN1`, `SFML_OPENGL_ES=OFF` | EGL plus HarmonyOS `libGLv4` (default) |
| `gles` | `SFML_HARMONY_DEVICE_FORM=2IN1`, `SFML_OPENGL_ES=ON` | EGL plus `libGLESv2` |

Both targets build the same shared NAPI diagnostics module, `libentry.so`, with SFML and its bundled dependencies linked into it. Keeping the ArkTS, NAPI, native diagnostics, and raw resources aligned between the two targets makes their graphics API the only intended behavioral difference.

## Requirements

- DevEco Studio 6.1.1 with the HarmonyOS 6.1.1/API 24 native SDK.
- HarmonyOS Emulator 6.1.1 with the `MateBook Pro`/`pc_all_arm` image.
- A DevEco signing configuration when the selected deployment requires signing.

Check the selected emulator image itself before treating it as a desktop-OpenGL test environment. Some API 24 PC images do not contain `libGLv4.so`, even though the API 24 native compile SDK contains the corresponding import library and headers. On those images `entry@desktopGl` fails while the system loader is resolving `libentry.so`, before the NAPI module or `OH_Graphics_QueryGL` can run. Use a PC image that exposes both `libGLv4.so` and the desktop-GL capability query for `desktopGl` acceptance. You may run `entry@gles` on the image to validate the independent GLES and Stage-host paths, but that result does not validate desktop OpenGL, and the desktop binary intentionally never falls back to GLES.

Open `examples/harmony-2in1` as the DevEco project. In **Build Variants**, keep the `default` product and select either the `entry@desktopGl` or `entry@gles` module target, then use **Build Hap(s)** or **Run 'entry'**.

The equivalent command-line builds, run from this directory, are:

```sh
JAVA_HOME=/Applications/DevEco-Studio.app/Contents/jbr/Contents/Home \
NODE_HOME=/Applications/DevEco-Studio.app/Contents/tools/node \
DEVECO_SDK_HOME=/Applications/DevEco-Studio.app/Contents/sdk \
  /Applications/DevEco-Studio.app/Contents/tools/hvigor/bin/hvigorw assembleHap \
    --mode module -p module=entry@desktopGl -p product=default
```

```sh
JAVA_HOME=/Applications/DevEco-Studio.app/Contents/jbr/Contents/Home \
NODE_HOME=/Applications/DevEco-Studio.app/Contents/tools/node \
DEVECO_SDK_HOME=/Applications/DevEco-Studio.app/Contents/sdk \
  /Applications/DevEco-Studio.app/Contents/tools/hvigor/bin/hvigorw assembleHap \
    --mode module -p module=entry@gles -p product=default
```

`DEVECO_SDK_HOME` must point to the SDK repository root shown above, not its `default` child. If DevEco reports stale native options after changing targets, clean the project before rebuilding so each target regenerates its own CMake cache.

Use a distinct install prefix for MOBILE, 2IN1 desktop GL, 2IN1 GLES, static, and shared packages. Their canonical SFML target, dependency-file, and archive names intentionally match, so installing different contracts into the same prefix would replace the earlier package.

## Emulator acceptance

Run both HAP variants on the API 24 MateBook Pro emulator and retain the on-screen diagnostics plus HiLog lines tagged `SFML-Harmony`.

1. For both variants, require `SFML_DEVICE_FORM=2in1`, a successful XComponent host initialization, a nonzero Stage window ID, and repeated resize/render activity without EGL errors.
2. For `desktopGl`, require `SFML_GRAPHICS_API=OpenGL`, `QueryGL desktop=yes`, a non-ES `GL_VERSION`, and valid `GL_VENDOR`, `GL_RENDERER`, and GLSL strings. The requested context must not exceed HarmonyOS OpenGL 4.2.
3. For `gles`, require `SFML_GRAPHICS_API=OpenGL ES` and an `OpenGL ES` version string. It must render the same scene and complete the same graphics diagnostics without loading the desktop GL path.
4. Switch the main window between full-screen and floating modes. Confirm SFML receives the new surface dimensions, the viewport and pointer coordinates remain correct, and the existing surface-loss/recreation diagnostics retain their resources.
5. Exercise keyboard focus, text input, all available mouse buttons and wheels, cursor visibility/shape, pointer confinement, and relative pointer movement. Cursor lock must fail safely if the system does not grant or expose the capability. An emulator image with no pointer device must use the last XComponent-local position without repeatedly querying the unavailable system pointer.
6. Run the 100-cycle XComponent surface stress and verify `100/100` with zero native failures for each graphics target. Complete the remaining resource, RenderTexture, clipboard, network, audio, controller, and optional Vulkan diagnostics as supported by the emulator.

PC-specific native controls are `F6` for alternate position/size/title/icon plus min/max constraints, `F7` for cursor grab, `F8` for minimize/restore, and `F9` for SFML windowed/fullscreen recreation. `F10` remains the ArkUI surface-stress shortcut when it is delivered to the page host.

Do not treat a successful HAP build as runtime graphics validation: the acceptance record must include the actual GL identity strings and the full-screen/floating transition results from the emulator.
