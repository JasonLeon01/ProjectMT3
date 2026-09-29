#include "System/bind_Clock.hpp"

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

namespace { constexpr std::array<std::string_view, 7> docs = {
    "\\brief Utility class that measures the elapsed time\n\nThe clock starts automatically after being constructed.",
    "\\brief Get the elapsed time\n\nThis function returns the time elapsed since the last call\nto `restart()` (or the construction of the instance if `restart()`\nhas not been called).\n\n\\return Time elapsed",
    "\\brief Check whether the clock is running\n\n\\return `true` if the clock is running, `false` otherwise",
    "\\brief Start the clock\n\n\\see `stop`",
    "\\brief Stop the clock\n\n\\see `start`",
    "\\brief Restart the clock\n\nThis function puts the time counter back to zero, returns\nthe elapsed time, and leaves the clock in a running state.\n\n\\return Time elapsed\n\n\\see `reset`",
    "\\brief Reset the clock\n\nThis function puts the time counter back to zero, returns\nthe elapsed time, and leaves the clock in a paused state.\n\n\\return Time elapsed\n\n\\see `restart`",
}; }

void bind_Clock(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    auto type_sf__Clock = lua_glue::BindClass<sf::Clock>(sf, "Clock");
    lua_glue::Table table_sf__Clock = sf["Clock"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Clock>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Clock");
    LUASF_STUB_FUNCTION("sf.Clock", "new", "fun(): sf.Clock");
    lua_glue::BindCallable(type_sf__Clock, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Clock>();
        }
    );
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Clock", "getElapsedTime", "fun(self: sf.Clock): sf.Time");
    lua_glue::BindCallable(type_sf__Clock, "getElapsedTime",
        [](const sf::Clock& self) -> sf::Time {
            return self.getElapsedTime();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Clock", "isRunning", "fun(self: sf.Clock): boolean");
    lua_glue::BindCallable(type_sf__Clock, "isRunning",
        [](const sf::Clock& self) -> bool {
            return self.isRunning();
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Clock", "start", "fun(self: sf.Clock)");
    lua_glue::BindCallable(type_sf__Clock, "start",
        [](sf::Clock& self) {
            self.start();
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Clock", "stop", "fun(self: sf.Clock)");
    lua_glue::BindCallable(type_sf__Clock, "stop",
        [](sf::Clock& self) {
            self.stop();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Clock", "restart", "fun(self: sf.Clock): sf.Time");
    lua_glue::BindCallable(type_sf__Clock, "restart",
        [](sf::Clock& self) -> sf::Time {
            return self.restart();
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Clock", "reset", "fun(self: sf.Clock): sf.Time");
    lua_glue::BindCallable(type_sf__Clock, "reset",
        [](sf::Clock& self) -> sf::Time {
            return self.reset();
        },
        docs[6]
    );
}
