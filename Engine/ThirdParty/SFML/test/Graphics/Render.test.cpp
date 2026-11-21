#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/GraphicsBackend.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/StencilMode.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

#include <SFML/Window/Context.hpp>

#include <SFML/System/Clock.hpp>

#include <catch2/catch_test_macros.hpp>

#include <GraphicsUtil.hpp>
#include <WindowUtil.hpp>
#include <algorithm>
#include <array>
#include <string>
#include <string_view>

TEST_CASE("[Graphics] Render Tests", runDisplayTests())
{
    SECTION("Stencil Tests")
    {
        sf::RenderTexture renderTexture({100, 100}, sf::ContextSettings{0 /* depthBits */, 8 /* stencilBits */});
        renderTexture.clear(sf::Color::Red, 127);

        sf::RectangleShape shape1({100, 100});
        shape1.setFillColor(sf::Color::Green);
        sf::RectangleShape shape2({100, 100});
        shape2.setFillColor(sf::Color::Blue);

        SECTION("Stencil-Only")
        {
            renderTexture.draw(shape1,
                               sf::StencilMode{sf::StencilComparison::Always, sf::StencilUpdateOperation::Keep, 1, 0xFF, true});
            renderTexture.display();
            CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
        }

        SECTION("Comparisons")
        {
            SECTION("Always")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Always, sf::StencilUpdateOperation::Keep, 1, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }

            SECTION("Equal")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Keep, 126, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Keep, 127, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }

            SECTION("Greater")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Greater, sf::StencilUpdateOperation::Keep, 126, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Greater, sf::StencilUpdateOperation::Keep, 127, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Greater, sf::StencilUpdateOperation::Keep, 128, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }

            SECTION("GreaterEqual")
            {
                renderTexture.draw(shape1,
                                   sf::StencilMode{sf::StencilComparison::GreaterEqual,
                                                   sf::StencilUpdateOperation::Keep,
                                                   126,
                                                   0xFF,
                                                   false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
                renderTexture.draw(shape1,
                                   sf::StencilMode{sf::StencilComparison::GreaterEqual,
                                                   sf::StencilUpdateOperation::Keep,
                                                   127,
                                                   0xFF,
                                                   false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
                renderTexture.draw(shape2,
                                   sf::StencilMode{sf::StencilComparison::GreaterEqual,
                                                   sf::StencilUpdateOperation::Keep,
                                                   128,
                                                   0xFF,
                                                   false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Blue);
            }

            SECTION("Less")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Less, sf::StencilUpdateOperation::Keep, 128, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Less, sf::StencilUpdateOperation::Keep, 127, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Less, sf::StencilUpdateOperation::Keep, 126, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }

            SECTION("LessEqual")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::LessEqual, sf::StencilUpdateOperation::Keep, 128, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::LessEqual, sf::StencilUpdateOperation::Keep, 127, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
                renderTexture
                    .draw(shape2,
                          sf::StencilMode{sf::StencilComparison::LessEqual, sf::StencilUpdateOperation::Keep, 126, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Blue);
            }

            SECTION("Never")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Never, sf::StencilUpdateOperation::Keep, 127, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
            }

            SECTION("NotEqual")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::NotEqual, sf::StencilUpdateOperation::Keep, 127, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Red);
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::NotEqual, sf::StencilUpdateOperation::Keep, 128, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }
        }

        SECTION("Updating")
        {
            SECTION("Decrement")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Always, sf::StencilUpdateOperation::Decrement, 127, 0xFF, true});
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Decrement, 126, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }

            SECTION("Increment")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Always, sf::StencilUpdateOperation::Increment, 127, 0xFF, true});
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Increment, 128, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }

            SECTION("Invert")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Always, sf::StencilUpdateOperation::Invert, 127, 0xFF, true});
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Invert, 0x80, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }

            SECTION("Keep")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Always, sf::StencilUpdateOperation::Keep, 127, 0xFF, true});
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Keep, 127, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }

            SECTION("Replace")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Always, sf::StencilUpdateOperation::Replace, 255, 0xFF, true});
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Replace, 255, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }

            SECTION("Zero")
            {
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Always, sf::StencilUpdateOperation::Zero, 127, 0xFF, true});
                renderTexture
                    .draw(shape1,
                          sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Zero, 0, 0xFF, false});
                renderTexture.display();
                CHECK(renderTexture.getTexture().copyToImage().getPixel({50, 50}) == sf::Color::Green);
            }
        }
    }
}

