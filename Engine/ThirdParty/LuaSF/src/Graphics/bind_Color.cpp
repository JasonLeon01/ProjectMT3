#include "Graphics/bind_Color.hpp"

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

namespace { constexpr std::array<std::string_view, 18> docs = {
    "\\brief Utility class for manipulating RGBA colors",
    "Red component",
    "Green component",
    "Blue component",
    "Alpha (opacity) component",
    "\\brief Default constructor\n\nConstructs an opaque black color. It is equivalent to\n`sf::Color(0, 0, 0, 255)`.",
    "\\brief Construct the color from its 4 RGBA components\n\n\\param red   Red component (in the range [0, 255])\n\\param green Green component (in the range [0, 255])\n\\param blue  Blue component (in the range [0, 255])\n\\param alpha Alpha (opacity) component (in the range [0, 255])",
    "\\brief Construct the color from 32-bit unsigned integer\n\n\\param color Number containing the RGBA components (in that order)",
    "\\brief Retrieve the color as a 32-bit unsigned integer\n\n\\return Color represented as a 32-bit unsigned integer",
    "Black predefined color",
    "White predefined color",
    "Red predefined color",
    "Green predefined color",
    "Blue predefined color",
    "Yellow predefined color",
    "Magenta predefined color",
    "Cyan predefined color",
    "Transparent (black) predefined color",
}; }

void bind_Color(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Color = lua_glue::BindStruct<sf::Color>(sf, "Color");
    lua_glue::Table table_sf__Color = sf["Color"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Color");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("r", "integer");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("g", "integer");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("b", "integer");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("a", "integer");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Color", "new", "fun(red: integer, green: integer, blue: integer, alpha?: integer): sf.Color");
    LUASF_STUB_OVERLOAD("sf.Color", "new", "fun(color: integer): sf.Color");
    LUASF_STUB_OVERLOAD("sf.Color", "new", "fun(): sf.Color");
    lua_glue::BindCallable(type_sf__Color, "new",
        [](lua_sf::LuaIntegral<std::uint8_t> red, lua_sf::LuaIntegral<std::uint8_t> green, lua_sf::LuaIntegral<std::uint8_t> blue, lua_sf::LuaIntegral<std::uint8_t> alpha) {
            return sf::Color{red.value(), green.value(), blue.value(), alpha.value()};
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned char>(255);
        }}},
        docs[6]
    );
    lua_glue::BindCallable(type_sf__Color, "new",
        [](lua_sf::LuaIntegral<std::uint32_t> color) {
            return sf::Color{color.value()};
        },
        docs[7]
    );
    lua_glue::BindCallable(type_sf__Color, "new",
        []() {
            return sf::Color{};
        },
        docs[5]
    );
    lua_glue::BindProperty(type_sf__Color, "r",
        [](const sf::Color& self) {
            return self.r;
        },
        [](sf::Color& self, lua_sf::LuaIntegral<std::uint8_t> value) {
            self.r = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Color, "g",
        [](const sf::Color& self) {
            return self.g;
        },
        [](sf::Color& self, lua_sf::LuaIntegral<std::uint8_t> value) {
            self.g = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Color, "b",
        [](const sf::Color& self) {
            return self.b;
        },
        [](sf::Color& self, lua_sf::LuaIntegral<std::uint8_t> value) {
            self.b = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Color, "a",
        [](const sf::Color& self) {
            return self.a;
        },
        [](sf::Color& self, lua_sf::LuaIntegral<std::uint8_t> value) {
            self.a = value.value();
        }
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Color", "toInteger", "fun(self: sf.Color): integer");
    lua_glue::BindCallable(type_sf__Color, "toInteger",
        [](const sf::Color& self) -> std::uint32_t {
            return self.toInteger();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_VALUE("sf.Color", "Black", "sf.Color");
    lua_glue::BindStaticAttr<const sf::Color>(table_sf__Color, "Black", &sf::Color::Black);
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_VALUE("sf.Color", "White", "sf.Color");
    lua_glue::BindStaticAttr<const sf::Color>(table_sf__Color, "White", &sf::Color::White);
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_VALUE("sf.Color", "Red", "sf.Color");
    lua_glue::BindStaticAttr<const sf::Color>(table_sf__Color, "Red", &sf::Color::Red);
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_VALUE("sf.Color", "Green", "sf.Color");
    lua_glue::BindStaticAttr<const sf::Color>(table_sf__Color, "Green", &sf::Color::Green);
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_VALUE("sf.Color", "Blue", "sf.Color");
    lua_glue::BindStaticAttr<const sf::Color>(table_sf__Color, "Blue", &sf::Color::Blue);
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_VALUE("sf.Color", "Yellow", "sf.Color");
    lua_glue::BindStaticAttr<const sf::Color>(table_sf__Color, "Yellow", &sf::Color::Yellow);
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_VALUE("sf.Color", "Magenta", "sf.Color");
    lua_glue::BindStaticAttr<const sf::Color>(table_sf__Color, "Magenta", &sf::Color::Magenta);
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_VALUE("sf.Color", "Cyan", "sf.Color");
    lua_glue::BindStaticAttr<const sf::Color>(table_sf__Color, "Cyan", &sf::Color::Cyan);
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_VALUE("sf.Color", "Transparent", "sf.Color");
    lua_glue::BindStaticAttr<const sf::Color>(table_sf__Color, "Transparent", &sf::Color::Transparent);
    LUASF_STUB_FUNCTION("sf.Color", "copy", "fun(self: sf.Color): sf.Color");
    LUASF_STUB_FUNCTION("sf.Color", "deepcopy", "fun(self: sf.Color): sf.Color");
}
