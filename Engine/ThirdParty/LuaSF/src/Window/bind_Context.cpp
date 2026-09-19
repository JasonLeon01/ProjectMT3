#include "Window/bind_Context.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Context(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    LUASF_STUB_ALIAS("sf.GlFunctionPointer", "sf.void (*)()");
    {
        const sol::object aliasValue = sf.raw_get<sol::object>("GlFunctionPointer");
        const sol::object aliasTarget = sf.raw_get<sol::object>("void (*)()");
        if ((!aliasValue.valid() || aliasValue.get_type() == sol::type::lua_nil) &&
            aliasTarget.valid() && aliasTarget.get_type() != sol::type::lua_nil)
            sf.raw_set("GlFunctionPointer", aliasTarget);
    }
    auto type_sf__Context = sf.new_usertype<sf::Context>("Context", sol::no_constructor);
    sol::table table_sf__Context = sf["Context"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Context>(lua);
    LUASF_STUB_DOC("\\brief Class holding a valid drawing context");
    LUASF_STUB_CLASS("sf.Context");
    LUASF_STUB_DOC("\\brief Construct a in-memory context\n\nThis constructor is for internal use, you don't need\nto bother with it.\n\n\\param settings Creation parameters\n\\param size     Back buffer size");
    LUASF_STUB_FUNCTION("sf.Context", "new", "fun(settings: sf.ContextSettings, size: sf.Vector2u): sf.Context");
    LUASF_STUB_OVERLOAD("sf.Context", "new", "fun(): sf.Context");
    type_sf__Context.set_function("new", sol::factories(
        [](const sf::ContextSettings& settings, sf::Vector2u size) {
            return lua_sf::makeLuaSharedObject<sf::Context>(settings, size);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::Context>();
        }
    ));
    LUASF_STUB_DOC("\\brief Activate or deactivate explicitly the context\n\n\\param active `true` to activate, `false` to deactivate\n\n\\return `true` on success, `false` on failure");
    LUASF_STUB_FUNCTION("sf.Context", "setActive", "fun(self: sf.Context, active: boolean): boolean");
    type_sf__Context.set_function("setActive",
        [](sf::Context& self, bool active) -> bool {
            return self.setActive(active);
        }
    );
    LUASF_STUB_DOC("\\brief Get the settings of the context\n\nNote that these settings may be different than the ones\npassed to the constructor; they are indeed adjusted if the\noriginal settings are not directly supported by the system.\n\n\\return Structure containing the settings");
    LUASF_STUB_FUNCTION("sf.Context", "getSettings", "fun(self: sf.Context): sf.ContextSettings");
    type_sf__Context.set_function("getSettings",
        sol::policies(
            [](sf::Context& self) {
                return std::cref(self.getSettings());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Check whether a given OpenGL extension is available\n\n\\param name Name of the extension to check for\n\n\\return `true` if available, `false` if unavailable");
    LUASF_STUB_FUNCTION("sf.Context", "isExtensionAvailable", "fun(name: string): boolean");
    type_sf__Context.set_function("isExtensionAvailable",
        [](std::string name) -> bool {
            return sf::Context::isExtensionAvailable(name);
        }
    );
    LUASF_STUB_DOC("\\brief Get the currently active context\n\nThis function will only return `sf::Context` objects.\nContexts created e.g. by RenderTargets or for internal\nuse will not be returned by this function.\n\n\\return The currently active context or `nullptr` if none is active");
    LUASF_STUB_FUNCTION("sf.Context", "getActiveContext", "fun(): sf.Context");
    type_sf__Context.set_function("getActiveContext",
        []() -> const sf::Context* {
            return sf::Context::getActiveContext();
        }
    );
    LUASF_STUB_DOC("\\brief Get the currently active context's ID\n\nThe context ID is used to identify contexts when\nmanaging unshareable OpenGL resources.\n\n\\return The active context's ID or 0 if no context is currently active");
    LUASF_STUB_FUNCTION("sf.Context", "getActiveContextId", "fun(): integer");
    type_sf__Context.set_function("getActiveContextId",
        []() -> std::uint64_t {
            return sf::Context::getActiveContextId();
        }
    );
    // Skipped sf::Context::getFunction(const char *): unsupported return type sf::GlFunctionPointer.
}