TEST_CASE("[Graphics] Render resources before opening a window", runDisplayTests())
{
    const sf::Texture texture(sf::Image({3, 5}, sf::Color::Magenta));
    sf::RenderWindow  window(sf::VideoMode({128, 96}), "Graphics backend test");
    REQUIRE(window.isOpen());

    sf::RenderTexture offscreen({32, 24});
    offscreen.clear(sf::Color::Yellow);
    offscreen.draw(sf::Sprite(texture));
    offscreen.display();

    window.clear(sf::Color::Blue);
    window.draw(sf::Sprite(offscreen.getTexture()));
    sf::Texture capture(window.getSize());
    capture.update(window);
    const auto pixels = capture.copyToImage();
    CHECK(pixels.getPixel({1, 1}) == sf::Color::Magenta);
    CHECK(pixels.getPixel({16, 12}) == sf::Color::Yellow);
    CHECK(pixels.getPixel({48, 48}) == sf::Color::Blue);
    window.display();

#if !defined(SFML_SYSTEM_ANDROID) && !defined(SFML_SYSTEM_IOS)
    sf::RenderWindow other(sf::VideoMode({64, 48}), "Shared texture test");
    REQUIRE(other.isOpen());
    other.clear(sf::Color::Black);
    other.draw(sf::Sprite(texture));
    sf::Texture sharedCapture(other.getSize());
    sharedCapture.update(other);
    CHECK(sharedCapture.copyToImage().getPixel({1, 1}) == sf::Color::Magenta);
    other.display();

    constexpr sf::Vector2u resizedSize{160, 120};
    window.setSize(resizedSize);

#if defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD) || defined(SFML_SYSTEM_OPENBSD) || \
    defined(SFML_SYSTEM_NETBSD)
    // X11 window managers can process resize requests asynchronously.
    const sf::Clock resizeTimeout;
    bool            resized = false;
    while (!resized && resizeTimeout.getElapsedTime() < sf::seconds(2))
    {
        if (const auto event = window.waitEvent(sf::milliseconds(50)))
            if (const auto* resize = event->getIf<sf::Event::Resized>())
                resized = resize->size == resizedSize;
    }
    REQUIRE(resized);
#endif

    window.clear(sf::Color::Cyan);
    REQUIRE(capture.resize(window.getSize()));
    capture.update(window);
    CHECK(capture.copyToImage().getPixel({140, 100}) == sf::Color::Cyan);
    window.display();
#endif

    const auto backend = sf::getGraphicsBackend();
    CHECK(backend != sf::GraphicsBackend::Automatic);
    CHECK_FALSE(sf::setGraphicsBackend(
        backend == sf::GraphicsBackend::DirectX ? sf::GraphicsBackend::Vulkan : sf::GraphicsBackend::DirectX));
    CHECK(sf::getGraphicsBackend() == backend);
    CHECK_FALSE(
        offscreen.resize({16, 16},
                         sf::ContextSettings{0,
                                             0,
                                             0,
                                             false,
                                             backend == sf::GraphicsBackend::OpenGL
                                                 ? sf::ContextSettings::BackendSettings{sf::OpenGLSettings{99, 0}}
                                             : backend == sf::GraphicsBackend::OpenGLES
                                                 ? sf::ContextSettings::BackendSettings{sf::OpenGLESSettings{99, 0}}
                                             : backend == sf::GraphicsBackend::DirectX
                                                 ? sf::ContextSettings::BackendSettings{sf::DirectXSettings{99, 0}}
                                                 : sf::ContextSettings::BackendSettings{sf::VulkanSettings{99, 0}}}));
    CHECK(offscreen.getSize() == sf::Vector2u(32, 24));

    window.close();
    window.create(sf::VideoMode({64, 48}), "Recreated render window");
    REQUIRE(window.isOpen());
    window.clear(sf::Color::Black);
    window.draw(sf::Sprite(texture));
    window.display();
}

