#include "Graphics/bind_StencilMode.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_StencilMode(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    LUASF_STUB_DOC("\\brief Enumeration of the stencil test comparisons that can be performed\n\nThe comparisons are mapped directly to their OpenGL equivalents,\nspecified by `glStencilFunc()`.");
    LUASF_STUB_CLASS("sf.StencilComparison");
    LUASF_STUB_DOC("The stencil test never passes");
    LUASF_STUB_FIELD("Never", "sf.StencilComparison");
    LUASF_STUB_DOC("The stencil test passes if the new value is less than the value in the stencil buffer");
    LUASF_STUB_FIELD("Less", "sf.StencilComparison");
    LUASF_STUB_DOC("The stencil test passes if the new value is less than or equal to the value in the stencil buffer");
    LUASF_STUB_FIELD("LessEqual", "sf.StencilComparison");
    LUASF_STUB_DOC("The stencil test passes if the new value is greater than the value in the stencil buffer");
    LUASF_STUB_FIELD("Greater", "sf.StencilComparison");
    LUASF_STUB_DOC("The stencil test passes if the new value is greater than or equal to the value in the stencil buffer");
    LUASF_STUB_FIELD("GreaterEqual", "sf.StencilComparison");
    LUASF_STUB_DOC("The stencil test passes if the new value is strictly equal to the value in the stencil buffer");
    LUASF_STUB_FIELD("Equal", "sf.StencilComparison");
    LUASF_STUB_DOC("The stencil test passes if the new value is strictly unequal to the value in the stencil buffer");
    LUASF_STUB_FIELD("NotEqual", "sf.StencilComparison");
    LUASF_STUB_DOC("The stencil test always passes");
    LUASF_STUB_FIELD("Always", "sf.StencilComparison");
    sf.new_enum("StencilComparison",
        "Never", sf::StencilComparison::Never,
        "Less", sf::StencilComparison::Less,
        "LessEqual", sf::StencilComparison::LessEqual,
        "Greater", sf::StencilComparison::Greater,
        "GreaterEqual", sf::StencilComparison::GreaterEqual,
        "Equal", sf::StencilComparison::Equal,
        "NotEqual", sf::StencilComparison::NotEqual,
        "Always", sf::StencilComparison::Always
    );
    LUASF_STUB_DOC("\\brief Enumeration of the stencil buffer update operations\n\nThe update operations are mapped directly to their OpenGL equivalents,\nspecified by `glStencilOp()`.");
    LUASF_STUB_CLASS("sf.StencilUpdateOperation");
    LUASF_STUB_DOC("If the stencil test passes, the value in the stencil buffer is not modified");
    LUASF_STUB_FIELD("Keep", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC("If the stencil test passes, the value in the stencil buffer is set to zero");
    LUASF_STUB_FIELD("Zero", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC("If the stencil test passes, the value in the stencil buffer is set to the new value");
    LUASF_STUB_FIELD("Replace", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC("If the stencil test passes, the value in the stencil buffer is incremented and if required clamped");
    LUASF_STUB_FIELD("Increment", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC("If the stencil test passes, the value in the stencil buffer is decremented and if required clamped");
    LUASF_STUB_FIELD("Decrement", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC("If the stencil test passes, the value in the stencil buffer is bitwise inverted");
    LUASF_STUB_FIELD("Invert", "sf.StencilUpdateOperation");
    sf.new_enum("StencilUpdateOperation",
        "Keep", sf::StencilUpdateOperation::Keep,
        "Zero", sf::StencilUpdateOperation::Zero,
        "Replace", sf::StencilUpdateOperation::Replace,
        "Increment", sf::StencilUpdateOperation::Increment,
        "Decrement", sf::StencilUpdateOperation::Decrement,
        "Invert", sf::StencilUpdateOperation::Invert
    );
    auto type_sf__StencilValue = sf.new_usertype<sf::StencilValue>("StencilValue", sol::no_constructor);
    sol::table table_sf__StencilValue = sf["StencilValue"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::StencilValue>(lua);
    LUASF_STUB_DOC("\\brief Stencil value type (also used as a mask)");
    LUASF_STUB_CLASS("sf.StencilValue");
    LUASF_STUB_DOC("The stored stencil value");
    LUASF_STUB_FIELD("value", "integer");
    LUASF_STUB_DOC("\\brief Construct a stencil value from a signed integer\n\n\\param theValue Signed integer value to use");
    LUASF_STUB_FUNCTION("sf.StencilValue", "new", "fun(theValue: integer): sf.StencilValue");
    LUASF_STUB_OVERLOAD("sf.StencilValue", "new", "fun(theValue: integer): sf.StencilValue");
    type_sf__StencilValue.set_function("new", sol::factories(
        [](lua_sf::LuaIntegral<int> theValue) {
            return lua_sf::makeLuaSharedObject<sf::StencilValue>(theValue.value());
        },
        [](lua_sf::LuaIntegral<unsigned int> theValue) {
            return lua_sf::makeLuaSharedObject<sf::StencilValue>(theValue.value());
        }
    ));
    type_sf__StencilValue.set("value", sol::property(
        [](sf::StencilValue& self) {
            return self.value;
        },
        [](sf::StencilValue& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.value = value.value();
        }
    ));
    auto type_sf__StencilMode = sf.new_usertype<sf::StencilMode>("StencilMode", sol::no_constructor);
    sol::table table_sf__StencilMode = sf["StencilMode"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::StencilMode>(lua);
    LUASF_STUB_DOC("\\brief Stencil modes for drawing");
    LUASF_STUB_CLASS("sf.StencilMode");
    LUASF_STUB_DOC("The comparison we're performing the stencil test with");
    LUASF_STUB_FIELD("stencilComparison", "sf.StencilComparison");
    LUASF_STUB_FIELD("stencilUpdateOperation", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC("The reference value we're performing the stencil test with");
    LUASF_STUB_FIELD("stencilReference", "sf.StencilValue");
    LUASF_STUB_DOC("The mask to apply to both the reference value and the value in the stencil buffer");
    LUASF_STUB_FIELD("stencilMask", "sf.StencilValue");
    LUASF_STUB_DOC("Whether we should update the color buffer in addition to the stencil buffer");
    LUASF_STUB_FIELD("stencilOnly", "boolean");
    LUASF_STUB_FUNCTION("sf.StencilMode", "new", "fun(): sf.StencilMode");
    type_sf__StencilMode.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::StencilMode>();
        }
    ));
    type_sf__StencilMode["stencilComparison"] = sol::policies(&sf::StencilMode::stencilComparison, sol::self_dependency{});
    type_sf__StencilMode["stencilUpdateOperation"] = sol::policies(&sf::StencilMode::stencilUpdateOperation, sol::self_dependency{});
    type_sf__StencilMode["stencilReference"] = sol::policies(&sf::StencilMode::stencilReference, sol::self_dependency{});
    type_sf__StencilMode["stencilMask"] = sol::policies(&sf::StencilMode::stencilMask, sol::self_dependency{});
    type_sf__StencilMode["stencilOnly"] = sol::policies(&sf::StencilMode::stencilOnly, sol::self_dependency{});
}
