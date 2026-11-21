#include <SFML/Graphics.hpp>

#include <SFML/System/FileInputStream.hpp>

#include <catch2/catch_test_macros.hpp>

#include <GraphicsUtil.hpp>
#include <WindowUtil.hpp>
#include <array>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>

namespace
{
constexpr std::string_view metalVertex = R"(
#include <metal_stdlib>
using namespace metal;
struct SFMLDraw { float4x4 sf_ModelViewProjectionMatrix; float4 sf_TextureTransform; };
struct In { float2 position [[attribute(0)]]; float4 color [[attribute(1)]]; float2 uv [[attribute(2)]]; };
struct Out { float4 position [[position]]; float4 color [[user(sf_color)]]; float2 uv [[user(sf_texcoord)]]; };
vertex Out main0(In input [[stage_in]], constant SFMLDraw& draw [[buffer(0)]]) {
    Out output;
    output.position = draw.sf_ModelViewProjectionMatrix * float4(input.position, 0, 1);
    output.color = input.color;
    output.uv = input.uv * draw.sf_TextureTransform.xy;
    return output;
})";

constexpr std::string_view metalGreen = R"(
#include <metal_stdlib>
using namespace metal;
fragment float4 main0() { return float4(0, 1, 0, 1); })";
} // namespace

TEST_CASE("[Graphics] Metal reports its settings and reads back the window", runDisplayTests())
{
    const sf::Texture texture(sf::Image({2, 2}, sf::Color::Green));
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    sf::RenderWindow window(sf::VideoMode({16, 16}), "Metal settings");
    REQUIRE(window.isOpen());
    CHECK(std::holds_alternative<sf::MetalSettings>(window.getSettings().backend));
    CHECK(sf::setGraphicsBackend(sf::GraphicsBackend::Metal, sf::MetalSettings{}));
    CHECK_FALSE(sf::setGraphicsBackend(sf::GraphicsBackend::OpenGL));
    CHECK_FALSE(sf::setGraphicsBackend(sf::GraphicsBackend::Metal, sf::OpenGLSettings{}));
    sf::RenderTexture target({8, 8});
    CHECK_FALSE(target.resize({16, 16}, sf::ContextSettings{0, 0, 0, false, sf::OpenGLSettings{}}));
    CHECK(target.getSize() == sf::Vector2u(8, 8));
    window.clear(sf::Color::Red);
    window.draw(sf::Sprite(texture));
    sf::Texture capture(window.getSize());
    capture.update(window);
    CHECK(capture.copyToImage().getPixel({1, 1}) == sf::Color::Green);
    CHECK(capture.copyToImage().getPixel({8, 8}) == sf::Color::Red);
    window.display();
    window.setSize({24, 20});
    window.clear(sf::Color::Blue);
    REQUIRE(capture.resize(window.getSize()));
    capture.update(window);
    CHECK(capture.copyToImage().getPixel({4, 4}) == sf::Color::Blue);
    window.display();
}

