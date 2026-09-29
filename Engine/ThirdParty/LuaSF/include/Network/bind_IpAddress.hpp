#pragma once

#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_IpAddress(lua_glue::StateView lua);
