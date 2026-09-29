#include "Window/bind_Cursor.hpp"

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

namespace { constexpr std::array<std::string_view, 27> docs = {
    "\\brief Cursor defines the appearance of a system cursor",
    "\\brief Construct a cursor with the provided image\n\n`pixels` must be an array of `size` pixels in\n32-bit RGBA format. If not, this will cause undefined behavior.\n\nIf `pixels` is `nullptr` or either of `size`'s\nproperties are 0, the current cursor is left unchanged\nand the function will return `false`.\n\nIn addition to specifying the pixel data, you can also\nspecify the location of the hotspot of the cursor. The\nhotspot is the pixel coordinate within the cursor image\nwhich will be located exactly where the mouse pointer\nposition is. Any mouse actions that are performed will\nreturn the window/screen location of the hotspot.\n\n\\warning On Unix platforms which do not support colored\ncursors, the pixels are mapped into a monochrome\nbitmap: pixels with an alpha channel to 0 are\ntransparent, black if the RGB channel are close\nto zero, and white otherwise.\n\n\\param pixels  Array of pixels of the image\n\\param size    Width and height of the image\n\\param hotspot (x,y) location of the hotspot\n\n\\throws sf::Exception if the cursor could not be constructed",
    "\\brief Create a native system cursor\n\nRefer to the list of cursor available on each system\n(see `sf::Cursor::Type`) to know whether a given cursor is\nexpected to load successfully or is not supported by\nthe operating system.\n\n\\param type Native system cursor type\n\n\\throws sf::Exception if the corresponding cursor\nis not natively supported by the operating\nsystem",
    "\\brief Create a cursor with the provided image\n\n`pixels` must be an array of `size` pixels\nin 32-bit RGBA format. If not, this will cause undefined behavior.\n\nIf `pixels` is `nullptr` or either of `size`'s\nproperties are 0, the current cursor is left unchanged\nand the function will return `false`.\n\nIn addition to specifying the pixel data, you can also\nspecify the location of the hotspot of the cursor. The\nhotspot is the pixel coordinate within the cursor image\nwhich will be located exactly where the mouse pointer\nposition is. Any mouse actions that are performed will\nreturn the window/screen location of the hotspot.\n\n\\warning On Unix platforms which do not support colored\ncursors, the pixels are mapped into a monochrome\nbitmap: pixels with an alpha channel to 0 are\ntransparent, black if the RGB channel are close\nto zero, and white otherwise.\n\n\\param pixels   Array of pixels of the image\n\\param size     Width and height of the image\n\\param hotspot  (x,y) location of the hotspot\n\\return Cursor if the cursor was successfully loaded;\n`std::nullopt` otherwise",
    "\\brief Create a native system cursor\n\nRefer to the list of cursor available on each system\n(see `sf::Cursor::Type`) to know whether a given cursor is\nexpected to load successfully or is not supported by\nthe operating system.\n\n\\param type Native system cursor type\n\\return Cursor if and only if the corresponding cursor is\nnatively supported by the operating system;\n`std::nullopt` otherwise",
    "\\brief Enumeration of the native system cursor types\n\nRefer to the following table to determine which cursor\nis available on which platform.\n\nType                                       | Linux | macOS | Windows  |\n--------------------------------------------|:-----:|:-----:|:--------:|\n`sf::Cursor::Type::Arrow`                  |  yes  | yes   |   yes    |\n`sf::Cursor::Type::ArrowWait`              |  no   | no    |   yes    |\n`sf::Cursor::Type::Wait`                   |  yes  | no    |   yes    |\n`sf::Cursor::Type::Text`                   |  yes  | yes   |   yes    |\n`sf::Cursor::Type::Hand`                   |  yes  | yes   |   yes    |\n`sf::Cursor::Type::SizeHorizontal`         |  yes  | yes   |   yes    |\n`sf::Cursor::Type::SizeVertical`           |  yes  | yes   |   yes    |\n`sf::Cursor::Type::SizeTopLeftBottomRight` |  no   | yes*  |   yes    |\n`sf::Cursor::Type::SizeBottomLeftTopRight` |  no   | yes*  |   yes    |\n`sf::Cursor::Type::SizeLeft`               |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeRight`              |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeTop`                |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeBottom`             |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeTopLeft`            |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeTopRight`           |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeBottomLeft`         |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeBottomRight`        |  yes  | yes** |   yes**  |\n`sf::Cursor::Type::SizeAll`                |  yes  | no    |   yes    |\n`sf::Cursor::Type::Cross`                  |  yes  | yes   |   yes    |\n`sf::Cursor::Type::Help`                   |  yes  | yes*  |   yes    |\n`sf::Cursor::Type::NotAllowed`             |  yes  | yes   |   yes    |\n\n* These cursor types are undocumented so may not\nbe available on all versions, but have been tested on 10.13\n\n** On Windows and macOS, double-headed arrows are used",
    "Arrow cursor (default)",
    "Busy arrow cursor",
    "Busy cursor",
    "I-beam, cursor when hovering over a field allowing text entry",
    "Pointing hand cursor",
    "Horizontal double arrow cursor",
    "Vertical double arrow cursor",
    "Double arrow cursor going from top-left to bottom-right",
    "Double arrow cursor going from bottom-left to top-right",
    "Left arrow cursor on Linux, same as SizeHorizontal on other platforms",
    "Right arrow cursor on Linux, same as SizeHorizontal on other platforms",
    "Up arrow cursor on Linux, same as SizeVertical on other platforms",
    "Down arrow cursor on Linux, same as SizeVertical on other platforms",
    "Top-left arrow cursor on Linux, same as SizeTopLeftBottomRight on other platforms",
    "Bottom-right arrow cursor on Linux, same as SizeTopLeftBottomRight on other platforms",
    "Bottom-left arrow cursor on Linux, same as SizeBottomLeftTopRight on other platforms",
    "Top-right arrow cursor on Linux, same as SizeBottomLeftTopRight on other platforms",
    "Combination of SizeHorizontal and SizeVertical",
    "Crosshair cursor",
    "Help cursor",
    "Action not allowed cursor",
}; }

