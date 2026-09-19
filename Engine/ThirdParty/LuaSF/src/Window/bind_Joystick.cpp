#include "Window/bind_Joystick.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Joystick(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    sol::table sf_Joystick = sf["Joystick"].get_or_create<sol::table>();
    LUASF_STUB_DOC("Maximum number of supported joysticks");
    LUASF_STUB_VALUE("sf.Joystick", "Count", "integer");
    sf_Joystick["Count"] = sf::Joystick::Count;
    LUASF_STUB_DOC("Maximum number of supported buttons");
    LUASF_STUB_VALUE("sf.Joystick", "ButtonCount", "integer");
    sf_Joystick["ButtonCount"] = sf::Joystick::ButtonCount;
    LUASF_STUB_DOC("Maximum number of supported axes");
    LUASF_STUB_VALUE("sf.Joystick", "AxisCount", "integer");
    sf_Joystick["AxisCount"] = sf::Joystick::AxisCount;
    LUASF_STUB_DOC("\\brief Axes supported by SFML joysticks");
    LUASF_STUB_CLASS("sf.Joystick.Axis");
    LUASF_STUB_DOC("The X axis");
    LUASF_STUB_FIELD("X", "sf.Joystick.Axis");
    LUASF_STUB_DOC("The Y axis");
    LUASF_STUB_FIELD("Y", "sf.Joystick.Axis");
    LUASF_STUB_DOC("The Z axis");
    LUASF_STUB_FIELD("Z", "sf.Joystick.Axis");
    LUASF_STUB_DOC("The R axis");
    LUASF_STUB_FIELD("R", "sf.Joystick.Axis");
    LUASF_STUB_DOC("The U axis");
    LUASF_STUB_FIELD("U", "sf.Joystick.Axis");
    LUASF_STUB_DOC("The V axis");
    LUASF_STUB_FIELD("V", "sf.Joystick.Axis");
    LUASF_STUB_DOC("The X axis of the point-of-view hat");
    LUASF_STUB_FIELD("PovX", "sf.Joystick.Axis");
    LUASF_STUB_DOC("The Y axis of the point-of-view hat");
    LUASF_STUB_FIELD("PovY", "sf.Joystick.Axis");
    sf_Joystick.new_enum("Axis",
        "X", sf::Joystick::Axis::X,
        "Y", sf::Joystick::Axis::Y,
        "Z", sf::Joystick::Axis::Z,
        "R", sf::Joystick::Axis::R,
        "U", sf::Joystick::Axis::U,
        "V", sf::Joystick::Axis::V,
        "PovX", sf::Joystick::Axis::PovX,
        "PovY", sf::Joystick::Axis::PovY
    );
    auto type_sf__Joystick__Identification = sf_Joystick.new_usertype<sf::Joystick::Identification>("Identification", sol::no_constructor);
    sol::table table_sf__Joystick__Identification = sf_Joystick["Identification"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Joystick::Identification>(lua);
    LUASF_STUB_DOC("\\brief Structure holding a joystick's identification");
    LUASF_STUB_CLASS("sf.Joystick.Identification");
    LUASF_STUB_DOC("Name of the joystick");
    LUASF_STUB_FIELD("name", "string");
    LUASF_STUB_DOC("Manufacturer identifier");
    LUASF_STUB_FIELD("vendorId", "integer");
    LUASF_STUB_DOC("Product identifier");
    LUASF_STUB_FIELD("productId", "integer");
    LUASF_STUB_FUNCTION("sf.Joystick.Identification", "new", "fun(): sf.Joystick.Identification");
    type_sf__Joystick__Identification.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Joystick::Identification>();
        }
    ));
    type_sf__Joystick__Identification.set("name", sol::property(
        [](sf::Joystick::Identification& self) -> std::string {
            return lua_sf::to_utf8_string(self.name);
        },
        [](sf::Joystick::Identification& self, std::string value) {
            self.name = lua_sf::to_sf_string(value);
        }
    ));
    type_sf__Joystick__Identification.set("vendorId", sol::property(
        [](sf::Joystick::Identification& self) {
            return self.vendorId;
        },
        [](sf::Joystick::Identification& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.vendorId = value.value();
        }
    ));
    type_sf__Joystick__Identification.set("productId", sol::property(
        [](sf::Joystick::Identification& self) {
            return self.productId;
        },
        [](sf::Joystick::Identification& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.productId = value.value();
        }
    ));
    LUASF_STUB_DOC("\\brief Check if a joystick is connected\n\n\\param joystick Index of the joystick to check\n\n\\return `true` if the joystick is connected, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Joystick", "isConnected", "fun(joystick: integer): boolean");
    sf_Joystick.set_function("isConnected",
        [](lua_sf::LuaIntegral<unsigned int> joystick) -> bool {
            return sf::Joystick::isConnected(joystick.value());
        }
    );
    LUASF_STUB_DOC("\\brief Return the number of buttons supported by a joystick\n\nIf the joystick is not connected, this function returns 0.\n\n\\param joystick Index of the joystick\n\n\\return Number of buttons supported by the joystick");
    LUASF_STUB_FUNCTION("sf.Joystick", "getButtonCount", "fun(joystick: integer): integer");
    sf_Joystick.set_function("getButtonCount",
        [](lua_sf::LuaIntegral<unsigned int> joystick) -> unsigned int {
            return sf::Joystick::getButtonCount(joystick.value());
        }
    );
    LUASF_STUB_DOC("\\brief Check if a joystick supports a given axis\n\nIf the joystick is not connected, this function returns `false`.\n\n\\param joystick Index of the joystick\n\\param axis     Axis to check\n\n\\return `true` if the joystick supports the axis, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Joystick", "hasAxis", "fun(joystick: integer, axis: sf.Joystick.Axis): boolean");
    sf_Joystick.set_function("hasAxis",
        [](lua_sf::LuaIntegral<unsigned int> joystick, sf::Joystick::Axis axis) -> bool {
            return sf::Joystick::hasAxis(joystick.value(), axis);
        }
    );
    LUASF_STUB_DOC("\\brief Check if a joystick button is pressed\n\nIf the joystick is not connected, this function returns `false`.\n\n\\param joystick Index of the joystick\n\\param button   Button to check\n\n\\return `true` if the button is pressed, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Joystick", "isButtonPressed", "fun(joystick: integer, button: integer): boolean");
    sf_Joystick.set_function("isButtonPressed",
        [](lua_sf::LuaIntegral<unsigned int> joystick, lua_sf::LuaIntegral<unsigned int> button) -> bool {
            return sf::Joystick::isButtonPressed(joystick.value(), button.value());
        }
    );
    LUASF_STUB_DOC("\\brief Get the current position of a joystick axis\n\nIf the joystick is not connected, this function returns 0.\n\n\\param joystick Index of the joystick\n\\param axis     Axis to check\n\n\\return Current position of the axis, in range [-100 .. 100]");
    LUASF_STUB_FUNCTION("sf.Joystick", "getAxisPosition", "fun(joystick: integer, axis: sf.Joystick.Axis): number");
    sf_Joystick.set_function("getAxisPosition",
        [](lua_sf::LuaIntegral<unsigned int> joystick, sf::Joystick::Axis axis) -> float {
            return sf::Joystick::getAxisPosition(joystick.value(), axis);
        }
    );
    LUASF_STUB_DOC("\\brief Get the joystick information\n\n\\param joystick Index of the joystick\n\n\\return Structure containing joystick information.");
    LUASF_STUB_FUNCTION("sf.Joystick", "getIdentification", "fun(joystick: integer): sf.Joystick.Identification");
    sf_Joystick.set_function("getIdentification",
        [](lua_sf::LuaIntegral<unsigned int> joystick) -> sf::Joystick::Identification {
            return sf::Joystick::getIdentification(joystick.value());
        }
    );
    LUASF_STUB_DOC("\\brief Update the states of all joysticks\n\nThis function is used internally by SFML, so you normally\ndon't have to call it explicitly. However, you may need to\ncall it if you have no window yet (or no window at all):\nin this case the joystick states are not updated automatically.");
    LUASF_STUB_FUNCTION("sf.Joystick", "update", "fun()");
    sf_Joystick.set_function("update",
        []() {
            sf::Joystick::update();
        }
    );
}
