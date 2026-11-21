#include <SFML/Graphics.hpp>

#include <SFML/System/FileInputStream.hpp>

#include <catch2/catch_test_macros.hpp>

#include <GraphicsUtil.hpp>
#include <WindowUtil.hpp>
#include <array>
#include <string_view>

namespace
{
constexpr std::string_view directXVertex = R"(
cbuffer SFMLDraw : register(b0) {
    column_major float4x4 sf_ModelViewProjectionMatrix;
    float4 sf_TextureTransform;
};
struct Output { float4 position : SV_POSITION; float4 color : COLOR0; float2 uv : TEXCOORD0; };
Output main(float2 position : POSITION, float4 color : COLOR0, float2 uv : TEXCOORD0) {
    Output result;
    result.position = mul(sf_ModelViewProjectionMatrix, float4(position, 0, 1));
    result.color = color;
    result.uv = uv * sf_TextureTransform.xy;
    return result;
})";
}

TEST_CASE("[Graphics] DirectX reports and preserves the active feature level", runDisplayTests())
{
    const sf::Texture texture(sf::Image({2, 2}, sf::Color::Green));
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::DirectX)
        SKIP("Requires DirectX");
    sf::RenderWindow window(sf::VideoMode({16, 16}), "DirectX feature level");
    REQUIRE(window.isOpen());
    const auto* actual = std::get_if<sf::DirectXSettings>(&window.getSettings().backend);
    REQUIRE(actual);
    CHECK(actual->majorVersion >= 10);
    CHECK(actual->majorVersion <= 12);
    CHECK(actual->minorVersion <= 1);
    CHECK(sf::setGraphicsBackend(sf::GraphicsBackend::DirectX, sf::DirectXSettings{}));
    if (actual->majorVersion < 12 || actual->minorVersion < 1)
    {
        CHECK_FALSE(sf::setGraphicsBackend(sf::GraphicsBackend::DirectX, sf::DirectXSettings{12, 1}));
        sf::RenderTexture target({8, 8});
        CHECK_FALSE(target.resize({16, 16}, sf::ContextSettings{0, 0, 0, false, sf::DirectXSettings{12, 1}}));
        CHECK(target.getSize() == sf::Vector2u(8, 8));
    }
    window.clear(sf::Color::Red);
    window.draw(sf::Sprite(texture));
    sf::Texture capture(window.getSize());
    capture.update(window);
    CHECK(capture.copyToImage().getPixel({1, 1}) == sf::Color::Green);
    window.display();
}

TEST_CASE("[Graphics] DirectX shader loaders and stages", runDisplayTests())
{
    sf::RenderTexture target({16, 16});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::DirectX)
        SKIP("Requires DirectX");
    REQUIRE(sf::Shader::isAvailable());
    REQUIRE(sf::Shader::isGeometryAvailable());
    sf::Shader shader;
    SECTION("File")
    {
        REQUIRE(shader.loadFromFile("shader-directx.frag.hlsl", sf::Shader::Type::Fragment));
        shader.setUniform("color", sf::Glsl::Vec4(0, 1, 0, 1));
    }
    SECTION("Stream")
    {
        sf::FileInputStream stream("shader-directx.frag.hlsl");
        REQUIRE(shader.loadFromStream(stream, sf::Shader::Type::Fragment));
        shader.setUniform("color", sf::Glsl::Vec4(0, 1, 0, 1));
    }
    SECTION("Memory, vertex and fragment")
    {
        REQUIRE(shader.loadFromMemory(directXVertex, "float4 main() : SV_TARGET { return float4(0,1,0,1); }"));
    }
    SECTION("Vertex only")
    {
        REQUIRE(shader.loadFromMemory(directXVertex, sf::Shader::Type::Vertex));
    }
    SECTION("Invalid HLSL preserves the existing program")
    {
        REQUIRE(shader.loadFromMemory(directXVertex, sf::Shader::Type::Vertex));
        CHECK_FALSE(shader.loadFromMemory("not a shader", sf::Shader::Type::Fragment));
        CHECK_FALSE(shader.loadFromMemory("float4 main(float2 uv : TEXCOORD0) : SV_TARGET { return float4(uv,0,1); }",
                                          sf::Shader::Type::Fragment));
        CHECK_FALSE(
            shader.loadFromMemory("cbuffer User : register(b0) { float4 color; }; float4 main() : SV_TARGET { return "
                                  "color; }",
                                  sf::Shader::Type::Fragment));
    }
    sf::RectangleShape rectangle({16.f, 16.f});
    rectangle.setFillColor(sf::Color::Green);
    target.clear(sf::Color::Red);
    target.draw(rectangle, &shader);
    target.display();
    CHECK(target.getTexture().copyToImage().getPixel({8, 8}) == sf::Color::Green);
    CHECK(shader.getNativeHandle() == 0);
}

