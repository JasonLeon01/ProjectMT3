#include "Window/bind_Touch.hpp"

#include <algorithm>
#include <array>
#include <string_view>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace { constexpr std::array<std::string_view, 3> docs = {
    "\\brief Check if a touch event is currently down\n\n\\deprecated Use `sf::Event::TouchBegan` and `sf::Event::TouchEnded`\n\n\\param finger Finger index\n\n\\return `true` if \\a finger is currently touching the screen, `false` otherwise",
    "\\brief Get the current position of a touch in desktop coordinates\n\nThis function returns the current touch position\nin global (desktop) coordinates.\n\n\\deprecated Use position member of `sf::Event::TouchBegan`, `sf::Event::TouchEnded` and `sf::Event::TouchMoved`\n\n\\param finger Finger index\n\n\\return Current position of \\a finger, or undefined if it's not down",
    "\\brief Get the current position of a touch in window coordinates\n\nThis function returns the current touch position\nrelative to the given window.\n\n\\deprecated Use position member of `sf::Event::TouchBegan`, `sf::Event::TouchEnded` and `sf::Event::TouchMoved`\n\n\\param finger Finger index\n\\param relativeTo Reference window\n\n\\return Current position of \\a finger, or undefined if it's not down",
}; }

void bind_Touch(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    lua_glue::Table sf_Touch = sf["Touch"].get_or_create<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_FUNCTION("sf.Touch", "isDown", "fun(finger: integer): boolean");
    lua_glue::BindCallable(sf_Touch, "isDown",
        [](lua_sf::LuaIntegral<unsigned int> finger) -> bool {
            return sf::Touch::isDown(finger.value());
        },
        docs[0]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Touch", "getPosition", "fun(finger: integer, relativeTo: sf.WindowBase): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.Touch", "getPosition", "fun(finger: integer): sf.Vector2i");
    lua_glue::BindCallable(sf_Touch, "getPosition",
        [](lua_sf::LuaIntegral<unsigned int> finger, const sf::WindowBase& relativeTo) -> sf::Vector2i {
            return sf::Touch::getPosition(finger.value(), relativeTo);
        },
        docs[2]
    );
    lua_glue::BindCallable(sf_Touch, "getPosition",
        [](lua_sf::LuaIntegral<unsigned int> finger) -> sf::Vector2i {
            return sf::Touch::getPosition(finger.value());
        },
        docs[1]
    );
}
