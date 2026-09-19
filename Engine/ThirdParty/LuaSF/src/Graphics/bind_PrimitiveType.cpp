#include "Graphics/bind_PrimitiveType.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_PrimitiveType(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    LUASF_STUB_DOC("\\ingroup graphics\n\\brief Types of primitives that a `sf::VertexArray` can render\n\nPoints and lines have no area, therefore their thickness\nwill always be 1 pixel, regardless the current transform\nand view.");
    LUASF_STUB_CLASS("sf.PrimitiveType");
    LUASF_STUB_DOC("List of individual points");
    LUASF_STUB_FIELD("Points", "sf.PrimitiveType");
    LUASF_STUB_DOC("List of individual lines");
    LUASF_STUB_FIELD("Lines", "sf.PrimitiveType");
    LUASF_STUB_DOC("List of connected lines, a point uses the previous point to form a line");
    LUASF_STUB_FIELD("LineStrip", "sf.PrimitiveType");
    LUASF_STUB_DOC("List of individual triangles");
    LUASF_STUB_FIELD("Triangles", "sf.PrimitiveType");
    LUASF_STUB_DOC("List of connected triangles, a point uses the two previous points to form a triangle");
    LUASF_STUB_FIELD("TriangleStrip", "sf.PrimitiveType");
    LUASF_STUB_DOC("List of connected triangles, a point uses the common center and the previous point to form a triangle");
    LUASF_STUB_FIELD("TriangleFan", "sf.PrimitiveType");
    sf.new_enum("PrimitiveType",
        "Points", sf::PrimitiveType::Points,
        "Lines", sf::PrimitiveType::Lines,
        "LineStrip", sf::PrimitiveType::LineStrip,
        "Triangles", sf::PrimitiveType::Triangles,
        "TriangleStrip", sf::PrimitiveType::TriangleStrip,
        "TriangleFan", sf::PrimitiveType::TriangleFan
    );
}
