#pragma once

#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Sleep(lua_glue::StateView lua);
