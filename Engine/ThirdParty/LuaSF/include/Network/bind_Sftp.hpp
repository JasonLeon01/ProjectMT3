#pragma once

#include <SFML/Network/Sftp.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/TimeoutWithPredicate.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Sftp(lua_glue::StateView lua);
