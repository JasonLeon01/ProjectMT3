#include "Window/bind_WindowHandle.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_WindowHandle(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    LUASF_STUB_ALIAS("sf.WindowHandle", "sf.void*");
    {
        const sol::object aliasValue = sf.raw_get<sol::object>("WindowHandle");
        const sol::object aliasTarget = sf.raw_get<sol::object>("void*");
        if ((!aliasValue.valid() || aliasValue.get_type() == sol::type::lua_nil) &&
            aliasTarget.valid() && aliasTarget.get_type() != sol::type::lua_nil)
            sf.raw_set("WindowHandle", aliasTarget);
    }
}
