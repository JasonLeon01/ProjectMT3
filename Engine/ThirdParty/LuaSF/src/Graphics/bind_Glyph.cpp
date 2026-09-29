#include "Graphics/bind_Glyph.hpp"

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

namespace { constexpr std::array<std::string_view, 6> docs = {
    "\\brief Structure describing a glyph",
    "Offset to move horizontally to the next character",
    "Left offset after forced autohint. Internally used by getKerning()",
    "Right offset after forced autohint. Internally used by getKerning()",
    "Bounding rectangle of the glyph, in coordinates relative to the baseline",
    "Texture coordinates of the glyph inside the font's texture",
}; }

void bind_Glyph(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Glyph = lua_glue::BindClass<sf::Glyph>(sf, "Glyph");
    lua_glue::Table table_sf__Glyph = sf["Glyph"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Glyph>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Glyph");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("advance", "number");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("lsbDelta", "integer");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("rsbDelta", "integer");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("bounds", "sf.FloatRect");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("textureRect", "sf.IntRect");
    LUASF_STUB_FUNCTION("sf.Glyph", "new", "fun(): sf.Glyph");
    lua_glue::BindCallable(type_sf__Glyph, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Glyph>();
        }
    );
    lua_glue::BindAttr<float>(type_sf__Glyph, "advance", &sf::Glyph::advance);
    lua_glue::BindProperty(type_sf__Glyph, "lsbDelta",
        [](const sf::Glyph& self) {
            return self.lsbDelta;
        },
        [](sf::Glyph& self, lua_sf::LuaIntegral<int> value) {
            self.lsbDelta = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Glyph, "rsbDelta",
        [](const sf::Glyph& self) {
            return self.rsbDelta;
        },
        [](sf::Glyph& self, lua_sf::LuaIntegral<int> value) {
            self.rsbDelta = value.value();
        }
    );
    lua_glue::BindAttr<sf::FloatRect>(type_sf__Glyph, "bounds", &sf::Glyph::bounds);
    lua_glue::BindAttr<sf::IntRect>(type_sf__Glyph, "textureRect", &sf::Glyph::textureRect);
}
