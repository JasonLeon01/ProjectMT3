#pragma once

#include <SFML/Window/Mouse.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/WindowBase.hpp>
#include "utils.hpp"

void bind_Mouse(sol::state_view lua);
