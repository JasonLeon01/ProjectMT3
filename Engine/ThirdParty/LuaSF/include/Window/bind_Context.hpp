#pragma once

#include <SFML/Window/Context.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include "utils.hpp"

void bind_Context(sol::state_view lua);
