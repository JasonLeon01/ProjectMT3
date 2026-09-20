#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_SoundBuffer(lua_glue::StateView lua);