TEST_CASE("[Graphics] Metal shader loaders and stages", runDisplayTests())
{
    sf::RenderTexture target({16, 16});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    REQUIRE(sf::Shader::isAvailable());
    CHECK_FALSE(sf::Shader::isGeometryAvailable());
    sf::Shader shader;
    SECTION("File")
    {
        REQUIRE(shader.loadFromFile("shader-metal.metal", sf::Shader::Type::Fragment));
        shader.setUniform("color", sf::Glsl::Vec4(0, 1, 0, 1));
    }
    SECTION("Stream")
    {
        sf::FileInputStream stream("shader-metal.metal");
        REQUIRE(shader.loadFromStream(stream, sf::Shader::Type::Fragment));
        shader.setUniform("color", sf::Glsl::Vec4(0, 1, 0, 1));
    }
    SECTION("Memory, vertex and fragment")
    {
        REQUIRE(shader.loadFromMemory(metalVertex, metalGreen));
    }
    SECTION("Vertex only")
    {
        REQUIRE(shader.loadFromMemory(metalVertex, sf::Shader::Type::Vertex));
    }
    SECTION("Single function without main0")
    {
        REQUIRE(shader.loadFromMemory(R"(
#include <metal_stdlib>
using namespace metal;
fragment float4 green() { return float4(0, 1, 0, 1); })",
                                      sf::Shader::Type::Fragment));
    }
    SECTION("Invalid MSL preserves the existing program")
    {
        REQUIRE(shader.loadFromMemory(metalVertex, sf::Shader::Type::Vertex));
        CHECK_FALSE(shader.loadFromMemory("not a shader", sf::Shader::Type::Fragment));
        CHECK_FALSE(shader.loadFromMemory(R"(
#include <metal_stdlib>
using namespace metal;
fragment float4 first() { return float4(1); }
fragment float4 second() { return float4(0); })",
                                          sf::Shader::Type::Fragment));
        CHECK_FALSE(shader.loadFromMemory(R"(
#include <metal_stdlib>
using namespace metal;
fragment float4 main0(constant float4& color [[buffer(0)]]) { return color; })",
                                          sf::Shader::Type::Fragment));
        CHECK_FALSE(shader.loadFromMemory(metalGreen, sf::Shader::Type::Geometry));
    }
    sf::RectangleShape rectangle({16.f, 16.f});
    rectangle.setFillColor(sf::Color::Green);
    target.clear(sf::Color::Red);
    target.draw(rectangle, &shader);
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({8, 8}) == sf::Color::Green);
    CHECK(shader.getNativeHandle() == 0);
}

