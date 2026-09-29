#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_Texture(lua_glue::StateView lua);
