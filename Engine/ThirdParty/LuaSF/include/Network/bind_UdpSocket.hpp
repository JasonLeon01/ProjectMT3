#pragma once

#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/Socket.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_UdpSocket(lua_glue::StateView lua);
