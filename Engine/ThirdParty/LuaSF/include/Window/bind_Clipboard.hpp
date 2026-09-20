#pragma once

#include <SFML/Window/Clipboard.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Clipboard(lua_glue::StateView lua);
