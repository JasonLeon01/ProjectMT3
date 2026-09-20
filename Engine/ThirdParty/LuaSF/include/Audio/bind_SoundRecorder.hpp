#pragma once

#include <SFML/Audio/SoundRecorder.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_SoundRecorder(lua_glue::StateView lua);