TEST_CASE("[Graphics] Metal precompiled library", runDisplayTests())
{
#ifndef SFML_METAL_LIBRARY_FIXTURE
    SKIP("The Metal Toolchain is not installed, so no metallib fixture was built");
#else
    sf::RenderTexture target({16, 16});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    std::ifstream     file(SFML_METAL_LIBRARY_FIXTURE, std::ios::binary);
    const std::string library((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    REQUIRE(library.substr(0, 4) == "MTLB");
    sf::Shader shader;
    SECTION("Memory")
    {
        REQUIRE(shader.loadFromMemory(std::string_view(library.data(), library.size()), sf::Shader::Type::Fragment));
    }
    SECTION("File")
    {
        REQUIRE(shader.loadFromFile(SFML_METAL_LIBRARY_FIXTURE, sf::Shader::Type::Fragment));
    }
    shader.setUniform("color", sf::Glsl::Vec4(0, 0, 1, 1));
    target.clear(sf::Color::Red);
    target.draw(sf::RectangleShape({16.f, 16.f}), &shader);
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({8, 8}) == sf::Color::Blue);
#endif
}

TEST_CASE("[Graphics] Metal color space and sampling", runDisplayTests())
{
    sf::RenderTexture target({4, 4});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    SECTION("sRGB encoding and decoding")
    {
        sf::RenderTexture srgb({4, 4}, sf::ContextSettings{0, 0, 0, true});
        REQUIRE(srgb.isSrgb());
        srgb.clear(sf::Color(128, 128, 128));
        srgb.display();
        const auto encoded = srgb.getTexture().copyToImage().getPixel({1, 1});
        CHECK(encoded.r >= 187);
        CHECK(encoded.r <= 189);
        target.draw(sf::Sprite(srgb.getTexture()));
        target.display();
        const auto decoded = target.getTexture().copyToImage().getPixel({1, 1});
        CHECK(decoded.r >= 127);
        CHECK(decoded.r <= 129);
    }
    SECTION("Filtering and repeat")
    {
        sf::Image pixels({2, 1}, sf::Color::Red);
        pixels.setPixel({1, 0}, sf::Color::Blue);
        sf::Texture texture(pixels);
        texture.setRepeated(true);
        target.draw(sf::Sprite(texture, {{0, 0}, {4, 1}}), sf::BlendNone);
        target.display();
        const auto repeated = target.getTexture().copyToImage();
        CHECK(repeated.getPixel({0, 0}) == sf::Color::Red);
        CHECK(repeated.getPixel({1, 0}) == sf::Color::Blue);
        CHECK(repeated.getPixel({2, 0}) == sf::Color::Red);
        CHECK(repeated.getPixel({3, 0}) == sf::Color::Blue);
        texture.setSmooth(true);
        sf::Sprite sprite(texture);
        sprite.setScale({0.5f, 1.f});
        target.draw(sprite, sf::BlendNone);
        target.display();
        const auto filtered = target.getTexture().copyToImage().getPixel({0, 0});
        CHECK(filtered.r >= 127);
        CHECK(filtered.r <= 129);
        CHECK(filtered.b >= 127);
        CHECK(filtered.b <= 129);
        REQUIRE(texture.generateMipmap());
        target.draw(sprite, sf::BlendNone);
        target.display();
        CHECK(target.getTexture().copyToImage().getPixel({0, 0}) == filtered);
    }
    SECTION("New textures start transparent")
    {
        const sf::Texture blank(sf::Vector2u(3, 3));
        CHECK(blank.copyToImage().getPixel({2, 2}) == sf::Color::Transparent);
    }
}

TEST_CASE("[Graphics] Metal blend equations and line primitives", runDisplayTests())
{
    sf::RenderTexture target({8, 8});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    SECTION("Blend equations")
    {
        struct BlendCase
        {
            sf::BlendMode::Equation equation{};
            sf::Color               expected;
        };
        const std::array   cases{BlendCase{sf::BlendMode::Equation::Add, sf::Color(160, 144, 96, 255)},
                               BlendCase{sf::BlendMode::Equation::Subtract, sf::Color(0, 112, 32, 255)},
                               BlendCase{sf::BlendMode::Equation::ReverseSubtract, sf::Color(96, 0, 0, 255)},
                               BlendCase{sf::BlendMode::Equation::Min, sf::Color(32, 16, 32, 255)},
                               BlendCase{sf::BlendMode::Equation::Max, sf::Color(128, 128, 64, 255)}};
        sf::RectangleShape rectangle({8, 8});
        rectangle.setFillColor(sf::Color(32, 128, 64));
        for (const auto& test : cases)
        {
            target.clear(sf::Color(128, 16, 32));
            target.draw(rectangle,
                        sf::BlendMode(sf::BlendMode::Factor::One,
                                      sf::BlendMode::Factor::One,
                                      test.equation,
                                      sf::BlendMode::Factor::One,
                                      sf::BlendMode::Factor::Zero,
                                      sf::BlendMode::Equation::Add));
            target.display();
            CHECK(target.getTexture().copyToImage().getPixel({4, 4}) == test.expected);
        }
    }
    SECTION("Points, lines and line strip")
    {
        const sf::Vertex                point{{1.5f, 1.5f}, sf::Color::Green};
        const std::array<sf::Vertex, 3> line{
            {{{1.5f, 3.5f}, sf::Color::Blue}, {{6.5f, 3.5f}, sf::Color::Blue}, {{6.5f, 6.5f}, sf::Color::Blue}}};
        target.clear(sf::Color::Red);
        target.draw(&point, 1, sf::PrimitiveType::Points);
        target.draw(line.data(), 2, sf::PrimitiveType::Lines);
        target.draw(line.data(), line.size(), sf::PrimitiveType::LineStrip);
        target.display();
        const auto pixels = target.getTexture().copyToImage();
        CHECK(pixels.getPixel({1, 1}) == sf::Color::Green);
        CHECK(pixels.getPixel({3, 3}) == sf::Color::Blue);
        CHECK(pixels.getPixel({6, 5}) == sf::Color::Blue);
    }
    SECTION("Large vertex arrays")
    {
        sf::VertexArray triangles(sf::PrimitiveType::Triangles);
        for (int index = 0; index < 400; ++index)
        {
            triangles.append({{0, 0}, sf::Color::Cyan});
            triangles.append({{8, 0}, sf::Color::Cyan});
            triangles.append({{0, 8}, sf::Color::Cyan});
        }
        target.clear(sf::Color::Red);
        target.draw(triangles);
        target.display();
        CHECK(target.getTexture().copyToImage().getPixel({1, 1}) == sf::Color::Cyan);
    }
}

TEST_CASE("[Graphics] Metal uniform packing", runDisplayTests())
{
    sf::RenderTexture target({8, 8});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    constexpr std::string_view fragment = R"(
#include <metal_stdlib>
using namespace metal;
struct User {
    float scalar; float2 vector2; float3 vector3; float4 vector4;
    int integer; int2 integer2; int3 integer3; int4 integer4;
    bool boolean; bool2 boolean2; bool3 boolean3; bool4 boolean4;
    float numbers[2]; float2 pairs[2]; float3 triples[2]; float4 quads[2];
    float3x3 matrix3; float4x4 matrix4; float3x3 matrices3[2]; float4x4 matrices4[2];
};
fragment float4 main0(constant User& u [[buffer(1)]]) {
    bool ok = u.scalar == 1 && all(u.vector2 == float2(2,3)) && all(u.vector3 == float3(4,5,6)) && all(u.vector4 == float4(7,8,9,10));
    ok = ok && u.integer == 11 && all(u.integer2 == int2(12,13)) && all(u.integer3 == int3(14,15,16)) && all(u.integer4 == int4(17,18,19,20));
    ok = ok && u.boolean && all(u.boolean2 == bool2(true,false)) && all(u.boolean3 == bool3(false,true,false)) && all(u.boolean4 == bool4(true,false,true,false));
    ok = ok && u.numbers[1] == 2 && u.pairs[1].y == 4 && u.triples[1].z == 6 && u.quads[1].w == 8;
    ok = ok && u.matrix3[2][0] == 0.25 && u.matrix3[2][1] == 0.5 && u.matrix4[3][0] == 0.25 && u.matrix4[3][1] == 0.5;
    ok = ok && u.matrices3[1][2][0] == 0.25 && u.matrices4[1][3][1] == 0.5;
    return ok ? float4(0,1,0,1) : float4(1,0,0,1);
})";
    sf::Shader                 shader(fragment, sf::Shader::Type::Fragment);
    shader.setUniform("scalar", 1.f);
    shader.setUniform("vector2", sf::Glsl::Vec2(2, 3));
    shader.setUniform("vector3", sf::Glsl::Vec3(4, 5, 6));
    shader.setUniform("vector4", sf::Glsl::Vec4(7, 8, 9, 10));
    shader.setUniform("integer", 11);
    shader.setUniform("integer2", sf::Glsl::Ivec2(12, 13));
    shader.setUniform("integer3", sf::Glsl::Ivec3(14, 15, 16));
    shader.setUniform("integer4", sf::Glsl::Ivec4(17, 18, 19, 20));
    shader.setUniform("boolean", true);
    shader.setUniform("boolean2", sf::Glsl::Bvec2(true, false));
    shader.setUniform("boolean3", sf::Glsl::Bvec3(false, true, false));
    shader.setUniform("boolean4", sf::Glsl::Bvec4(true, false, true, false));
    const std::array numbers{1.f, 2.f};
    const std::array pairs{sf::Glsl::Vec2(1, 2), sf::Glsl::Vec2(3, 4)};
    const std::array triples{sf::Glsl::Vec3(1, 2, 3), sf::Glsl::Vec3(4, 5, 6)};
    const std::array quads{sf::Glsl::Vec4(1, 2, 3, 4), sf::Glsl::Vec4(5, 6, 7, 8)};
    shader.setUniformArray("numbers", numbers.data(), numbers.size());
    shader.setUniformArray("pairs", pairs.data(), pairs.size());
    shader.setUniformArray("triples", triples.data(), triples.size());
    shader.setUniformArray("quads", quads.data(), quads.size());
    const sf::Transform  transform = sf::Transform().translate({0.25f, 0.5f});
    const sf::Glsl::Mat3 matrix3(transform);
    const sf::Glsl::Mat4 matrix4(transform);
    const std::array     matrices3{sf::Glsl::Mat3(sf::Transform::Identity), matrix3};
    const std::array     matrices4{sf::Glsl::Mat4(sf::Transform::Identity), matrix4};
    shader.setUniform("matrix3", matrix3);
    shader.setUniform("matrix4", matrix4);
    shader.setUniformArray("matrices3", matrices3.data(), matrices3.size());
    shader.setUniformArray("matrices4", matrices4.data(), matrices4.size());
    target.draw(sf::RectangleShape({8.f, 8.f}), &shader);
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({4, 4}) == sf::Color::Green);
}

