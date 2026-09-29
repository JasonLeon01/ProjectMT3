#pragma once

#include <SFML/Window/WindowEnums.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_WindowEnums(lua_glue::StateView lua);
