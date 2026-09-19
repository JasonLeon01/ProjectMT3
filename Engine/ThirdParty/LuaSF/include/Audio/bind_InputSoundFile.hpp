#pragma once

#include <SFML/Audio/InputSoundFile.hpp>
#include <SFML/Audio/SoundChannel.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"

void bind_InputSoundFile(sol::state_view lua);
