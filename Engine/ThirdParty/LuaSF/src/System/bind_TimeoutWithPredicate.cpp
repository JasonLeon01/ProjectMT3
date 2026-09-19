#include "System/bind_TimeoutWithPredicate.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_TimeoutWithPredicate(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__TimeoutWithPredicate = sf.new_usertype<sf::TimeoutWithPredicate>("TimeoutWithPredicate", sol::no_constructor);
    sol::table table_sf__TimeoutWithPredicate = sf["TimeoutWithPredicate"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::TimeoutWithPredicate>(lua);
    LUASF_STUB_DOC("\\brief Utility class providing hybrid functionality\nof a timeout and a continuation predicate");
    LUASF_STUB_CLASS("sf.TimeoutWithPredicate");
    LUASF_STUB_DOC("\\brief Constructor\n\nThis constructor constructs a `TimeoutWithPredicate` object\nthat times out after the given amount of time.\n\n\\param timeout Time to timeout after");
    LUASF_STUB_FUNCTION("sf.TimeoutWithPredicate", "new", "fun(timeout: sf.Time): sf.TimeoutWithPredicate");
    LUASF_STUB_OVERLOAD("sf.TimeoutWithPredicate", "new", "fun(predicate: fun(): boolean, period: sf.Time): sf.TimeoutWithPredicate");
    LUASF_STUB_OVERLOAD("sf.TimeoutWithPredicate", "new", "fun(predicate: fun(): boolean): sf.TimeoutWithPredicate");
    type_sf__TimeoutWithPredicate.set_function("new", sol::factories(
        [](sf::Time timeout) {
            return lua_sf::makeLuaSharedObject<sf::TimeoutWithPredicate>(timeout);
        },
        [](sol::object predicate, sf::Time period) {
            return lua_sf::makeLuaSharedObject<sf::TimeoutWithPredicate>(lua_sf::callback::from_object<std::function<bool()>, lua_sf::callback::GenericCallbackCodec>(predicate, lua_sf::callback::CallbackOptions{"sf::TimeoutWithPredicate::TimeoutWithPredicate.predicate", false}), period);
        },
        [](sol::object predicate) {
            return lua_sf::makeLuaSharedObject<sf::TimeoutWithPredicate>(lua_sf::callback::from_object<std::function<bool()>, lua_sf::callback::GenericCallbackCodec>(predicate, lua_sf::callback::CallbackOptions{"sf::TimeoutWithPredicate::TimeoutWithPredicate.predicate", false}));
        }
    ));
    LUASF_STUB_DOC("\\brief Get the period\n\n\\return The period");
    LUASF_STUB_FUNCTION("sf.TimeoutWithPredicate", "getPeriod", "fun(self: sf.TimeoutWithPredicate): sf.Time");
    type_sf__TimeoutWithPredicate.set_function("getPeriod",
        sol::policies(
            [](sf::TimeoutWithPredicate& self) {
                return std::cref(self.getPeriod());
            },
            sol::self_dependency{}
        )
    );
    // Skipped sf::TimeoutWithPredicate::getPredicate(): unsupported return type const std::function<bool ()>&.
}
