#pragma once

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_View(lua_glue::StateView lua);
