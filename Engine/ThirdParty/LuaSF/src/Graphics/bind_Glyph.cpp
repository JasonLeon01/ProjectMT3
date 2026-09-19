#include "Graphics/bind_Glyph.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Glyph(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Glyph = sf.new_usertype<sf::Glyph>("Glyph", sol::no_constructor);
    sol::table table_sf__Glyph = sf["Glyph"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Glyph>(lua);
    LUASF_STUB_DOC("\\brief Structure describing a glyph");
    LUASF_STUB_CLASS("sf.Glyph");
    LUASF_STUB_DOC("Offset to move horizontally to the next character");
    LUASF_STUB_FIELD("advance", "number");
    LUASF_STUB_DOC("Left offset after forced autohint. Internally used by getKerning()");
    LUASF_STUB_FIELD("lsbDelta", "integer");
    LUASF_STUB_DOC("Right offset after forced autohint. Internally used by getKerning()");
    LUASF_STUB_FIELD("rsbDelta", "integer");
    LUASF_STUB_DOC("Bounding rectangle of the glyph, in coordinates relative to the baseline");
    LUASF_STUB_FIELD("bounds", "sf.FloatRect");
    LUASF_STUB_DOC("Texture coordinates of the glyph inside the font's texture");
    LUASF_STUB_FIELD("textureRect", "sf.IntRect");
    LUASF_STUB_FUNCTION("sf.Glyph", "new", "fun(): sf.Glyph");
    type_sf__Glyph.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Glyph>();
        }
    ));
    type_sf__Glyph["advance"] = sol::policies(&sf::Glyph::advance, sol::self_dependency{});
    type_sf__Glyph.set("lsbDelta", sol::property(
        [](sf::Glyph& self) {
            return self.lsbDelta;
        },
        [](sf::Glyph& self, lua_sf::LuaIntegral<int> value) {
            self.lsbDelta = value.value();
        }
    ));
    type_sf__Glyph.set("rsbDelta", sol::property(
        [](sf::Glyph& self) {
            return self.rsbDelta;
        },
        [](sf::Glyph& self, lua_sf::LuaIntegral<int> value) {
            self.rsbDelta = value.value();
        }
    ));
    type_sf__Glyph["bounds"] = sol::policies(&sf::Glyph::bounds, sol::self_dependency{});
    type_sf__Glyph["textureRect"] = sol::policies(&sf::Glyph::textureRect, sol::self_dependency{});
}
