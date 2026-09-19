#include "Window/bind_VideoMode.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_VideoMode(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__VideoMode = sf.new_usertype<sf::VideoMode>("VideoMode", sol::no_constructor);
    sol::table table_sf__VideoMode = sf["VideoMode"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::VideoMode>(lua);
    LUASF_STUB_DOC("\\brief VideoMode defines a video mode (size, bpp)");
    LUASF_STUB_CLASS("sf.VideoMode");
    LUASF_STUB_DOC("Video mode width and height, in pixels");
    LUASF_STUB_FIELD("size", "sf.Vector2u");
    LUASF_STUB_DOC("Video mode pixel depth, in bits per pixels");
    LUASF_STUB_FIELD("bitsPerPixel", "integer");
    LUASF_STUB_DOC("\\brief Construct the video mode with its attributes\n\n\\param modeSize         Width and height in pixels\n\\param modeBitsPerPixel Pixel depths in bits per pixel");
    LUASF_STUB_FUNCTION("sf.VideoMode", "new", "fun(modeSize: sf.Vector2u, modeBitsPerPixel: integer): sf.VideoMode");
    LUASF_STUB_OVERLOAD("sf.VideoMode", "new", "fun(modeSize: sf.Vector2u): sf.VideoMode");
    LUASF_STUB_OVERLOAD("sf.VideoMode", "new", "fun(): sf.VideoMode");
    type_sf__VideoMode.set_function("new", sol::factories(
        [](sf::Vector2u modeSize, lua_sf::LuaIntegral<unsigned int> modeBitsPerPixel) {
            return lua_sf::makeLuaSharedObject<sf::VideoMode>(modeSize, modeBitsPerPixel.value());
        },
        [](sf::Vector2u modeSize) {
            return lua_sf::makeLuaSharedObject<sf::VideoMode>(modeSize);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::VideoMode>();
        }
    ));
    type_sf__VideoMode["size"] = sol::policies(&sf::VideoMode::size, sol::self_dependency{});
    type_sf__VideoMode.set("bitsPerPixel", sol::property(
        [](sf::VideoMode& self) {
            return self.bitsPerPixel;
        },
        [](sf::VideoMode& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.bitsPerPixel = value.value();
        }
    ));
    LUASF_STUB_DOC("\\brief Get the current desktop video mode\n\n\\return Current desktop video mode");
    LUASF_STUB_FUNCTION("sf.VideoMode", "getDesktopMode", "fun(): sf.VideoMode");
    type_sf__VideoMode.set_function("getDesktopMode",
        []() -> sf::VideoMode {
            return sf::VideoMode::getDesktopMode();
        }
    );
    LUASF_STUB_DOC("\\brief Retrieve all the video modes supported in fullscreen mode\n\nWhen creating a fullscreen window, the video mode is restricted\nto be compatible with what the graphics driver and monitor\nsupport. This function returns the complete list of all video\nmodes that can be used in fullscreen mode.\nThe returned array is sorted from best to worst, so that\nthe first element will always give the best mode (higher\nwidth, height and bits-per-pixel).\n\n\\return Array containing all the supported fullscreen modes");
    LUASF_STUB_FUNCTION("sf.VideoMode", "getFullscreenModes", "fun(): sf.VideoMode[]");
    type_sf__VideoMode.set_function("getFullscreenModes",
        [lua]() -> sol::object {
            return lua_sf::vector_to_object(lua, sf::VideoMode::getFullscreenModes());
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether or not the video mode is valid\n\nThe validity of video modes is only relevant when using\nfullscreen windows; otherwise any video mode can be used\nwith no restriction.\n\n\\return `true` if the video mode is valid for fullscreen mode");
    LUASF_STUB_FUNCTION("sf.VideoMode", "isValid", "fun(self: sf.VideoMode): boolean");
    type_sf__VideoMode.set_function("isValid",
        [](sf::VideoMode& self) -> bool {
            return self.isValid();
        }
    );
}
