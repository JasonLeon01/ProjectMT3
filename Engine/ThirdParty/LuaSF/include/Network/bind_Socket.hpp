#pragma once

#include <SFML/Network/Socket.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Socket(lua_glue::StateView lua);