void bind_Cursor(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    auto type_sf__Cursor = lua_glue::BindClass<sf::Cursor>(sf, "Cursor");
    lua_glue::Table table_sf__Cursor = sf["Cursor"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Cursor>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Cursor");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Cursor", "new", "fun(type: sf.Cursor.Type): sf.Cursor");
    LUASF_STUB_OVERLOAD("sf.Cursor", "new", "fun(pixels: any, size: sf.Vector2u, hotspot: sf.Vector2u): sf.Cursor");
    lua_glue::BindCallable(type_sf__Cursor, "new",
        [](sf::Cursor::Type type) {
            return lua_sf::makeLuaSharedObject<sf::Cursor>(type);
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__Cursor, "new",
        [](lua_glue::Object pixels, sf::Vector2u size, sf::Vector2u hotspot) {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            return lua_sf::makeLuaSharedObject<sf::Cursor>(pixels_buffer.data(), size, hotspot);
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Cursor", "createFromPixels", "fun(pixels: any, size: sf.Vector2u, hotspot: sf.Vector2u): sf.Cursor|nil");
    lua_glue::BindCallable(type_sf__Cursor, "createFromPixels",
        [lua](lua_glue::Object pixels, sf::Vector2u size, sf::Vector2u hotspot) -> lua_glue::Object {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            return lua_sf::optional_to_object(lua, sf::Cursor::createFromPixels(pixels_buffer.data(), size, hotspot));
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Cursor", "createFromSystem", "fun(type: sf.Cursor.Type): sf.Cursor|nil");
    lua_glue::BindCallable(type_sf__Cursor, "createFromSystem",
        [lua](sf::Cursor::Type type) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, sf::Cursor::createFromSystem(type));
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_CLASS("sf.Cursor.Type");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("Arrow", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FIELD("ArrowWait", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FIELD("Wait", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FIELD("Text", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FIELD("Hand", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FIELD("SizeHorizontal", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FIELD("SizeVertical", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FIELD("SizeTopLeftBottomRight", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FIELD("SizeBottomLeftTopRight", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FIELD("SizeLeft", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FIELD("SizeRight", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FIELD("SizeTop", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FIELD("SizeBottom", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FIELD("SizeTopLeft", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FIELD("SizeBottomRight", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FIELD("SizeBottomLeft", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FIELD("SizeTopRight", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FIELD("SizeAll", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FIELD("Cross", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FIELD("Help", "sf.Cursor.Type");
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FIELD("NotAllowed", "sf.Cursor.Type");
    lua_glue::BindEnum<sf::Cursor::Type>(table_sf__Cursor, "Type", {
        {"Arrow", sf::Cursor::Type::Arrow},
        {"ArrowWait", sf::Cursor::Type::ArrowWait},
        {"Wait", sf::Cursor::Type::Wait},
        {"Text", sf::Cursor::Type::Text},
        {"Hand", sf::Cursor::Type::Hand},
        {"SizeHorizontal", sf::Cursor::Type::SizeHorizontal},
        {"SizeVertical", sf::Cursor::Type::SizeVertical},
        {"SizeTopLeftBottomRight", sf::Cursor::Type::SizeTopLeftBottomRight},
        {"SizeBottomLeftTopRight", sf::Cursor::Type::SizeBottomLeftTopRight},
        {"SizeLeft", sf::Cursor::Type::SizeLeft},
        {"SizeRight", sf::Cursor::Type::SizeRight},
        {"SizeTop", sf::Cursor::Type::SizeTop},
        {"SizeBottom", sf::Cursor::Type::SizeBottom},
        {"SizeTopLeft", sf::Cursor::Type::SizeTopLeft},
        {"SizeBottomRight", sf::Cursor::Type::SizeBottomRight},
        {"SizeBottomLeft", sf::Cursor::Type::SizeBottomLeft},
        {"SizeTopRight", sf::Cursor::Type::SizeTopRight},
        {"SizeAll", sf::Cursor::Type::SizeAll},
        {"Cross", sf::Cursor::Type::Cross},
        {"Help", sf::Cursor::Type::Help},
        {"NotAllowed", sf::Cursor::Type::NotAllowed}
    });
}
