#include "Graphics/bind_RenderTexture.hpp"

#include <algorithm>
#include <array>
#include <string_view>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace { constexpr std::array<std::string_view, 37> docs = {
    "\\brief Target for off-screen 2D rendering into a texture",
    "\\brief Default constructor\n\nConstructs a render-texture with width 0 and height 0.\n\n\\see `resize`",
    "\\brief Construct a render-texture\n\nThe last parameter, `settings`, is useful if you want to enable\nmulti-sampling or use the render-texture for OpenGL rendering that\nrequires a depth or stencil buffer. Otherwise it is unnecessary, and\nyou should leave this parameter at its default value.\n\nAfter creation, the contents of the render-texture are undefined.\nCall `RenderTexture::clear` first to ensure a single color fill.\n\n\\param size     Width and height of the render-texture\n\\param settings Additional settings for the underlying OpenGL texture and context\n\n\\throws sf::Exception if creation was unsuccessful",
    "\\brief Clear the entire target with a single color\n\nThis function is usually called once every frame,\nto clear the previous contents of the target.\n\n\\param color Fill color to use to clear the render target",
    "\\brief Clear the stencil buffer to a specific value\n\nThe specified value is truncated to the bit width of\nthe current stencil buffer.\n\n\\param stencilValue Stencil value to clear to",
    "\\brief Clear the entire target with a single color and stencil value\n\nThe specified stencil value is truncated to the bit\nwidth of the current stencil buffer.\n\n\\param color        Fill color to use to clear the render target\n\\param stencilValue Stencil value to clear to",
    "\\brief Change the current active view\n\nThe view is like a 2D camera, it controls which part of\nthe 2D scene is visible, and how it is viewed in the\nrender target.\nThe new view will affect everything that is drawn, until\nanother view is set.\nThe render target keeps its own copy of the view object,\nso it is not necessary to keep the original one alive\nafter calling this function.\nTo restore the original view of the target, you can pass\nthe result of `getDefaultView()` to this function.\n\n\\param view New view to use\n\n\\see `getView`, `getDefaultView`",
    "\\brief Get the view currently in use in the render target\n\n\\return The view object that is currently used\n\n\\see `setView`, `getDefaultView`",
    "\\brief Get the default view of the render target\n\nThe default view has the initial size of the render target,\nand never changes after the target has been created.\n\n\\return The default view of the render target\n\n\\see `setView`, `getView`",
    "\\brief Get the viewport of a view, applied to this render target\n\nThe viewport is defined in the view as a ratio, this function\nsimply applies this ratio to the current dimensions of the\nrender target to calculate the pixels rectangle that the viewport\nactually covers in the target.\n\n\\param view The view for which we want to compute the viewport\n\n\\return Viewport rectangle, expressed in pixels",
    "\\brief Get the scissor rectangle of a view, applied to this render target\n\nThe scissor rectangle is defined in the view as a ratio. This\nfunction simply applies this ratio to the current dimensions\nof the render target to calculate the pixels rectangle\nthat the scissor rectangle actually covers in the target.\n\n\\param view The view for which we want to compute the scissor rectangle\n\n\\return Scissor rectangle, expressed in pixels",
    "\\brief Convert a point from target coordinates to world\ncoordinates, using the current view\n\nThis function is an overload of the mapPixelToCoords\nfunction that implicitly uses the current view.\nIt is equivalent to:\n\\code\ntarget.mapPixelToCoords(point, target.getView());\n\\endcode\n\n\\param point Pixel to convert\n\n\\return The converted point, in \"world\" coordinates\n\n\\see `mapCoordsToPixel`",
    "\\brief Convert a point from target coordinates to world coordinates\n\nThis function finds the 2D position that matches the\ngiven pixel of the render target. In other words, it does\nthe inverse of what the graphics card does, to find the\ninitial position of a rendered pixel.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (10, 50) in your render target may map to the point\n(150, 75) in your 2D world -- if the view is translated by (140, 25).\n\nFor render-windows, this function is typically used to find\nwhich point (or object) is located below the mouse cursor.\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Pixel to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in \"world\" units\n\n\\see `mapCoordsToPixel`",
    "\\brief Convert a point from world coordinates to target\ncoordinates, using the current view\n\nThis function is an overload of the `mapCoordsToPixel`\nfunction that implicitly uses the current view.\nIt is equivalent to:\n\\code\ntarget.mapCoordsToPixel(point, target.getView());\n\\endcode\n\n\\param point Point to convert\n\n\\return The converted point, in target coordinates (pixels)\n\n\\see `mapPixelToCoords`",
    "\\brief Convert a point from world coordinates to target coordinates\n\nThis function finds the pixel of the render target that matches\nthe given 2D point. In other words, it goes through the same process\nas the graphics card, to compute the final position of a rendered point.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (150, 75) in your 2D world may map to the pixel\n(10, 50) of your render target -- if the view is translated by (140, 25).\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Point to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in target coordinates (pixels)\n\n\\see `mapPixelToCoords`",
    "\\brief Draw a drawable object to the render target\n\n\\param drawable Object to draw\n\\param states   Render states to use for drawing",
    "\\brief Draw primitives defined by an array of vertices\n\n\\param vertices    Pointer to the vertices\n\\param vertexCount Number of vertices in the array\n\\param type        Type of primitives to draw\n\\param states      Render states to use for drawing",
    "\\brief Draw primitives defined by a vertex buffer\n\n\\param vertexBuffer Vertex buffer\n\\param states       Render states to use for drawing",
    "\\brief Draw primitives defined by a vertex buffer\n\n\\param vertexBuffer Vertex buffer\n\\param firstVertex  Index of the first vertex to render\n\\param vertexCount  Number of vertices to render\n\\param states       Render states to use for drawing",
    "\\brief Return the size of the rendering region of the target\n\n\\return Size in pixels",
    "\\brief Tell if the render target will use sRGB encoding when drawing on it\n\n\\return `true` if the render target use sRGB encoding, `false` otherwise",
    "\\brief Activate or deactivate the render target for rendering\n\nThis function makes the render target's context current for\nfuture OpenGL rendering operations (so you shouldn't care\nabout it if you're not doing direct OpenGL stuff).\nA render target's context is active only on the current thread,\nif you want to make it active on another thread you have\nto deactivate it on the previous thread first if it was active.\nOnly one context can be current in a thread, so if you\nwant to draw OpenGL geometry to another render target\ndon't forget to activate it again. Activating a render\ntarget will automatically deactivate the previously active\ncontext (if any).\n\n\\param active `true` to activate, `false` to deactivate\n\n\\return `true` if operation was successful, `false` otherwise",
    "\\brief Save the OpenGL render states modified by SFML\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering. Combined with popGLStates,\nit ensures that:\n\\li SFML's internal states are not messed up by your OpenGL code\n\\li your OpenGL states are not modified by a call to a SFML function\n\nMore specifically, it must be used around code that\ncalls `draw` functions. Example:\n\\code\n// OpenGL code here...\nwindow.pushGLStates();\nwindow.draw(...);\nwindow.draw(...);\nwindow.popGLStates();\n// OpenGL code here...\n\\endcode\n\nNote that this function is quite expensive: it saves the\nprogram, textures, vertex attributes and the other OpenGL\nstates that SFML drawing can modify. State outside this set\nis deliberately not covered.\nIt is provided for convenience, but the best results will\nbe achieved if you handle OpenGL states yourself (because\nyou know which states have really changed, and need to be\nsaved and restored). Take a look at the resetGLStates\nfunction if you do so.\n\n\\see `popGLStates`",
    "\\brief Restore the previously saved OpenGL render states\n\nSee the description of `pushGLStates` to get a detailed\ndescription of these functions.\n\n\\see `pushGLStates`",
    "\\brief Reset the internal OpenGL states so that the target is ready for drawing\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering, if you choose not to use\n`pushGLStates`/`popGLStates`. It makes sure that all OpenGL\nstates needed by SFML are set, so that subsequent `draw()`\ncalls will work as expected.\n\nExample:\n\\code\n// OpenGL code here...\nwindow.resetGLStates();\nwindow.draw(...);\nwindow.draw(...);\n// OpenGL code here...\n\\endcode",
    "\\brief Resize the render-texture\n\nThe last parameter, `settings`, is useful if you want to enable\nmulti-sampling or use the render-texture for OpenGL rendering that\nrequires a depth or stencil buffer. Otherwise it is unnecessary, and\nyou should leave this parameter at its default value.\n\nAfter resizing, the contents of the render-texture are undefined.\nCall `RenderTexture::clear` first to ensure a single color fill.\n\n\\param size     Width and height of the render-texture\n\\param settings Additional settings for the underlying OpenGL texture and context\n\n\\return `true` if resizing has been successful, `false` if it failed",
    "\\brief Get the maximum anti-aliasing level supported by the system\n\n\\return The maximum anti-aliasing level supported by the system",
    "\\brief Enable or disable texture smoothing\n\nThis function is similar to `Texture::setSmooth`.\nThis parameter is disabled by default.\n\n\\param smooth `true` to enable smoothing, `false` to disable it\n\n\\see `isSmooth`",
    "\\brief Tell whether the smooth filtering is enabled or not\n\n\\return `true` if texture smoothing is enabled\n\n\\see `setSmooth`",
    "\\brief Enable or disable texture repeating\n\nThis function is similar to `Texture::setRepeated`.\nThis parameter is disabled by default.\n\n\\param repeated `true` to enable repeating, `false` to disable it\n\n\\see `isRepeated`",
    "\\brief Tell whether the texture is repeated or not\n\n\\return `true` if texture is repeated\n\n\\see `setRepeated`",
    "\\brief Generate a mipmap using the current texture data\n\nThis function is similar to `Texture::generateMipmap` and operates\non the texture used as the target for drawing.\nBe aware that any draw operation may modify the base level image data.\nFor this reason, calling this function only makes sense after all\ndrawing is completed and display has been called. Not calling display\nafter subsequent drawing will lead to undefined behavior if a mipmap\nhad been previously generated.\n\n\\return `true` if mipmap generation was successful, `false` if unsuccessful",
    "\\brief Activate or deactivate the render-texture for rendering\n\nThis function makes the render-texture's context current for\nfuture OpenGL rendering operations (so you shouldn't care\nabout it if you're not doing direct OpenGL stuff).\nOnly one context can be current in a thread, so if you\nwant to draw OpenGL geometry to another render target\n(like a RenderWindow) don't forget to activate it again.\n\n\\param active `true` to activate, `false` to deactivate\n\n\\return `true` if operation was successful, `false` otherwise",
    "\\brief Update the contents of the target texture\n\nThis function updates the target texture with what\nhas been drawn so far. Like for windows, calling this\nfunction is mandatory at the end of rendering. Not calling\nit may leave the texture in an undefined state.",
    "\\brief Return the size of the rendering region of the texture\n\nThe returned value is the size that you passed to\nthe create function.\n\n\\return Size in pixels",
    "\\brief Tell if the render-texture will use sRGB encoding when drawing on it\n\nYou can request sRGB encoding for a render-texture\nby having the sRgbCapable flag set for the context parameter of `create()` method\n\n\\return `true` if the render-texture use sRGB encoding, `false` otherwise",
    "\\brief Get a read-only reference to the target texture\n\nAfter drawing to the render-texture and calling Display,\nyou can retrieve the updated texture using this function,\nand draw it using a sprite (for example).\nThe internal `sf::Texture` of a render-texture is always the\nsame instance, so that it is possible to call this function\nonce and keep a reference to the texture even after it is\nmodified.\n\n\\return Const reference to the texture",
}; }

