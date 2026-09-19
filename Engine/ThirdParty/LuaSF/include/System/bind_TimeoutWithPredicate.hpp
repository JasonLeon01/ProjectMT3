#pragma once

#include <SFML/System/TimeoutWithPredicate.hpp>
#include <SFML/System/Time.hpp>
#include "utils.hpp"

void bind_TimeoutWithPredicate(sol::state_view lua);
