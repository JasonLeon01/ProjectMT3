#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Glyph.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/InputStream.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Font(lua_glue::StateView lua);
