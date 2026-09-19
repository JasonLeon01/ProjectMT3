#include "Graphics/bind_Vertex.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Vertex(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Vertex = sf.new_usertype<sf::Vertex>("Vertex", sol::no_constructor);
    sol::table table_sf__Vertex = sf["Vertex"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Vertex>(lua);
    LUASF_STUB_DOC("\\brief Point with color and texture coordinates\n\nBy default, the vertex color is white and texture coordinates are (0, 0).");
    LUASF_STUB_CLASS("sf.Vertex");
    LUASF_STUB_DOC("2D position of the vertex");
    LUASF_STUB_FIELD("position", "sf.Vector2f");
    LUASF_STUB_DOC("Color of the vertex");
    LUASF_STUB_FIELD("color", "sf.Color");
    LUASF_STUB_DOC("Coordinates of the texture's pixel to map to the vertex NOLINT(readability-redundant-member-init)");
    LUASF_STUB_FIELD("texCoords", "sf.Vector2f");
    LUASF_STUB_FUNCTION("sf.Vertex", "new", "fun(): sf.Vertex");
    type_sf__Vertex.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Vertex>();
        }
    ));
    type_sf__Vertex["position"] = sol::policies(&sf::Vertex::position, sol::self_dependency{});
    type_sf__Vertex["color"] = sol::policies(&sf::Vertex::color, sol::self_dependency{});
    type_sf__Vertex["texCoords"] = sol::policies(&sf::Vertex::texCoords, sol::self_dependency{});
}
