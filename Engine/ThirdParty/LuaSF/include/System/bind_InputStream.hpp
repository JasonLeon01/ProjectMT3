#pragma once

#include <SFML/System/InputStream.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_InputStream(lua_glue::StateView lua);
