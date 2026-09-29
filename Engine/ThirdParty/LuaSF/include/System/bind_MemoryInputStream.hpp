#pragma once

#include <SFML/System/MemoryInputStream.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_MemoryInputStream(lua_glue::StateView lua);
