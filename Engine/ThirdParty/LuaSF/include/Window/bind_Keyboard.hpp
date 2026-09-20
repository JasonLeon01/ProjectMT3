#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Keyboard(lua_glue::StateView lua);
