#include "Graphics/bind_PrimitiveType.hpp"

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

namespace { constexpr std::array<std::string_view, 7> docs = {
    "\\ingroup graphics\n\\brief Types of primitives that a `sf::VertexArray` can render\n\nPoints and lines have no area, therefore their thickness\nwill always be 1 pixel, regardless the current transform\nand view.",
    "List of individual points",
    "List of individual lines",
    "List of connected lines, a point uses the previous point to form a line",
    "List of individual triangles",
    "List of connected triangles, a point uses the two previous points to form a triangle",
    "List of connected triangles, a point uses the common center and the previous point to form a triangle",
}; }

void bind_PrimitiveType(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.PrimitiveType");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("Points", "sf.PrimitiveType");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("Lines", "sf.PrimitiveType");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("LineStrip", "sf.PrimitiveType");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("Triangles", "sf.PrimitiveType");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("TriangleStrip", "sf.PrimitiveType");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("TriangleFan", "sf.PrimitiveType");
    lua_glue::BindEnum<sf::PrimitiveType>(sf, "PrimitiveType", {
        {"Points", sf::PrimitiveType::Points},
        {"Lines", sf::PrimitiveType::Lines},
        {"LineStrip", sf::PrimitiveType::LineStrip},
        {"Triangles", sf::PrimitiveType::Triangles},
        {"TriangleStrip", sf::PrimitiveType::TriangleStrip},
        {"TriangleFan", sf::PrimitiveType::TriangleFan}
    });
}
