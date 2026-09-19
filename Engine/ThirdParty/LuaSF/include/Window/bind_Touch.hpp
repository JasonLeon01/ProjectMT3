#pragma once

#include <SFML/Window/Touch.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/WindowBase.hpp>
#include "utils.hpp"

void bind_Touch(sol::state_view lua);
