#include "Window/bind_Joystick.hpp"

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

namespace { constexpr std::array<std::string_view, 23> docs = {
    "Maximum number of supported joysticks",
    "Maximum number of supported buttons",
    "Maximum number of supported axes",
    "\\brief Axes supported by SFML joysticks",
    "The X axis",
    "The Y axis",
    "The Z axis",
    "The R axis",
    "The U axis",
    "The V axis",
    "The X axis of the point-of-view hat",
    "The Y axis of the point-of-view hat",
    "\\brief Structure holding a joystick's identification",
    "Name of the joystick",
    "Manufacturer identifier",
    "Product identifier",
    "\\brief Check if a joystick is connected\n\n\\param joystick Index of the joystick to check\n\n\\return `true` if the joystick is connected, `false` otherwise",
    "\\brief Return the number of buttons supported by a joystick\n\nIf the joystick is not connected, this function returns 0.\n\n\\param joystick Index of the joystick\n\n\\return Number of buttons supported by the joystick",
    "\\brief Check if a joystick supports a given axis\n\nIf the joystick is not connected, this function returns `false`.\n\n\\param joystick Index of the joystick\n\\param axis     Axis to check\n\n\\return `true` if the joystick supports the axis, `false` otherwise",
    "\\brief Check if a joystick button is pressed\n\nIf the joystick is not connected, this function returns `false`.\n\n\\param joystick Index of the joystick\n\\param button   Button to check\n\n\\return `true` if the button is pressed, `false` otherwise",
    "\\brief Get the current position of a joystick axis\n\nIf the joystick is not connected, this function returns 0.\n\n\\param joystick Index of the joystick\n\\param axis     Axis to check\n\n\\return Current position of the axis, in range [-100 .. 100]",
    "\\brief Get the joystick information\n\n\\param joystick Index of the joystick\n\n\\return Structure containing joystick information.",
    "\\brief Update the states of all joysticks\n\nThis function is used internally by SFML, so you normally\ndon't have to call it explicitly. However, you may need to\ncall it if you have no window yet (or no window at all):\nin this case the joystick states are not updated automatically.",
}; }

void bind_Joystick(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    lua_glue::Table sf_Joystick = sf["Joystick"].get_or_create<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_VALUE("sf.Joystick", "Count", "integer");
    lua_glue::BindStaticAttr<const unsigned int>(sf_Joystick, "Count", &sf::Joystick::Count);
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_VALUE("sf.Joystick", "ButtonCount", "integer");
    lua_glue::BindStaticAttr<const unsigned int>(sf_Joystick, "ButtonCount", &sf::Joystick::ButtonCount);
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_VALUE("sf.Joystick", "AxisCount", "integer");
    lua_glue::BindStaticAttr<const unsigned int>(sf_Joystick, "AxisCount", &sf::Joystick::AxisCount);
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_CLASS("sf.Joystick.Axis");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("X", "sf.Joystick.Axis");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("Y", "sf.Joystick.Axis");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("Z", "sf.Joystick.Axis");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FIELD("R", "sf.Joystick.Axis");
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FIELD("U", "sf.Joystick.Axis");
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FIELD("V", "sf.Joystick.Axis");
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FIELD("PovX", "sf.Joystick.Axis");
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FIELD("PovY", "sf.Joystick.Axis");
    lua_glue::BindEnum<sf::Joystick::Axis>(sf_Joystick, "Axis", {
        {"X", sf::Joystick::Axis::X},
        {"Y", sf::Joystick::Axis::Y},
        {"Z", sf::Joystick::Axis::Z},
        {"R", sf::Joystick::Axis::R},
        {"U", sf::Joystick::Axis::U},
        {"V", sf::Joystick::Axis::V},
        {"PovX", sf::Joystick::Axis::PovX},
        {"PovY", sf::Joystick::Axis::PovY}
    });
    auto type_sf__Joystick__Identification = lua_glue::BindClass<sf::Joystick::Identification>(sf_Joystick, "Identification");
    lua_glue::Table table_sf__Joystick__Identification = sf_Joystick["Identification"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Joystick::Identification>(lua);
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_CLASS("sf.Joystick.Identification");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FIELD("name", "string");
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FIELD("vendorId", "integer");
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FIELD("productId", "integer");
    LUASF_STUB_FUNCTION("sf.Joystick.Identification", "new", "fun(): sf.Joystick.Identification");
    lua_glue::BindCallable(type_sf__Joystick__Identification, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Joystick::Identification>();
        }
    );
    lua_glue::BindProperty(type_sf__Joystick__Identification, "name",
        [](const sf::Joystick::Identification& self) -> std::string {
            return lua_sf::to_utf8_string(self.name);
        },
        [](sf::Joystick::Identification& self, std::string value) {
            self.name = lua_sf::to_sf_string(value);
        }
    );
    lua_glue::BindProperty(type_sf__Joystick__Identification, "vendorId",
        [](const sf::Joystick::Identification& self) {
            return self.vendorId;
        },
        [](sf::Joystick::Identification& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.vendorId = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Joystick__Identification, "productId",
        [](const sf::Joystick::Identification& self) {
            return self.productId;
        },
        [](sf::Joystick::Identification& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.productId = value.value();
        }
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Joystick", "isConnected", "fun(joystick: integer): boolean");
    lua_glue::BindCallable(sf_Joystick, "isConnected",
        [](lua_sf::LuaIntegral<unsigned int> joystick) -> bool {
            return sf::Joystick::isConnected(joystick.value());
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Joystick", "getButtonCount", "fun(joystick: integer): integer");
    lua_glue::BindCallable(sf_Joystick, "getButtonCount",
        [](lua_sf::LuaIntegral<unsigned int> joystick) -> unsigned int {
            return sf::Joystick::getButtonCount(joystick.value());
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.Joystick", "hasAxis", "fun(joystick: integer, axis: sf.Joystick.Axis): boolean");
    lua_glue::BindCallable(sf_Joystick, "hasAxis",
        [](lua_sf::LuaIntegral<unsigned int> joystick, sf::Joystick::Axis axis) -> bool {
            return sf::Joystick::hasAxis(joystick.value(), axis);
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Joystick", "isButtonPressed", "fun(joystick: integer, button: integer): boolean");
    lua_glue::BindCallable(sf_Joystick, "isButtonPressed",
        [](lua_sf::LuaIntegral<unsigned int> joystick, lua_sf::LuaIntegral<unsigned int> button) -> bool {
            return sf::Joystick::isButtonPressed(joystick.value(), button.value());
        },
        docs[19]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Joystick", "getAxisPosition", "fun(joystick: integer, axis: sf.Joystick.Axis): number");
    lua_glue::BindCallable(sf_Joystick, "getAxisPosition",
        [](lua_sf::LuaIntegral<unsigned int> joystick, sf::Joystick::Axis axis) -> float {
            return sf::Joystick::getAxisPosition(joystick.value(), axis);
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Joystick", "getIdentification", "fun(joystick: integer): sf.Joystick.Identification");
    lua_glue::BindCallable(sf_Joystick, "getIdentification",
        [](lua_sf::LuaIntegral<unsigned int> joystick) -> sf::Joystick::Identification {
            return sf::Joystick::getIdentification(joystick.value());
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Joystick", "update", "fun()");
    lua_glue::BindCallable(sf_Joystick, "update",
        []() {
            sf::Joystick::update();
        },
        docs[22]
    );
}
