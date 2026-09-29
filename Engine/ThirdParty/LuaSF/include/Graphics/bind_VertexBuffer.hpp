#pragma once

#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_VertexBuffer(lua_glue::StateView lua);
