#pragma once

#include <SFML/Window/ContextSettings.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_ContextSettings(lua_glue::StateView lua);
