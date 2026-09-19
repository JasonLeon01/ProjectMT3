#pragma once

#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/Socket.hpp>
#include "utils.hpp"

void bind_UdpSocket(sol::state_view lua);
