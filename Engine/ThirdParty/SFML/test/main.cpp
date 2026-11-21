#include <catch2/catch_session.hpp>

#include <SFML/Main.hpp>

#ifdef SFML_SYSTEM_IOS
#include <array>
#include <string>

#include <crt_externs.h>
#include <cstdlib>

void              waitForActiveScene();
[[noreturn]] void showTestResult(int result, const char* mode);
#endif

#ifdef SFML_DIRECTX_TESTS
#include <SFML/Graphics/DirectX/DirectXTest.hpp>
#endif

#ifdef SFML_GRAPHICS_TEST_MAIN
#include <SFML/Graphics/GraphicsBackend.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <SFML/Window/Vulkan.hpp>

#include <string_view>
#endif

namespace
{
////////////////////////////////////////////////////////////
int runTests(int argc, char** argv)
{
#ifdef SFML_DIRECTX_TESTS
    // Test runners may have no hardware adapter. Production builds never enable WARP implicitly.
    sf::priv::configureDirectXTestDevice(true, false, 0, false);
#endif
#ifdef SFML_GRAPHICS_TEST_MAIN
    if (argc > 1)
    {
        const std::string_view mode(argv[1]);
        if (mode == "--sfml-vulkan" || mode == "--sfml-gl-core" || mode == "--sfml-vulkan-high" ||
            mode == "--sfml-opengl" || mode == "--sfml-directx" || mode == "--sfml-directx-warp" ||
            mode == "--sfml-directx-min" || mode == "--sfml-directx-high" || mode == "--sfml-directx-debug" ||
            mode == "--sfml-directx-fallback" || mode == "--sfml-directx-unavailable" || mode == "--sfml-gles-high" ||
            mode == "--sfml-gl-high" || mode == "--sfml-metal")
        {
            bool selected = false;
            if (mode == "--sfml-metal")
            {
                if (sf::setGraphicsBackend(sf::GraphicsBackend::OpenGL, sf::MetalSettings{}) ||
                    sf::setGraphicsBackend(sf::GraphicsBackend::Metal, sf::OpenGLSettings{}) ||
                    sf::setGraphicsBackend(sf::GraphicsBackend::OpenGL, sf::OpenGLSettings{1, 1}) ||
                    sf::setGraphicsBackend(sf::GraphicsBackend::OpenGLES, sf::OpenGLESSettings{1, 1}))
                    return 1;
                selected = sf::setGraphicsBackend(sf::GraphicsBackend::Metal, sf::MetalSettings{});
            }
            else if (mode.substr(0, 14) == "--sfml-directx")
            {
#ifdef SFML_DIRECTX_TESTS
                const bool fallback = mode == "--sfml-directx-fallback";
                const bool minimum  = mode == "--sfml-directx-min" || mode == "--sfml-directx-unavailable";
                sf::priv::configureDirectXTestDevice(mode != "--sfml-directx",
                                                     mode == "--sfml-directx-debug",
                                                     minimum ? 0xA000u : 0u,
                                                     fallback);
                if (sf::setGraphicsBackend(sf::GraphicsBackend::DirectX, sf::DirectXSettings{9, 3}) ||
                    sf::setGraphicsBackend(sf::GraphicsBackend::DirectX, sf::DirectXSettings{99, 0}))
                    return 1;
                if (mode == "--sfml-directx-unavailable")
                {
                    if (!sf::setGraphicsBackend(sf::GraphicsBackend::DirectX, sf::DirectXSettings{11, 0}))
                        return 1;
                    sf::RenderTexture unsupported;
                    if (unsupported.resize({1, 1}) || sf::getGraphicsBackend() != sf::GraphicsBackend::Automatic)
                        return 1;
                }
                selected = sf::setGraphicsBackend(fallback ? sf::GraphicsBackend::Automatic : sf::GraphicsBackend::DirectX,
                                                  fallback
                                                      ? sf::ContextSettings::BackendSettings{}
                                                      : sf::DirectXSettings{mode == "--sfml-directx-high" ? 11u : 10u, 0});
#endif
            }
            else if (mode == "--sfml-opengl")
            {
#ifdef SFML_RENDER_OPENGLES
                selected = sf::setGraphicsBackend(sf::GraphicsBackend::OpenGLES);
#else
                selected = sf::setGraphicsBackend(sf::GraphicsBackend::OpenGL);
#endif
            }
            else if (mode == "--sfml-vulkan" || mode == "--sfml-vulkan-high")
            {
                if (!sf::Vulkan::isAvailable())
                    return 77;
                selected = sf::setGraphicsBackend(sf::GraphicsBackend::Vulkan,
                                                  mode == "--sfml-vulkan" ? sf::VulkanSettings{} : sf::VulkanSettings{1, 3});
            }
            else if (mode == "--sfml-gles-high")
            {
                selected = sf::setGraphicsBackend(sf::GraphicsBackend::OpenGLES, sf::OpenGLESSettings{3, 1});
            }
            else if (mode == "--sfml-gl-high")
            {
                selected = sf::setGraphicsBackend(sf::GraphicsBackend::OpenGL,
                                                  sf::OpenGLSettings{4, 5, sf::OpenGLProfile::Compatibility});
            }
            else
            {
                selected = sf::setGraphicsBackend(sf::GraphicsBackend::OpenGL,
                                                  sf::OpenGLSettings{3, 2, sf::OpenGLProfile::Core});
            }
            if (!selected)
                return 1;
            sf::RenderTexture probe;
            if (!probe.resize({1, 1}))
                return mode == "--sfml-vulkan" ? 77 : 1;
            if (mode == "--sfml-directx-fallback" && sf::getGraphicsBackend() == sf::GraphicsBackend::DirectX)
                return 1;
            argv[1] = argv[0];
            ++argv;
            --argc;
        }
    }
#endif
    const int result = Catch::Session().run(argc, argv);
#ifdef SFML_DIRECTX_TESTS
    if (!sf::priv::directXTestDebugLayerClean())
        return 1;
#endif
    return result;
}
} // namespace

////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
#ifdef SFML_SYSTEM_IOS
    // SFML passes no arguments to main on iOS devices
    if (argc == 0)
    {
        argc = *_NSGetArgc();
        argv = *_NSGetArgv();
    }
    waitForActiveScene();
    const bool fromHomeScreen = argc == 1;
#ifdef SFML_TEST_DEFAULT_MODE
    std::string          defaultMode = SFML_TEST_DEFAULT_MODE;
    std::array<char*, 3> defaultArgs{argv[0], defaultMode.data(), nullptr};
    if (fromHomeScreen)
    {
        argc = 2;
        argv = defaultArgs.data();
    }
    const char* const mode = SFML_TEST_DEFAULT_MODE;
#else
    const char* const mode = "";
#endif
    const int result = runTests(argc, argv);
    // Keep the outcome on screen instead of quitting straight away
    if (fromHomeScreen)
        showTestResult(result, mode);
    // SFML keeps the application alive after main returns, which would leave the test runner hanging
    std::exit(result);
#else
    return runTests(argc, argv);
#endif
}
