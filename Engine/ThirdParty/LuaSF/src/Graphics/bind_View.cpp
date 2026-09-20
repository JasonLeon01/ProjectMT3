#include "Graphics/bind_View.hpp"

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

namespace { constexpr std::array<std::string_view, 19> docs = {
    "\\brief 2D camera that defines what region is shown on screen",
    "\\brief Default constructor\n\nThis constructor creates a default view of (0, 0, 1000, 1000)",
    "\\brief Construct the view from a rectangle\n\n\\param rectangle Rectangle defining the zone to display",
    "\\brief Construct the view from its center and size\n\n\\param center Center of the zone to display\n\\param size   Size of zone to display",
    "\\brief Set the center of the view\n\n\\param center New center\n\n\\see `setSize`, `getCenter`",
    "\\brief Set the size of the view\n\n\\param size New size\n\n\\see `setCenter`, `getCenter`",
    "\\brief Set the orientation of the view\n\nThe default rotation of a view is 0 degree.\n\n\\param angle New angle\n\n\\see `getRotation`",
    "\\brief Set the target viewport\n\nThe viewport is the rectangle into which the contents of the\nview are displayed, expressed as a factor (between 0 and 1)\nof the size of the RenderTarget to which the view is applied.\nFor example, a view which takes the left side of the target would\nbe defined with `view.setViewport(sf::FloatRect({0.f, 0.f}, {0.5f, 1.f}))`.\nBy default, a view has a viewport which covers the entire target.\n\n\\param viewport New viewport rectangle\n\n\\see `getViewport`",
    "\\brief Set the target scissor rectangle\n\nThe scissor rectangle, expressed as a factor (between 0 and 1) of\nthe RenderTarget, specifies the region of the RenderTarget whose\npixels are able to be modified by draw or clear operations.\nAny pixels which lie outside of the scissor rectangle will\nnot be modified by draw or clear operations.\nFor example, a scissor rectangle which only allows modifications\nto the right side of the target would be defined\nwith `view.setScissor(sf::FloatRect({0.5f, 0.f}, {0.5f, 1.f}))`.\nBy default, a view has a scissor rectangle which allows\nmodifications to the entire target. This is equivalent to\ndisabling the scissor test entirely. Passing the default\nscissor rectangle to this function will also disable\nscissor testing.\n\n\\param scissor New scissor rectangle\n\n\\see `getScissor`",
    "\\brief Get the center of the view\n\n\\return Center of the view\n\n\\see `getSize`, `setCenter`",
    "\\brief Get the size of the view\n\n\\return Size of the view\n\n\\see `getCenter`, `setSize`",
    "\\brief Get the current orientation of the view\n\n\\return Rotation angle of the view\n\n\\see `setRotation`",
    "\\brief Get the target viewport rectangle of the view\n\n\\return Viewport rectangle, expressed as a factor of the target size\n\n\\see `setViewport`",
    "\\brief Get the scissor rectangle of the view\n\n\\return Scissor rectangle, expressed as a factor of the target size\n\n\\see `setScissor`",
    "\\brief Move the view relative to its current position\n\n\\param offset Move offset\n\n\\see `setCenter`, `rotate`, `zoom`",
    "\\brief Rotate the view relative to its current orientation\n\n\\param angle Angle to rotate\n\n\\see `setRotation`, `move`, `zoom`",
    "\\brief Resize the view rectangle relative to its current size\n\nResizing the view simulates a zoom, as the zone displayed on\nscreen grows or shrinks.\n\\a factor is a multiplier:\n\\li 1 keeps the size unchanged\n\\li > 1 makes the view bigger (objects appear smaller)\n\\li < 1 makes the view smaller (objects appear bigger)\n\n\\param factor Zoom factor to apply\n\n\\see `setSize`, `move`, `rotate`",
    "\\brief Get the projection transform of the view\n\nThis function is meant for internal use only.\n\n\\return Projection transform defining the view\n\n\\see `getInverseTransform`",
    "\\brief Get the inverse projection transform of the view\n\nThis function is meant for internal use only.\n\n\\return Inverse of the projection transform defining the view\n\n\\see `getTransform`",
}; }

