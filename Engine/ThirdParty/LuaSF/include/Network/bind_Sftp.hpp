#pragma once

#include <SFML/Network/Sftp.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/TimeoutWithPredicate.hpp>
#include "utils.hpp"

void bind_Sftp(sol::state_view lua);
