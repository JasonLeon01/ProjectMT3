#pragma once

#include <SFML/Window/WindowHandle.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_WindowHandle(lua_glue::StateView lua);
