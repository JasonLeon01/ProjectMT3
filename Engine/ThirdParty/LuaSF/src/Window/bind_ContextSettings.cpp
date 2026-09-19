#include "Window/bind_ContextSettings.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_ContextSettings(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__ContextSettings = sf.new_usertype<sf::ContextSettings>("ContextSettings", sol::no_constructor);
    sol::table table_sf__ContextSettings = sf["ContextSettings"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::ContextSettings>(lua);
    LUASF_STUB_DOC("\\brief Structure defining the settings of the OpenGL\ncontext attached to a window");
    LUASF_STUB_CLASS("sf.ContextSettings");
    LUASF_STUB_DOC("Bits of the depth buffer");
    LUASF_STUB_FIELD("depthBits", "integer");
    LUASF_STUB_DOC("Bits of the stencil buffer");
    LUASF_STUB_FIELD("stencilBits", "integer");
    LUASF_STUB_DOC("Level of anti-aliasing");
    LUASF_STUB_FIELD("antiAliasingLevel", "integer");
    LUASF_STUB_DOC("Major number of the context version to create");
    LUASF_STUB_FIELD("majorVersion", "integer");
    LUASF_STUB_DOC("Minor number of the context version to create");
    LUASF_STUB_FIELD("minorVersion", "integer");
    LUASF_STUB_DOC("The attribute flags to create the context with");
    LUASF_STUB_FIELD("attributeFlags", "integer");
    LUASF_STUB_DOC("Whether the context framebuffer is sRGB capable");
    LUASF_STUB_FIELD("sRgbCapable", "boolean");
    LUASF_STUB_FUNCTION("sf.ContextSettings", "new", "fun(): sf.ContextSettings");
    type_sf__ContextSettings.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::ContextSettings>();
        }
    ));
    type_sf__ContextSettings.set("depthBits", sol::property(
        [](sf::ContextSettings& self) {
            return self.depthBits;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.depthBits = value.value();
        }
    ));
    type_sf__ContextSettings.set("stencilBits", sol::property(
        [](sf::ContextSettings& self) {
            return self.stencilBits;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.stencilBits = value.value();
        }
    ));
    type_sf__ContextSettings.set("antiAliasingLevel", sol::property(
        [](sf::ContextSettings& self) {
            return self.antiAliasingLevel;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.antiAliasingLevel = value.value();
        }
    ));
    type_sf__ContextSettings.set("majorVersion", sol::property(
        [](sf::ContextSettings& self) {
            return self.majorVersion;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.majorVersion = value.value();
        }
    ));
    type_sf__ContextSettings.set("minorVersion", sol::property(
        [](sf::ContextSettings& self) {
            return self.minorVersion;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.minorVersion = value.value();
        }
    ));
    type_sf__ContextSettings.set("attributeFlags", sol::property(
        [](sf::ContextSettings& self) {
            return self.attributeFlags;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<std::uint32_t> value) {
            self.attributeFlags = value.value();
        }
    ));
    type_sf__ContextSettings["sRgbCapable"] = sol::policies(&sf::ContextSettings::sRgbCapable, sol::self_dependency{});
    LUASF_STUB_DOC("\\brief Enumeration of the context attribute flags");
    LUASF_STUB_CLASS("sf.ContextSettings.Attribute");
    LUASF_STUB_DOC("Non-debug, compatibility context (this and the core attribute are mutually exclusive)");
    LUASF_STUB_FIELD("Default", "integer");
    LUASF_STUB_DOC("Core attribute");
    LUASF_STUB_FIELD("Core", "integer");
    LUASF_STUB_DOC("Debug attribute");
    LUASF_STUB_FIELD("Debug", "integer");
    table_sf__ContextSettings.new_enum("Attribute",
        "Default", sf::ContextSettings::Attribute::Default,
        "Core", sf::ContextSettings::Attribute::Core,
        "Debug", sf::ContextSettings::Attribute::Debug
    );
}