TEST_CASE("[Graphics] DirectX color space and sampling", runDisplayTests())
{
    sf::RenderTexture target({4, 4});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::DirectX)
        SKIP("Requires DirectX");
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
}

TEST_CASE("[Graphics] DirectX blend equations and line primitives", runDisplayTests())
{
    sf::RenderTexture target({8, 8});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::DirectX)
        SKIP("Requires DirectX");
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
}

TEST_CASE("[Graphics] DirectX uniform packing", runDisplayTests())
{
    sf::RenderTexture target({8, 8});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::DirectX)
        SKIP("Requires DirectX");
    constexpr std::string_view fragment = R"(
cbuffer User : register(b1) {
    float scalar; float2 vector2; float3 vector3; float4 vector4;
    int integer; int2 integer2; int3 integer3; int4 integer4;
    bool boolean; bool2 boolean2; bool3 boolean3; bool4 boolean4;
    float numbers[2]; float2 pairs[2]; float3 triples[2]; float4 quads[2];
    row_major float3x3 matrix3; column_major float4x4 matrix4;
    column_major float3x3 matrices3[2]; row_major float4x4 matrices4[2];
};
float4 main() : SV_TARGET {
    bool ok = scalar == 1 && all(vector2 == float2(2,3)) && all(vector3 == float3(4,5,6)) && all(vector4 == float4(7,8,9,10));
    ok = ok && integer == 11 && all(integer2 == int2(12,13)) && all(integer3 == int3(14,15,16)) && all(integer4 == int4(17,18,19,20));
    ok = ok && boolean && all(boolean2 == bool2(true,false)) && all(boolean3 == bool3(false,true,false)) && all(boolean4 == bool4(true,false,true,false));
    ok = ok && numbers[1] == 2 && pairs[1].y == 4 && triples[1].z == 6 && quads[1].w == 8;
    ok = ok && matrix3[0][2] == 0.25 && matrix3[1][2] == 0.5 && matrix4[0][3] == 0.25 && matrix4[1][3] == 0.5;
    ok = ok && matrices3[1][0][2] == 0.25 && matrices4[1][1][3] == 0.5;
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

TEST_CASE("[Graphics] DirectX geometry and samplers", runDisplayTests())
{
    sf::RenderTexture target({16, 16});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::DirectX)
        SKIP("Requires DirectX");
    SECTION("Geometry-only shader with built-in stages")
    {
        constexpr std::string_view source = R"(
struct V { float4 position : SV_POSITION; float4 color : COLOR0; float2 uv : TEXCOORD0; };
[maxvertexcount(4)]
void main(point V input[1], inout TriangleStream<V> output) {
    V v = input[0];
    v.position = float4(-0.5,-0.5,0,1); output.Append(v);
    v.position = float4( 0.5,-0.5,0,1); output.Append(v);
    v.position = float4(-0.5, 0.5,0,1); output.Append(v);
    v.position = float4( 0.5, 0.5,0,1); output.Append(v);
})";
        const sf::Shader           shader(source, sf::Shader::Type::Geometry);
        const sf::Vertex           vertex{{8, 8}, sf::Color::Green};
        target.clear(sf::Color::Red);
        target.draw(&vertex, 1, sf::PrimitiveType::Points, &shader);
        target.display();
        CHECK(target.getTexture().copyToImage().getPixel({8, 8}) == sf::Color::Green);
        CHECK(target.getTexture().copyToImage().getPixel({0, 0}) == sf::Color::Red);
    }
    SECTION("Current texture and additional named texture")
    {
        constexpr std::string_view fragment = R"(
Texture2D current : register(t0); SamplerState currentSampler : register(s0);
Texture2D extra : register(t1); SamplerState extraSampler : register(s1);
float4 main(float4 position : SV_POSITION, float4 color : COLOR0, float2 uv : TEXCOORD0) : SV_TARGET {
    return current.Sample(currentSampler, uv) + extra.Sample(extraSampler, uv);
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
    }
}

TEST_CASE("[Graphics] DirectX partial clear and triangle fans", runDisplayTests())
{
    sf::RenderTexture target({32, 32}, sf::ContextSettings{0, 8, 4});
    if (sf::getGraphicsBackend() != sf::GraphicsBackend::DirectX)
        SKIP("Requires DirectX");
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
}
