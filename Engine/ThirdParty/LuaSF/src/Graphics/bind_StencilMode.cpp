#include "Graphics/bind_StencilMode.hpp"

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

namespace { constexpr std::array<std::string_view, 25> docs = {
    "\\brief Enumeration of the stencil test comparisons that can be performed\n\nThe comparisons are mapped directly to their OpenGL equivalents,\nspecified by `glStencilFunc()`.",
    "The stencil test never passes",
    "The stencil test passes if the new value is less than the value in the stencil buffer",
    "The stencil test passes if the new value is less than or equal to the value in the stencil buffer",
    "The stencil test passes if the new value is greater than the value in the stencil buffer",
    "The stencil test passes if the new value is greater than or equal to the value in the stencil buffer",
    "The stencil test passes if the new value is strictly equal to the value in the stencil buffer",
    "The stencil test passes if the new value is strictly unequal to the value in the stencil buffer",
    "The stencil test always passes",
    "\\brief Enumeration of the stencil buffer update operations\n\nThe update operations are mapped directly to their OpenGL equivalents,\nspecified by `glStencilOp()`.",
    "If the stencil test passes, the value in the stencil buffer is not modified",
    "If the stencil test passes, the value in the stencil buffer is set to zero",
    "If the stencil test passes, the value in the stencil buffer is set to the new value",
    "If the stencil test passes, the value in the stencil buffer is incremented and if required clamped",
    "If the stencil test passes, the value in the stencil buffer is decremented and if required clamped",
    "If the stencil test passes, the value in the stencil buffer is bitwise inverted",
    "\\brief Stencil value type (also used as a mask)",
    "The stored stencil value",
    "\\brief Construct a stencil value from a signed integer\n\n\\param theValue Signed integer value to use",
    "\\brief Construct a stencil value from an unsigned integer\n\n\\param theValue Unsigned integer value to use",
    "\\brief Stencil modes for drawing",
    "The comparison we're performing the stencil test with",
    "The reference value we're performing the stencil test with",
    "The mask to apply to both the reference value and the value in the stencil buffer",
    "Whether we should update the color buffer in addition to the stencil buffer",
}; }

void bind_StencilMode(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.StencilComparison");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("Never", "sf.StencilComparison");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("Less", "sf.StencilComparison");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("LessEqual", "sf.StencilComparison");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("Greater", "sf.StencilComparison");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("GreaterEqual", "sf.StencilComparison");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("Equal", "sf.StencilComparison");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FIELD("NotEqual", "sf.StencilComparison");
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FIELD("Always", "sf.StencilComparison");
    lua_glue::BindEnum<sf::StencilComparison>(sf, "StencilComparison", {
        {"Never", sf::StencilComparison::Never},
        {"Less", sf::StencilComparison::Less},
        {"LessEqual", sf::StencilComparison::LessEqual},
        {"Greater", sf::StencilComparison::Greater},
        {"GreaterEqual", sf::StencilComparison::GreaterEqual},
        {"Equal", sf::StencilComparison::Equal},
        {"NotEqual", sf::StencilComparison::NotEqual},
        {"Always", sf::StencilComparison::Always}
    });
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_CLASS("sf.StencilUpdateOperation");
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FIELD("Keep", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FIELD("Zero", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FIELD("Replace", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FIELD("Increment", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FIELD("Decrement", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FIELD("Invert", "sf.StencilUpdateOperation");
    lua_glue::BindEnum<sf::StencilUpdateOperation>(sf, "StencilUpdateOperation", {
        {"Keep", sf::StencilUpdateOperation::Keep},
        {"Zero", sf::StencilUpdateOperation::Zero},
        {"Replace", sf::StencilUpdateOperation::Replace},
        {"Increment", sf::StencilUpdateOperation::Increment},
        {"Decrement", sf::StencilUpdateOperation::Decrement},
        {"Invert", sf::StencilUpdateOperation::Invert}
    });
    auto type_sf__StencilValue = lua_glue::BindClass<sf::StencilValue>(sf, "StencilValue");
    lua_glue::Table table_sf__StencilValue = sf["StencilValue"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::StencilValue>(lua);
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_CLASS("sf.StencilValue");
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FIELD("value", "integer");
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.StencilValue", "new", "fun(theValue: integer): sf.StencilValue");
    LUASF_STUB_OVERLOAD("sf.StencilValue", "new", "fun(theValue: integer): sf.StencilValue");
    lua_glue::BindCallable(type_sf__StencilValue, "new",
        [](lua_sf::LuaIntegral<int> theValue) {
            return lua_sf::makeLuaSharedObject<sf::StencilValue>(theValue.value());
        },
        docs[18]
    );
    lua_glue::BindCallable(type_sf__StencilValue, "new",
        [](lua_sf::LuaIntegral<unsigned int> theValue) {
            return lua_sf::makeLuaSharedObject<sf::StencilValue>(theValue.value());
        },
        docs[19]
    );
    lua_glue::BindProperty(type_sf__StencilValue, "value",
        [](const sf::StencilValue& self) {
            return self.value;
        },
        [](sf::StencilValue& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.value = value.value();
        }
    );
    auto type_sf__StencilMode = lua_glue::BindClass<sf::StencilMode>(sf, "StencilMode");
    lua_glue::Table table_sf__StencilMode = sf["StencilMode"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::StencilMode>(lua);
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_CLASS("sf.StencilMode");
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FIELD("stencilComparison", "sf.StencilComparison");
    LUASF_STUB_FIELD("stencilUpdateOperation", "sf.StencilUpdateOperation");
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FIELD("stencilReference", "sf.StencilValue");
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FIELD("stencilMask", "sf.StencilValue");
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FIELD("stencilOnly", "boolean");
    LUASF_STUB_FUNCTION("sf.StencilMode", "new", "fun(): sf.StencilMode");
    lua_glue::BindCallable(type_sf__StencilMode, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::StencilMode>();
        }
    );
    lua_glue::BindAttr<sf::StencilComparison>(type_sf__StencilMode, "stencilComparison", &sf::StencilMode::stencilComparison);
    lua_glue::BindAttr<sf::StencilUpdateOperation>(type_sf__StencilMode, "stencilUpdateOperation", &sf::StencilMode::stencilUpdateOperation);
    lua_glue::BindAttr<sf::StencilValue>(type_sf__StencilMode, "stencilReference", &sf::StencilMode::stencilReference);
    lua_glue::BindAttr<sf::StencilValue>(type_sf__StencilMode, "stencilMask", &sf::StencilMode::stencilMask);
    lua_glue::BindAttr<bool>(type_sf__StencilMode, "stencilOnly", &sf::StencilMode::stencilOnly);
}
