#include "System/bind_Sleep.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Sleep(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    LUASF_STUB_DOC("\\ingroup system\n\\brief Make the current thread sleep for a given duration\n\n`sf::sleep` is the best way to block a program or one of its\nthreads, as it doesn't consume any CPU power. Compared to\nthe standard `std::this_thread::sleep_for` function, this\none provides more accurate sleeping time thanks to some\nplatform-specific tweaks.\n\n`sf::sleep` only guarantees millisecond precision. Sleeping\nfor a duration less than 1 millisecond is prone to result\nin the actual sleep duration being less than what is\nrequested.\n\n\\param duration Time to sleep");
    LUASF_STUB_FUNCTION("sf", "sleep", "fun(duration: sf.Time)");
    sf.set_function("sleep",
        [](sf::Time duration) {
            sf::sleep(duration);
        }
    );
}
