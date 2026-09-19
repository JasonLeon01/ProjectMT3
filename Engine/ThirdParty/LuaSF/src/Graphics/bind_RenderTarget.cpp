#include "Graphics/bind_RenderTarget.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_RenderTarget(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__RenderTarget = sf.new_usertype<sf::RenderTarget>("RenderTarget", sol::no_constructor);
    sol::table table_sf__RenderTarget = sf["RenderTarget"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::RenderTarget>(lua);
    LUASF_STUB_DOC("\\brief Base class for all render targets (window, texture, ...)");
    LUASF_STUB_CLASS("sf.RenderTarget");
    // sf::RenderTarget is abstract; constructor binding is omitted.
    LUASF_STUB_DOC("\\brief Clear the entire target with a single color and stencil value\n\nThe specified stencil value is truncated to the bit\nwidth of the current stencil buffer.\n\n\\param color        Fill color to use to clear the render target\n\\param stencilValue Stencil value to clear to");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "clear", "fun(self: sf.RenderTarget, color: sf.Color, stencilValue: sf.StencilValue)");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "clear", "fun(self: sf.RenderTarget, color: sf.Color)");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "clear", "fun(self: sf.RenderTarget)");
    type_sf__RenderTarget.set_function("clear",
        sol::overload(
            [](sf::RenderTarget& self, sf::Color color, sf::StencilValue stencilValue) {
                self.clear(color, stencilValue);
            },
            [](sf::RenderTarget& self, sf::Color color) {
                self.clear(color);
            },
            [](sf::RenderTarget& self) {
                self.clear();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Clear the stencil buffer to a specific value\n\nThe specified value is truncated to the bit width of\nthe current stencil buffer.\n\n\\param stencilValue Stencil value to clear to");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "clearStencil", "fun(self: sf.RenderTarget, stencilValue: sf.StencilValue)");
    type_sf__RenderTarget.set_function("clearStencil",
        [](sf::RenderTarget& self, sf::StencilValue stencilValue) {
            self.clearStencil(stencilValue);
        }
    );
    LUASF_STUB_DOC("\\brief Change the current active view\n\nThe view is like a 2D camera, it controls which part of\nthe 2D scene is visible, and how it is viewed in the\nrender target.\nThe new view will affect everything that is drawn, until\nanother view is set.\nThe render target keeps its own copy of the view object,\nso it is not necessary to keep the original one alive\nafter calling this function.\nTo restore the original view of the target, you can pass\nthe result of `getDefaultView()` to this function.\n\n\\param view New view to use\n\n\\see `getView`, `getDefaultView`");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "setView", "fun(self: sf.RenderTarget, view: sf.View)");
    type_sf__RenderTarget.set_function("setView",
        [](sf::RenderTarget& self, const sf::View& view) {
            self.setView(view);
        }
    );
    LUASF_STUB_DOC("\\brief Get the view currently in use in the render target\n\n\\return The view object that is currently used\n\n\\see `setView`, `getDefaultView`");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "getView", "fun(self: sf.RenderTarget): sf.View");
    type_sf__RenderTarget.set_function("getView",
        sol::policies(
            [](sf::RenderTarget& self) {
                return std::cref(self.getView());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the default view of the render target\n\nThe default view has the initial size of the render target,\nand never changes after the target has been created.\n\n\\return The default view of the render target\n\n\\see `setView`, `getView`");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "getDefaultView", "fun(self: sf.RenderTarget): sf.View");
    type_sf__RenderTarget.set_function("getDefaultView",
        sol::policies(
            [](sf::RenderTarget& self) {
                return std::cref(self.getDefaultView());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the viewport of a view, applied to this render target\n\nThe viewport is defined in the view as a ratio, this function\nsimply applies this ratio to the current dimensions of the\nrender target to calculate the pixels rectangle that the viewport\nactually covers in the target.\n\n\\param view The view for which we want to compute the viewport\n\n\\return Viewport rectangle, expressed in pixels");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "getViewport", "fun(self: sf.RenderTarget, view: sf.View): sf.IntRect");
    type_sf__RenderTarget.set_function("getViewport",
        [](sf::RenderTarget& self, const sf::View& view) -> sf::IntRect {
            return self.getViewport(view);
        }
    );
    LUASF_STUB_DOC("\\brief Get the scissor rectangle of a view, applied to this render target\n\nThe scissor rectangle is defined in the view as a ratio. This\nfunction simply applies this ratio to the current dimensions\nof the render target to calculate the pixels rectangle\nthat the scissor rectangle actually covers in the target.\n\n\\param view The view for which we want to compute the scissor rectangle\n\n\\return Scissor rectangle, expressed in pixels");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "getScissor", "fun(self: sf.RenderTarget, view: sf.View): sf.IntRect");
    type_sf__RenderTarget.set_function("getScissor",
        [](sf::RenderTarget& self, const sf::View& view) -> sf::IntRect {
            return self.getScissor(view);
        }
    );
    LUASF_STUB_DOC("\\brief Convert a point from target coordinates to world coordinates\n\nThis function finds the 2D position that matches the\ngiven pixel of the render target. In other words, it does\nthe inverse of what the graphics card does, to find the\ninitial position of a rendered pixel.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (10, 50) in your render target may map to the point\n(150, 75) in your 2D world -- if the view is translated by (140, 25).\n\nFor render-windows, this function is typically used to find\nwhich point (or object) is located below the mouse cursor.\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Pixel to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in \"world\" units\n\n\\see `mapCoordsToPixel`");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "mapPixelToCoords", "fun(self: sf.RenderTarget, point: sf.Vector2i, view: sf.View): sf.Vector2f");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "mapPixelToCoords", "fun(self: sf.RenderTarget, point: sf.Vector2i): sf.Vector2f");
    type_sf__RenderTarget.set_function("mapPixelToCoords",
        sol::overload(
            [](sf::RenderTarget& self, sf::Vector2i point, const sf::View& view) -> sf::Vector2f {
                return self.mapPixelToCoords(point, view);
            },
            [](sf::RenderTarget& self, sf::Vector2i point) -> sf::Vector2f {
                return self.mapPixelToCoords(point);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Convert a point from world coordinates to target coordinates\n\nThis function finds the pixel of the render target that matches\nthe given 2D point. In other words, it goes through the same process\nas the graphics card, to compute the final position of a rendered point.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (150, 75) in your 2D world may map to the pixel\n(10, 50) of your render target -- if the view is translated by (140, 25).\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Point to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in target coordinates (pixels)\n\n\\see `mapPixelToCoords`");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "mapCoordsToPixel", "fun(self: sf.RenderTarget, point: sf.Vector2f, view: sf.View): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "mapCoordsToPixel", "fun(self: sf.RenderTarget, point: sf.Vector2f): sf.Vector2i");
    type_sf__RenderTarget.set_function("mapCoordsToPixel",
        sol::overload(
            [](sf::RenderTarget& self, sf::Vector2f point, const sf::View& view) -> sf::Vector2i {
                return self.mapCoordsToPixel(point, view);
            },
            [](sf::RenderTarget& self, sf::Vector2f point) -> sf::Vector2i {
                return self.mapCoordsToPixel(point);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Draw primitives defined by a vertex buffer\n\n\\param vertexBuffer Vertex buffer\n\\param firstVertex  Index of the first vertex to render\n\\param vertexCount  Number of vertices to render\n\\param states       Render states to use for drawing");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "draw", "fun(self: sf.RenderTarget, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "draw", "fun(self: sf.RenderTarget, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer)");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "draw", "fun(self: sf.RenderTarget, drawable: sf.Drawable, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "draw", "fun(self: sf.RenderTarget, vertexBuffer: sf.VertexBuffer, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "draw", "fun(self: sf.RenderTarget, drawable: sf.Drawable)");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "draw", "fun(self: sf.RenderTarget, vertexBuffer: sf.VertexBuffer)");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "draw", "fun(self: sf.RenderTarget, vertices: any, type: sf.PrimitiveType, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "draw", "fun(self: sf.RenderTarget, vertices: any, type: sf.PrimitiveType)");
    type_sf__RenderTarget.set_function("draw",
        sol::overload(
            [](sf::RenderTarget& self, const sf::VertexBuffer& vertexBuffer, lua_sf::LuaIntegral<std::size_t> firstVertex, lua_sf::LuaIntegral<std::size_t> vertexCount, const sf::RenderStates& states) {
                self.draw(vertexBuffer, firstVertex.value(), vertexCount.value(), states);
            },
            [](sf::RenderTarget& self, const sf::VertexBuffer& vertexBuffer, lua_sf::LuaIntegral<std::size_t> firstVertex, lua_sf::LuaIntegral<std::size_t> vertexCount) {
                self.draw(vertexBuffer, firstVertex.value(), vertexCount.value());
            },
            [](sf::RenderTarget& self, const sf::Drawable& drawable, const sf::RenderStates& states) {
                self.draw(drawable, states);
            },
            [](sf::RenderTarget& self, const sf::VertexBuffer& vertexBuffer, const sf::RenderStates& states) {
                self.draw(vertexBuffer, states);
            },
            [](sf::RenderTarget& self, const sf::Drawable& drawable) {
                self.draw(drawable);
            },
            [](sf::RenderTarget& self, const sf::VertexBuffer& vertexBuffer) {
                self.draw(vertexBuffer);
            },
            [](sf::RenderTarget& self, sol::object vertices, sf::PrimitiveType type, const sf::RenderStates& states) {
                auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
                self.draw(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), type, states);
            },
            [](sf::RenderTarget& self, sol::object vertices, sf::PrimitiveType type) {
                auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
                self.draw(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), type);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Return the size of the rendering region of the target\n\n\\return Size in pixels");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "getSize", "fun(self: sf.RenderTarget): sf.Vector2u");
    type_sf__RenderTarget.set_function("getSize",
        [](sf::RenderTarget& self) -> sf::Vector2u {
            return self.getSize();
        }
    );
    LUASF_STUB_DOC("\\brief Tell if the render target will use sRGB encoding when drawing on it\n\n\\return `true` if the render target use sRGB encoding, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "isSrgb", "fun(self: sf.RenderTarget): boolean");
    type_sf__RenderTarget.set_function("isSrgb",
        [](sf::RenderTarget& self) -> bool {
            return self.isSrgb();
        }
    );
    LUASF_STUB_DOC("\\brief Activate or deactivate the render target for rendering\n\nThis function makes the render target's context current for\nfuture OpenGL rendering operations (so you shouldn't care\nabout it if you're not doing direct OpenGL stuff).\nA render target's context is active only on the current thread,\nif you want to make it active on another thread you have\nto deactivate it on the previous thread first if it was active.\nOnly one context can be current in a thread, so if you\nwant to draw OpenGL geometry to another render target\ndon't forget to activate it again. Activating a render\ntarget will automatically deactivate the previously active\ncontext (if any).\n\n\\param active `true` to activate, `false` to deactivate\n\n\\return `true` if operation was successful, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "setActive", "fun(self: sf.RenderTarget, active: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.RenderTarget", "setActive", "fun(self: sf.RenderTarget): boolean");
    type_sf__RenderTarget.set_function("setActive",
        sol::overload(
            [](sf::RenderTarget& self, bool active) -> bool {
                return self.setActive(active);
            },
            [](sf::RenderTarget& self) -> bool {
                return self.setActive();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Save the OpenGL render states modified by SFML\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering. Combined with popGLStates,\nit ensures that:\n\\li SFML's internal states are not messed up by your OpenGL code\n\\li your OpenGL states are not modified by a call to a SFML function\n\nMore specifically, it must be used around code that\ncalls `draw` functions. Example:\n\\code\n// OpenGL code here...\nwindow.pushGLStates();\nwindow.draw(...);\nwindow.draw(...);\nwindow.popGLStates();\n// OpenGL code here...\n\\endcode\n\nNote that this function is quite expensive: it saves the\nprogram, textures, vertex attributes and the other OpenGL\nstates that SFML drawing can modify. State outside this set\nis deliberately not covered.\nIt is provided for convenience, but the best results will\nbe achieved if you handle OpenGL states yourself (because\nyou know which states have really changed, and need to be\nsaved and restored). Take a look at the resetGLStates\nfunction if you do so.\n\n\\see `popGLStates`");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "pushGLStates", "fun(self: sf.RenderTarget)");
    type_sf__RenderTarget.set_function("pushGLStates",
        [](sf::RenderTarget& self) {
            self.pushGLStates();
        }
    );
    LUASF_STUB_DOC("\\brief Restore the previously saved OpenGL render states\n\nSee the description of `pushGLStates` to get a detailed\ndescription of these functions.\n\n\\see `pushGLStates`");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "popGLStates", "fun(self: sf.RenderTarget)");
    type_sf__RenderTarget.set_function("popGLStates",
        [](sf::RenderTarget& self) {
            self.popGLStates();
        }
    );
    LUASF_STUB_DOC("\\brief Reset the internal OpenGL states so that the target is ready for drawing\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering, if you choose not to use\n`pushGLStates`/`popGLStates`. It makes sure that all OpenGL\nstates needed by SFML are set, so that subsequent `draw()`\ncalls will work as expected.\n\nExample:\n\\code\n// OpenGL code here...\nwindow.resetGLStates();\nwindow.draw(...);\nwindow.draw(...);\n// OpenGL code here...\n\\endcode");
    LUASF_STUB_FUNCTION("sf.RenderTarget", "resetGLStates", "fun(self: sf.RenderTarget)");
    type_sf__RenderTarget.set_function("resetGLStates",
        [](sf::RenderTarget& self) {
            self.resetGLStates();
        }
    );
}
