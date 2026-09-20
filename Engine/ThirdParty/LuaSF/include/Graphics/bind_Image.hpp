#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Vector2.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Image(lua_glue::StateView lua);
