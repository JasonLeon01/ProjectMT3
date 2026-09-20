#pragma once

#include <SFML/Audio/PlaybackDevice.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_PlaybackDevice(lua_glue::StateView lua);
