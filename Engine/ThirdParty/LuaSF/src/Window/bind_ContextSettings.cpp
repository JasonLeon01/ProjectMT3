#include "Window/bind_ContextSettings.hpp"

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

namespace { constexpr std::array<std::string_view, 12> docs = {
    "\\brief Structure defining the settings of the OpenGL\ncontext attached to a window",
    "Bits of the depth buffer",
    "Bits of the stencil buffer",
    "Level of anti-aliasing",
    "Major number of the context version to create",
    "Minor number of the context version to create",
    "The attribute flags to create the context with",
    "Whether the context framebuffer is sRGB capable",
    "\\brief Enumeration of the context attribute flags",
    "Non-debug, compatibility context (this and the core attribute are mutually exclusive)",
    "Core attribute",
    "Debug attribute",
}; }

void bind_ContextSettings(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__ContextSettings = lua_glue::BindStruct<sf::ContextSettings>(sf, "ContextSettings");
    lua_glue::Table table_sf__ContextSettings = sf["ContextSettings"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.ContextSettings");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("depthBits", "integer");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("stencilBits", "integer");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("antiAliasingLevel", "integer");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("majorVersion", "integer");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("minorVersion", "integer");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("attributeFlags", "integer");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FIELD("sRgbCapable", "boolean");
    LUASF_STUB_FUNCTION("sf.ContextSettings", "new", "fun(): sf.ContextSettings");
    lua_glue::BindCallable(type_sf__ContextSettings, "new",
        []() {
            return sf::ContextSettings{};
        }
    );
    lua_glue::BindProperty(type_sf__ContextSettings, "depthBits",
        [](const sf::ContextSettings& self) {
            return self.depthBits;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.depthBits = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__ContextSettings, "stencilBits",
        [](const sf::ContextSettings& self) {
            return self.stencilBits;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.stencilBits = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__ContextSettings, "antiAliasingLevel",
        [](const sf::ContextSettings& self) {
            return self.antiAliasingLevel;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.antiAliasingLevel = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__ContextSettings, "majorVersion",
        [](const sf::ContextSettings& self) {
            return self.majorVersion;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.majorVersion = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__ContextSettings, "minorVersion",
        [](const sf::ContextSettings& self) {
            return self.minorVersion;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.minorVersion = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__ContextSettings, "attributeFlags",
        [](const sf::ContextSettings& self) {
            return self.attributeFlags;
        },
        [](sf::ContextSettings& self, lua_sf::LuaIntegral<std::uint32_t> value) {
            self.attributeFlags = value.value();
        }
    );
    lua_glue::BindAttr<bool>(type_sf__ContextSettings, "sRgbCapable", &sf::ContextSettings::sRgbCapable);
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_CLASS("sf.ContextSettings.Attribute");
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FIELD("Default", "integer");
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FIELD("Core", "integer");
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FIELD("Debug", "integer");
    lua_glue::BindEnum<sf::ContextSettings::Attribute>(table_sf__ContextSettings, "Attribute", {
        {"Default", sf::ContextSettings::Attribute::Default},
        {"Core", sf::ContextSettings::Attribute::Core},
        {"Debug", sf::ContextSettings::Attribute::Debug}
    });
    LUASF_STUB_FUNCTION("sf.ContextSettings", "copy", "fun(self: sf.ContextSettings): sf.ContextSettings");
    LUASF_STUB_FUNCTION("sf.ContextSettings", "deepcopy", "fun(self: sf.ContextSettings): sf.ContextSettings");
}
