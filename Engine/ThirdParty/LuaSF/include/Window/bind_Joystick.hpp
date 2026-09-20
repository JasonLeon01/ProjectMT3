#pragma once

#include <SFML/Window/Joystick.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Joystick(lua_glue::StateView lua);
