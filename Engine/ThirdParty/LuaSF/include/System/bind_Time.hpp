#pragma once

#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Time(lua_glue::StateView lua);
