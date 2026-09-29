#pragma once

#include <SFML/System/Version.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Version(lua_glue::StateView lua);