void bind_RenderTexture(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    auto type_sf__RenderTexture = lua_glue::BindClass<sf::RenderTexture>(sf, "RenderTexture");
    lua_glue::BindBase<sf::RenderTexture, sf::RenderTarget>(type_sf__RenderTexture);
    lua_glue::Table table_sf__RenderTexture = sf["RenderTexture"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::RenderTexture>(lua);
    lua_glue::Table native_bases_sf__RenderTexture = lua.create_table();
    native_bases_sf__RenderTexture.add(lua["sf"]["RenderTarget"].get<lua_glue::Table>());
    table_sf__RenderTexture.raw_set("__nativeBases", native_bases_sf__RenderTexture);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.RenderTexture", "sf.RenderTarget");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "new", "fun(size: sf.Vector2u, settings?: sf.ContextSettings): sf.RenderTexture");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "new", "fun(): sf.RenderTexture");
    lua_glue::BindCallable(type_sf__RenderTexture, "new",
        [](sf::Vector2u size, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::RenderTexture>(size, settings);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::ContextSettings{ };
        }}},
        docs[2]
    );
    lua_glue::BindCallable(type_sf__RenderTexture, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::RenderTexture>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "clear", "fun(self: sf.RenderTexture, color: sf.Color, stencilValue: sf.StencilValue)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "clear", "fun(self: sf.RenderTexture, color?: sf.Color)");
    lua_glue::BindCallable(type_sf__RenderTexture, "clear",
        [](sf::RenderTexture& self, sf::Color color, sf::StencilValue stencilValue) {
            static_cast<sf::RenderTarget&>(self).clear(color, stencilValue);
        },
        docs[5]
    );
    lua_glue::BindCallable(type_sf__RenderTexture, "clear",
        [](sf::RenderTexture& self, sf::Color color) {
            static_cast<sf::RenderTarget&>(self).clear(color);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Color>(sf::Color::Black);
        }}},
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "clearStencil", "fun(self: sf.RenderTexture, stencilValue: sf.StencilValue)");
    lua_glue::BindCallable(type_sf__RenderTexture, "clearStencil",
        [](sf::RenderTexture& self, sf::StencilValue stencilValue) {
            static_cast<sf::RenderTarget&>(self).clearStencil(stencilValue);
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "setView", "fun(self: sf.RenderTexture, view: sf.View)");
    lua_glue::BindCallable(type_sf__RenderTexture, "setView",
        [](sf::RenderTexture& self, const sf::View& view) {
            static_cast<sf::RenderTarget&>(self).setView(view);
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getView", "fun(self: sf.RenderTexture): sf.View");
    lua_glue::BindCallable(type_sf__RenderTexture, "getView",
        [](const sf::RenderTexture& self) {
            return std::cref(static_cast<const sf::RenderTarget&>(self).getView());
        },
        docs[7],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getDefaultView", "fun(self: sf.RenderTexture): sf.View");
    lua_glue::BindCallable(type_sf__RenderTexture, "getDefaultView",
        [](const sf::RenderTexture& self) {
            return std::cref(static_cast<const sf::RenderTarget&>(self).getDefaultView());
        },
        docs[8],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getViewport", "fun(self: sf.RenderTexture, view: sf.View): sf.IntRect");
    lua_glue::BindCallable(type_sf__RenderTexture, "getViewport",
        [](const sf::RenderTexture& self, const sf::View& view) -> sf::IntRect {
            return static_cast<const sf::RenderTarget&>(self).getViewport(view);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getScissor", "fun(self: sf.RenderTexture, view: sf.View): sf.IntRect");
    lua_glue::BindCallable(type_sf__RenderTexture, "getScissor",
        [](const sf::RenderTexture& self, const sf::View& view) -> sf::IntRect {
            return static_cast<const sf::RenderTarget&>(self).getScissor(view);
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "mapPixelToCoords", "fun(self: sf.RenderTexture, point: sf.Vector2i, view: sf.View): sf.Vector2f");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "mapPixelToCoords", "fun(self: sf.RenderTexture, point: sf.Vector2i): sf.Vector2f");
    lua_glue::BindCallable(type_sf__RenderTexture, "mapPixelToCoords",
        [](const sf::RenderTexture& self, sf::Vector2i point, const sf::View& view) -> sf::Vector2f {
            return static_cast<const sf::RenderTarget&>(self).mapPixelToCoords(point, view);
        },
        docs[12]
    );
    lua_glue::BindCallable(type_sf__RenderTexture, "mapPixelToCoords",
        [](const sf::RenderTexture& self, sf::Vector2i point) -> sf::Vector2f {
            return static_cast<const sf::RenderTarget&>(self).mapPixelToCoords(point);
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "mapCoordsToPixel", "fun(self: sf.RenderTexture, point: sf.Vector2f, view: sf.View): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "mapCoordsToPixel", "fun(self: sf.RenderTexture, point: sf.Vector2f): sf.Vector2i");
    lua_glue::BindCallable(type_sf__RenderTexture, "mapCoordsToPixel",
        [](const sf::RenderTexture& self, sf::Vector2f point, const sf::View& view) -> sf::Vector2i {
            return static_cast<const sf::RenderTarget&>(self).mapCoordsToPixel(point, view);
        },
        docs[14]
    );
    lua_glue::BindCallable(type_sf__RenderTexture, "mapCoordsToPixel",
        [](const sf::RenderTexture& self, sf::Vector2f point) -> sf::Vector2i {
            return static_cast<const sf::RenderTarget&>(self).mapCoordsToPixel(point);
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer, states?: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, drawable: sf.Drawable, states?: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, vertexBuffer: sf.VertexBuffer, states?: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, vertices: any, type: sf.PrimitiveType, states?: sf.RenderStates)");
    lua_glue::BindCallable(type_sf__RenderTexture, "draw",
        [](sf::RenderTexture& self, const sf::VertexBuffer& vertexBuffer, lua_sf::LuaIntegral<std::size_t> firstVertex, lua_sf::LuaIntegral<std::size_t> vertexCount, const sf::RenderStates& states) {
            static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, firstVertex.value(), vertexCount.value(), states);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::RenderStates>(sf::RenderStates::Default);
        }}},
        docs[18]
    );
    lua_glue::BindCallable(type_sf__RenderTexture, "draw",
        [](sf::RenderTexture& self, const sf::Drawable& drawable, const sf::RenderStates& states) {
            static_cast<sf::RenderTarget&>(self).draw(drawable, states);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::RenderStates>(sf::RenderStates::Default);
        }}},
        docs[15]
    );
    lua_glue::BindCallable(type_sf__RenderTexture, "draw",
        [](sf::RenderTexture& self, const sf::VertexBuffer& vertexBuffer, const sf::RenderStates& states) {
            static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, states);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::RenderStates>(sf::RenderStates::Default);
        }}},
        docs[17]
    );
    lua_glue::BindCallable(type_sf__RenderTexture, "draw",
        [](sf::RenderTexture& self, lua_glue::Object vertices, sf::PrimitiveType type, const sf::RenderStates& states) {
            auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
            static_cast<sf::RenderTarget&>(self).draw(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), type, states);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::RenderStates>(sf::RenderStates::Default);
        }}},
        docs[16]
    );
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getSize", "fun(self: sf.RenderTexture): sf.Vector2u");
    lua_glue::BindCallable(type_sf__RenderTexture, "getSize",
        [](const sf::RenderTexture& self) -> sf::Vector2u {
            return self.getSize();
        },
        docs[34]
    );
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "isSrgb", "fun(self: sf.RenderTexture): boolean");
    lua_glue::BindCallable(type_sf__RenderTexture, "isSrgb",
        [](const sf::RenderTexture& self) -> bool {
            return self.isSrgb();
        },
        docs[35]
    );
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "setActive", "fun(self: sf.RenderTexture, active?: boolean): boolean");
    lua_glue::BindCallable(type_sf__RenderTexture, "setActive",
        [](sf::RenderTexture& self, bool active) -> bool {
            return self.setActive(active);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(true);
        }}},
        docs[32]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "pushGLStates", "fun(self: sf.RenderTexture)");
    lua_glue::BindCallable(type_sf__RenderTexture, "pushGLStates",
        [](sf::RenderTexture& self) {
            static_cast<sf::RenderTarget&>(self).pushGLStates();
        },
        docs[22]
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "popGLStates", "fun(self: sf.RenderTexture)");
    lua_glue::BindCallable(type_sf__RenderTexture, "popGLStates",
        [](sf::RenderTexture& self) {
            static_cast<sf::RenderTarget&>(self).popGLStates();
        },
        docs[23]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "resetGLStates", "fun(self: sf.RenderTexture)");
    lua_glue::BindCallable(type_sf__RenderTexture, "resetGLStates",
        [](sf::RenderTexture& self) {
            static_cast<sf::RenderTarget&>(self).resetGLStates();
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "resize", "fun(self: sf.RenderTexture, size: sf.Vector2u, settings?: sf.ContextSettings): boolean");
    lua_glue::BindCallable(type_sf__RenderTexture, "resize",
        [](sf::RenderTexture& self, sf::Vector2u size, const sf::ContextSettings& settings) -> bool {
            return self.resize(size, settings);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::ContextSettings{ };
        }}},
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getMaximumAntiAliasingLevel", "fun(): integer");
    lua_glue::BindCallable(type_sf__RenderTexture, "getMaximumAntiAliasingLevel",
        []() -> unsigned int {
            return sf::RenderTexture::getMaximumAntiAliasingLevel();
        },
        docs[26]
    );
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "setSmooth", "fun(self: sf.RenderTexture, smooth: boolean)");
    lua_glue::BindCallable(type_sf__RenderTexture, "setSmooth",
        [](sf::RenderTexture& self, bool smooth) {
            self.setSmooth(smooth);
        },
        docs[27]
    );
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "isSmooth", "fun(self: sf.RenderTexture): boolean");
    lua_glue::BindCallable(type_sf__RenderTexture, "isSmooth",
        [](const sf::RenderTexture& self) -> bool {
            return self.isSmooth();
        },
        docs[28]
    );
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "setRepeated", "fun(self: sf.RenderTexture, repeated: boolean)");
    lua_glue::BindCallable(type_sf__RenderTexture, "setRepeated",
        [](sf::RenderTexture& self, bool repeated) {
            self.setRepeated(repeated);
        },
        docs[29]
    );
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "isRepeated", "fun(self: sf.RenderTexture): boolean");
    lua_glue::BindCallable(type_sf__RenderTexture, "isRepeated",
        [](const sf::RenderTexture& self) -> bool {
            return self.isRepeated();
        },
        docs[30]
    );
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "generateMipmap", "fun(self: sf.RenderTexture): boolean");
    lua_glue::BindCallable(type_sf__RenderTexture, "generateMipmap",
        [](sf::RenderTexture& self) -> bool {
            return self.generateMipmap();
        },
        docs[31]
    );
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "display", "fun(self: sf.RenderTexture)");
    lua_glue::BindCallable(type_sf__RenderTexture, "display",
        [](sf::RenderTexture& self) {
            self.display();
        },
        docs[33]
    );
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getTexture", "fun(self: sf.RenderTexture): sf.Texture");
    lua_glue::BindCallable(type_sf__RenderTexture, "getTexture",
        [](const sf::RenderTexture& self) {
            return std::cref(self.getTexture());
        },
        docs[36],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
}
