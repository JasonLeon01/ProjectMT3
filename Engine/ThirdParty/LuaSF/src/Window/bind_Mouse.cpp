#include "Window/bind_Mouse.hpp"

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

namespace { constexpr std::array<std::string_view, 15> docs = {
    "\\brief Mouse buttons",
    "The left mouse button",
    "The right mouse button",
    "The middle (wheel) mouse button",
    "The first extra mouse button",
    "The second extra mouse button",
    "The total number of mouse buttons",
    "\\brief Mouse wheels",
    "The vertical mouse wheel",
    "The horizontal mouse wheel",
    "\\brief Check if a mouse button is pressed\n\n\\warning Checking the state of buttons `Mouse::Button::Extra1` and\n`Mouse::Button::Extra2` is not supported on Linux with X11.\n\n\\param button Button to check\n\n\\return `true` if the button is pressed, `false` otherwise",
    "\\brief Get the current position of the mouse in desktop coordinates\n\nThis function returns the global position of the mouse\ncursor on the desktop.\n\n\\return Current position of the mouse",
    "\\brief Get the current position of the mouse in window coordinates\n\nThis function returns the current position of the mouse\ncursor, relative to the given window.\n\n\\param relativeTo Reference window\n\n\\return Current position of the mouse",
    "\\brief Set the current position of the mouse in desktop coordinates\n\nThis function sets the global position of the mouse\ncursor on the desktop.\n\n\\param position New position of the mouse\n\n\\warning On macOS the OS API used for `setPosition` requires granting\nof Accessibility permission for your application.\nSee also: https://support.apple.com/guide/mac-help/allow-accessibility-apps-to-access-your-mac-mh43185/",
    "\\brief Set the current position of the mouse in window coordinates\n\nThis function sets the current position of the mouse\ncursor, relative to the given window.\n\n\\param position New position of the mouse\n\\param relativeTo Reference window\n\n\\warning On macOS the OS API used for `setPosition` requires granting\nof Accessibility permission for your application.\nSee also: https://support.apple.com/guide/mac-help/allow-accessibility-apps-to-access-your-mac-mh43185/",
}; }

void bind_Mouse(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    lua_glue::Table sf_Mouse = sf["Mouse"].get_or_create<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Mouse.Button");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("Left", "sf.Mouse.Button");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("Right", "sf.Mouse.Button");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("Middle", "sf.Mouse.Button");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("Extra1", "sf.Mouse.Button");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("Extra2", "sf.Mouse.Button");
    lua_glue::BindEnum<sf::Mouse::Button>(sf_Mouse, "Button", {
        {"Left", sf::Mouse::Button::Left},
        {"Right", sf::Mouse::Button::Right},
        {"Middle", sf::Mouse::Button::Middle},
        {"Extra1", sf::Mouse::Button::Extra1},
        {"Extra2", sf::Mouse::Button::Extra2}
    });
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_VALUE("sf.Mouse", "ButtonCount", "integer");
    lua_glue::BindStaticAttr<const unsigned int>(sf_Mouse, "ButtonCount", &sf::Mouse::ButtonCount);
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_CLASS("sf.Mouse.Wheel");
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FIELD("Vertical", "sf.Mouse.Wheel");
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FIELD("Horizontal", "sf.Mouse.Wheel");
    lua_glue::BindEnum<sf::Mouse::Wheel>(sf_Mouse, "Wheel", {
        {"Vertical", sf::Mouse::Wheel::Vertical},
        {"Horizontal", sf::Mouse::Wheel::Horizontal}
    });
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Mouse", "isButtonPressed", "fun(button: sf.Mouse.Button): boolean");
    lua_glue::BindCallable(sf_Mouse, "isButtonPressed",
        [](sf::Mouse::Button button) -> bool {
            return sf::Mouse::isButtonPressed(button);
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Mouse", "getPosition", "fun(relativeTo: sf.WindowBase): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.Mouse", "getPosition", "fun(): sf.Vector2i");
    lua_glue::BindCallable(sf_Mouse, "getPosition",
        [](const sf::WindowBase& relativeTo) -> sf::Vector2i {
            return sf::Mouse::getPosition(relativeTo);
        },
        docs[12]
    );
    lua_glue::BindCallable(sf_Mouse, "getPosition",
        []() -> sf::Vector2i {
            return sf::Mouse::getPosition();
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Mouse", "setPosition", "fun(position: sf.Vector2i, relativeTo: sf.WindowBase)");
    LUASF_STUB_OVERLOAD("sf.Mouse", "setPosition", "fun(position: sf.Vector2i)");
    lua_glue::BindCallable(sf_Mouse, "setPosition",
        [](sf::Vector2i position, const sf::WindowBase& relativeTo) {
            sf::Mouse::setPosition(position, relativeTo);
        },
        docs[14]
    );
    lua_glue::BindCallable(sf_Mouse, "setPosition",
        [](sf::Vector2i position) {
            sf::Mouse::setPosition(position);
        },
        docs[13]
    );
}
