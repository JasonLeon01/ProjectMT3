#pragma once

#include <SFML/System/FileInputStream.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_FileInputStream(lua_glue::StateView lua);
