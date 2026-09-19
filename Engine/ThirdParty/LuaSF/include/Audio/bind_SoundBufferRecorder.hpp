#pragma once

#include <SFML/Audio/SoundBufferRecorder.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include "utils.hpp"

void bind_SoundBufferRecorder(sol::state_view lua);
