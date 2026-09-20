#pragma once

#include <SFML/Audio/SoundFileWriter.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_SoundFileWriter(lua_glue::StateView lua);
