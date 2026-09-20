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
    // No bindable public declarations were found in WindowHandle.
}
