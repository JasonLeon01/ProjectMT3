#pragma once

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include "utils.hpp"

void bind_Shader(sol::state_view lua);
