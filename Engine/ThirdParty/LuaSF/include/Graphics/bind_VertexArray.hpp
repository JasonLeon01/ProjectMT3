#pragma once

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_VertexArray(lua_glue::StateView lua);
