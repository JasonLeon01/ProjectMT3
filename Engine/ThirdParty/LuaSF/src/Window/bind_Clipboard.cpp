#include "Window/bind_Clipboard.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Clipboard(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    sol::table sf_Clipboard = sf["Clipboard"].get_or_create<sol::table>();
    LUASF_STUB_DOC("\\brief Get the content of the clipboard as string data\n\nThis function returns the content of the clipboard\nas a string. If the clipboard does not contain string\nit returns an empty `sf::String` object.\n\n\\return Clipboard contents as `sf::String` object");
    LUASF_STUB_FUNCTION("sf.Clipboard", "getString", "fun(): string");
    sf_Clipboard.set_function("getString",
        []() -> std::string {
            return lua_sf::to_utf8_string(sf::Clipboard::getString());
        }
    );
    LUASF_STUB_DOC("\\brief Set the content of the clipboard as string data\n\nThis function sets the content of the clipboard as a\nstring.\n\n\\warning Due to limitations on some operating systems,\nsetting the clipboard contents is only\nguaranteed to work if there is currently an\nopen window for which events are being handled.\n\n\\param text `sf::String` containing the data to be sent\nto the clipboard");
    LUASF_STUB_FUNCTION("sf.Clipboard", "setString", "fun(text: string)");
    sf_Clipboard.set_function("setString",
        [](std::string text) {
            sf::Clipboard::setString(lua_sf::to_sf_string(text));
        }
    );
}
