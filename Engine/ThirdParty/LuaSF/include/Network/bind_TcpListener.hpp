#pragma once

#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_TcpListener(lua_glue::StateView lua);
