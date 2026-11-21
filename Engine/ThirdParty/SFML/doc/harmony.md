# OpenHarmony and HarmonyOS

Use the DevEco native CMake toolchain with `OHOS_COMPATIBLE_SDK_VERSION=22` or newer and `OHOS_STL=c++_shared`. Set `OHOS_ARCH=arm64-v8a` for an Arm64 HAP. One SFML build serves phones, tablets and 2-in-1 devices. C++ code can test `SFML_SYSTEM_HARMONY`.

The OpenGL backend uses EGL and OpenGL ES 3.0. The Vulkan backend presents to an XComponent through `VK_OHOS_surface`. Enable either or both with `SFML_BUILD_OPENGL_BACKEND` and `SFML_BUILD_VULKAN_BACKEND`. Select an explicit backend with `sf::setGraphicsBackend` before creating the first GPU resource. An XComponent replacement recreates the presentation surface while the game retains its graphics device.

The Stage host links `SFML::Main` and registers its native XComponent before starting the game. It forwards foreground, background, focus and destroy notifications. The host supplies the resource manager and window callbacks. Phones use the Stage fullscreen policy. On 2-in-1 devices, window commands support positioning, sizing and cursor confinement when offered by the host. `sf::WindowHandle` identifies the registered native window.

Ordinary paths address the app filesystem. Relative paths that cannot be opened there are looked up in packaged rawfiles. Use `rawfile:/path` to address a packaged resource explicitly. SFML uses OHAudio for playback and recording.
