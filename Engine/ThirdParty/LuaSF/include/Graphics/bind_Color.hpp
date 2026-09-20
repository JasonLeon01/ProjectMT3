#pragma once

#include <SFML/Graphics/Color.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Color(lua_glue::StateView lua);
