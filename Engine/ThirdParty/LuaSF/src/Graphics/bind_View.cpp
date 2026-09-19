#include "Graphics/bind_View.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_View(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__View = sf.new_usertype<sf::View>("View", sol::no_constructor);
    sol::table table_sf__View = sf["View"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::View>(lua);
    LUASF_STUB_DOC("\\brief 2D camera that defines what region is shown on screen");
    LUASF_STUB_CLASS("sf.View");
    LUASF_STUB_DOC("\\brief Construct the view from its center and size\n\n\\param center Center of the zone to display\n\\param size   Size of zone to display");
    LUASF_STUB_FUNCTION("sf.View", "new", "fun(center: sf.Vector2f, size: sf.Vector2f): sf.View");
    LUASF_STUB_OVERLOAD("sf.View", "new", "fun(rectangle: sf.FloatRect): sf.View");
    LUASF_STUB_OVERLOAD("sf.View", "new", "fun(): sf.View");
    type_sf__View.set_function("new", sol::factories(
        [](sf::Vector2f center, sf::Vector2f size) {
            return lua_sf::makeLuaSharedObject<sf::View>(center, size);
        },
        [](const sf::FloatRect& rectangle) {
            return lua_sf::makeLuaSharedObject<sf::View>(rectangle);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::View>();
        }
    ));
    LUASF_STUB_DOC("\\brief Set the center of the view\n\n\\param center New center\n\n\\see `setSize`, `getCenter`");
    LUASF_STUB_FUNCTION("sf.View", "setCenter", "fun(self: sf.View, center: sf.Vector2f)");
    type_sf__View.set_function("setCenter",
        [](sf::View& self, sf::Vector2f center) {
            self.setCenter(center);
        }
    );
    LUASF_STUB_DOC("\\brief Set the size of the view\n\n\\param size New size\n\n\\see `setCenter`, `getCenter`");
    LUASF_STUB_FUNCTION("sf.View", "setSize", "fun(self: sf.View, size: sf.Vector2f)");
    type_sf__View.set_function("setSize",
        [](sf::View& self, sf::Vector2f size) {
            self.setSize(size);
        }
    );
    LUASF_STUB_DOC("\\brief Set the orientation of the view\n\nThe default rotation of a view is 0 degree.\n\n\\param angle New angle\n\n\\see `getRotation`");
    LUASF_STUB_FUNCTION("sf.View", "setRotation", "fun(self: sf.View, angle: sf.Angle)");
    type_sf__View.set_function("setRotation",
        [](sf::View& self, sf::Angle angle) {
            self.setRotation(angle);
        }
    );
    LUASF_STUB_DOC("\\brief Set the target viewport\n\nThe viewport is the rectangle into which the contents of the\nview are displayed, expressed as a factor (between 0 and 1)\nof the size of the RenderTarget to which the view is applied.\nFor example, a view which takes the left side of the target would\nbe defined with `view.setViewport(sf::FloatRect({0.f, 0.f}, {0.5f, 1.f}))`.\nBy default, a view has a viewport which covers the entire target.\n\n\\param viewport New viewport rectangle\n\n\\see `getViewport`");
    LUASF_STUB_FUNCTION("sf.View", "setViewport", "fun(self: sf.View, viewport: sf.FloatRect)");
    type_sf__View.set_function("setViewport",
        [](sf::View& self, const sf::FloatRect& viewport) {
            self.setViewport(viewport);
        }
    );
    LUASF_STUB_DOC("\\brief Set the target scissor rectangle\n\nThe scissor rectangle, expressed as a factor (between 0 and 1) of\nthe RenderTarget, specifies the region of the RenderTarget whose\npixels are able to be modified by draw or clear operations.\nAny pixels which lie outside of the scissor rectangle will\nnot be modified by draw or clear operations.\nFor example, a scissor rectangle which only allows modifications\nto the right side of the target would be defined\nwith `view.setScissor(sf::FloatRect({0.5f, 0.f}, {0.5f, 1.f}))`.\nBy default, a view has a scissor rectangle which allows\nmodifications to the entire target. This is equivalent to\ndisabling the scissor test entirely. Passing the default\nscissor rectangle to this function will also disable\nscissor testing.\n\n\\param scissor New scissor rectangle\n\n\\see `getScissor`");
    LUASF_STUB_FUNCTION("sf.View", "setScissor", "fun(self: sf.View, scissor: sf.FloatRect)");
    type_sf__View.set_function("setScissor",
        [](sf::View& self, const sf::FloatRect& scissor) {
            self.setScissor(scissor);
        }
    );
    LUASF_STUB_DOC("\\brief Get the center of the view\n\n\\return Center of the view\n\n\\see `getSize`, `setCenter`");
    LUASF_STUB_FUNCTION("sf.View", "getCenter", "fun(self: sf.View): sf.Vector2f");
    type_sf__View.set_function("getCenter",
        [](sf::View& self) -> sf::Vector2f {
            return self.getCenter();
        }
    );
    LUASF_STUB_DOC("\\brief Get the size of the view\n\n\\return Size of the view\n\n\\see `getCenter`, `setSize`");
    LUASF_STUB_FUNCTION("sf.View", "getSize", "fun(self: sf.View): sf.Vector2f");
    type_sf__View.set_function("getSize",
        [](sf::View& self) -> sf::Vector2f {
            return self.getSize();
        }
    );
    LUASF_STUB_DOC("\\brief Get the current orientation of the view\n\n\\return Rotation angle of the view\n\n\\see `setRotation`");
    LUASF_STUB_FUNCTION("sf.View", "getRotation", "fun(self: sf.View): sf.Angle");
    type_sf__View.set_function("getRotation",
        [](sf::View& self) -> sf::Angle {
            return self.getRotation();
        }
    );
    LUASF_STUB_DOC("\\brief Get the target viewport rectangle of the view\n\n\\return Viewport rectangle, expressed as a factor of the target size\n\n\\see `setViewport`");
    LUASF_STUB_FUNCTION("sf.View", "getViewport", "fun(self: sf.View): sf.FloatRect");
    type_sf__View.set_function("getViewport",
        sol::policies(
            [](sf::View& self) {
                return std::cref(self.getViewport());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the scissor rectangle of the view\n\n\\return Scissor rectangle, expressed as a factor of the target size\n\n\\see `setScissor`");
    LUASF_STUB_FUNCTION("sf.View", "getScissor", "fun(self: sf.View): sf.FloatRect");
    type_sf__View.set_function("getScissor",
        sol::policies(
            [](sf::View& self) {
                return std::cref(self.getScissor());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Move the view relative to its current position\n\n\\param offset Move offset\n\n\\see `setCenter`, `rotate`, `zoom`");
    LUASF_STUB_FUNCTION("sf.View", "move", "fun(self: sf.View, offset: sf.Vector2f)");
    type_sf__View.set_function("move",
        [](sf::View& self, sf::Vector2f offset) {
            self.move(offset);
        }
    );
    LUASF_STUB_DOC("\\brief Rotate the view relative to its current orientation\n\n\\param angle Angle to rotate\n\n\\see `setRotation`, `move`, `zoom`");
    LUASF_STUB_FUNCTION("sf.View", "rotate", "fun(self: sf.View, angle: sf.Angle)");
    type_sf__View.set_function("rotate",
        [](sf::View& self, sf::Angle angle) {
            self.rotate(angle);
        }
    );
    LUASF_STUB_DOC("\\brief Resize the view rectangle relative to its current size\n\nResizing the view simulates a zoom, as the zone displayed on\nscreen grows or shrinks.\n\\a factor is a multiplier:\n\\li 1 keeps the size unchanged\n\\li > 1 makes the view bigger (objects appear smaller)\n\\li < 1 makes the view smaller (objects appear bigger)\n\n\\param factor Zoom factor to apply\n\n\\see `setSize`, `move`, `rotate`");
    LUASF_STUB_FUNCTION("sf.View", "zoom", "fun(self: sf.View, factor: number)");
    type_sf__View.set_function("zoom",
        [](sf::View& self, float factor) {
            self.zoom(factor);
        }
    );
    LUASF_STUB_DOC("\\brief Get the projection transform of the view\n\nThis function is meant for internal use only.\n\n\\return Projection transform defining the view\n\n\\see `getInverseTransform`");
    LUASF_STUB_FUNCTION("sf.View", "getTransform", "fun(self: sf.View): sf.Transform");
    type_sf__View.set_function("getTransform",
        sol::policies(
            [](sf::View& self) {
                return std::cref(self.getTransform());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the inverse projection transform of the view\n\nThis function is meant for internal use only.\n\n\\return Inverse of the projection transform defining the view\n\n\\see `getTransform`");
    LUASF_STUB_FUNCTION("sf.View", "getInverseTransform", "fun(self: sf.View): sf.Transform");
    type_sf__View.set_function("getInverseTransform",
        sol::policies(
            [](sf::View& self) {
                return std::cref(self.getInverseTransform());
            },
            sol::self_dependency{}
        )
    );
}
