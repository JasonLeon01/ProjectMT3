#include "Window/bind_WindowEnums.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_WindowEnums(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    sol::table sf_Style = sf["Style"].get_or_create<sol::table>();
    LUASF_STUB_DOC("No border / title bar (this flag and all others are mutually exclusive)");
    LUASF_STUB_VALUE("sf.Style", "None", "integer");
    sf_Style["None"] = sf::Style::None;
    LUASF_STUB_DOC("Title bar + fixed border");
    LUASF_STUB_VALUE("sf.Style", "Titlebar", "integer");
    sf_Style["Titlebar"] = sf::Style::Titlebar;
    LUASF_STUB_DOC("Title bar + resizable border + maximize button");
    LUASF_STUB_VALUE("sf.Style", "Resize", "integer");
    sf_Style["Resize"] = sf::Style::Resize;
    LUASF_STUB_DOC("Title bar + close button (see note)");
    LUASF_STUB_VALUE("sf.Style", "Close", "integer");
    sf_Style["Close"] = sf::Style::Close;
    LUASF_STUB_DOC("Default window style");
    LUASF_STUB_VALUE("sf.Style", "Default", "integer");
    sf_Style["Default"] = sf::Style::Default;
    LUASF_STUB_DOC("\\ingroup window\n\\brief Enumeration of the window states");
    LUASF_STUB_CLASS("sf.State");
    LUASF_STUB_DOC("Floating window");
    LUASF_STUB_FIELD("Windowed", "sf.State");
    LUASF_STUB_DOC("Fullscreen window");
    LUASF_STUB_FIELD("Fullscreen", "sf.State");
    sf.new_enum("State",
        "Windowed", sf::State::Windowed,
        "Fullscreen", sf::State::Fullscreen
    );
}
