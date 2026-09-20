#pragma once

#include <SFML/Window/WindowBase.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Cursor.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include "utils.hpp"
#include "LuaSFValueTraits.hpp"

void bind_WindowBase(lua_glue::StateView lua);
