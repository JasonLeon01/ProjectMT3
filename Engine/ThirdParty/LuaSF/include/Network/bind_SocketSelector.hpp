#pragma once

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"

void bind_SocketSelector(sol::state_view lua);
