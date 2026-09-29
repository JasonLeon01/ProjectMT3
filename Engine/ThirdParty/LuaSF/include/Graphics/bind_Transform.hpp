#pragma once

#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Transform(lua_glue::StateView lua);
