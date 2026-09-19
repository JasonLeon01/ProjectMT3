#pragma once

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector3.hpp>
#include "utils.hpp"

void bind_Sound(sol::state_view lua);
