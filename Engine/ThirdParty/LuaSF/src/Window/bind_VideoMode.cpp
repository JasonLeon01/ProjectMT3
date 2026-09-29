#include "Window/bind_VideoMode.hpp"

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
    "\\brief VideoMode defines a video mode (size, bpp)",
    "Video mode width and height, in pixels",
    "Video mode pixel depth, in bits per pixels",
    "\\brief Default constructor\n\nThis constructors initializes all members to 0.",
    "\\brief Construct the video mode with its attributes\n\n\\param modeSize         Width and height in pixels\n\\param modeBitsPerPixel Pixel depths in bits per pixel",
    "\\brief Get the current desktop video mode\n\n\\return Current desktop video mode",
    "\\brief Retrieve all the video modes supported in fullscreen mode\n\nWhen creating a fullscreen window, the video mode is restricted\nto be compatible with what the graphics driver and monitor\nsupport. This function returns the complete list of all video\nmodes that can be used in fullscreen mode.\nThe returned array is sorted from best to worst, so that\nthe first element will always give the best mode (higher\nwidth, height and bits-per-pixel).\n\n\\return Array containing all the supported fullscreen modes",
    "\\brief Tell whether or not the video mode is valid\n\nThe validity of video modes is only relevant when using\nfullscreen windows; otherwise any video mode can be used\nwith no restriction.\n\n\\return `true` if the video mode is valid for fullscreen mode",
}; }

void bind_VideoMode(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__VideoMode = lua_glue::BindStruct<sf::VideoMode>(sf, "VideoMode");
    lua_glue::Table table_sf__VideoMode = sf["VideoMode"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.VideoMode");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("size", "sf.Vector2u");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("bitsPerPixel", "integer");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.VideoMode", "new", "fun(modeSize: sf.Vector2u, modeBitsPerPixel?: integer): sf.VideoMode");
    LUASF_STUB_OVERLOAD("sf.VideoMode", "new", "fun(): sf.VideoMode");
    lua_glue::BindCallable(type_sf__VideoMode, "new",
        [](sf::Vector2u modeSize, lua_sf::LuaIntegral<unsigned int> modeBitsPerPixel) {
            return sf::VideoMode{modeSize, modeBitsPerPixel.value()};
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned int>(32);
        }}},
        docs[4]
    );
    lua_glue::BindCallable(type_sf__VideoMode, "new",
        []() {
            return sf::VideoMode{};
        },
        docs[3]
    );
    lua_glue::BindAttr<sf::Vector2u>(type_sf__VideoMode, "size", &sf::VideoMode::size);
    lua_glue::BindProperty(type_sf__VideoMode, "bitsPerPixel",
        [](const sf::VideoMode& self) {
            return self.bitsPerPixel;
        },
        [](sf::VideoMode& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.bitsPerPixel = value.value();
        }
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.VideoMode", "getDesktopMode", "fun(): sf.VideoMode");
    lua_glue::BindCallable(type_sf__VideoMode, "getDesktopMode",
        []() -> sf::VideoMode {
            return sf::VideoMode::getDesktopMode();
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.VideoMode", "getFullscreenModes", "fun(): sf.VideoMode[]");
    lua_glue::BindCallable(type_sf__VideoMode, "getFullscreenModes",
        [lua]() -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, sf::VideoMode::getFullscreenModes());
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.VideoMode", "isValid", "fun(self: sf.VideoMode): boolean");
    lua_glue::BindCallable(type_sf__VideoMode, "isValid",
        [](const sf::VideoMode& self) -> bool {
            return self.isValid();
        },
        docs[7]
    );
    LUASF_STUB_FUNCTION("sf.VideoMode", "copy", "fun(self: sf.VideoMode): sf.VideoMode");
    LUASF_STUB_FUNCTION("sf.VideoMode", "deepcopy", "fun(self: sf.VideoMode): sf.VideoMode");
}
