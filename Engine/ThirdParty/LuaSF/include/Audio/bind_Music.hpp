#pragma once

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Audio/SoundStream.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector3.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Music(lua_glue::StateView lua);
