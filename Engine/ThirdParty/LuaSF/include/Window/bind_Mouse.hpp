#pragma once

#include <SFML/Window/Mouse.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/WindowBase.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Mouse(lua_glue::StateView lua);
