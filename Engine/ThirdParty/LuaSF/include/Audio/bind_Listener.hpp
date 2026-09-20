#pragma once

#include <SFML/Audio/Listener.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector3.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Listener(lua_glue::StateView lua);
