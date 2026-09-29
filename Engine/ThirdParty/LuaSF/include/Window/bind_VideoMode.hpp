#pragma once

#include <SFML/Window/VideoMode.hpp>
#include <SFML/System/Vector2.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_VideoMode(lua_glue::StateView lua);
