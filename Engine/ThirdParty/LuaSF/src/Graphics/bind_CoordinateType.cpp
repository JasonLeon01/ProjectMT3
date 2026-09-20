#include "Graphics/bind_CoordinateType.hpp"

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

namespace { constexpr std::array<std::string_view, 3> docs = {
    "\\ingroup graphics\n\\brief Types of texture coordinates that can be used for rendering\n\n\\see `sf::RenderStates::coordinateType`",
    "Texture coordinates in range [0 .. 1]",
    "Texture coordinates in range [0 .. size]",
}; }

void bind_CoordinateType(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.CoordinateType");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("Normalized", "sf.CoordinateType");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("Pixels", "sf.CoordinateType");
    lua_glue::BindEnum<sf::CoordinateType>(sf, "CoordinateType", {
        {"Normalized", sf::CoordinateType::Normalized},
        {"Pixels", sf::CoordinateType::Pixels}
    });
}
