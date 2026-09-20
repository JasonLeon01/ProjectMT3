#include "Graphics/bind_Vertex.hpp"

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

namespace { constexpr std::array<std::string_view, 4> docs = {
    "\\brief Point with color and texture coordinates\n\nBy default, the vertex color is white and texture coordinates are (0, 0).",
    "2D position of the vertex",
    "Color of the vertex",
    "Coordinates of the texture's pixel to map to the vertex NOLINT(readability-redundant-member-init)",
}; }

void bind_Vertex(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Vertex = lua_glue::BindStruct<sf::Vertex>(sf, "Vertex");
    lua_glue::Table table_sf__Vertex = sf["Vertex"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Vertex");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("position", "sf.Vector2f");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("color", "sf.Color");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("texCoords", "sf.Vector2f");
    LUASF_STUB_FUNCTION("sf.Vertex", "new", "fun(): sf.Vertex");
    lua_glue::BindCallable(type_sf__Vertex, "new",
        []() {
            return sf::Vertex{};
        }
    );
    lua_glue::BindAttr<sf::Vector2f>(type_sf__Vertex, "position", &sf::Vertex::position);
    lua_glue::BindAttr<sf::Color>(type_sf__Vertex, "color", &sf::Vertex::color);
    lua_glue::BindAttr<sf::Vector2f>(type_sf__Vertex, "texCoords", &sf::Vertex::texCoords);
    LUASF_STUB_FUNCTION("sf.Vertex", "copy", "fun(self: sf.Vertex): sf.Vertex");
    LUASF_STUB_FUNCTION("sf.Vertex", "deepcopy", "fun(self: sf.Vertex): sf.Vertex");
}
