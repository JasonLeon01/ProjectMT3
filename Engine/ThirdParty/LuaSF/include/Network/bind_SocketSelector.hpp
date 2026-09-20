#pragma once

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_SocketSelector(lua_glue::StateView lua);
