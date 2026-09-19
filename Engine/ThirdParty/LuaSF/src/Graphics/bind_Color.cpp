#include "Graphics/bind_Color.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Color(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Color = sf.new_usertype<sf::Color>("Color", sol::no_constructor);
    sol::table table_sf__Color = sf["Color"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Color>(lua);
    LUASF_STUB_DOC("\\brief Utility class for manipulating RGBA colors");
    LUASF_STUB_CLASS("sf.Color");
    LUASF_STUB_DOC("Red component");
    LUASF_STUB_FIELD("r", "integer");
    LUASF_STUB_DOC("Green component");
    LUASF_STUB_FIELD("g", "integer");
    LUASF_STUB_DOC("Blue component");
    LUASF_STUB_FIELD("b", "integer");
    LUASF_STUB_DOC("Alpha (opacity) component");
    LUASF_STUB_FIELD("a", "integer");
    LUASF_STUB_DOC("\\brief Construct the color from its 4 RGBA components\n\n\\param red   Red component (in the range [0, 255])\n\\param green Green component (in the range [0, 255])\n\\param blue  Blue component (in the range [0, 255])\n\\param alpha Alpha (opacity) component (in the range [0, 255])");
    LUASF_STUB_FUNCTION("sf.Color", "new", "fun(red: integer, green: integer, blue: integer, alpha: integer): sf.Color");
    LUASF_STUB_OVERLOAD("sf.Color", "new", "fun(red: integer, green: integer, blue: integer): sf.Color");
    LUASF_STUB_OVERLOAD("sf.Color", "new", "fun(color: integer): sf.Color");
    LUASF_STUB_OVERLOAD("sf.Color", "new", "fun(): sf.Color");
    type_sf__Color.set_function("new", sol::factories(
        [](lua_sf::LuaIntegral<std::uint8_t> red, lua_sf::LuaIntegral<std::uint8_t> green, lua_sf::LuaIntegral<std::uint8_t> blue, lua_sf::LuaIntegral<std::uint8_t> alpha) {
            return lua_sf::makeLuaSharedObject<sf::Color>(red.value(), green.value(), blue.value(), alpha.value());
        },
        [](lua_sf::LuaIntegral<std::uint8_t> red, lua_sf::LuaIntegral<std::uint8_t> green, lua_sf::LuaIntegral<std::uint8_t> blue) {
            return lua_sf::makeLuaSharedObject<sf::Color>(red.value(), green.value(), blue.value());
        },
        [](lua_sf::LuaIntegral<std::uint32_t> color) {
            return lua_sf::makeLuaSharedObject<sf::Color>(color.value());
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::Color>();
        }
    ));
    type_sf__Color.set("r", sol::property(
        [](sf::Color& self) {
            return self.r;
        },
        [](sf::Color& self, lua_sf::LuaIntegral<std::uint8_t> value) {
            self.r = value.value();
        }
    ));
    type_sf__Color.set("g", sol::property(
        [](sf::Color& self) {
            return self.g;
        },
        [](sf::Color& self, lua_sf::LuaIntegral<std::uint8_t> value) {
            self.g = value.value();
        }
    ));
    type_sf__Color.set("b", sol::property(
        [](sf::Color& self) {
            return self.b;
        },
        [](sf::Color& self, lua_sf::LuaIntegral<std::uint8_t> value) {
            self.b = value.value();
        }
    ));
    type_sf__Color.set("a", sol::property(
        [](sf::Color& self) {
            return self.a;
        },
        [](sf::Color& self, lua_sf::LuaIntegral<std::uint8_t> value) {
            self.a = value.value();
        }
    ));
    LUASF_STUB_DOC("\\brief Retrieve the color as a 32-bit unsigned integer\n\n\\return Color represented as a 32-bit unsigned integer");
    LUASF_STUB_FUNCTION("sf.Color", "toInteger", "fun(self: sf.Color): integer");
    type_sf__Color.set_function("toInteger",
        [](sf::Color& self) -> std::uint32_t {
            return self.toInteger();
        }
    );
    LUASF_STUB_DOC("Black predefined color");
    LUASF_STUB_VALUE("sf.Color", "Black", "sf.Color");
    table_sf__Color["Black"] = sf::Color::Black;
    LUASF_STUB_DOC("White predefined color");
    LUASF_STUB_VALUE("sf.Color", "White", "sf.Color");
    table_sf__Color["White"] = sf::Color::White;
    LUASF_STUB_DOC("Red predefined color");
    LUASF_STUB_VALUE("sf.Color", "Red", "sf.Color");
    table_sf__Color["Red"] = sf::Color::Red;
    LUASF_STUB_DOC("Green predefined color");
    LUASF_STUB_VALUE("sf.Color", "Green", "sf.Color");
    table_sf__Color["Green"] = sf::Color::Green;
    LUASF_STUB_DOC("Blue predefined color");
    LUASF_STUB_VALUE("sf.Color", "Blue", "sf.Color");
    table_sf__Color["Blue"] = sf::Color::Blue;
    LUASF_STUB_DOC("Yellow predefined color");
    LUASF_STUB_VALUE("sf.Color", "Yellow", "sf.Color");
    table_sf__Color["Yellow"] = sf::Color::Yellow;
    LUASF_STUB_DOC("Magenta predefined color");
    LUASF_STUB_VALUE("sf.Color", "Magenta", "sf.Color");
    table_sf__Color["Magenta"] = sf::Color::Magenta;
    LUASF_STUB_DOC("Cyan predefined color");
    LUASF_STUB_VALUE("sf.Color", "Cyan", "sf.Color");
    table_sf__Color["Cyan"] = sf::Color::Cyan;
    LUASF_STUB_DOC("Transparent (black) predefined color");
    LUASF_STUB_VALUE("sf.Color", "Transparent", "sf.Color");
    table_sf__Color["Transparent"] = sf::Color::Transparent;
}
