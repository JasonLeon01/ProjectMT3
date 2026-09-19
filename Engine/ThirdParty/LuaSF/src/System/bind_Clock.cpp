#include "System/bind_Clock.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Clock(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    auto type_sf__Clock = sf.new_usertype<sf::Clock>("Clock", sol::no_constructor);
    sol::table table_sf__Clock = sf["Clock"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Clock>(lua);
    LUASF_STUB_DOC("\\brief Utility class that measures the elapsed time\n\nThe clock starts automatically after being constructed.");
    LUASF_STUB_CLASS("sf.Clock");
    LUASF_STUB_FUNCTION("sf.Clock", "new", "fun(): sf.Clock");
    type_sf__Clock.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Clock>();
        }
    ));
    LUASF_STUB_DOC("\\brief Get the elapsed time\n\nThis function returns the time elapsed since the last call\nto `restart()` (or the construction of the instance if `restart()`\nhas not been called).\n\n\\return Time elapsed");
    LUASF_STUB_FUNCTION("sf.Clock", "getElapsedTime", "fun(self: sf.Clock): sf.Time");
    type_sf__Clock.set_function("getElapsedTime",
        [](sf::Clock& self) -> sf::Time {
            return self.getElapsedTime();
        }
    );
    LUASF_STUB_DOC("\\brief Check whether the clock is running\n\n\\return `true` if the clock is running, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Clock", "isRunning", "fun(self: sf.Clock): boolean");
    type_sf__Clock.set_function("isRunning",
        [](sf::Clock& self) -> bool {
            return self.isRunning();
        }
    );
    LUASF_STUB_DOC("\\brief Start the clock\n\n\\see `stop`");
    LUASF_STUB_FUNCTION("sf.Clock", "start", "fun(self: sf.Clock)");
    type_sf__Clock.set_function("start",
        [](sf::Clock& self) {
            self.start();
        }
    );
    LUASF_STUB_DOC("\\brief Stop the clock\n\n\\see `start`");
    LUASF_STUB_FUNCTION("sf.Clock", "stop", "fun(self: sf.Clock)");
    type_sf__Clock.set_function("stop",
        [](sf::Clock& self) {
            self.stop();
        }
    );
    LUASF_STUB_DOC("\\brief Restart the clock\n\nThis function puts the time counter back to zero, returns\nthe elapsed time, and leaves the clock in a running state.\n\n\\return Time elapsed\n\n\\see `reset`");
    LUASF_STUB_FUNCTION("sf.Clock", "restart", "fun(self: sf.Clock): sf.Time");
    type_sf__Clock.set_function("restart",
        [](sf::Clock& self) -> sf::Time {
            return self.restart();
        }
    );
    LUASF_STUB_DOC("\\brief Reset the clock\n\nThis function puts the time counter back to zero, returns\nthe elapsed time, and leaves the clock in a paused state.\n\n\\return Time elapsed\n\n\\see `restart`");
    LUASF_STUB_FUNCTION("sf.Clock", "reset", "fun(self: sf.Clock): sf.Time");
    type_sf__Clock.set_function("reset",
        [](sf::Clock& self) -> sf::Time {
            return self.reset();
        }
    );
}
