#pragma once

#include <SFML/Network/Ftp.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"

void bind_Ftp(sol::state_view lua);
