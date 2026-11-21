////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>

#include <array>
#include <filesystem>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <string_view>

#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace
{
std::random_device rd;
std::mt19937       rng(rd());

// The other backends use translations of the GLSL resources: MSL (.metal), DXBC (.cso) or HLSL (.hlsl), and
// SPIR-V compiled from resources/vulkan with glslang -V --target-env vulkan1.0
sf::GraphicsBackend backend = sf::GraphicsBackend::OpenGL;

std::string shaderFile(const std::string& name)
{
    switch (backend)
    {
        case sf::GraphicsBackend::Metal:
            return "resources/" + name + ".metal";
        case sf::GraphicsBackend::DirectX:
            if (auto bytecode = "resources/" + name + ".cso"; std::filesystem::is_regular_file(bytecode))
                return bytecode;
            return "resources/" + name + ".hlsl";
        case sf::GraphicsBackend::Vulkan:
            return "resources/vulkan/" + name + ".spv";
        default:
            return "resources/" + name;
    }
}

// HLSL and GLSL 4.50 reserve the name texture, so those translations sample sf_Texture, the current texture
void useCurrentTexture(sf::Shader& shader)
{
    if (backend == sf::GraphicsBackend::OpenGL || backend == sf::GraphicsBackend::Metal)
        shader.setUniform("texture", sf::Shader::CurrentTexture);
}


////////////////////////////////////////////////////////////
// Base class for effects
////////////////////////////////////////////////////////////
struct Effect : sf::Drawable
{
    virtual void update(float time, float x, float y) = 0;
};


////////////////////////////////////////////////////////////
// "Pixelate" fragment shader
////////////////////////////////////////////////////////////
class Pixelate : public Effect
{
public:
    explicit Pixelate(sf::Texture&& texture, sf::Shader&& shader) :
        m_texture(std::move(texture)),
        m_shader(std::move(shader))
    {
        useCurrentTexture(m_shader);
    }

    void update(float /* time */, float x, float y) override
    {
        m_shader.setUniform("pixel_threshold", (x + y) / 30);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.shader = &m_shader;
        target.draw(sf::Sprite{m_texture}, states);
    }

private:
    sf::Texture m_texture;
    sf::Shader  m_shader;
};


////////////////////////////////////////////////////////////
// "Wave" vertex shader + "blur" fragment shader
////////////////////////////////////////////////////////////
class WaveBlur : public Effect
{
public:
    void update(float time, float x, float y) override
    {
        m_shader.setUniform("wave_phase", time);
        m_shader.setUniform("wave_amplitude", sf::Vector2f(x * 40, y * 40));
        m_shader.setUniform("blur_radius", (x + y) * 0.008f);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.shader = &m_shader;
        target.draw(m_text, states);
    }

    explicit WaveBlur(const sf::Font& font, sf::Shader&& shader) :
        m_text(font,
               "Praesent suscipit augue in velit pulvinar hendrerit varius purus aliquam.\n"
               "Mauris mi odio, bibendum quis fringilla a, laoreet vel orci. Proin vitae vulputate tortor.\n"
               "Praesent cursus ultrices justo, ut feugiat ante vehicula quis.\n"
               "Donec fringilla scelerisque mauris et viverra.\n"
               "Maecenas adipiscing ornare scelerisque. Nullam at libero elit.\n"
               "Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas.\n"
               "Nullam leo urna, tincidunt id semper eget, ultricies sed mi.\n"
               "Morbi mauris massa, commodo id dignissim vel, lobortis et elit.\n"
               "Fusce vel libero sed neque scelerisque venenatis.\n"
               "Integer mattis tincidunt quam vitae iaculis.\n"
               "Vivamus fringilla sem non velit venenatis fermentum.\n"
               "Vivamus varius tincidunt nisi id vehicula.\n"
               "Integer ullamcorper, enim vitae euismod rutrum, massa nisl semper ipsum,\n"
               "vestibulum sodales sem ante in massa.\n"
               "Vestibulum in augue non felis convallis viverra.\n"
               "Mauris ultricies dolor sed massa convallis sed aliquet augue fringilla.\n"
               "Duis erat eros, porta in accumsan in, blandit quis sem.\n"
               "In hac habitasse platea dictumst. Etiam fringilla est id odio dapibus sit amet semper dui laoreet.\n",
               22),
        m_shader(std::move(shader))
    {
        m_text.setPosition({30.f, 20.f});
    }

private:
    sf::Text   m_text;
    sf::Shader m_shader;
};


////////////////////////////////////////////////////////////
// "Storm" vertex shader + "blink" fragment shader
////////////////////////////////////////////////////////////
class StormBlink : public Effect
{
public:
    void update(float time, float x, float y) override
    {
        const float radius = 200 + std::cos(time) * 150;

        m_shader.setUniform("storm_position", sf::Vector2f(x * 800, y * 600));
        m_shader.setUniform("storm_inner_radius", radius / 3);
        m_shader.setUniform("storm_total_radius", radius);
        m_shader.setUniform("blink_alpha", 0.5f + std::cos(time * 3) * 0.25f);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.shader = &m_shader;
        target.draw(m_points, states);
    }

    explicit StormBlink(sf::Shader&& shader) : m_shader(std::move(shader))
    {
        std::uniform_real_distribution<float>        xDistribution(0, 800);
        std::uniform_real_distribution<float>        yDistribution(0, 600);
        std::uniform_int_distribution<std::uint16_t> colorDistribution(0, 255);

        // Create the points
        m_points.setPrimitiveType(sf::PrimitiveType::Points);

        for (int i = 0; i < 40'000; ++i)
        {
            const auto x = xDistribution(rng);
            const auto y = yDistribution(rng);

            const auto r = static_cast<std::uint8_t>(colorDistribution(rng));
            const auto g = static_cast<std::uint8_t>(colorDistribution(rng));
            const auto b = static_cast<std::uint8_t>(colorDistribution(rng));

            m_points.append({{x, y}, {r, g, b}});
        }
    }

private:
    sf::VertexArray m_points;
    sf::Shader      m_shader;
};


////////////////////////////////////////////////////////////
// "Edge" post-effect fragment shader
////////////////////////////////////////////////////////////
class Edge : public Effect
{
public:
    void update(float time, float x, float y) override
    {
        m_shader.setUniform("edge_threshold", 1 - (x + y) / 2);

        // Render the updated scene to the off-screen surface
        m_surface.clear(sf::Color::White);

        sf::Sprite backgroundSprite{m_backgroundTexture};
        backgroundSprite.setPosition({135.f, 100.f});
        m_surface.draw(backgroundSprite);

        // Update the position of the moving entities
        constexpr int numEntities = 6;

        for (int i = 0; i < 6; ++i)
        {
            sf::Sprite entity{m_entityTexture, sf::IntRect({96 * i, 0}, {96, 96})};

            entity.setPosition(
                {std::cos(0.25f * (time * static_cast<float>(i) + static_cast<float>(numEntities - i))) * 300 + 350,
                 std::sin(0.25f * (time * static_cast<float>(numEntities - i) + static_cast<float>(i))) * 200 + 250});

            m_surface.draw(entity);
        }

        m_surface.display();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        states.shader = &m_shader;
        target.draw(sf::Sprite{m_surface.getTexture()}, states);
    }

    explicit Edge(sf::RenderTexture&& surface, sf::Texture&& backgroundTexture, sf::Texture&& entityTexture, sf::Shader&& shader) :
        m_surface(std::move(surface)),
        m_backgroundTexture(std::move(backgroundTexture)),
        m_entityTexture(std::move(entityTexture)),
        m_shader(std::move(shader))
    {
    }

private:
    sf::RenderTexture m_surface;
    sf::Texture       m_backgroundTexture;
    sf::Texture       m_entityTexture;
    sf::Shader        m_shader;
};


////////////////////////////////////////////////////////////
// "Geometry" geometry shader example
////////////////////////////////////////////////////////////
class Geometry : public Effect
{
public:
    void update(float /* time */, float x, float y) override
    {
        // Reset our transformation matrix
        m_transform = sf::Transform::Identity;

        // Move to the center of the window
        m_transform.translate({400.f, 300.f});

        // Rotate everything based on cursor position
        m_transform.rotate(sf::degrees(x * 360.f));

        // Adjust billboard size to scale between 25 and 75
        const float size = 25 + std::abs(y) * 50;

        // Update the shader parameter
        m_shader.setUniform("size", sf::Vector2f{size, size});
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        // Prepare the render state
        states.shader    = &m_shader;
        states.texture   = &m_logoTexture;
        states.transform = m_transform;

        // Draw the point cloud
        target.draw(m_pointCloud, states);
    }

    explicit Geometry(sf::Texture&& logoTexture, sf::Shader&& shader) :
        m_logoTexture(std::move(logoTexture)),
        m_shader(std::move(shader)),
        m_pointCloud(sf::PrimitiveType::Points, 10'000)
    {
        // Move the points in the point cloud to random positions
        for (std::size_t i = 0; i < 10'000; ++i)
        {
            // Spread the coordinates from -480 to +480 so they'll always fill the viewport at 800x600
            std::uniform_real_distribution<float> positionDistribution(-480, 480);
            m_pointCloud[i].position = {positionDistribution(rng), positionDistribution(rng)};
        }
    }

private:
    sf::Texture     m_logoTexture;
    sf::Transform   m_transform;
    sf::Shader      m_shader;
    sf::VertexArray m_pointCloud;
};


////////////////////////////////////////////////////////////
// Effect loading factory functions
////////////////////////////////////////////////////////////
std::optional<Pixelate> tryLoadPixelate()
{
    sf::Texture texture;
    if (!texture.loadFromFile("resources/background.jpg"))
        return std::nullopt;

    sf::Shader shader;
    if (!shader.loadFromFile(shaderFile("pixelate.frag"), sf::Shader::Type::Fragment))
        return std::nullopt;

    return std::make_optional<Pixelate>(std::move(texture), std::move(shader));
}

std::optional<WaveBlur> tryLoadWaveBlur(const sf::Font& font)
{
    sf::Shader shader;
    if (!shader.loadFromFile(shaderFile("wave.vert"), shaderFile("blur.frag")))
        return std::nullopt;

    return std::make_optional<WaveBlur>(font, std::move(shader));
}

std::optional<StormBlink> tryLoadStormBlink()
{
    sf::Shader shader;
    if (!shader.loadFromFile(shaderFile("storm.vert"), shaderFile("blink.frag")))
        return std::nullopt;

    return std::make_optional<StormBlink>(std::move(shader));
}

std::optional<Edge> tryLoadEdge()
{
    // Create the off-screen surface
    sf::RenderTexture surface;
    if (!surface.resize({800, 600}))
        return std::nullopt;

    surface.setSmooth(true);

    // Load the background texture
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("resources/sfml.png"))
        return std::nullopt;

    backgroundTexture.setSmooth(true);

    // Load the entity texture
    sf::Texture entityTexture;
    if (!entityTexture.loadFromFile("resources/devices.png"))
        return std::nullopt;

    entityTexture.setSmooth(true);

    // Load the shader
    sf::Shader shader;
    if (!shader.loadFromFile(shaderFile("edge.frag"), sf::Shader::Type::Fragment))
        return std::nullopt;

    useCurrentTexture(shader);

    return std::make_optional<Edge>(std::move(surface), std::move(backgroundTexture), std::move(entityTexture), std::move(shader));
}

std::optional<Geometry> tryLoadGeometry()
{
    // Check if geometry shaders are supported; Metal draws the billboards as point sprites instead
    const bool pointSprites = backend == sf::GraphicsBackend::Metal;
    if (!pointSprites && !sf::Shader::isGeometryAvailable())
        return std::nullopt;

    // Load the logo texture
    sf::Texture logoTexture;
    if (!logoTexture.loadFromFile("resources/logo.png"))
        return std::nullopt;

    logoTexture.setSmooth(true);

    // Load the shader
    sf::Shader shader;
    if (pointSprites
            ? !shader.loadFromFile(shaderFile("billboard.vert"), shaderFile("billboard.frag"))
            : !shader.loadFromFile(shaderFile("billboard.vert"), shaderFile("billboard.geom"), shaderFile("billboard.frag")))
        return std::nullopt;

    useCurrentTexture(shader);

    // Set the render resolution (used for proper scaling); point sprites are sized in pixels
    if (!pointSprites)
        shader.setUniform("resolution", sf::Vector2f(800, 600));

    return std::make_optional<Geometry>(std::move(logoTexture), std::move(shader));
}

} // namespace


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // Without an option, Metal is preferred when it is built and OpenGL is used otherwise
    const std::string_view backendName = (argc == 3 && std::string_view{argv[1]} == "--backend") ? argv[2] : "";
    if (backendName == "metal")
        backend = sf::GraphicsBackend::Metal;
    else if (backendName == "directx")
        backend = sf::GraphicsBackend::DirectX;
    else if (backendName == "vulkan")
        backend = sf::GraphicsBackend::Vulkan;
    else if (backendName == "opengl")
        backend = sf::GraphicsBackend::OpenGL;
    else if (argc != 1)
    {
        std::cerr << "Usage: shader [--backend opengl|metal|directx|vulkan]" << std::endl;
        return EXIT_FAILURE;
    }
#ifdef SFML_RENDER_METAL
    if (argc == 1)
        backend = sf::GraphicsBackend::Metal;
#endif
    const std::string backendTitle = backend == sf::GraphicsBackend::Metal     ? "Metal"
                                     : backend == sf::GraphicsBackend::DirectX ? "DirectX"
                                     : backend == sf::GraphicsBackend::Vulkan  ? "Vulkan"
                                                                               : "OpenGL";

    // The GLSL resources use desktop compatibility OpenGL
    if (backend == sf::GraphicsBackend::OpenGL
            ? !sf::setGraphicsBackend(backend, sf::OpenGLSettings{2, 0, sf::OpenGLProfile::Compatibility})
            : !sf::setGraphicsBackend(backend))
    {
        std::cerr << "The " << backendTitle << " backend is not available" << std::endl;
        return EXIT_FAILURE;
    }

    // Exit early if shaders are not available
    if (!sf::Shader::isAvailable())
    {
        std::cerr << "Shaders not supported on current system, aborting" << std::endl;
        return EXIT_FAILURE;
    }

    // Create the main window
    sf::RenderWindow window(sf::VideoMode({800, 600}),
                            "SFML Shader (" + backendTitle + ")",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    // Open the application font
    const sf::Font font("resources/tuffy.ttf");

    // Create the effects
    std::optional pixelateEffect   = tryLoadPixelate();
    std::optional waveBlurEffect   = tryLoadWaveBlur(font);
    std::optional stormBlinkEffect = tryLoadStormBlink();
    std::optional edgeEffect       = tryLoadEdge();
    std::optional geometryEffect   = tryLoadGeometry();

    const auto optionalToPtr = [](auto& effect) -> Effect* { return effect.has_value() ? &*effect : nullptr; };

    const std::array<Effect*, 5> effects{optionalToPtr(pixelateEffect),
                                         optionalToPtr(waveBlurEffect),
                                         optionalToPtr(stormBlinkEffect),
                                         optionalToPtr(edgeEffect),
                                         optionalToPtr(geometryEffect)};

    const std::array<std::string, 5>
        effectNames{"Pixelate",
                    "Wave + Blur",
                    "Storm + Blink",
                    "Edge Post-effect",
                    backend == sf::GraphicsBackend::Metal ? "Point Sprite Billboards" : "Geometry Shader Billboards"};

    // Index of currently selected effect
    std::size_t current = 0;

    // Create the messages background
    const sf::Texture textBackgroundTexture("resources/text-background.png");
    sf::Sprite        textBackground(textBackgroundTexture);
    textBackground.setPosition({0.f, 520.f});
    textBackground.setColor(sf::Color(255, 255, 255, 200));

    // Create the description text
    sf::Text description(font, "Current effect: " + effectNames[current], 20);
    description.setPosition({10.f, 530.f});
    description.setFillColor(sf::Color(80, 80, 80));

    // Create the instructions text
    sf::Text instructions(font, "Press left and right arrows to change the current shader", 20);
    instructions.setPosition({280.f, 555.f});
    instructions.setFillColor(sf::Color(80, 80, 80));

    // Start the game loop
    const sf::Clock clock;
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                break;
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                switch (keyPressed->code)
                {
                    // Escape key: exit
                    case sf::Keyboard::Key::Escape:
                    {
                        window.close();
                        break;
                    }

                    // Left arrow key: previous shader
                    case sf::Keyboard::Key::Left:
                    {
                        if (current == 0)
                            current = effects.size() - 1;
                        else
                            --current;

                        break;
                    }

                    // Right arrow key: next shader
                    case sf::Keyboard::Key::Right:
                    {
                        if (current == effects.size() - 1)
                            current = 0;
                        else
                            ++current;

                        break;
                    }

                    default:
                        break;
                }

                description.setString("Current effect: " + effectNames[current]);
            }
        }

        // If the current example was loaded successfully...
        if (Effect* currentEffect = effects[current])
        {
            // Update the current example
            const auto [x,
                        y] = sf::Vector2f(sf::Mouse::getPosition(window)).componentWiseDiv(sf::Vector2f(window.getSize()));
            currentEffect->update(clock.getElapsedTime().asSeconds(), x, y);

            // Clear the window
            window.clear(currentEffect == &*edgeEffect ? sf::Color::White : sf::Color(50, 50, 50));

            // Draw the current example
            window.draw(*currentEffect);
        }
        else
        {
            // Clear the window to grey to make sure the text is always readable
            window.clear(sf::Color(50, 50, 50));

            sf::Text error(font, "Shader not\nsupported", 36);
            error.setPosition({320.f, 200.f});

            window.draw(error);
        }

        // Draw the text
        window.draw(textBackground);
        window.draw(instructions);
        window.draw(description);

        // Finally, display the rendered frame on screen
        window.display();
    }
}
