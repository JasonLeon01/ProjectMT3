#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Angle.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Vector2(lua_glue::StateView lua);
