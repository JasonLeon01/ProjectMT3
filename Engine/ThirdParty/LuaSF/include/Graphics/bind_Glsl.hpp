#pragma once

#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Glsl(lua_glue::StateView lua);
