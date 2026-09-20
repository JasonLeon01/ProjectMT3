#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Clock(lua_glue::StateView lua);
