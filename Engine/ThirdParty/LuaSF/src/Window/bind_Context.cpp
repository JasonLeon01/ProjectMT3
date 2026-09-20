#include "Window/bind_Context.hpp"

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

namespace { constexpr std::array<std::string_view, 8> docs = {
    "\\brief Class holding a valid drawing context",
    "\\brief Default constructor\n\nThe constructor creates and activates the context",
    "\\brief Construct a in-memory context\n\nThis constructor is for internal use, you don't need\nto bother with it.\n\n\\param settings Creation parameters\n\\param size     Back buffer size",
    "\\brief Activate or deactivate explicitly the context\n\n\\param active `true` to activate, `false` to deactivate\n\n\\return `true` on success, `false` on failure",
    "\\brief Get the settings of the context\n\nNote that these settings may be different than the ones\npassed to the constructor; they are indeed adjusted if the\noriginal settings are not directly supported by the system.\n\n\\return Structure containing the settings",
    "\\brief Check whether a given OpenGL extension is available\n\n\\param name Name of the extension to check for\n\n\\return `true` if available, `false` if unavailable",
    "\\brief Get the currently active context\n\nThis function will only return `sf::Context` objects.\nContexts created e.g. by RenderTargets or for internal\nuse will not be returned by this function.\n\n\\return The currently active context or `nullptr` if none is active",
    "\\brief Get the currently active context's ID\n\nThe context ID is used to identify contexts when\nmanaging unshareable OpenGL resources.\n\n\\return The active context's ID or 0 if no context is currently active",
}; }

void bind_Context(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    LUASF_STUB_ALIAS("sf.GlFunctionPointer", "sf.void (*)()");
    {
        const lua_glue::Object aliasValue = sf.raw_get<lua_glue::Object>("GlFunctionPointer");
        const lua_glue::Object aliasTarget = sf.raw_get<lua_glue::Object>("void (*)()");
        if ((!aliasValue.valid() || aliasValue.get_type() == lua_glue::Type::Nil) &&
            aliasTarget.valid() && aliasTarget.get_type() != lua_glue::Type::Nil)
            sf.raw_set("GlFunctionPointer", aliasTarget);
    }
    auto type_sf__Context = lua_glue::BindClass<sf::Context>(sf, "Context");
    lua_glue::Table table_sf__Context = sf["Context"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Context>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Context");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Context", "new", "fun(settings: sf.ContextSettings, size: sf.Vector2u): sf.Context");
    LUASF_STUB_OVERLOAD("sf.Context", "new", "fun(): sf.Context");
    lua_glue::BindCallable(type_sf__Context, "new",
        [](const sf::ContextSettings& settings, sf::Vector2u size) {
            return lua_sf::makeLuaSharedObject<sf::Context>(settings, size);
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__Context, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Context>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Context", "setActive", "fun(self: sf.Context, active: boolean): boolean");
    lua_glue::BindCallable(type_sf__Context, "setActive",
        [](sf::Context& self, bool active) -> bool {
            return self.setActive(active);
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Context", "getSettings", "fun(self: sf.Context): sf.ContextSettings");
    lua_glue::BindCallable(type_sf__Context, "getSettings",
        [](const sf::Context& self) {
            return std::cref(self.getSettings());
        },
        docs[4],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Context", "isExtensionAvailable", "fun(name: string): boolean");
    lua_glue::BindCallable(type_sf__Context, "isExtensionAvailable",
        [](std::string name) -> bool {
            return sf::Context::isExtensionAvailable(name);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Context", "getActiveContext", "fun(): sf.Context");
    lua_glue::BindCallable(type_sf__Context, "getActiveContext",
        []() -> const sf::Context* {
            return sf::Context::getActiveContext();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Context", "getActiveContextId", "fun(): integer");
    lua_glue::BindCallable(type_sf__Context, "getActiveContextId",
        []() -> std::uint64_t {
            return sf::Context::getActiveContextId();
        },
        docs[7]
    );
    // Skipped sf::Context::getFunction(const char *): unsupported return type sf::GlFunctionPointer.
}
