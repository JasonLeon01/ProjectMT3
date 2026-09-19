#include "Window/bind_Mouse.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Mouse(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    sol::table sf_Mouse = sf["Mouse"].get_or_create<sol::table>();
    LUASF_STUB_DOC("\\brief Mouse buttons");
    LUASF_STUB_CLASS("sf.Mouse.Button");
    LUASF_STUB_DOC("The left mouse button");
    LUASF_STUB_FIELD("Left", "sf.Mouse.Button");
    LUASF_STUB_DOC("The right mouse button");
    LUASF_STUB_FIELD("Right", "sf.Mouse.Button");
    LUASF_STUB_DOC("The middle (wheel) mouse button");
    LUASF_STUB_FIELD("Middle", "sf.Mouse.Button");
    LUASF_STUB_DOC("The first extra mouse button");
    LUASF_STUB_FIELD("Extra1", "sf.Mouse.Button");
    LUASF_STUB_DOC("The second extra mouse button");
    LUASF_STUB_FIELD("Extra2", "sf.Mouse.Button");
    sf_Mouse.new_enum("Button",
        "Left", sf::Mouse::Button::Left,
        "Right", sf::Mouse::Button::Right,
        "Middle", sf::Mouse::Button::Middle,
        "Extra1", sf::Mouse::Button::Extra1,
        "Extra2", sf::Mouse::Button::Extra2
    );
    LUASF_STUB_DOC("The total number of mouse buttons");
    LUASF_STUB_VALUE("sf.Mouse", "ButtonCount", "integer");
    sf_Mouse["ButtonCount"] = sf::Mouse::ButtonCount;
    LUASF_STUB_DOC("\\brief Mouse wheels");
    LUASF_STUB_CLASS("sf.Mouse.Wheel");
    LUASF_STUB_DOC("The vertical mouse wheel");
    LUASF_STUB_FIELD("Vertical", "sf.Mouse.Wheel");
    LUASF_STUB_DOC("The horizontal mouse wheel");
    LUASF_STUB_FIELD("Horizontal", "sf.Mouse.Wheel");
    sf_Mouse.new_enum("Wheel",
        "Vertical", sf::Mouse::Wheel::Vertical,
        "Horizontal", sf::Mouse::Wheel::Horizontal
    );
    LUASF_STUB_DOC("\\brief Check if a mouse button is pressed\n\n\\warning Checking the state of buttons `Mouse::Button::Extra1` and\n`Mouse::Button::Extra2` is not supported on Linux with X11.\n\n\\param button Button to check\n\n\\return `true` if the button is pressed, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Mouse", "isButtonPressed", "fun(button: sf.Mouse.Button): boolean");
    sf_Mouse.set_function("isButtonPressed",
        [](sf::Mouse::Button button) -> bool {
            return sf::Mouse::isButtonPressed(button);
        }
    );
    LUASF_STUB_DOC("\\brief Get the current position of the mouse in window coordinates\n\nThis function returns the current position of the mouse\ncursor, relative to the given window.\n\n\\param relativeTo Reference window\n\n\\return Current position of the mouse");
    LUASF_STUB_FUNCTION("sf.Mouse", "getPosition", "fun(relativeTo: sf.WindowBase): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.Mouse", "getPosition", "fun(): sf.Vector2i");
    sf_Mouse.set_function("getPosition",
        sol::overload(
            [](const sf::WindowBase& relativeTo) -> sf::Vector2i {
                return sf::Mouse::getPosition(relativeTo);
            },
            []() -> sf::Vector2i {
                return sf::Mouse::getPosition();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Set the current position of the mouse in window coordinates\n\nThis function sets the current position of the mouse\ncursor, relative to the given window.\n\n\\param position New position of the mouse\n\\param relativeTo Reference window\n\n\\warning On macOS the OS API used for `setPosition` requires granting\nof Accessibility permission for your application.\nSee also: https://support.apple.com/guide/mac-help/allow-accessibility-apps-to-access-your-mac-mh43185/");
    LUASF_STUB_FUNCTION("sf.Mouse", "setPosition", "fun(position: sf.Vector2i, relativeTo: sf.WindowBase)");
    LUASF_STUB_OVERLOAD("sf.Mouse", "setPosition", "fun(position: sf.Vector2i)");
    sf_Mouse.set_function("setPosition",
        sol::overload(
            [](sf::Vector2i position, const sf::WindowBase& relativeTo) {
                sf::Mouse::setPosition(position, relativeTo);
            },
            [](sf::Vector2i position) {
                sf::Mouse::setPosition(position);
            }
        )
    );
}