void bind_View(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__View = lua_glue::BindClass<sf::View>(sf, "View");
    lua_glue::Table table_sf__View = sf["View"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::View>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.View");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.View", "new", "fun(center: sf.Vector2f, size: sf.Vector2f): sf.View");
    LUASF_STUB_OVERLOAD("sf.View", "new", "fun(rectangle: sf.FloatRect): sf.View");
    LUASF_STUB_OVERLOAD("sf.View", "new", "fun(): sf.View");
    lua_glue::BindCallable(type_sf__View, "new",
        [](sf::Vector2f center, sf::Vector2f size) {
            return lua_sf::makeLuaSharedObject<sf::View>(center, size);
        },
        docs[3]
    );
    lua_glue::BindCallable(type_sf__View, "new",
        [](const sf::FloatRect& rectangle) {
            return lua_sf::makeLuaSharedObject<sf::View>(rectangle);
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__View, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::View>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.View", "setCenter", "fun(self: sf.View, center: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__View, "setCenter",
        [](sf::View& self, sf::Vector2f center) {
            self.setCenter(center);
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.View", "setSize", "fun(self: sf.View, size: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__View, "setSize",
        [](sf::View& self, sf::Vector2f size) {
            self.setSize(size);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.View", "setRotation", "fun(self: sf.View, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__View, "setRotation",
        [](sf::View& self, sf::Angle angle) {
            self.setRotation(angle);
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.View", "setViewport", "fun(self: sf.View, viewport: sf.FloatRect)");
    lua_glue::BindCallable(type_sf__View, "setViewport",
        [](sf::View& self, const sf::FloatRect& viewport) {
            self.setViewport(viewport);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.View", "setScissor", "fun(self: sf.View, scissor: sf.FloatRect)");
    lua_glue::BindCallable(type_sf__View, "setScissor",
        [](sf::View& self, const sf::FloatRect& scissor) {
            self.setScissor(scissor);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.View", "getCenter", "fun(self: sf.View): sf.Vector2f");
    lua_glue::BindCallable(type_sf__View, "getCenter",
        [](const sf::View& self) -> sf::Vector2f {
            return self.getCenter();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.View", "getSize", "fun(self: sf.View): sf.Vector2f");
    lua_glue::BindCallable(type_sf__View, "getSize",
        [](const sf::View& self) -> sf::Vector2f {
            return self.getSize();
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.View", "getRotation", "fun(self: sf.View): sf.Angle");
    lua_glue::BindCallable(type_sf__View, "getRotation",
        [](const sf::View& self) -> sf::Angle {
            return self.getRotation();
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.View", "getViewport", "fun(self: sf.View): sf.FloatRect");
    lua_glue::BindCallable(type_sf__View, "getViewport",
        [](const sf::View& self) {
            return std::cref(self.getViewport());
        },
        docs[12],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.View", "getScissor", "fun(self: sf.View): sf.FloatRect");
    lua_glue::BindCallable(type_sf__View, "getScissor",
        [](const sf::View& self) {
            return std::cref(self.getScissor());
        },
        docs[13],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.View", "move", "fun(self: sf.View, offset: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__View, "move",
        [](sf::View& self, sf::Vector2f offset) {
            self.move(offset);
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.View", "rotate", "fun(self: sf.View, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__View, "rotate",
        [](sf::View& self, sf::Angle angle) {
            self.rotate(angle);
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.View", "zoom", "fun(self: sf.View, factor: number)");
    lua_glue::BindCallable(type_sf__View, "zoom",
        [](sf::View& self, float factor) {
            self.zoom(factor);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.View", "getTransform", "fun(self: sf.View): sf.Transform");
    lua_glue::BindCallable(type_sf__View, "getTransform",
        [](const sf::View& self) {
            return std::cref(self.getTransform());
        },
        docs[17],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.View", "getInverseTransform", "fun(self: sf.View): sf.Transform");
    lua_glue::BindCallable(type_sf__View, "getInverseTransform",
        [](const sf::View& self) {
            return std::cref(self.getInverseTransform());
        },
        docs[18],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
}
