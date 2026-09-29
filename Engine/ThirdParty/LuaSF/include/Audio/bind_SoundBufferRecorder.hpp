#pragma once

#include <SFML/Audio/SoundBufferRecorder.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_SoundBufferRecorder(lua_glue::StateView lua);
