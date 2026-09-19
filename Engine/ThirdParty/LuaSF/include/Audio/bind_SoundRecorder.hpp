#pragma once

#include <SFML/Audio/SoundRecorder.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include "utils.hpp"

void bind_SoundRecorder(sol::state_view lua);
