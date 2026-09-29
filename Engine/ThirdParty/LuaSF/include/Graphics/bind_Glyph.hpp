#pragma once

#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Glyph(lua_glue::StateView lua);
