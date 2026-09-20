#pragma once

#include <SFML/Network/Dns.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Dns(lua_glue::StateView lua);
