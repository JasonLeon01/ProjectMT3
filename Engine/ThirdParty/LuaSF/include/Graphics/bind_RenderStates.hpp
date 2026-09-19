#pragma once

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/CoordinateType.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/StencilMode.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transform.hpp>
#include "utils.hpp"

void bind_RenderStates(sol::state_view lua);
