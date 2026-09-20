#pragma once

#include <SFML/System/TimeoutWithPredicate.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_TimeoutWithPredicate(lua_glue::StateView lua);
