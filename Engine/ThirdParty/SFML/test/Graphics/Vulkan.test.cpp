#include <SFML/Graphics/GraphicsBackend.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RHI/Device.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <GraphicsUtil.hpp>
#include <WindowUtil.hpp>
#include <algorithm>
#include <array>
#include <utility>
#include <vector>

#include <cstdint>

TEST_CASE("[Graphics] Vulkan keeps submitted draws and their descriptor contents ordered", runDisplayTests())
{
    const auto device = sf::rhi::getDevice();
    if (!device || device->backend() != sf::GraphicsBackend::Vulkan)
        SKIP("Requires Vulkan");

    sf::RenderTexture  source({16, 16}, sf::ContextSettings{0, 8, 4});
    sf::RenderTexture  target({32, 16});
    sf::RectangleShape tile({8, 16});
    sf::Shader         shader;
    REQUIRE(shader.loadFromFile("shader-vulkan.frag.spv", sf::Shader::Type::Fragment));
    const sf::Sprite first(source.getTexture());
    sf::Sprite       second(source.getTexture());
    second.setPosition({16, 0});

    // Alternate attachment and sampled use, including across submissions. A later
    // draw changes a uniform while earlier commands still refer to its old value.
    for (unsigned int frame = 0; frame < 4; ++frame)
    {
        const auto left = frame % 2 ? sf::Color::Blue : sf::Color::Red;
        source.clear(sf::Color::Green);
        tile.setFillColor(left);
        source.draw(tile);
        source.display();
        target.clear();
        target.draw(first);
        REQUIRE(device->submit(false));

        source.clear();
        const std::array colors{sf::Glsl::Vec4(0, 0, 1, 1), sf::Glsl::Vec4(0, 1, 0, 1)};
        shader.setUniformArray("shades", colors.data(), colors.size());
        tile.setPosition({8, 0});
        source.draw(tile, &shader);
        shader.setUniform("shades", sf::Glsl::Vec4(1, 0, 0, 1));
        tile.setPosition({0, 0});
        source.draw(tile, &shader);
        source.display();
        target.draw(second);
        target.display();
        REQUIRE(device->submit(false));
        REQUIRE(device->submit(true));

        const auto pixels = target.getTexture().copyToImage();
        REQUIRE(pixels.getSize() == sf::Vector2u(32, 16));
        CHECK(pixels.getPixel({4, 8}) == left);
        CHECK(pixels.getPixel({12, 8}) == sf::Color::Green);
        CHECK(pixels.getPixel({20, 8}) == sf::Color::Yellow);
        CHECK(pixels.getPixel({28, 8}) == sf::Color::Cyan);
    }
}

TEST_CASE("[Graphics] Vulkan buffer writes preserve earlier submitted draws", runDisplayTests())
{
    const auto device = sf::rhi::getDevice();
    if (!device || device->backend() != sf::GraphicsBackend::Vulkan)
        SKIP("Requires Vulkan");

    sf::RenderTexture target({16, 8});
    const auto        renderTarget = sf::rhi::getRenderTarget(target);
    const auto        state        = sf::rhi::getTargetState(target);
    std::array        vertices{sf::Vertex{{0, 0}, sf::Color::Red},
                        sf::Vertex{{0, 8}, sf::Color::Red},
                        sf::Vertex{{8, 0}, sf::Color::Red},
                        sf::Vertex{{8, 8}, sf::Color::Red}};
    const auto        buffer = device->createBuffer(sizeof(vertices));
    REQUIRE(buffer);
    REQUIRE(buffer->upload(vertices.data(), sizeof(vertices)));
    target.clear();
    renderTarget->commands().draw(state, *buffer, 0, vertices.size(), sf::PrimitiveType::TriangleStrip);
    REQUIRE(device->submit(false));
    for (auto& vertex : vertices)
    {
        vertex.position.x += 8;
        vertex.color = sf::Color::Green;
    }
    REQUIRE(buffer->upload(vertices.data(), sizeof(vertices)));
    renderTarget->commands().draw(state, *buffer, 0, vertices.size(), sf::PrimitiveType::TriangleStrip);
    target.display();
    const auto pixels = target.getTexture().copyToImage();
    CHECK(pixels.getPixel({4, 4}) == sf::Color::Red);
    CHECK(pixels.getPixel({12, 4}) == sf::Color::Green);
}

TEST_CASE("[Graphics] Vulkan preserves pending texture uploads when growing a glyph atlas", runDisplayTests())
{
    const auto device = sf::rhi::getDevice();
    if (!device || device->backend() != sf::GraphicsBackend::Vulkan)
        SKIP("Requires Vulkan");

    const sf::Image   first({38, 39}, sf::Color::Red);
    const bool        sampleBeforeUpload = GENERATE(false, true);
    sf::RenderTexture target({256, 256});

    const auto matchesPatch = [](const sf::Image& pixels, const sf::Image& patch, sf::Vector2u origin)
    {
        if (origin.x + patch.getSize().x > pixels.getSize().x || origin.y + patch.getSize().y > pixels.getSize().y)
            return false;
        for (unsigned int y = 0; y < patch.getSize().y; ++y)
        {
            const auto* row    = patch.getPixelsPtr() + 4 * y * patch.getSize().x;
            const auto* actual = pixels.getPixelsPtr() + 4 * ((origin.y + y) * pixels.getSize().x + origin.x);
            if (!std::equal(row, row + 4 * patch.getSize().x, actual))
                return false;
        }
        return true;
    };

    struct Page
    {
        sf::Texture old;
        sf::Texture grown;
        sf::Image   second;
    };
    for (unsigned int batch = 0; batch < 6; ++batch)
    {
        // Record the patch writes and copies for several pages before any CPU readback.
        std::vector<Page> pages;
        for (unsigned int i = 0; i < 8; ++i)
        {
            sf::Image second({38, 38}, sf::Color::Transparent);
            for (unsigned int y = 2; y < 36; ++y)
                for (unsigned int x = 2; x < 36; ++x)
                    second.setPixel({x, y},
                                    {255, 255, 255, static_cast<std::uint8_t>(1 + (3 * x + 7 * y + 8 * batch + i) % 255)});

            sf::Texture old(sf::Image({128, 128}, sf::Color::Transparent));
            old.update(first, {35, 70});
            if (sampleBeforeUpload)
                target.draw(sf::Sprite(old), sf::BlendNone);
            old.update(second, {73, 70});
            sf::Texture grown(sf::Vector2u{256, 256});
            grown.update(old);
            grown.update(first, {0, 146});
            pages.push_back({std::move(old), std::move(grown), std::move(second)});
        }
        for (const auto& page : pages)
        {
            const sf::Sprite glyph(page.grown, {{73, 70}, sf::Vector2i(page.second.getSize())});
            target.draw(glyph, sf::BlendNone);
            target.display();
            const auto pixels = page.grown.copyToImage();
            REQUIRE(matchesPatch(page.old.copyToImage(), page.second, {73, 70}));
            REQUIRE(matchesPatch(pixels, first, {35, 70}));
            REQUIRE(matchesPatch(pixels, page.second, {73, 70}));
            REQUIRE(matchesPatch(pixels, first, {0, 146}));
            REQUIRE(matchesPatch(target.getTexture().copyToImage(), page.second, {}));
        }
    }
}
