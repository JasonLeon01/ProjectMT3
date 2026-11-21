#pragma once

#include <SFML/Graphics/GraphicsBackend.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_GraphicsBackend(lua_glue::StateView lua);
