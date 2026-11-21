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

namespace { constexpr std::array<std::string_view, 4> docs = {
    "The profile requested for a desktop OpenGL context.",
    "Minimum Direct3D feature level, using the Direct3D 11 API.\nSupported levels are 10_0, 10_1, 11_0, 11_1, 12_0 and 12_1.",
    "Selects the Metal backend on macOS and iOS.\nThe available Metal version is determined by the deployment target\n(macOS 13.0 / iOS 16.0 at minimum), so no version is requested here.",
    "Framebuffer options and minimum API version for a rendering target.\nAn empty backend setting inherits the current graphics device's configuration.\nWindow::getSettings() reports the actual settings selected by the driver.",
}; }

void bind_ContextSettings(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.OpenGLProfile");
    LUASF_STUB_FIELD("Automatic", "sf.OpenGLProfile");
    LUASF_STUB_FIELD("Compatibility", "sf.OpenGLProfile");
    LUASF_STUB_FIELD("Core", "sf.OpenGLProfile");
    lua_glue::BindEnum<sf::OpenGLProfile>(sf, "OpenGLProfile", {
        {"Automatic", sf::OpenGLProfile::Automatic},
        {"Compatibility", sf::OpenGLProfile::Compatibility},
        {"Core", sf::OpenGLProfile::Core}
    });
    auto type_sf__OpenGLSettings = lua_glue::BindClass<sf::OpenGLSettings>(sf, "OpenGLSettings");
    lua_glue::Table table_sf__OpenGLSettings = sf["OpenGLSettings"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::OpenGLSettings>(lua);
    LUASF_STUB_CLASS("sf.OpenGLSettings");
    LUASF_STUB_FIELD("majorVersion", "integer");
    LUASF_STUB_FIELD("minorVersion", "integer");
    LUASF_STUB_FIELD("profile", "sf.OpenGLProfile");
    LUASF_STUB_FIELD("debug", "boolean");
    LUASF_STUB_FUNCTION("sf.OpenGLSettings", "new", "fun(): sf.OpenGLSettings");
    lua_glue::BindCallable(type_sf__OpenGLSettings, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::OpenGLSettings>();
        }
    );
    lua_glue::BindProperty(type_sf__OpenGLSettings, "majorVersion",
        [](const sf::OpenGLSettings& self) {
            return self.majorVersion;
        },
        [](sf::OpenGLSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.majorVersion = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__OpenGLSettings, "minorVersion",
        [](const sf::OpenGLSettings& self) {
            return self.minorVersion;
        },
        [](sf::OpenGLSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.minorVersion = value.value();
        }
    );
    lua_glue::BindAttr<sf::OpenGLProfile>(type_sf__OpenGLSettings, "profile", &sf::OpenGLSettings::profile);
    lua_glue::BindAttr<bool>(type_sf__OpenGLSettings, "debug", &sf::OpenGLSettings::debug);
    auto type_sf__OpenGLESSettings = lua_glue::BindClass<sf::OpenGLESSettings>(sf, "OpenGLESSettings");
    lua_glue::Table table_sf__OpenGLESSettings = sf["OpenGLESSettings"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::OpenGLESSettings>(lua);
    LUASF_STUB_CLASS("sf.OpenGLESSettings");
    LUASF_STUB_FIELD("majorVersion", "integer");
    LUASF_STUB_FIELD("minorVersion", "integer");
    LUASF_STUB_FUNCTION("sf.OpenGLESSettings", "new", "fun(): sf.OpenGLESSettings");
    lua_glue::BindCallable(type_sf__OpenGLESSettings, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::OpenGLESSettings>();
        }
    );
    lua_glue::BindProperty(type_sf__OpenGLESSettings, "majorVersion",
        [](const sf::OpenGLESSettings& self) {
            return self.majorVersion;
        },
        [](sf::OpenGLESSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.majorVersion = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__OpenGLESSettings, "minorVersion",
        [](const sf::OpenGLESSettings& self) {
            return self.minorVersion;
        },
        [](sf::OpenGLESSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.minorVersion = value.value();
        }
    );
    auto type_sf__VulkanSettings = lua_glue::BindClass<sf::VulkanSettings>(sf, "VulkanSettings");
    lua_glue::Table table_sf__VulkanSettings = sf["VulkanSettings"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::VulkanSettings>(lua);
    LUASF_STUB_CLASS("sf.VulkanSettings");
    LUASF_STUB_FIELD("majorVersion", "integer");
    LUASF_STUB_FIELD("minorVersion", "integer");
    LUASF_STUB_FUNCTION("sf.VulkanSettings", "new", "fun(): sf.VulkanSettings");
    lua_glue::BindCallable(type_sf__VulkanSettings, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::VulkanSettings>();
        }
    );
    lua_glue::BindProperty(type_sf__VulkanSettings, "majorVersion",
        [](const sf::VulkanSettings& self) {
            return self.majorVersion;
        },
        [](sf::VulkanSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.majorVersion = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__VulkanSettings, "minorVersion",
        [](const sf::VulkanSettings& self) {
            return self.minorVersion;
        },
        [](sf::VulkanSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.minorVersion = value.value();
        }
    );
    auto type_sf__DirectXSettings = lua_glue::BindClass<sf::DirectXSettings>(sf, "DirectXSettings");
    lua_glue::Table table_sf__DirectXSettings = sf["DirectXSettings"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::DirectXSettings>(lua);
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_CLASS("sf.DirectXSettings");
    LUASF_STUB_FIELD("majorVersion", "integer");
    LUASF_STUB_FIELD("minorVersion", "integer");
    LUASF_STUB_FUNCTION("sf.DirectXSettings", "new", "fun(): sf.DirectXSettings");
    lua_glue::BindCallable(type_sf__DirectXSettings, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::DirectXSettings>();
        }
    );
    lua_glue::BindProperty(type_sf__DirectXSettings, "majorVersion",
        [](const sf::DirectXSettings& self) {
            return self.majorVersion;
        },
        [](sf::DirectXSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.majorVersion = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__DirectXSettings, "minorVersion",
        [](const sf::DirectXSettings& self) {
            return self.minorVersion;
        },
        [](sf::DirectXSettings& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.minorVersion = value.value();
        }
    );
    auto type_sf__MetalSettings = lua_glue::BindClass<sf::MetalSettings>(sf, "MetalSettings");
    lua_glue::Table table_sf__MetalSettings = sf["MetalSettings"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::MetalSettings>(lua);
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_CLASS("sf.MetalSettings");
    LUASF_STUB_FUNCTION("sf.MetalSettings", "new", "fun(): sf.MetalSettings");
    lua_glue::BindCallable(type_sf__MetalSettings, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::MetalSettings>();
        }
    );
    auto type_sf__ContextSettings = lua_glue::BindStruct<sf::ContextSettings>(sf, "ContextSettings");
    lua_glue::Table table_sf__ContextSettings = sf["ContextSettings"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_CLASS("sf.ContextSettings");
    LUASF_STUB_FIELD("depthBits", "integer");
    LUASF_STUB_FIELD("stencilBits", "integer");
    LUASF_STUB_FIELD("antiAliasingLevel", "integer");
    LUASF_STUB_FIELD("sRgbCapable", "boolean");
    LUASF_STUB_FIELD("backend", "nil|sf.OpenGLSettings|sf.OpenGLESSettings|sf.VulkanSettings|sf.DirectXSettings|sf.MetalSettings");
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
    lua_glue::BindAttr<bool>(type_sf__ContextSettings, "sRgbCapable", &sf::ContextSettings::sRgbCapable);
    lua_glue::BindAttr<std::variant<std::monostate, sf::OpenGLSettings, sf::OpenGLESSettings, sf::VulkanSettings, sf::DirectXSettings, sf::MetalSettings>>(type_sf__ContextSettings, "backend", &sf::ContextSettings::backend);
    LUASF_STUB_ALIAS("sf.ContextSettings.BackendSettings", "nil|sf.OpenGLSettings|sf.OpenGLESSettings|sf.VulkanSettings|sf.DirectXSettings|sf.MetalSettings");
    LUASF_STUB_FUNCTION("sf.ContextSettings", "copy", "fun(self: sf.ContextSettings): sf.ContextSettings");
    LUASF_STUB_FUNCTION("sf.ContextSettings", "deepcopy", "fun(self: sf.ContextSettings): sf.ContextSettings");
}
