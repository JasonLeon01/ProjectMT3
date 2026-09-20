#pragma once

#include <SFML/Network/Ftp.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Ftp(lua_glue::StateView lua);
