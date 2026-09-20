#pragma once

#include <SFML/System/Vector3.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Vector3(lua_glue::StateView lua);
