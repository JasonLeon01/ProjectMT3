#include "Window/bind_Cursor.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Cursor(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    auto type_sf__Cursor = sf.new_usertype<sf::Cursor>("Cursor", sol::no_constructor);
    sol::table table_sf__Cursor = sf["Cursor"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Cursor>(lua);
    LUASF_STUB_DOC("\\brief Cursor defines the appearance of a system cursor");
    LUASF_STUB_CLASS("sf.Cursor");
    LUASF_STUB_DOC("\\brief Create a native system cursor\n\nRefer to the list of cursor available on each system\n(see `sf::Cursor::Type`) to know whether a given cursor is\nexpected to load successfully or is not supported by\nthe operating system.\n\n\\param type Native system cursor type\n\n\\throws sf::Exception if the corresponding cursor\nis not natively supported by the operating\nsystem");
    LUASF_STUB_FUNCTION("sf.Cursor", "new", "fun(type: sf.Cursor.Type): sf.Cursor");
    LUASF_STUB_OVERLOAD("sf.Cursor", "new", "fun(pixels: any, size: sf.Vector2u, hotspot: sf.Vector2u): sf.Cursor");
    type_sf__Cursor.set_function("new", sol::factories(
        [](sf::Cursor::Type type) {
            return lua_sf::makeLuaSharedObject<sf::Cursor>(type);
        },
        [](sol::object pixels, sf::Vector2u size, sf::Vector2u hotspot) {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            return lua_sf::makeLuaSharedObject<sf::Cursor>(pixels_buffer.data(), size, hotspot);
        }
    ));
    LUASF_STUB_DOC("\\brief Create a cursor with the provided image\n\n`pixels` must be an array of `size` pixels\nin 32-bit RGBA format. If not, this will cause undefined behavior.\n\nIf `pixels` is `nullptr` or either of `size`'s\nproperties are 0, the current cursor is left unchanged\nand the function will return `false`.\n\nIn addition to specifying the pixel data, you can also\nspecify the location of the hotspot of the cursor. The\nhotspot is the pixel coordinate within the cursor image\nwhich will be located exactly where the mouse pointer\nposition is. Any mouse actions that are performed will\nreturn the window/screen location of the hotspot.\n\n\\warning On Unix platforms which do not support colored\ncursors, the pixels are mapped into a monochrome\nbitmap: pixels with an alpha channel to 0 are\ntransparent, black if the RGB channel are close\nto zero, and white otherwise.\n\n\\param pixels   Array of pixels of the image\n\\param size     Width and height of the image\n\\param hotspot  (x,y) location of the hotspot\n\\return Cursor if the cursor was successfully loaded;\n`std::nullopt` otherwise");
    LUASF_STUB_FUNCTION("sf.Cursor", "createFromPixels", "fun(pixels: any, size: sf.Vector2u, hotspot: sf.Vector2u): sf.Cursor|nil");
    type_sf__Cursor.set_function("createFromPixels",
        [lua](sol::object pixels, sf::Vector2u size, sf::Vector2u hotspot) -> sol::object {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            return lua_sf::optional_to_object(lua, sf::Cursor::createFromPixels(pixels_buffer.data(), size, hotspot));
        }
    );
    LUASF_STUB_DOC("\\brief Create a native system cursor\n\nRefer to the list of cursor available on each system\n(see `sf::Cursor::Type`) to know whether a given cursor is\nexpected to load successfully or is not supported by\nthe operating system.\n\n\\param type Native system cursor type\n\\return Cursor if and only if the corresponding cursor is\nnatively supported by the operating system;\n`std::nullopt` otherwise");
    LUASF_STUB_FUNCTION("sf.Cursor", "createFromSystem", "fun(type: sf.Cursor.Type): sf.Cursor|nil");
    type_sf__Cursor.set_function("createFromSystem",
        [lua](sf::Cursor::Type type) -> sol::object {
            return lua_sf::optional_to_object(lua, sf::Cursor::createFromSystem(type));
        }
    );
    LUASF_STUB_DOC("\\brief Enumeration of the native system cursor types\n\nRefer to the following table to determine which cursor\nis available on which platform.\n\nType                                       | Linux | macOS | Windows  |\n--------------------------------------------|:-----:|:-----:|:--------:|\n`sf::Cursor::Type::Arrow`                  |  yes  | yes   |   yes    |\n`sf::Cursor::Type::ArrowWait`              |  no   | no    |   yes    |\n`sf::Cursor::Type::Wait`                   |  yes  | no    |   yes    |\n`sf::Cursor::Type::Text`                   |  yes  | yes   |   yes    |\n`sf::Cursor::Type::Hand`                   |  yes  | yes   |   yes    |\n`sf::Cursor::Type::SizeHorizontal`         |  yes  | yes   |   yes    |\n`sf::Cursor::Type::SizeVertical`           |  yes  | yes   |   yes    |\n`sf::Cursor::Type::SizeTopLeftBottomRight` |  no   | yes*  |   yes    |\n`sf::Cursor::Type::SizeBottomLeftTopRight` |  no   | yes*  |   yes    |\n`sf::Cursor::Type::SizeLeft`               |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeRight`              |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeTop`                |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeBottom`             |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeTopLeft`            |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeTopRight`           |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeBottomLeft`         |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeBottomRight`        |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeAll`                |  yes  | no    |   yes    |\n`sf::Cursor::Type::Cross`                  |  yes  | yes   |   yes    |\n`sf::Cursor::Type::Help`                   |  yes  | yes*  |   yes    |\n`sf::Cursor::Type::NotAllowed`             |  yes  | yes   |   yes    |\n\n* These cursor types are undocumented so may not\nbe available on all versions, but have been tested on 10.13\n\n** On Windows and macOS, double-headed arrows are used");
    LUASF_STUB_CLASS("sf.Cursor.Type");
    LUASF_STUB_DOC("Arrow cursor (default)");
    LUASF_STUB_FIELD("Arrow", "sf.Cursor.Type");
    LUASF_STUB_DOC("Busy arrow cursor");
    LUASF_STUB_FIELD("ArrowWait", "sf.Cursor.Type");
    LUASF_STUB_DOC("Busy cursor");
    LUASF_STUB_FIELD("Wait", "sf.Cursor.Type");
    LUASF_STUB_DOC("I-beam, cursor when hovering over a field allowing text entry");
    LUASF_STUB_FIELD("Text", "sf.Cursor.Type");
    LUASF_STUB_DOC("Pointing hand cursor");
    LUASF_STUB_FIELD("Hand", "sf.Cursor.Type");
    LUASF_STUB_DOC("Horizontal double arrow cursor");
    LUASF_STUB_FIELD("SizeHorizontal", "sf.Cursor.Type");
    LUASF_STUB_DOC("Vertical double arrow cursor");
    LUASF_STUB_FIELD("SizeVertical", "sf.Cursor.Type");
    LUASF_STUB_DOC("Double arrow cursor going from top-left to bottom-right");
    LUASF_STUB_FIELD("SizeTopLeftBottomRight", "sf.Cursor.Type");
    LUASF_STUB_DOC("Double arrow cursor going from bottom-left to top-right");
    LUASF_STUB_FIELD("SizeBottomLeftTopRight", "sf.Cursor.Type");
    LUASF_STUB_DOC("Left arrow cursor on Linux, same as SizeHorizontal on other platforms");
    LUASF_STUB_FIELD("SizeLeft", "sf.Cursor.Type");
    LUASF_STUB_DOC("Right arrow cursor on Linux, same as SizeHorizontal on other platforms");
    LUASF_STUB_FIELD("SizeRight", "sf.Cursor.Type");
    LUASF_STUB_DOC("Up arrow cursor on Linux, same as SizeVertical on other platforms");
    LUASF_STUB_FIELD("SizeTop", "sf.Cursor.Type");
    LUASF_STUB_DOC("Down arrow cursor on Linux, same as SizeVertical on other platforms");
    LUASF_STUB_FIELD("SizeBottom", "sf.Cursor.Type");
    LUASF_STUB_DOC("Top-left arrow cursor on Linux, same as SizeTopLeftBottomRight on other platforms");
    LUASF_STUB_FIELD("SizeTopLeft", "sf.Cursor.Type");
    LUASF_STUB_DOC("Bottom-right arrow cursor on Linux, same as SizeTopLeftBottomRight on other platforms");
    LUASF_STUB_FIELD("SizeBottomRight", "sf.Cursor.Type");
    LUASF_STUB_DOC("Bottom-left arrow cursor on Linux, same as SizeBottomLeftTopRight on other platforms");
    LUASF_STUB_FIELD("SizeBottomLeft", "sf.Cursor.Type");
    LUASF_STUB_DOC("Top-right arrow cursor on Linux, same as SizeBottomLeftTopRight on other platforms");
    LUASF_STUB_FIELD("SizeTopRight", "sf.Cursor.Type");
    LUASF_STUB_DOC("Combination of SizeHorizontal and SizeVertical");
    LUASF_STUB_FIELD("SizeAll", "sf.Cursor.Type");
    LUASF_STUB_DOC("Crosshair cursor");
    LUASF_STUB_FIELD("Cross", "sf.Cursor.Type");
    LUASF_STUB_DOC("Help cursor");
    LUASF_STUB_FIELD("Help", "sf.Cursor.Type");
    LUASF_STUB_DOC("Action not allowed cursor");
    LUASF_STUB_FIELD("NotAllowed", "sf.Cursor.Type");
    table_sf__Cursor.new_enum("Type",
        "Arrow", sf::Cursor::Type::Arrow,
        "ArrowWait", sf::Cursor::Type::ArrowWait,
        "Wait", sf::Cursor::Type::Wait,
        "Text", sf::Cursor::Type::Text,
        "Hand", sf::Cursor::Type::Hand,
        "SizeHorizontal", sf::Cursor::Type::SizeHorizontal,
        "SizeVertical", sf::Cursor::Type::SizeVertical,
        "SizeTopLeftBottomRight", sf::Cursor::Type::SizeTopLeftBottomRight,
        "SizeBottomLeftTopRight", sf::Cursor::Type::SizeBottomLeftTopRight,
        "SizeLeft", sf::Cursor::Type::SizeLeft,
        "SizeRight", sf::Cursor::Type::SizeRight,
        "SizeTop", sf::Cursor::Type::SizeTop,
        "SizeBottom", sf::Cursor::Type::SizeBottom,
        "SizeTopLeft", sf::Cursor::Type::SizeTopLeft,
        "SizeBottomRight", sf::Cursor::Type::SizeBottomRight,
        "SizeBottomLeft", sf::Cursor::Type::SizeBottomLeft,
        "SizeTopRight", sf::Cursor::Type::SizeTopRight,
        "SizeAll", sf::Cursor::Type::SizeAll,
        "Cross", sf::Cursor::Type::Cross,
        "Help", sf::Cursor::Type::Help,
        "NotAllowed", sf::Cursor::Type::NotAllowed
    );
}
