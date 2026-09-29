#pragma once

#include <SFML/Network/Packet.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Packet(lua_glue::StateView lua);
