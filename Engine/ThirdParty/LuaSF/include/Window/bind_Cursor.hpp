#pragma once

#include <SFML/Window/Cursor.hpp>
#include <SFML/System/Vector2.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Cursor(lua_glue::StateView lua);
