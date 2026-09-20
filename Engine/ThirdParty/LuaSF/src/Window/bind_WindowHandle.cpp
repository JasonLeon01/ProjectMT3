#include "Window/bind_WindowHandle.hpp"

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

namespace { constexpr std::array<std::string_view, 0> docs = {
}; }

void bind_WindowHandle(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    // No bindable public declarations were found in WindowHandle.
}
