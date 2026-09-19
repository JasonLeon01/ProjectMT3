#pragma once

#include <SFML/Window/WindowBase.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Cursor.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include "utils.hpp"

void bind_WindowBase(sol::state_view lua);
