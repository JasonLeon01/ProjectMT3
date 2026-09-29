#pragma once

#include <SFML/Audio/SoundFileReader.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include <SFML/System/InputStream.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_SoundFileReader(lua_glue::StateView lua);
