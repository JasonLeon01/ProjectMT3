#include "Graphics/bind_RenderTexture.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_RenderTexture(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    auto type_sf__RenderTexture = sf.new_usertype<sf::RenderTexture>("RenderTexture",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::RenderTarget>()
    );
    sol::table table_sf__RenderTexture = sf["RenderTexture"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::RenderTexture>(lua);
    sol::table native_bases_sf__RenderTexture = lua.create_table();
    native_bases_sf__RenderTexture.add(lua["sf"]["RenderTarget"].get<sol::table>());
    table_sf__RenderTexture.raw_set("__nativeBases", native_bases_sf__RenderTexture);
    LUASF_STUB_DOC("\\brief Target for off-screen 2D rendering into a texture");
    LUASF_STUB_CLASS("sf.RenderTexture", "sf.RenderTarget");
    LUASF_STUB_DOC("\\brief Construct a render-texture\n\nThe last parameter, `settings`, is useful if you want to enable\nmulti-sampling or use the render-texture for OpenGL rendering that\nrequires a depth or stencil buffer. Otherwise it is unnecessary, and\nyou should leave this parameter at its default value.\n\nAfter creation, the contents of the render-texture are undefined.\nCall `RenderTexture::clear` first to ensure a single color fill.\n\n\\param size     Width and height of the render-texture\n\\param settings Additional settings for the underlying OpenGL texture and context\n\n\\throws sf::Exception if creation was unsuccessful");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "new", "fun(size: sf.Vector2u, settings: sf.ContextSettings): sf.RenderTexture");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "new", "fun(size: sf.Vector2u): sf.RenderTexture");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "new", "fun(): sf.RenderTexture");
    type_sf__RenderTexture.set_function("new", sol::factories(
        [](sf::Vector2u size, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::RenderTexture>(size, settings);
        },
        [](sf::Vector2u size) {
            return lua_sf::makeLuaSharedObject<sf::RenderTexture>(size);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::RenderTexture>();
        }
    ));
    LUASF_STUB_DOC("\\brief Clear the entire target with a single color and stencil value\n\nThe specified stencil value is truncated to the bit\nwidth of the current stencil buffer.\n\n\\param color        Fill color to use to clear the render target\n\\param stencilValue Stencil value to clear to");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "clear", "fun(self: sf.RenderTexture, color: sf.Color, stencilValue: sf.StencilValue)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "clear", "fun(self: sf.RenderTexture, color: sf.Color)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "clear", "fun(self: sf.RenderTexture)");
    type_sf__RenderTexture.set_function("clear",
        sol::overload(
            [](sf::RenderTexture& self, sf::Color color, sf::StencilValue stencilValue) {
                static_cast<sf::RenderTarget&>(self).clear(color, stencilValue);
            },
            [](sf::RenderTexture& self, sf::Color color) {
                static_cast<sf::RenderTarget&>(self).clear(color);
            },
            [](sf::RenderTexture& self) {
                static_cast<sf::RenderTarget&>(self).clear();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Clear the stencil buffer to a specific value\n\nThe specified value is truncated to the bit width of\nthe current stencil buffer.\n\n\\param stencilValue Stencil value to clear to");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "clearStencil", "fun(self: sf.RenderTexture, stencilValue: sf.StencilValue)");
    type_sf__RenderTexture.set_function("clearStencil",
        [](sf::RenderTexture& self, sf::StencilValue stencilValue) {
            static_cast<sf::RenderTarget&>(self).clearStencil(stencilValue);
        }
    );
    LUASF_STUB_DOC("\\brief Change the current active view\n\nThe view is like a 2D camera, it controls which part of\nthe 2D scene is visible, and how it is viewed in the\nrender target.\nThe new view will affect everything that is drawn, until\nanother view is set.\nThe render target keeps its own copy of the view object,\nso it is not necessary to keep the original one alive\nafter calling this function.\nTo restore the original view of the target, you can pass\nthe result of `getDefaultView()` to this function.\n\n\\param view New view to use\n\n\\see `getView`, `getDefaultView`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "setView", "fun(self: sf.RenderTexture, view: sf.View)");
    type_sf__RenderTexture.set_function("setView",
        [](sf::RenderTexture& self, const sf::View& view) {
            static_cast<sf::RenderTarget&>(self).setView(view);
        }
    );
    LUASF_STUB_DOC("\\brief Get the view currently in use in the render target\n\n\\return The view object that is currently used\n\n\\see `setView`, `getDefaultView`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getView", "fun(self: sf.RenderTexture): sf.View");
    type_sf__RenderTexture.set_function("getView",
        sol::policies(
            [](sf::RenderTexture& self) {
                return std::cref(static_cast<sf::RenderTarget&>(self).getView());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the default view of the render target\n\nThe default view has the initial size of the render target,\nand never changes after the target has been created.\n\n\\return The default view of the render target\n\n\\see `setView`, `getView`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getDefaultView", "fun(self: sf.RenderTexture): sf.View");
    type_sf__RenderTexture.set_function("getDefaultView",
        sol::policies(
            [](sf::RenderTexture& self) {
                return std::cref(static_cast<sf::RenderTarget&>(self).getDefaultView());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the viewport of a view, applied to this render target\n\nThe viewport is defined in the view as a ratio, this function\nsimply applies this ratio to the current dimensions of the\nrender target to calculate the pixels rectangle that the viewport\nactually covers in the target.\n\n\\param view The view for which we want to compute the viewport\n\n\\return Viewport rectangle, expressed in pixels");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getViewport", "fun(self: sf.RenderTexture, view: sf.View): sf.IntRect");
    type_sf__RenderTexture.set_function("getViewport",
        [](sf::RenderTexture& self, const sf::View& view) -> sf::IntRect {
            return static_cast<sf::RenderTarget&>(self).getViewport(view);
        }
    );
    LUASF_STUB_DOC("\\brief Get the scissor rectangle of a view, applied to this render target\n\nThe scissor rectangle is defined in the view as a ratio. This\nfunction simply applies this ratio to the current dimensions\nof the render target to calculate the pixels rectangle\nthat the scissor rectangle actually covers in the target.\n\n\\param view The view for which we want to compute the scissor rectangle\n\n\\return Scissor rectangle, expressed in pixels");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getScissor", "fun(self: sf.RenderTexture, view: sf.View): sf.IntRect");
    type_sf__RenderTexture.set_function("getScissor",
        [](sf::RenderTexture& self, const sf::View& view) -> sf::IntRect {
            return static_cast<sf::RenderTarget&>(self).getScissor(view);
        }
    );
    LUASF_STUB_DOC("\\brief Convert a point from target coordinates to world coordinates\n\nThis function finds the 2D position that matches the\ngiven pixel of the render target. In other words, it does\nthe inverse of what the graphics card does, to find the\ninitial position of a rendered pixel.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (10, 50) in your render target may map to the point\n(150, 75) in your 2D world -- if the view is translated by (140, 25).\n\nFor render-windows, this function is typically used to find\nwhich point (or object) is located below the mouse cursor.\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Pixel to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in \"world\" units\n\n\\see `mapCoordsToPixel`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "mapPixelToCoords", "fun(self: sf.RenderTexture, point: sf.Vector2i, view: sf.View): sf.Vector2f");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "mapPixelToCoords", "fun(self: sf.RenderTexture, point: sf.Vector2i): sf.Vector2f");
    type_sf__RenderTexture.set_function("mapPixelToCoords",
        sol::overload(
            [](sf::RenderTexture& self, sf::Vector2i point, const sf::View& view) -> sf::Vector2f {
                return static_cast<sf::RenderTarget&>(self).mapPixelToCoords(point, view);
            },
            [](sf::RenderTexture& self, sf::Vector2i point) -> sf::Vector2f {
                return static_cast<sf::RenderTarget&>(self).mapPixelToCoords(point);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Convert a point from world coordinates to target coordinates\n\nThis function finds the pixel of the render target that matches\nthe given 2D point. In other words, it goes through the same process\nas the graphics card, to compute the final position of a rendered point.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (150, 75) in your 2D world may map to the pixel\n(10, 50) of your render target -- if the view is translated by (140, 25).\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Point to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in target coordinates (pixels)\n\n\\see `mapPixelToCoords`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "mapCoordsToPixel", "fun(self: sf.RenderTexture, point: sf.Vector2f, view: sf.View): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "mapCoordsToPixel", "fun(self: sf.RenderTexture, point: sf.Vector2f): sf.Vector2i");
    type_sf__RenderTexture.set_function("mapCoordsToPixel",
        sol::overload(
            [](sf::RenderTexture& self, sf::Vector2f point, const sf::View& view) -> sf::Vector2i {
                return static_cast<sf::RenderTarget&>(self).mapCoordsToPixel(point, view);
            },
            [](sf::RenderTexture& self, sf::Vector2f point) -> sf::Vector2i {
                return static_cast<sf::RenderTarget&>(self).mapCoordsToPixel(point);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Draw primitives defined by a vertex buffer\n\n\\param vertexBuffer Vertex buffer\n\\param firstVertex  Index of the first vertex to render\n\\param vertexCount  Number of vertices to render\n\\param states       Render states to use for drawing");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, drawable: sf.Drawable, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, vertexBuffer: sf.VertexBuffer, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, drawable: sf.Drawable)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, vertexBuffer: sf.VertexBuffer)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, vertices: any, type: sf.PrimitiveType, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "draw", "fun(self: sf.RenderTexture, vertices: any, type: sf.PrimitiveType)");
    type_sf__RenderTexture.set_function("draw",
        sol::overload(
            [](sf::RenderTexture& self, const sf::VertexBuffer& vertexBuffer, lua_sf::LuaIntegral<std::size_t> firstVertex, lua_sf::LuaIntegral<std::size_t> vertexCount, const sf::RenderStates& states) {
                static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, firstVertex.value(), vertexCount.value(), states);
            },
            [](sf::RenderTexture& self, const sf::VertexBuffer& vertexBuffer, lua_sf::LuaIntegral<std::size_t> firstVertex, lua_sf::LuaIntegral<std::size_t> vertexCount) {
                static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, firstVertex.value(), vertexCount.value());
            },
            [](sf::RenderTexture& self, const sf::Drawable& drawable, const sf::RenderStates& states) {
                static_cast<sf::RenderTarget&>(self).draw(drawable, states);
            },
            [](sf::RenderTexture& self, const sf::VertexBuffer& vertexBuffer, const sf::RenderStates& states) {
                static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, states);
            },
            [](sf::RenderTexture& self, const sf::Drawable& drawable) {
                static_cast<sf::RenderTarget&>(self).draw(drawable);
            },
            [](sf::RenderTexture& self, const sf::VertexBuffer& vertexBuffer) {
                static_cast<sf::RenderTarget&>(self).draw(vertexBuffer);
            },
            [](sf::RenderTexture& self, sol::object vertices, sf::PrimitiveType type, const sf::RenderStates& states) {
                auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
                static_cast<sf::RenderTarget&>(self).draw(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), type, states);
            },
            [](sf::RenderTexture& self, sol::object vertices, sf::PrimitiveType type) {
                auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
                static_cast<sf::RenderTarget&>(self).draw(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), type);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Return the size of the rendering region of the texture\n\nThe returned value is the size that you passed to\nthe create function.\n\n\\return Size in pixels");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getSize", "fun(self: sf.RenderTexture): sf.Vector2u");
    type_sf__RenderTexture.set_function("getSize",
        [](sf::RenderTexture& self) -> sf::Vector2u {
            return self.getSize();
        }
    );
    LUASF_STUB_DOC("\\brief Tell if the render-texture will use sRGB encoding when drawing on it\n\nYou can request sRGB encoding for a render-texture\nby having the sRgbCapable flag set for the context parameter of `create()` method\n\n\\return `true` if the render-texture use sRGB encoding, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "isSrgb", "fun(self: sf.RenderTexture): boolean");
    type_sf__RenderTexture.set_function("isSrgb",
        [](sf::RenderTexture& self) -> bool {
            return self.isSrgb();
        }
    );
    LUASF_STUB_DOC("\\brief Activate or deactivate the render-texture for rendering\n\nThis function makes the render-texture's context current for\nfuture OpenGL rendering operations (so you shouldn't care\nabout it if you're not doing direct OpenGL stuff).\nOnly one context can be current in a thread, so if you\nwant to draw OpenGL geometry to another render target\n(like a RenderWindow) don't forget to activate it again.\n\n\\param active `true` to activate, `false` to deactivate\n\n\\return `true` if operation was successful, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "setActive", "fun(self: sf.RenderTexture, active: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "setActive", "fun(self: sf.RenderTexture): boolean");
    type_sf__RenderTexture.set_function("setActive",
        sol::overload(
            [](sf::RenderTexture& self, bool active) -> bool {
                return self.setActive(active);
            },
            [](sf::RenderTexture& self) -> bool {
                return self.setActive();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Save the OpenGL render states modified by SFML\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering. Combined with popGLStates,\nit ensures that:\n\\li SFML's internal states are not messed up by your OpenGL code\n\\li your OpenGL states are not modified by a call to a SFML function\n\nMore specifically, it must be used around code that\ncalls `draw` functions. Example:\n\\code\n// OpenGL code here...\nwindow.pushGLStates();\nwindow.draw(...);\nwindow.draw(...);\nwindow.popGLStates();\n// OpenGL code here...\n\\endcode\n\nNote that this function is quite expensive: it saves the\nprogram, textures, vertex attributes and the other OpenGL\nstates that SFML drawing can modify. State outside this set\nis deliberately not covered.\nIt is provided for convenience, but the best results will\nbe achieved if you handle OpenGL states yourself (because\nyou know which states have really changed, and need to be\nsaved and restored). Take a look at the resetGLStates\nfunction if you do so.\n\n\\see `popGLStates`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "pushGLStates", "fun(self: sf.RenderTexture)");
    type_sf__RenderTexture.set_function("pushGLStates",
        [](sf::RenderTexture& self) {
            static_cast<sf::RenderTarget&>(self).pushGLStates();
        }
    );
    LUASF_STUB_DOC("\\brief Restore the previously saved OpenGL render states\n\nSee the description of `pushGLStates` to get a detailed\ndescription of these functions.\n\n\\see `pushGLStates`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "popGLStates", "fun(self: sf.RenderTexture)");
    type_sf__RenderTexture.set_function("popGLStates",
        [](sf::RenderTexture& self) {
            static_cast<sf::RenderTarget&>(self).popGLStates();
        }
    );
    LUASF_STUB_DOC("\\brief Reset the internal OpenGL states so that the target is ready for drawing\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering, if you choose not to use\n`pushGLStates`/`popGLStates`. It makes sure that all OpenGL\nstates needed by SFML are set, so that subsequent `draw()`\ncalls will work as expected.\n\nExample:\n\\code\n// OpenGL code here...\nwindow.resetGLStates();\nwindow.draw(...);\nwindow.draw(...);\n// OpenGL code here...\n\\endcode");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "resetGLStates", "fun(self: sf.RenderTexture)");
    type_sf__RenderTexture.set_function("resetGLStates",
        [](sf::RenderTexture& self) {
            static_cast<sf::RenderTarget&>(self).resetGLStates();
        }
    );
    LUASF_STUB_DOC("\\brief Resize the render-texture\n\nThe last parameter, `settings`, is useful if you want to enable\nmulti-sampling or use the render-texture for OpenGL rendering that\nrequires a depth or stencil buffer. Otherwise it is unnecessary, and\nyou should leave this parameter at its default value.\n\nAfter resizing, the contents of the render-texture are undefined.\nCall `RenderTexture::clear` first to ensure a single color fill.\n\n\\param size     Width and height of the render-texture\n\\param settings Additional settings for the underlying OpenGL texture and context\n\n\\return `true` if resizing has been successful, `false` if it failed");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "resize", "fun(self: sf.RenderTexture, size: sf.Vector2u, settings: sf.ContextSettings): boolean");
    LUASF_STUB_OVERLOAD("sf.RenderTexture", "resize", "fun(self: sf.RenderTexture, size: sf.Vector2u): boolean");
    type_sf__RenderTexture.set_function("resize",
        sol::overload(
            [](sf::RenderTexture& self, sf::Vector2u size, const sf::ContextSettings& settings) -> bool {
                return self.resize(size, settings);
            },
            [](sf::RenderTexture& self, sf::Vector2u size) -> bool {
                return self.resize(size);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the maximum anti-aliasing level supported by the system\n\n\\return The maximum anti-aliasing level supported by the system");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getMaximumAntiAliasingLevel", "fun(): integer");
    type_sf__RenderTexture.set_function("getMaximumAntiAliasingLevel",
        []() -> unsigned int {
            return sf::RenderTexture::getMaximumAntiAliasingLevel();
        }
    );
    LUASF_STUB_DOC("\\brief Enable or disable texture smoothing\n\nThis function is similar to `Texture::setSmooth`.\nThis parameter is disabled by default.\n\n\\param smooth `true` to enable smoothing, `false` to disable it\n\n\\see `isSmooth`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "setSmooth", "fun(self: sf.RenderTexture, smooth: boolean)");
    type_sf__RenderTexture.set_function("setSmooth",
        [](sf::RenderTexture& self, bool smooth) {
            self.setSmooth(smooth);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the smooth filtering is enabled or not\n\n\\return `true` if texture smoothing is enabled\n\n\\see `setSmooth`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "isSmooth", "fun(self: sf.RenderTexture): boolean");
    type_sf__RenderTexture.set_function("isSmooth",
        [](sf::RenderTexture& self) -> bool {
            return self.isSmooth();
        }
    );
    LUASF_STUB_DOC("\\brief Enable or disable texture repeating\n\nThis function is similar to `Texture::setRepeated`.\nThis parameter is disabled by default.\n\n\\param repeated `true` to enable repeating, `false` to disable it\n\n\\see `isRepeated`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "setRepeated", "fun(self: sf.RenderTexture, repeated: boolean)");
    type_sf__RenderTexture.set_function("setRepeated",
        [](sf::RenderTexture& self, bool repeated) {
            self.setRepeated(repeated);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the texture is repeated or not\n\n\\return `true` if texture is repeated\n\n\\see `setRepeated`");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "isRepeated", "fun(self: sf.RenderTexture): boolean");
    type_sf__RenderTexture.set_function("isRepeated",
        [](sf::RenderTexture& self) -> bool {
            return self.isRepeated();
        }
    );
    LUASF_STUB_DOC("\\brief Generate a mipmap using the current texture data\n\nThis function is similar to `Texture::generateMipmap` and operates\non the texture used as the target for drawing.\nBe aware that any draw operation may modify the base level image data.\nFor this reason, calling this function only makes sense after all\ndrawing is completed and display has been called. Not calling display\nafter subsequent drawing will lead to undefined behavior if a mipmap\nhad been previously generated.\n\n\\return `true` if mipmap generation was successful, `false` if unsuccessful");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "generateMipmap", "fun(self: sf.RenderTexture): boolean");
    type_sf__RenderTexture.set_function("generateMipmap",
        [](sf::RenderTexture& self) -> bool {
            return self.generateMipmap();
        }
    );
    LUASF_STUB_DOC("\\brief Update the contents of the target texture\n\nThis function updates the target texture with what\nhas been drawn so far. Like for windows, calling this\nfunction is mandatory at the end of rendering. Not calling\nit may leave the texture in an undefined state.");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "display", "fun(self: sf.RenderTexture)");
    type_sf__RenderTexture.set_function("display",
        [](sf::RenderTexture& self) {
            self.display();
        }
    );
    LUASF_STUB_DOC("\\brief Get a read-only reference to the target texture\n\nAfter drawing to the render-texture and calling Display,\nyou can retrieve the updated texture using this function,\nand draw it using a sprite (for example).\nThe internal `sf::Texture` of a render-texture is always the\nsame instance, so that it is possible to call this function\nonce and keep a reference to the texture even after it is\nmodified.\n\n\\return Const reference to the texture");
    LUASF_STUB_FUNCTION("sf.RenderTexture", "getTexture", "fun(self: sf.RenderTexture): sf.Texture");
    type_sf__RenderTexture.set_function("getTexture",
        sol::policies(
            [](sf::RenderTexture& self) {
                return std::cref(self.getTexture());
            },
            sol::self_dependency{}
        )
    );
}