TEST_CASE("[Graphics] Render custom shader on selected backend", runDisplayTests())
{
    sf::RenderTexture target({16, 16});
    sf::Shader        shader;
    if (sf::getGraphicsBackend() == sf::GraphicsBackend::Vulkan)
    {
        REQUIRE(shader.loadFromFile("shader-vulkan.frag.spv", sf::Shader::Type::Fragment));
    }
    else if (sf::getGraphicsBackend() == sf::GraphicsBackend::DirectX)
    {
        constexpr std::string_view source = R"(
cbuffer Colors : register(b1) { float4 shades[2]; };
float4 main() : SV_TARGET { return float4(shades[0].r, shades[1].g, shades[0].b, 1.0); }
)";
        REQUIRE(shader.loadFromMemory(source, sf::Shader::Type::Fragment));
    }
    else if (sf::getGraphicsBackend() == sf::GraphicsBackend::Metal)
    {
        constexpr std::string_view source = R"(
#include <metal_stdlib>
using namespace metal;
struct Colors { float4 shades[2]; };
fragment float4 main0(constant Colors& colors [[buffer(1)]]) {
    return float4(colors.shades[0].r, colors.shades[1].g, colors.shades[0].b, 1.0);
}
)";
        REQUIRE(shader.loadFromMemory(source, sf::Shader::Type::Fragment));
    }
    else
    {
        std::string source;
        if (sf::getGraphicsBackend() == sf::GraphicsBackend::OpenGLES)
            source = "#version 300 es\nprecision highp float;\nout vec4 color;\n";
        else
        {
            const sf::Context context;
            if (std::get<sf::OpenGLSettings>(context.getSettings().backend).profile == sf::OpenGLProfile::Core)
                source = "#version 150\nout vec4 color;\n";
            else
                source = "#version 110\n#define color gl_FragColor\n";
        }
        source += "uniform vec4 shades[2]; void main() { color = vec4(shades[0].r, shades[1].g, shades[0].b, 1.0); }";
        REQUIRE(shader.loadFromMemory(std::string_view(source), sf::Shader::Type::Fragment));
    }

    const std::array colors{sf::Glsl::Vec4(0.f, 0.f, 0.f, 1.f), sf::Glsl::Vec4(0.f, 1.f, 0.f, 1.f)};
    shader.setUniformArray("shades", colors.data(), colors.size());
    target.clear();
    target.draw(sf::RectangleShape({16.f, 16.f}), &shader);
    shader.setUniform("shades", sf::Glsl::Vec4(0.f, 0.f, 1.f, 1.f));
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({8, 8}) == sf::Color::Green);
}

TEST_CASE("[Graphics] Render streamed vertices and glyphs with attachments", runDisplayTests())
{
    const auto        samples = std::min(sf::RenderTexture::getMaximumAntiAliasingLevel(), 4u);
    sf::RenderTexture target({64, 64}, sf::ContextSettings{24, 8, samples});
    target.clear(sf::Color::Black, 0);

    std::array       vertices{sf::Vertex{{0.f, 0.f}, sf::Color::Red},
                        sf::Vertex{{16.f, 0.f}, sf::Color::Red},
                        sf::Vertex{{0.f, 16.f}, sf::Color::Red},
                        sf::Vertex{{16.f, 16.f}, sf::Color::Red}};
    sf::VertexBuffer buffer(sf::PrimitiveType::TriangleStrip, sf::VertexBuffer::Usage::Dynamic);
    REQUIRE(buffer.create(vertices.size()));
    REQUIRE(buffer.update(vertices.data()));
    target.draw(buffer);

    for (auto& vertex : vertices)
    {
        vertex.position.x += 16.f;
        vertex.color = sf::Color::Green;
    }
    REQUIRE(buffer.update(vertices.data()));
    target.draw(buffer);

    const sf::Font font("tuffy.ttf");
    sf::Text       text(font, "SFML", 16);
    text.setPosition({2.f, 22.f});
    target.draw(text);
    target.display();

    const auto pixels = target.getTexture().copyToImage();
    CHECK(pixels.getPixel({4, 4}) == sf::Color::Red);
    CHECK(pixels.getPixel({20, 4}) == sf::Color::Green);
    bool glyphVisible = false;
    for (unsigned int y = 22; y < 48 && !glyphVisible; ++y)
        for (unsigned int x = 2; x < 62 && !glyphVisible; ++x)
            glyphVisible = pixels.getPixel({x, y}) != sf::Color::Black;
    CHECK(glyphVisible);
}
