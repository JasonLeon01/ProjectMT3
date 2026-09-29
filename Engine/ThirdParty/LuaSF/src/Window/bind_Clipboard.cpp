#include "Window/bind_Clipboard.hpp"

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

namespace { constexpr std::array<std::string_view, 2> docs = {
    "\\brief Get the content of the clipboard as string data\n\nThis function returns the content of the clipboard\nas a string. If the clipboard does not contain string\nit returns an empty `sf::String` object.\n\n\\return Clipboard contents as `sf::String` object",
    "\\brief Set the content of the clipboard as string data\n\nThis function sets the content of the clipboard as a\nstring.\n\n\\warning Due to limitations on some operating systems,\nsetting the clipboard contents is only\nguaranteed to work if there is currently an\nopen window for which events are being handled.\n\n\\param text `sf::String` containing the data to be sent\nto the clipboard",
}; }

void bind_Clipboard(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    lua_glue::Table sf_Clipboard = sf["Clipboard"].get_or_create<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_FUNCTION("sf.Clipboard", "getString", "fun(): string");
    lua_glue::BindCallable(sf_Clipboard, "getString",
        []() -> std::string {
            return lua_sf::to_utf8_string(sf::Clipboard::getString());
        },
        docs[0]
    );
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Clipboard", "setString", "fun(text: string)");
    lua_glue::BindCallable(sf_Clipboard, "setString",
        [](std::string text) {
            sf::Clipboard::setString(lua_sf::to_sf_string(text));
        },
        docs[1]
    );
}