TEST_CASE("[Graphics] Metal named textures", runDisplayTests())
{
    sf::RenderTexture target({16, 16});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    constexpr std::string_view fragment = R"(
#include <metal_stdlib>
using namespace metal;
struct In { float4 color [[user(sf_color)]]; float2 uv [[user(sf_texcoord)]]; };
fragment float4 main0(In input [[stage_in]],
                      texture2d<float> current [[texture(0)]], sampler currentSampler [[sampler(0)]],
                      texture2d<float> extra [[texture(1)]], sampler extraSampler [[sampler(1)]]) {
    return current.sample(currentSampler, input.uv) + extra.sample(extraSampler, input.uv);
})";
    sf::Shader                 shader(fragment, sf::Shader::Type::Fragment);
    const sf::Texture          red(sf::Image({2, 2}, sf::Color(255, 0, 0, 0)));
    const sf::Texture          green(sf::Image({2, 2}, sf::Color(0, 255, 0, 255)));
    shader.setUniform("current", sf::Shader::CurrentTexture);
    shader.setUniform("extra", green);
    sf::Sprite sprite(red);
    sprite.setScale({8, 8});
    target.draw(sprite, &shader);
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({8, 8}) == sf::Color::Yellow);

    shader.setUniform("extra", target.getTexture());
    target.clear(sf::Color::Blue);
    target.draw(sprite, &shader);
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({8, 8}) == sf::Color::Blue);
}

