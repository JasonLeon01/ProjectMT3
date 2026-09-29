#include "Window/bind_Keyboard.hpp"

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

namespace { constexpr std::array<std::string_view, 258> docs = {
    "\\brief Key codes\n\nThe enumerators refer to the \"localized\" key; i.e. depending\non the layout set by the operating system, a key can be mapped\nto `Y` or `Z`.",
    "Unhandled key",
    "The A key",
    "The B key",
    "The C key",
    "The D key",
    "The E key",
    "The F key",
    "The G key",
    "The H key",
    "The I key",
    "The J key",
    "The K key",
    "The L key",
    "The M key",
    "The N key",
    "The O key",
    "The P key",
    "The Q key",
    "The R key",
    "The S key",
    "The T key",
    "The U key",
    "The V key",
    "The W key",
    "The X key",
    "The Y key",
    "The Z key",
    "The 0 key",
    "The 1 key",
    "The 2 key",
    "The 3 key",
    "The 4 key",
    "The 5 key",
    "The 6 key",
    "The 7 key",
    "The 8 key",
    "The 9 key",
    "The Escape key",
    "The left Control key",
    "The left Shift key",
    "The left Alt key",
    "The left OS specific key: window (Windows and Linux), apple (macOS), ...",
    "The right Control key",
    "The right Shift key",
    "The right Alt key",
    "The right OS specific key: window (Windows and Linux), apple (macOS), ...",
    "The Menu key",
    "The [ key",
    "The ] key",
    "The ; key",
    "The , key",
    "The . key",
    "The ' key",
    "The / key",
    "The \\ key",
    "The ` key",
    "The = key",
    "The - key (hyphen)",
    "The Space key",
    "The Enter/Return keys",
    "The Backspace key",
    "The Tabulation key",
    "The Page up key",
    "The Page down key",
    "The End key",
    "The Home key",
    "The Insert key",
    "The Delete key",
    "The + key",
    "The - key (minus, usually from numpad)",
    "The * key",
    "Left arrow",
    "Right arrow",
    "Up arrow",
    "Down arrow",
    "The numpad 0 key",
    "The numpad 1 key",
    "The numpad 2 key",
    "The numpad 3 key",
    "The numpad 4 key",
    "The numpad 5 key",
    "The numpad 6 key",
    "The numpad 7 key",
    "The numpad 8 key",
    "The numpad 9 key",
    "The F1 key",
    "The F2 key",
    "The F3 key",
    "The F4 key",
    "The F5 key",
    "The F6 key",
    "The F7 key",
    "The F8 key",
    "The F9 key",
    "The F10 key",
    "The F11 key",
    "The F12 key",
    "The F13 key",
    "The F14 key",
    "The F15 key",
    "The Pause key",
    "\\brief The total number of keyboard keys, ignoring `Key::Unknown`",
    "\\brief Scancodes\n\nThe enumerators are bound to a physical key and do not depend on\nthe keyboard layout used by the operating system. Usually, the AT-101\nkeyboard can be used as reference for the physical position of the keys.",
    "Represents any scancode not present in this enum",
    "Keyboard a and A key",
    "Keyboard b and B key",
    "Keyboard c and C key",
    "Keyboard d and D key",
    "Keyboard e and E key",
    "Keyboard f and F key",
    "Keyboard g and G key",
    "Keyboard h and H key",
    "Keyboard i and I key",
    "Keyboard j and J key",
    "Keyboard k and K key",
    "Keyboard l and L key",
    "Keyboard m and M key",
    "Keyboard n and N key",
    "Keyboard o and O key",
    "Keyboard p and P key",
    "Keyboard q and Q key",
    "Keyboard r and R key",
    "Keyboard s and S key",
    "Keyboard t and T key",
    "Keyboard u and U key",
    "Keyboard v and V key",
    "Keyboard w and W key",
    "Keyboard x and X key",
    "Keyboard y and Y key",
    "Keyboard z and Z key",
    "Keyboard 1 and ! key",
    "Keyboard 2 and @ key",
    "Keyboard 3 and # key",
    "Keyboard 4 and $ key",
    "Keyboard 5 and % key",
    "Keyboard 6 and ^ key",
    "Keyboard 7 and & key",
    "Keyboard 8 and * key",
    "Keyboard 9 and ) key",
    "Keyboard 0 and ) key",
    "Keyboard Enter/Return key",
    "Keyboard Escape key",
    "Keyboard Backspace key",
    "Keyboard Tab key",
    "Keyboard Space key",
    "Keyboard - and _ key",
    "Keyboard = and +",
    "Keyboard [ and { key",
    "Keyboard ] and } key",
    "Keyboard \\ and | key OR various keys for Non-US keyboards",
    "Keyboard ; and : key",
    "Keyboard ' and \" key",
    "Keyboard ` and ~ key",
    "Keyboard , and < key",
    "Keyboard . and > key",
    "Keyboard / and ? key",
    "Keyboard F1 key",
    "Keyboard F2 key",
    "Keyboard F3 key",
    "Keyboard F4 key",
    "Keyboard F5 key",
    "Keyboard F6 key",
    "Keyboard F7 key",
    "Keyboard F8 key",
    "Keyboard F9 key",
    "Keyboard F10 key",
    "Keyboard F11 key",
    "Keyboard F12 key",
    "Keyboard F13 key",
    "Keyboard F14 key",
    "Keyboard F15 key",
    "Keyboard F16 key",
    "Keyboard F17 key",
    "Keyboard F18 key",
    "Keyboard F19 key",
    "Keyboard F20 key",
    "Keyboard F21 key",
    "Keyboard F22 key",
    "Keyboard F23 key",
    "Keyboard F24 key",
    "Keyboard Caps %Lock key",
    "Keyboard Print Screen key",
    "Keyboard Scroll %Lock key",
    "Keyboard Pause key",
    "Keyboard Insert key",
    "Keyboard Home key",
    "Keyboard Page Up key",
    "Keyboard Delete Forward key",
    "Keyboard End key",
    "Keyboard Page Down key",
    "Keyboard Right Arrow key",
    "Keyboard Left Arrow key",
    "Keyboard Down Arrow key",
    "Keyboard Up Arrow key",
    "Keypad Num %Lock and Clear key",
    "Keypad / key",
    "Keypad * key",
    "Keypad - key",
    "Keypad + key",
    "keypad = key",
    "Keypad Enter/Return key",
    "Keypad . and Delete key",
    "Keypad 1 and End key",
    "Keypad 2 and Down Arrow key",
    "Keypad 3 and Page Down key",
    "Keypad 4 and Left Arrow key",
    "Keypad 5 key",
    "Keypad 6 and Right Arrow key",
    "Keypad 7 and Home key",
    "Keypad 8 and Up Arrow key",
    "Keypad 9 and Page Up key",
    "Keypad 0 and Insert key",
    "Keyboard Non-US \\ and | key",
    "Keyboard Application key",
    "Keyboard Execute key",
    "Keyboard Mode Change key",
    "Keyboard Help key",
    "Keyboard Menu key",
    "Keyboard Select key",
    "Keyboard Redo key",
    "Keyboard Undo key",
    "Keyboard Cut key",
    "Keyboard Copy key",
    "Keyboard Paste key",
    "Keyboard Volume Mute key",
    "Keyboard Volume Up key",
    "Keyboard Volume Down key",
    "Keyboard Media Play Pause key",
    "Keyboard Media Stop key",
    "Keyboard Media Next Track key",
    "Keyboard Media Previous Track key",
    "Keyboard Left Control key",
    "Keyboard Left Shift key",
    "Keyboard Left Alt key",
    "Keyboard Left System key",
    "Keyboard Right Control key",
    "Keyboard Right Shift key",
    "Keyboard Right Alt key",
    "Keyboard Right System key",
    "Keyboard Back key",
    "Keyboard Forward key",
    "Keyboard Refresh key",
    "Keyboard Stop key",
    "Keyboard Search key",
    "Keyboard Favorites key",
    "Keyboard Home Page key",
    "Keyboard Launch Application 1 key",
    "Keyboard Launch Application 2 key",
    "Keyboard Launch Mail key",
    "Keyboard Launch Media Select key",
    "\\brief The total number of scancodes, ignoring `Scan::Unknown`",
    "\\brief Check if a key is pressed\n\n\\warning On macOS you're required to grant input monitoring access for\nyour application in order for `isKeyPressed` to work.\n\n\\param key Key to check\n\n\\return `true` if the key is pressed, `false` otherwise",
    "\\brief Check if a key is pressed\n\n\\warning On macOS you're required to grant input monitoring access for\nyour application in order for `isKeyPressed` to work.\n\n\\param code Scancode to check\n\n\\return `true` if the physical key is pressed, `false` otherwise",
    "\\brief Localize a physical key to a logical one\n\n\\param code Scancode to localize\n\n\\return The key corresponding to the scancode under the current\nkeyboard layout used by the operating system, or\n`sf::Keyboard::Key::Unknown` when the scancode cannot be mapped\nto a Key.\n\n\\see `delocalize`",
    "\\brief Identify the physical key corresponding to a logical one\n\n\\param key Key to \"delocalize\"\n\n\\return The scancode corresponding to the key under the current\nkeyboard layout used by the operating system, or\n`sf::Keyboard::Scan::Unknown` when the key cannot be mapped\nto a `sf::Keyboard::Scancode`.\n\n\\see `localize`",
    "\\brief Provide a string representation for a given scancode\n\nThe returned string is a short, non-technical description of\nthe key represented with the given scancode. Most effectively\nused in user interfaces, as the description for the key takes\nthe users keyboard layout into consideration.\n\n\\warning The result is OS-dependent: for example, `sf::Keyboard::Scan::LSystem`\nis \"Left Meta\" on Linux, \"Left Windows\" on Windows and\n\"Left Command\" on macOS.\n\nThe current keyboard layout set by the operating system is used to\ninterpret the scancode: for example, `sf::Keyboard::Key::Semicolon` is\nmapped to \";\" for layout and to \"\u00e9\" for others.\n\n\\param code Scancode to check\n\n\\return The localized description of the code",
    "\\brief Show or hide the virtual keyboard\n\n\\warning The virtual keyboard is not supported on all\nsystems. It will typically be implemented on mobile OSes\n(Android, iOS) but not on desktop OSes (Windows, Linux, ...).\n\nIf the virtual keyboard is not available, this function does\nnothing.\n\n\\param visible `true` to show, `false` to hide",
}; }

