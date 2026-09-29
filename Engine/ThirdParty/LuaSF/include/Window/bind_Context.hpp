#pragma once

#include <SFML/Window/Context.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Context(lua_glue::StateView lua);