TEST_CASE("[Graphics] Metal unassigned textures sample the current texture", runDisplayTests())
{
    sf::RenderTexture target({8, 8});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    const sf::Shader  shader(std::string_view(R"(
#include <metal_stdlib>
using namespace metal;
struct In { float4 color [[user(sf_color)]]; float2 uv [[user(sf_texcoord)]]; };
fragment float4 main0(In input [[stage_in]], texture2d<float> image [[texture(0)]], sampler imageSampler [[sampler(0)]]) {
    return image.sample(imageSampler, input.uv);
})"),
                            sf::Shader::Type::Fragment);
    const sf::Texture green(sf::Image({2, 2}, sf::Color::Green));
    sf::Sprite        sprite(green);
    sprite.setScale({4, 4});
    target.clear(sf::Color::Red);
    target.draw(sprite, &shader);
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({4, 4}) == sf::Color::Green);
}

TEST_CASE("[Graphics] Metal model-view and projection matrices", runDisplayTests())
{
    sf::RenderTexture target({16, 16});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    const sf::Shader shader(std::string_view(R"(
#include <metal_stdlib>
using namespace metal;
struct SFMLDraw {
    float4x4 sf_ModelViewProjectionMatrix; float4 sf_TextureTransform;
    float4x4 sf_ModelViewMatrix; float4x4 sf_ProjectionMatrix;
};
struct In { float2 position [[attribute(0)]]; };
struct Out { float4 position [[position]]; float4 color [[user(sf_color)]]; float2 uv [[user(sf_texcoord)]]; };
vertex Out main0(In input [[stage_in]], constant SFMLDraw& draw [[buffer(0)]]) {
    const float4 world = draw.sf_ModelViewMatrix * float4(input.position, 0, 1);
    const float4 combined = draw.sf_ModelViewProjectionMatrix * float4(input.position, 0, 1);
    Out output;
    output.position = draw.sf_ProjectionMatrix * world;
    const bool ok = all(abs(world.xy - (input.position + float2(8, 8))) < 0.001) && all(abs(output.position - combined) < 0.001);
    output.color = ok ? float4(0, 1, 0, 1) : float4(0, 0, 1, 1);
    output.uv = float2(0);
    return output;
})"),
                            sf::Shader::Type::Vertex);
    sf::RenderStates states(&shader);
    states.transform.translate({8.f, 8.f});
    target.clear(sf::Color::Red);
    target.draw(sf::RectangleShape({8.f, 8.f}), states);
    target.display();
    const auto pixels = target.getTexture().copyToImage();
    CHECK(pixels.getPixel({12, 12}) == sf::Color::Green);
    CHECK(pixels.getPixel({4, 4}) == sf::Color::Red);
}

