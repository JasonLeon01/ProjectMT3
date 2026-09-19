#include "Graphics/bind_CoordinateType.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_CoordinateType(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    LUASF_STUB_DOC("\\ingroup graphics\n\\brief Types of texture coordinates that can be used for rendering\n\n\\see `sf::RenderStates::coordinateType`");
    LUASF_STUB_CLASS("sf.CoordinateType");
    LUASF_STUB_DOC("Texture coordinates in range [0 .. 1]");
    LUASF_STUB_FIELD("Normalized", "sf.CoordinateType");
    LUASF_STUB_DOC("Texture coordinates in range [0 .. size]");
    LUASF_STUB_FIELD("Pixels", "sf.CoordinateType");
    sf.new_enum("CoordinateType",
        "Normalized", sf::CoordinateType::Normalized,
        "Pixels", sf::CoordinateType::Pixels
    );
}
