#pragma once

#include <SFML/Window/Touch.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/WindowBase.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Touch(lua_glue::StateView lua);