TEST_CASE("[Graphics] Metal partial clear, stencil, MSAA and triangle fans", runDisplayTests())
{
    sf::RenderTexture target({32, 32}, sf::ContextSettings{0, 8, 4});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    CHECK(target.getTexture().getSize() == sf::Vector2u(32, 32));
    CHECK(sf::RenderTexture::getMaximumAntiAliasingLevel() >= 4);
    target.clear(sf::Color::Red, sf::StencilValue(0));
    auto view = target.getView();
    view.setScissor({{0.25f, 0.25f}, {0.5f, 0.5f}});
    target.setView(view);
    target.clear(sf::Color::Green, sf::StencilValue(7));
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({1, 1}) == sf::Color::Red);
    CHECK(target.getTexture().copyToImage().getPixel({16, 16}) == sf::Color::Green);
    target.setView(target.getDefaultView());
    const std::array<sf::Vertex, 4> vertices{
        {{{0, 0}, sf::Color::Blue}, {{32, 0}, sf::Color::Blue}, {{32, 32}, sf::Color::Blue}, {{0, 32}, sf::Color::Blue}}};
    sf::VertexBuffer buffer(sf::PrimitiveType::TriangleFan);
    REQUIRE(buffer.create(vertices.size()));
    REQUIRE(buffer.update(vertices.data()));
    target.draw(buffer, sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Keep, 7, 0xFF, false});
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({16, 16}) == sf::Color::Blue);
    CHECK(target.getTexture().copyToImage().getPixel({1, 1}) == sf::Color::Red);

    const sf::VertexBuffer    copy(buffer);
    std::array<sf::Vertex, 2> updated{{{{0, 0}, sf::Color::White}, {{32, 0}, sf::Color::White}}};
    REQUIRE(buffer.update(updated.data(), updated.size(), 0));
    target.clear(sf::Color::Black, sf::StencilValue(7));
    target.draw(copy, sf::StencilMode{sf::StencilComparison::Equal, sf::StencilUpdateOperation::Keep, 7, 0xFF, false});
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({16, 16}) == sf::Color::Blue);
}

TEST_CASE("[Graphics] Metal pipeline cache", runDisplayTests())
{
    sf::RenderTexture target({8, 8});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::Metal)
        SKIP("Requires Metal");
    target.clear(sf::Color::Red);
    target.draw(sf::RectangleShape({4.f, 4.f}));
    target.display();
    const auto data = sf::getPipelineCacheData();
    REQUIRE(data);
    CHECK(!data->empty());
    CHECK(sf::loadPipelineCacheFromMemory(data->data(), data->size()));
    const std::array<std::uint8_t, 16> garbage{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    CHECK_FALSE(sf::loadPipelineCacheFromMemory(garbage.data(), garbage.size()));
    CHECK_FALSE(sf::loadPipelineCacheFromMemory(nullptr, 0));
    const sf::Shader shader(metalGreen, sf::Shader::Type::Fragment);
    target.clear(sf::Color::Red);
    target.draw(sf::RectangleShape({8.f, 8.f}), &shader);
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({4, 4}) == sf::Color::Green);
}
