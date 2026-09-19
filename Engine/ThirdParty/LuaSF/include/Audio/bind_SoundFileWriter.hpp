#pragma once

#include <SFML/Audio/SoundFileWriter.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include "utils.hpp"

void bind_SoundFileWriter(sol::state_view lua);
