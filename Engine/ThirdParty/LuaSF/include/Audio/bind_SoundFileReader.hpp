#pragma once

#include <SFML/Audio/SoundFileReader.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include <SFML/System/InputStream.hpp>
#include "utils.hpp"

void bind_SoundFileReader(sol::state_view lua);
