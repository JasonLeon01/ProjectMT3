#pragma once

#include <SFML/Audio/InputSoundFile.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_InputSoundFile(lua_glue::StateView lua);
