#include "Window/bind_WindowEnums.hpp"

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

namespace { constexpr std::array<std::string_view, 8> docs = {
    "No border / title bar (this flag and all others are mutually exclusive)",
    "Title bar + fixed border",
    "Title bar + resizable border + maximize button",
    "Title bar + close button (see note)",
    "Default window style",
    "\\ingroup window\n\\brief Enumeration of the window states",
    "Floating window",
    "Fullscreen window",
}; }

void bind_WindowEnums(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    lua_glue::Table sf_Style = sf["Style"].get_or_create<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_VALUE("sf.Style", "None", "integer");
    sf_Style.raw_set("None", static_cast<unsigned int>(sf::Style::None));
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_VALUE("sf.Style", "Titlebar", "integer");
    sf_Style.raw_set("Titlebar", static_cast<unsigned int>(sf::Style::Titlebar));
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_VALUE("sf.Style", "Resize", "integer");
    sf_Style.raw_set("Resize", static_cast<unsigned int>(sf::Style::Resize));
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_VALUE("sf.Style", "Close", "integer");
    sf_Style.raw_set("Close", static_cast<unsigned int>(sf::Style::Close));
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_VALUE("sf.Style", "Default", "integer");
    sf_Style.raw_set("Default", static_cast<unsigned int>(sf::Style::Default));
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_CLASS("sf.State");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("Windowed", "sf.State");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FIELD("Fullscreen", "sf.State");
    lua_glue::BindEnum<sf::State>(sf, "State", {
        {"Windowed", sf::State::Windowed},
        {"Fullscreen", sf::State::Fullscreen}
    });
}
