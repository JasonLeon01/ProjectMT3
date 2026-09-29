#pragma once

#include <SFML/Audio/OutputSoundFile.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_OutputSoundFile(lua_glue::StateView lua);
