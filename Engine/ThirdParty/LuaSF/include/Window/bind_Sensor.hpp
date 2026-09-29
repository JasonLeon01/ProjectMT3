#pragma once

#include <SFML/Window/Sensor.hpp>
#include <SFML/System/Vector3.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Sensor(lua_glue::StateView lua);
