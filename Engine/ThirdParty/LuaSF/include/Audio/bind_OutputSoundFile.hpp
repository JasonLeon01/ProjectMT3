#pragma once

#include <SFML/Audio/OutputSoundFile.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include "utils.hpp"

void bind_OutputSoundFile(sol::state_view lua);
