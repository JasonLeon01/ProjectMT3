#pragma once

#include <SFML/Audio/SoundChannel.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_SoundChannel(lua_glue::StateView lua);
