#include <SFML/Window/ContextSettings.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

TEST_CASE("[Window] sf::ContextSettings")
{
    SECTION("Type traits")
    {
        STATIC_CHECK(std::is_trivially_copy_constructible_v<sf::ContextSettings>);
        STATIC_CHECK(std::is_trivially_copy_assignable_v<sf::ContextSettings>);
        STATIC_CHECK(std::is_trivially_move_constructible_v<sf::ContextSettings>);
        STATIC_CHECK(std::is_trivially_move_assignable_v<sf::ContextSettings>);
    }

    SECTION("Construction")
    {
        SECTION("DirectX feature level")
        {
            constexpr sf::DirectXSettings defaults;
            STATIC_CHECK(defaults.majorVersion == 10);
            STATIC_CHECK(defaults.minorVersion == 0);
            constexpr sf::ContextSettings settings{0, 8, 4, true, sf::DirectXSettings{11, 1}};
            constexpr auto                directX = std::get<sf::DirectXSettings>(settings.backend);
            STATIC_CHECK(directX.majorVersion == 11);
            STATIC_CHECK(directX.minorVersion == 1);
        }
        SECTION("Metal selection")
        {
            constexpr sf::ContextSettings settings{0, 8, 4, false, sf::MetalSettings{}};
            STATIC_CHECK(std::holds_alternative<sf::MetalSettings>(settings.backend));
            STATIC_CHECK(std::is_empty_v<sf::MetalSettings>);
        }
        SECTION("OpenGL defaults never request version 1")
        {
            STATIC_CHECK(sf::OpenGLSettings{}.majorVersion >= 2);
            STATIC_CHECK(sf::OpenGLESSettings{}.majorVersion >= 3);
        }
        SECTION("Aggregate initialization -- Nothing")
        {
            constexpr sf::ContextSettings contextSettings;
            STATIC_CHECK(contextSettings.depthBits == 0);
            STATIC_CHECK(contextSettings.stencilBits == 0);
            STATIC_CHECK(contextSettings.antiAliasingLevel == 0);
            STATIC_CHECK(std::holds_alternative<std::monostate>(contextSettings.backend));
            STATIC_CHECK(contextSettings.sRgbCapable == false);
        }

        SECTION("Aggregate initialization -- Everything")
        {
            constexpr sf::ContextSettings contextSettings{1, 1, 2, true, sf::OpenGLSettings{4, 5, sf::OpenGLProfile::Core}};
            STATIC_CHECK(contextSettings.depthBits == 1);
            STATIC_CHECK(contextSettings.stencilBits == 1);
            STATIC_CHECK(contextSettings.antiAliasingLevel == 2);
            constexpr auto gl = std::get<sf::OpenGLSettings>(contextSettings.backend);
            STATIC_CHECK(gl.majorVersion == 4);
            STATIC_CHECK(gl.minorVersion == 5);
            STATIC_CHECK(gl.profile == sf::OpenGLProfile::Core);
            STATIC_CHECK(contextSettings.sRgbCapable == true);
        }
    }
}
