#pragma once

#include <SFML/Graphics/BlendMode.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_BlendMode(lua_glue::StateView lua);
