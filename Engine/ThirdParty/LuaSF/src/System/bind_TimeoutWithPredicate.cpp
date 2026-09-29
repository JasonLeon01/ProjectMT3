#include "System/bind_TimeoutWithPredicate.hpp"

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
    "\\brief Utility class providing hybrid functionality\nof a timeout and a continuation predicate",
    "\\brief Constructor\n\nThis constructor constructs a `TimeoutWithPredicate` object\nthat times out after the given amount of time.\n\n\\param timeout Time to timeout after",
    "\\brief Constructor\n\nThis constructor constructs a `TimeoutWithPredicate` object\nthat times out when the given predicate returns `false`.\nThe frequency at which predicate is checked is specified\nby `period`.\n\nIf an empty predicate is passed, it will be set to a\npredicate that returns `false` when called.\n\n\\param predicate Predicate that returns `true` to continue or `false` to timeout\n\\param period    The period between checks of the predicate",
    "\\brief Get the period\n\n\\return The period",
}; }

void bind_TimeoutWithPredicate(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__TimeoutWithPredicate = lua_glue::BindClass<sf::TimeoutWithPredicate>(sf, "TimeoutWithPredicate");
    lua_glue::Table table_sf__TimeoutWithPredicate = sf["TimeoutWithPredicate"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::TimeoutWithPredicate>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.TimeoutWithPredicate");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.TimeoutWithPredicate", "new", "fun(timeout: sf.Time): sf.TimeoutWithPredicate");
    LUASF_STUB_OVERLOAD("sf.TimeoutWithPredicate", "new", "fun(predicate: fun(): boolean, period?: sf.Time): sf.TimeoutWithPredicate");
    lua_glue::BindCallable(type_sf__TimeoutWithPredicate, "new",
        [](sf::Time timeout) {
            return lua_sf::makeLuaSharedObject<sf::TimeoutWithPredicate>(timeout);
        },
        docs[1]
    );
    lua_glue::BindCallable(type_sf__TimeoutWithPredicate, "new",
        [](lua_glue::Object predicate, sf::Time period) {
            return lua_sf::makeLuaSharedObject<sf::TimeoutWithPredicate>(lua_sf::callback::from_object<std::function<bool()>, lua_sf::callback::GenericCallbackCodec>(predicate, lua_sf::callback::CallbackOptions{"sf::TimeoutWithPredicate::TimeoutWithPredicate.predicate", false}), period);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Time>(milliseconds(1));
        }}},
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.TimeoutWithPredicate", "getPeriod", "fun(self: sf.TimeoutWithPredicate): sf.Time");
    lua_glue::BindCallable(type_sf__TimeoutWithPredicate, "getPeriod",
        [](const sf::TimeoutWithPredicate& self) {
            return std::cref(self.getPeriod());
        },
        docs[3],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    // Skipped sf::TimeoutWithPredicate::getPredicate(): unsupported return type const std::function<bool ()>&.
}
