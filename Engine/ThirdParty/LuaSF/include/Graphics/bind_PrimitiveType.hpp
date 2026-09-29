#pragma once

#include <SFML/Graphics/PrimitiveType.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_PrimitiveType(lua_glue::StateView lua);