void bind_Keyboard(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    lua_glue::Table sf_Keyboard = sf["Keyboard"].get_or_create<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("Unknown", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("A", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("B", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("C", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("D", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("E", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FIELD("F", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FIELD("G", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FIELD("H", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FIELD("I", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FIELD("J", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FIELD("K", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FIELD("L", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FIELD("M", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FIELD("N", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FIELD("O", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FIELD("P", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FIELD("Q", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FIELD("R", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FIELD("S", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FIELD("T", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FIELD("U", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FIELD("V", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FIELD("W", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FIELD("X", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FIELD("Y", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FIELD("Z", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FIELD("Num0", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_FIELD("Num1", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FIELD("Num2", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FIELD("Num3", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FIELD("Num4", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FIELD("Num5", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FIELD("Num6", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FIELD("Num7", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FIELD("Num8", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[37]);
    LUASF_STUB_FIELD("Num9", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FIELD("Escape", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FIELD("LControl", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[40]);
    LUASF_STUB_FIELD("LShift", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[41]);
    LUASF_STUB_FIELD("LAlt", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[42]);
    LUASF_STUB_FIELD("LSystem", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[43]);
    LUASF_STUB_FIELD("RControl", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[44]);
    LUASF_STUB_FIELD("RShift", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[45]);
    LUASF_STUB_FIELD("RAlt", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[46]);
    LUASF_STUB_FIELD("RSystem", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[47]);
    LUASF_STUB_FIELD("Menu", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[48]);
    LUASF_STUB_FIELD("LBracket", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[49]);
    LUASF_STUB_FIELD("RBracket", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[50]);
    LUASF_STUB_FIELD("Semicolon", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[51]);
    LUASF_STUB_FIELD("Comma", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[52]);
    LUASF_STUB_FIELD("Period", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[53]);
    LUASF_STUB_FIELD("Apostrophe", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[54]);
    LUASF_STUB_FIELD("Slash", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[55]);
    LUASF_STUB_FIELD("Backslash", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[56]);
    LUASF_STUB_FIELD("Grave", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[57]);
    LUASF_STUB_FIELD("Equal", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[58]);
    LUASF_STUB_FIELD("Hyphen", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[59]);
    LUASF_STUB_FIELD("Space", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[60]);
    LUASF_STUB_FIELD("Enter", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[61]);
    LUASF_STUB_FIELD("Backspace", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[62]);
    LUASF_STUB_FIELD("Tab", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[63]);
    LUASF_STUB_FIELD("PageUp", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[64]);
    LUASF_STUB_FIELD("PageDown", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[65]);
    LUASF_STUB_FIELD("End", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[66]);
    LUASF_STUB_FIELD("Home", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[67]);
    LUASF_STUB_FIELD("Insert", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[68]);
    LUASF_STUB_FIELD("Delete", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[69]);
    LUASF_STUB_FIELD("Add", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[70]);
    LUASF_STUB_FIELD("Subtract", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[71]);
    LUASF_STUB_FIELD("Multiply", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[54]);
    LUASF_STUB_FIELD("Divide", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[72]);
    LUASF_STUB_FIELD("Left", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[73]);
    LUASF_STUB_FIELD("Right", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[74]);
    LUASF_STUB_FIELD("Up", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[75]);
    LUASF_STUB_FIELD("Down", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[76]);
    LUASF_STUB_FIELD("Numpad0", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[77]);
    LUASF_STUB_FIELD("Numpad1", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[78]);
    LUASF_STUB_FIELD("Numpad2", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[79]);
    LUASF_STUB_FIELD("Numpad3", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[80]);
    LUASF_STUB_FIELD("Numpad4", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[81]);
    LUASF_STUB_FIELD("Numpad5", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[82]);
    LUASF_STUB_FIELD("Numpad6", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[83]);
    LUASF_STUB_FIELD("Numpad7", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[84]);
    LUASF_STUB_FIELD("Numpad8", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[85]);
    LUASF_STUB_FIELD("Numpad9", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[86]);
    LUASF_STUB_FIELD("F1", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[87]);
    LUASF_STUB_FIELD("F2", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[88]);
    LUASF_STUB_FIELD("F3", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[89]);
    LUASF_STUB_FIELD("F4", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[90]);
    LUASF_STUB_FIELD("F5", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[91]);
    LUASF_STUB_FIELD("F6", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[92]);
    LUASF_STUB_FIELD("F7", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[93]);
    LUASF_STUB_FIELD("F8", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[94]);
    LUASF_STUB_FIELD("F9", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[95]);
    LUASF_STUB_FIELD("F10", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[96]);
    LUASF_STUB_FIELD("F11", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[97]);
    LUASF_STUB_FIELD("F12", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[98]);
    LUASF_STUB_FIELD("F13", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[99]);
    LUASF_STUB_FIELD("F14", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[100]);
    LUASF_STUB_FIELD("F15", "sf.Keyboard.Key");
    LUASF_STUB_DOC(docs[101]);
    LUASF_STUB_FIELD("Pause", "sf.Keyboard.Key");
    lua_glue::BindEnum<sf::Keyboard::Key>(sf_Keyboard, "Key", {
        {"Unknown", sf::Keyboard::Key::Unknown},
        {"A", sf::Keyboard::Key::A},
        {"B", sf::Keyboard::Key::B},
        {"C", sf::Keyboard::Key::C},
        {"D", sf::Keyboard::Key::D},
        {"E", sf::Keyboard::Key::E},
        {"F", sf::Keyboard::Key::F},
        {"G", sf::Keyboard::Key::G},
        {"H", sf::Keyboard::Key::H},
        {"I", sf::Keyboard::Key::I},
        {"J", sf::Keyboard::Key::J},
        {"K", sf::Keyboard::Key::K},
        {"L", sf::Keyboard::Key::L},
        {"M", sf::Keyboard::Key::M},
        {"N", sf::Keyboard::Key::N},
        {"O", sf::Keyboard::Key::O},
        {"P", sf::Keyboard::Key::P},
        {"Q", sf::Keyboard::Key::Q},
        {"R", sf::Keyboard::Key::R},
        {"S", sf::Keyboard::Key::S},
        {"T", sf::Keyboard::Key::T},
        {"U", sf::Keyboard::Key::U},
        {"V", sf::Keyboard::Key::V},
        {"W", sf::Keyboard::Key::W},
        {"X", sf::Keyboard::Key::X},
        {"Y", sf::Keyboard::Key::Y},
        {"Z", sf::Keyboard::Key::Z},
        {"Num0", sf::Keyboard::Key::Num0},
        {"Num1", sf::Keyboard::Key::Num1},
        {"Num2", sf::Keyboard::Key::Num2},
        {"Num3", sf::Keyboard::Key::Num3},
        {"Num4", sf::Keyboard::Key::Num4},
        {"Num5", sf::Keyboard::Key::Num5},
        {"Num6", sf::Keyboard::Key::Num6},
        {"Num7", sf::Keyboard::Key::Num7},
        {"Num8", sf::Keyboard::Key::Num8},
        {"Num9", sf::Keyboard::Key::Num9},
        {"Escape", sf::Keyboard::Key::Escape},
        {"LControl", sf::Keyboard::Key::LControl},
        {"LShift", sf::Keyboard::Key::LShift},
        {"LAlt", sf::Keyboard::Key::LAlt},
        {"LSystem", sf::Keyboard::Key::LSystem},
        {"RControl", sf::Keyboard::Key::RControl},
        {"RShift", sf::Keyboard::Key::RShift},
        {"RAlt", sf::Keyboard::Key::RAlt},
        {"RSystem", sf::Keyboard::Key::RSystem},
        {"Menu", sf::Keyboard::Key::Menu},
        {"LBracket", sf::Keyboard::Key::LBracket},
        {"RBracket", sf::Keyboard::Key::RBracket},
        {"Semicolon", sf::Keyboard::Key::Semicolon},
        {"Comma", sf::Keyboard::Key::Comma},
        {"Period", sf::Keyboard::Key::Period},
        {"Apostrophe", sf::Keyboard::Key::Apostrophe},
        {"Slash", sf::Keyboard::Key::Slash},
        {"Backslash", sf::Keyboard::Key::Backslash},
        {"Grave", sf::Keyboard::Key::Grave},
        {"Equal", sf::Keyboard::Key::Equal},
        {"Hyphen", sf::Keyboard::Key::Hyphen},
        {"Space", sf::Keyboard::Key::Space},
        {"Enter", sf::Keyboard::Key::Enter},
        {"Backspace", sf::Keyboard::Key::Backspace},
        {"Tab", sf::Keyboard::Key::Tab},
        {"PageUp", sf::Keyboard::Key::PageUp},
        {"PageDown", sf::Keyboard::Key::PageDown},
        {"End", sf::Keyboard::Key::End},
        {"Home", sf::Keyboard::Key::Home},
        {"Insert", sf::Keyboard::Key::Insert},
        {"Delete", sf::Keyboard::Key::Delete},
        {"Add", sf::Keyboard::Key::Add},
        {"Subtract", sf::Keyboard::Key::Subtract},
        {"Multiply", sf::Keyboard::Key::Multiply},
        {"Divide", sf::Keyboard::Key::Divide},
        {"Left", sf::Keyboard::Key::Left},
        {"Right", sf::Keyboard::Key::Right},
        {"Up", sf::Keyboard::Key::Up},
        {"Down", sf::Keyboard::Key::Down},
        {"Numpad0", sf::Keyboard::Key::Numpad0},
        {"Numpad1", sf::Keyboard::Key::Numpad1},
        {"Numpad2", sf::Keyboard::Key::Numpad2},
        {"Numpad3", sf::Keyboard::Key::Numpad3},
        {"Numpad4", sf::Keyboard::Key::Numpad4},
        {"Numpad5", sf::Keyboard::Key::Numpad5},
        {"Numpad6", sf::Keyboard::Key::Numpad6},
        {"Numpad7", sf::Keyboard::Key::Numpad7},
        {"Numpad8", sf::Keyboard::Key::Numpad8},
        {"Numpad9", sf::Keyboard::Key::Numpad9},
        {"F1", sf::Keyboard::Key::F1},
        {"F2", sf::Keyboard::Key::F2},
        {"F3", sf::Keyboard::Key::F3},
        {"F4", sf::Keyboard::Key::F4},
        {"F5", sf::Keyboard::Key::F5},
        {"F6", sf::Keyboard::Key::F6},
        {"F7", sf::Keyboard::Key::F7},
        {"F8", sf::Keyboard::Key::F8},
        {"F9", sf::Keyboard::Key::F9},
        {"F10", sf::Keyboard::Key::F10},
        {"F11", sf::Keyboard::Key::F11},
        {"F12", sf::Keyboard::Key::F12},
        {"F13", sf::Keyboard::Key::F13},
        {"F14", sf::Keyboard::Key::F14},
        {"F15", sf::Keyboard::Key::F15},
        {"Pause", sf::Keyboard::Key::Pause}
    });
    LUASF_STUB_DOC(docs[102]);
    LUASF_STUB_VALUE("sf.Keyboard", "KeyCount", "integer");
    lua_glue::BindStaticAttr<const unsigned int>(sf_Keyboard, "KeyCount", &sf::Keyboard::KeyCount);
    LUASF_STUB_DOC(docs[103]);
    LUASF_STUB_CLASS("sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[104]);
    LUASF_STUB_FIELD("Unknown", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[105]);
    LUASF_STUB_FIELD("A", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[106]);
    LUASF_STUB_FIELD("B", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[107]);
    LUASF_STUB_FIELD("C", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[108]);
    LUASF_STUB_FIELD("D", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[109]);
    LUASF_STUB_FIELD("E", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[110]);
    LUASF_STUB_FIELD("F", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[111]);
    LUASF_STUB_FIELD("G", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[112]);
    LUASF_STUB_FIELD("H", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[113]);
    LUASF_STUB_FIELD("I", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[114]);
    LUASF_STUB_FIELD("J", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[115]);
    LUASF_STUB_FIELD("K", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[116]);
    LUASF_STUB_FIELD("L", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[117]);
    LUASF_STUB_FIELD("M", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[118]);
    LUASF_STUB_FIELD("N", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[119]);
    LUASF_STUB_FIELD("O", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[120]);
    LUASF_STUB_FIELD("P", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[121]);
    LUASF_STUB_FIELD("Q", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[122]);
    LUASF_STUB_FIELD("R", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[123]);
    LUASF_STUB_FIELD("S", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[124]);
    LUASF_STUB_FIELD("T", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[125]);
    LUASF_STUB_FIELD("U", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[126]);
    LUASF_STUB_FIELD("V", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[127]);
    LUASF_STUB_FIELD("W", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[128]);
    LUASF_STUB_FIELD("X", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[129]);
    LUASF_STUB_FIELD("Y", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[130]);
    LUASF_STUB_FIELD("Z", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[131]);
    LUASF_STUB_FIELD("Num1", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[132]);
    LUASF_STUB_FIELD("Num2", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[133]);
    LUASF_STUB_FIELD("Num3", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[134]);
    LUASF_STUB_FIELD("Num4", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[135]);
    LUASF_STUB_FIELD("Num5", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[136]);
    LUASF_STUB_FIELD("Num6", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[137]);
    LUASF_STUB_FIELD("Num7", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[138]);
    LUASF_STUB_FIELD("Num8", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[139]);
    LUASF_STUB_FIELD("Num9", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[140]);
    LUASF_STUB_FIELD("Num0", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[141]);
    LUASF_STUB_FIELD("Enter", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[142]);
    LUASF_STUB_FIELD("Escape", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[143]);
    LUASF_STUB_FIELD("Backspace", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[144]);
    LUASF_STUB_FIELD("Tab", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[145]);
    LUASF_STUB_FIELD("Space", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[146]);
    LUASF_STUB_FIELD("Hyphen", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[147]);
    LUASF_STUB_FIELD("Equal", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[148]);
    LUASF_STUB_FIELD("LBracket", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[149]);
    LUASF_STUB_FIELD("RBracket", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[150]);
    LUASF_STUB_FIELD("Backslash", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[151]);
    LUASF_STUB_FIELD("Semicolon", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[152]);
    LUASF_STUB_FIELD("Apostrophe", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[153]);
    LUASF_STUB_FIELD("Grave", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[154]);
    LUASF_STUB_FIELD("Comma", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[155]);
    LUASF_STUB_FIELD("Period", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[156]);
    LUASF_STUB_FIELD("Slash", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[157]);
    LUASF_STUB_FIELD("F1", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[158]);
    LUASF_STUB_FIELD("F2", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[159]);
    LUASF_STUB_FIELD("F3", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[160]);
    LUASF_STUB_FIELD("F4", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[161]);
    LUASF_STUB_FIELD("F5", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[162]);
    LUASF_STUB_FIELD("F6", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[163]);
    LUASF_STUB_FIELD("F7", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[164]);
    LUASF_STUB_FIELD("F8", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[165]);
    LUASF_STUB_FIELD("F9", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[166]);
    LUASF_STUB_FIELD("F10", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[167]);
    LUASF_STUB_FIELD("F11", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[168]);
    LUASF_STUB_FIELD("F12", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[169]);
    LUASF_STUB_FIELD("F13", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[170]);
    LUASF_STUB_FIELD("F14", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[171]);
    LUASF_STUB_FIELD("F15", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[172]);
    LUASF_STUB_FIELD("F16", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[173]);
    LUASF_STUB_FIELD("F17", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[174]);
    LUASF_STUB_FIELD("F18", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[175]);
    LUASF_STUB_FIELD("F19", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[176]);
    LUASF_STUB_FIELD("F20", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[177]);
    LUASF_STUB_FIELD("F21", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[178]);
    LUASF_STUB_FIELD("F22", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[179]);
    LUASF_STUB_FIELD("F23", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[180]);
    LUASF_STUB_FIELD("F24", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[181]);
    LUASF_STUB_FIELD("CapsLock", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[182]);
    LUASF_STUB_FIELD("PrintScreen", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[183]);
    LUASF_STUB_FIELD("ScrollLock", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[184]);
    LUASF_STUB_FIELD("Pause", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[185]);
    LUASF_STUB_FIELD("Insert", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[186]);
    LUASF_STUB_FIELD("Home", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[187]);
    LUASF_STUB_FIELD("PageUp", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[188]);
    LUASF_STUB_FIELD("Delete", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[189]);
    LUASF_STUB_FIELD("End", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[190]);
    LUASF_STUB_FIELD("PageDown", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[191]);
    LUASF_STUB_FIELD("Right", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[192]);
    LUASF_STUB_FIELD("Left", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[193]);
    LUASF_STUB_FIELD("Down", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[194]);
    LUASF_STUB_FIELD("Up", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[195]);
    LUASF_STUB_FIELD("NumLock", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[196]);
    LUASF_STUB_FIELD("NumpadDivide", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[197]);
    LUASF_STUB_FIELD("NumpadMultiply", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[198]);
    LUASF_STUB_FIELD("NumpadMinus", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[199]);
    LUASF_STUB_FIELD("NumpadPlus", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[200]);
    LUASF_STUB_FIELD("NumpadEqual", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[201]);
    LUASF_STUB_FIELD("NumpadEnter", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[202]);
    LUASF_STUB_FIELD("NumpadDecimal", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[203]);
    LUASF_STUB_FIELD("Numpad1", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[204]);
    LUASF_STUB_FIELD("Numpad2", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[205]);
    LUASF_STUB_FIELD("Numpad3", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[206]);
    LUASF_STUB_FIELD("Numpad4", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[207]);
    LUASF_STUB_FIELD("Numpad5", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[208]);
    LUASF_STUB_FIELD("Numpad6", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[209]);
    LUASF_STUB_FIELD("Numpad7", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[210]);
    LUASF_STUB_FIELD("Numpad8", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[211]);
    LUASF_STUB_FIELD("Numpad9", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[212]);
    LUASF_STUB_FIELD("Numpad0", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[213]);
    LUASF_STUB_FIELD("NonUsBackslash", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[214]);
    LUASF_STUB_FIELD("Application", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[215]);
    LUASF_STUB_FIELD("Execute", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[216]);
    LUASF_STUB_FIELD("ModeChange", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[217]);
    LUASF_STUB_FIELD("Help", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[218]);
    LUASF_STUB_FIELD("Menu", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[219]);
    LUASF_STUB_FIELD("Select", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[220]);
    LUASF_STUB_FIELD("Redo", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[221]);
    LUASF_STUB_FIELD("Undo", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[222]);
    LUASF_STUB_FIELD("Cut", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[223]);
    LUASF_STUB_FIELD("Copy", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[224]);
    LUASF_STUB_FIELD("Paste", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[225]);
    LUASF_STUB_FIELD("VolumeMute", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[226]);
    LUASF_STUB_FIELD("VolumeUp", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[227]);
    LUASF_STUB_FIELD("VolumeDown", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[228]);
    LUASF_STUB_FIELD("MediaPlayPause", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[229]);
    LUASF_STUB_FIELD("MediaStop", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[230]);
    LUASF_STUB_FIELD("MediaNextTrack", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[231]);
    LUASF_STUB_FIELD("MediaPreviousTrack", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[232]);
    LUASF_STUB_FIELD("LControl", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[233]);
    LUASF_STUB_FIELD("LShift", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[234]);
    LUASF_STUB_FIELD("LAlt", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[235]);
    LUASF_STUB_FIELD("LSystem", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[236]);
    LUASF_STUB_FIELD("RControl", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[237]);
    LUASF_STUB_FIELD("RShift", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[238]);
    LUASF_STUB_FIELD("RAlt", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[239]);
    LUASF_STUB_FIELD("RSystem", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[240]);
    LUASF_STUB_FIELD("Back", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[241]);
    LUASF_STUB_FIELD("Forward", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[242]);
    LUASF_STUB_FIELD("Refresh", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[243]);
    LUASF_STUB_FIELD("Stop", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[244]);
    LUASF_STUB_FIELD("Search", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[245]);
    LUASF_STUB_FIELD("Favorites", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[246]);
    LUASF_STUB_FIELD("HomePage", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[247]);
    LUASF_STUB_FIELD("LaunchApplication1", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[248]);
    LUASF_STUB_FIELD("LaunchApplication2", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[249]);
    LUASF_STUB_FIELD("LaunchMail", "sf.Keyboard.Scan");
    LUASF_STUB_DOC(docs[250]);
    LUASF_STUB_FIELD("LaunchMediaSelect", "sf.Keyboard.Scan");
    lua_glue::BindEnum<sf::Keyboard::Scan>(sf_Keyboard, "Scan", {
        {"Unknown", sf::Keyboard::Scan::Unknown},
        {"A", sf::Keyboard::Scan::A},
        {"B", sf::Keyboard::Scan::B},
        {"C", sf::Keyboard::Scan::C},
        {"D", sf::Keyboard::Scan::D},
        {"E", sf::Keyboard::Scan::E},
        {"F", sf::Keyboard::Scan::F},
        {"G", sf::Keyboard::Scan::G},
        {"H", sf::Keyboard::Scan::H},
        {"I", sf::Keyboard::Scan::I},
        {"J", sf::Keyboard::Scan::J},
        {"K", sf::Keyboard::Scan::K},
        {"L", sf::Keyboard::Scan::L},
        {"M", sf::Keyboard::Scan::M},
        {"N", sf::Keyboard::Scan::N},
        {"O", sf::Keyboard::Scan::O},
        {"P", sf::Keyboard::Scan::P},
        {"Q", sf::Keyboard::Scan::Q},
        {"R", sf::Keyboard::Scan::R},
        {"S", sf::Keyboard::Scan::S},
        {"T", sf::Keyboard::Scan::T},
        {"U", sf::Keyboard::Scan::U},
        {"V", sf::Keyboard::Scan::V},
        {"W", sf::Keyboard::Scan::W},
        {"X", sf::Keyboard::Scan::X},
        {"Y", sf::Keyboard::Scan::Y},
        {"Z", sf::Keyboard::Scan::Z},
        {"Num1", sf::Keyboard::Scan::Num1},
        {"Num2", sf::Keyboard::Scan::Num2},
        {"Num3", sf::Keyboard::Scan::Num3},
        {"Num4", sf::Keyboard::Scan::Num4},
        {"Num5", sf::Keyboard::Scan::Num5},
        {"Num6", sf::Keyboard::Scan::Num6},
        {"Num7", sf::Keyboard::Scan::Num7},
        {"Num8", sf::Keyboard::Scan::Num8},
        {"Num9", sf::Keyboard::Scan::Num9},
        {"Num0", sf::Keyboard::Scan::Num0},
        {"Enter", sf::Keyboard::Scan::Enter},
        {"Escape", sf::Keyboard::Scan::Escape},
        {"Backspace", sf::Keyboard::Scan::Backspace},
        {"Tab", sf::Keyboard::Scan::Tab},
        {"Space", sf::Keyboard::Scan::Space},
        {"Hyphen", sf::Keyboard::Scan::Hyphen},
        {"Equal", sf::Keyboard::Scan::Equal},
        {"LBracket", sf::Keyboard::Scan::LBracket},
        {"RBracket", sf::Keyboard::Scan::RBracket},
        {"Backslash", sf::Keyboard::Scan::Backslash},
        {"Semicolon", sf::Keyboard::Scan::Semicolon},
        {"Apostrophe", sf::Keyboard::Scan::Apostrophe},
        {"Grave", sf::Keyboard::Scan::Grave},
        {"Comma", sf::Keyboard::Scan::Comma},
        {"Period", sf::Keyboard::Scan::Period},
        {"Slash", sf::Keyboard::Scan::Slash},
        {"F1", sf::Keyboard::Scan::F1},
        {"F2", sf::Keyboard::Scan::F2},
        {"F3", sf::Keyboard::Scan::F3},
        {"F4", sf::Keyboard::Scan::F4},
        {"F5", sf::Keyboard::Scan::F5},
        {"F6", sf::Keyboard::Scan::F6},
        {"F7", sf::Keyboard::Scan::F7},
        {"F8", sf::Keyboard::Scan::F8},
        {"F9", sf::Keyboard::Scan::F9},
        {"F10", sf::Keyboard::Scan::F10},
        {"F11", sf::Keyboard::Scan::F11},
        {"F12", sf::Keyboard::Scan::F12},
        {"F13", sf::Keyboard::Scan::F13},
        {"F14", sf::Keyboard::Scan::F14},
        {"F15", sf::Keyboard::Scan::F15},
        {"F16", sf::Keyboard::Scan::F16},
        {"F17", sf::Keyboard::Scan::F17},
        {"F18", sf::Keyboard::Scan::F18},
        {"F19", sf::Keyboard::Scan::F19},
        {"F20", sf::Keyboard::Scan::F20},
        {"F21", sf::Keyboard::Scan::F21},
        {"F22", sf::Keyboard::Scan::F22},
        {"F23", sf::Keyboard::Scan::F23},
        {"F24", sf::Keyboard::Scan::F24},
        {"CapsLock", sf::Keyboard::Scan::CapsLock},
        {"PrintScreen", sf::Keyboard::Scan::PrintScreen},
        {"ScrollLock", sf::Keyboard::Scan::ScrollLock},
        {"Pause", sf::Keyboard::Scan::Pause},
        {"Insert", sf::Keyboard::Scan::Insert},
        {"Home", sf::Keyboard::Scan::Home},
        {"PageUp", sf::Keyboard::Scan::PageUp},
        {"Delete", sf::Keyboard::Scan::Delete},
        {"End", sf::Keyboard::Scan::End},
        {"PageDown", sf::Keyboard::Scan::PageDown},
        {"Right", sf::Keyboard::Scan::Right},
        {"Left", sf::Keyboard::Scan::Left},
        {"Down", sf::Keyboard::Scan::Down},
        {"Up", sf::Keyboard::Scan::Up},
        {"NumLock", sf::Keyboard::Scan::NumLock},
        {"NumpadDivide", sf::Keyboard::Scan::NumpadDivide},
        {"NumpadMultiply", sf::Keyboard::Scan::NumpadMultiply},
        {"NumpadMinus", sf::Keyboard::Scan::NumpadMinus},
        {"NumpadPlus", sf::Keyboard::Scan::NumpadPlus},
        {"NumpadEqual", sf::Keyboard::Scan::NumpadEqual},
        {"NumpadEnter", sf::Keyboard::Scan::NumpadEnter},
        {"NumpadDecimal", sf::Keyboard::Scan::NumpadDecimal},
        {"Numpad1", sf::Keyboard::Scan::Numpad1},
        {"Numpad2", sf::Keyboard::Scan::Numpad2},
        {"Numpad3", sf::Keyboard::Scan::Numpad3},
        {"Numpad4", sf::Keyboard::Scan::Numpad4},
        {"Numpad5", sf::Keyboard::Scan::Numpad5},
        {"Numpad6", sf::Keyboard::Scan::Numpad6},
        {"Numpad7", sf::Keyboard::Scan::Numpad7},
        {"Numpad8", sf::Keyboard::Scan::Numpad8},
        {"Numpad9", sf::Keyboard::Scan::Numpad9},
        {"Numpad0", sf::Keyboard::Scan::Numpad0},
        {"NonUsBackslash", sf::Keyboard::Scan::NonUsBackslash},
        {"Application", sf::Keyboard::Scan::Application},
        {"Execute", sf::Keyboard::Scan::Execute},
        {"ModeChange", sf::Keyboard::Scan::ModeChange},
        {"Help", sf::Keyboard::Scan::Help},
        {"Menu", sf::Keyboard::Scan::Menu},
        {"Select", sf::Keyboard::Scan::Select},
        {"Redo", sf::Keyboard::Scan::Redo},
        {"Undo", sf::Keyboard::Scan::Undo},
        {"Cut", sf::Keyboard::Scan::Cut},
        {"Copy", sf::Keyboard::Scan::Copy},
        {"Paste", sf::Keyboard::Scan::Paste},
        {"VolumeMute", sf::Keyboard::Scan::VolumeMute},
        {"VolumeUp", sf::Keyboard::Scan::VolumeUp},
        {"VolumeDown", sf::Keyboard::Scan::VolumeDown},
        {"MediaPlayPause", sf::Keyboard::Scan::MediaPlayPause},
        {"MediaStop", sf::Keyboard::Scan::MediaStop},
        {"MediaNextTrack", sf::Keyboard::Scan::MediaNextTrack},
        {"MediaPreviousTrack", sf::Keyboard::Scan::MediaPreviousTrack},
        {"LControl", sf::Keyboard::Scan::LControl},
        {"LShift", sf::Keyboard::Scan::LShift},
        {"LAlt", sf::Keyboard::Scan::LAlt},
        {"LSystem", sf::Keyboard::Scan::LSystem},
        {"RControl", sf::Keyboard::Scan::RControl},
        {"RShift", sf::Keyboard::Scan::RShift},
        {"RAlt", sf::Keyboard::Scan::RAlt},
        {"RSystem", sf::Keyboard::Scan::RSystem},
        {"Back", sf::Keyboard::Scan::Back},
        {"Forward", sf::Keyboard::Scan::Forward},
        {"Refresh", sf::Keyboard::Scan::Refresh},
        {"Stop", sf::Keyboard::Scan::Stop},
        {"Search", sf::Keyboard::Scan::Search},
        {"Favorites", sf::Keyboard::Scan::Favorites},
        {"HomePage", sf::Keyboard::Scan::HomePage},
        {"LaunchApplication1", sf::Keyboard::Scan::LaunchApplication1},
        {"LaunchApplication2", sf::Keyboard::Scan::LaunchApplication2},
        {"LaunchMail", sf::Keyboard::Scan::LaunchMail},
        {"LaunchMediaSelect", sf::Keyboard::Scan::LaunchMediaSelect}
    });
    LUASF_STUB_ALIAS("sf.Keyboard.Scancode", "sf.Keyboard.Scan");
    {
        const lua_glue::Object aliasValue = sf_Keyboard.raw_get<lua_glue::Object>("Scancode");
        const lua_glue::Object aliasTarget = sf_Keyboard.raw_get<lua_glue::Object>("Scan");
        if ((!aliasValue.valid() || aliasValue.get_type() == lua_glue::Type::Nil) &&
            aliasTarget.valid() && aliasTarget.get_type() != lua_glue::Type::Nil)
            sf_Keyboard.raw_set("Scancode", aliasTarget);
    }
    LUASF_STUB_DOC(docs[251]);
    LUASF_STUB_VALUE("sf.Keyboard", "ScancodeCount", "integer");
    lua_glue::BindStaticAttr<const unsigned int>(sf_Keyboard, "ScancodeCount", &sf::Keyboard::ScancodeCount);
    LUASF_STUB_DOC(docs[252]);
    LUASF_STUB_FUNCTION("sf.Keyboard", "isKeyPressed", "fun(key: sf.Keyboard.Key): boolean");
    LUASF_STUB_OVERLOAD("sf.Keyboard", "isKeyPressed", "fun(code: sf.Keyboard.Scan): boolean");
    lua_glue::BindCallable(sf_Keyboard, "isKeyPressed",
        [](sf::Keyboard::Key key) -> bool {
            return sf::Keyboard::isKeyPressed(key);
        },
        docs[252]
    );
    lua_glue::BindCallable(sf_Keyboard, "isKeyPressed",
        [](sf::Keyboard::Scancode code) -> bool {
            return sf::Keyboard::isKeyPressed(code);
        },
        docs[253]
    );
    LUASF_STUB_DOC(docs[254]);
    LUASF_STUB_FUNCTION("sf.Keyboard", "localize", "fun(code: sf.Keyboard.Scan): sf.Keyboard.Key");
    lua_glue::BindCallable(sf_Keyboard, "localize",
        [](sf::Keyboard::Scancode code) -> sf::Keyboard::Key {
            return sf::Keyboard::localize(code);
        },
        docs[254]
    );
    LUASF_STUB_DOC(docs[255]);
    LUASF_STUB_FUNCTION("sf.Keyboard", "delocalize", "fun(key: sf.Keyboard.Key): sf.Keyboard.Scancode");
    lua_glue::BindCallable(sf_Keyboard, "delocalize",
        [](sf::Keyboard::Key key) -> sf::Keyboard::Scancode {
            return sf::Keyboard::delocalize(key);
        },
        docs[255]
    );
    LUASF_STUB_DOC(docs[256]);
    LUASF_STUB_FUNCTION("sf.Keyboard", "getDescription", "fun(code: sf.Keyboard.Scan): string");
    lua_glue::BindCallable(sf_Keyboard, "getDescription",
        [](sf::Keyboard::Scancode code) -> std::string {
            return lua_sf::to_utf8_string(sf::Keyboard::getDescription(code));
        },
        docs[256]
    );
    LUASF_STUB_DOC(docs[257]);
    LUASF_STUB_FUNCTION("sf.Keyboard", "setVirtualKeyboardVisible", "fun(visible: boolean)");
    lua_glue::BindCallable(sf_Keyboard, "setVirtualKeyboardVisible",
        [](bool visible) {
            sf::Keyboard::setVirtualKeyboardVisible(visible);
        },
        docs[257]
    );
}
