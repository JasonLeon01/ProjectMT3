#pragma once

#include <SFML/Network/Dns.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"

void bind_Dns(sol::state_view lua);
