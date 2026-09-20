#pragma once

#include <SFML/Audio/SoundSource.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector3.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_SoundSource(lua_glue::StateView lua);
