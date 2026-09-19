#include "Window/bind_Touch.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Touch(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    sol::table sf_Touch = sf["Touch"].get_or_create<sol::table>();
    LUASF_STUB_DOC("\\brief Check if a touch event is currently down\n\n\\deprecated Use `sf::Event::TouchBegan` and `sf::Event::TouchEnded`\n\n\\param finger Finger index\n\n\\return `true` if \\a finger is currently touching the screen, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Touch", "isDown", "fun(finger: integer): boolean");
    sf_Touch.set_function("isDown",
        [](lua_sf::LuaIntegral<unsigned int> finger) -> bool {
            return sf::Touch::isDown(finger.value());
        }
    );
    LUASF_STUB_DOC("\\brief Get the current position of a touch in window coordinates\n\nThis function returns the current touch position\nrelative to the given window.\n\n\\deprecated Use position member of `sf::Event::TouchBegan`, `sf::Event::TouchEnded` and `sf::Event::TouchMoved`\n\n\\param finger Finger index\n\\param relativeTo Reference window\n\n\\return Current position of \\a finger, or undefined if it's not down");
    LUASF_STUB_FUNCTION("sf.Touch", "getPosition", "fun(finger: integer, relativeTo: sf.WindowBase): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.Touch", "getPosition", "fun(finger: integer): sf.Vector2i");
    sf_Touch.set_function("getPosition",
        sol::overload(
            [](lua_sf::LuaIntegral<unsigned int> finger, const sf::WindowBase& relativeTo) -> sf::Vector2i {
                return sf::Touch::getPosition(finger.value(), relativeTo);
            },
            [](lua_sf::LuaIntegral<unsigned int> finger) -> sf::Vector2i {
                return sf::Touch::getPosition(finger.value());
            }
        )
    );
}
