#include "Window/bind_Keyboard.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Keyboard(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    sol::table sf_Keyboard = sf["Keyboard"].get_or_create<sol::table>();
    LUASF_STUB_DOC("\\brief Key codes\n\nThe enumerators refer to the \"localized\" key; i.e. depending\non the layout set by the operating system, a key can be mapped\nto `Y` or `Z`.");
    LUASF_STUB_CLASS("sf.Keyboard.Key");
    LUASF_STUB_DOC("Unhandled key");
    LUASF_STUB_FIELD("Unknown", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The A key");
    LUASF_STUB_FIELD("A", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The B key");
    LUASF_STUB_FIELD("B", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The C key");
    LUASF_STUB_FIELD("C", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The D key");
    LUASF_STUB_FIELD("D", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The E key");
    LUASF_STUB_FIELD("E", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F key");
    LUASF_STUB_FIELD("F", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The G key");
    LUASF_STUB_FIELD("G", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The H key");
    LUASF_STUB_FIELD("H", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The I key");
    LUASF_STUB_FIELD("I", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The J key");
    LUASF_STUB_FIELD("J", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The K key");
    LUASF_STUB_FIELD("K", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The L key");
    LUASF_STUB_FIELD("L", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The M key");
    LUASF_STUB_FIELD("M", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The N key");
    LUASF_STUB_FIELD("N", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The O key");
    LUASF_STUB_FIELD("O", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The P key");
    LUASF_STUB_FIELD("P", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Q key");
    LUASF_STUB_FIELD("Q", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The R key");
    LUASF_STUB_FIELD("R", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The S key");
    LUASF_STUB_FIELD("S", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The T key");
    LUASF_STUB_FIELD("T", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The U key");
    LUASF_STUB_FIELD("U", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The V key");
    LUASF_STUB_FIELD("V", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The W key");
    LUASF_STUB_FIELD("W", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The X key");
    LUASF_STUB_FIELD("X", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Y key");
    LUASF_STUB_FIELD("Y", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Z key");
    LUASF_STUB_FIELD("Z", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 0 key");
    LUASF_STUB_FIELD("Num0", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 1 key");
    LUASF_STUB_FIELD("Num1", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 2 key");
    LUASF_STUB_FIELD("Num2", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 3 key");
    LUASF_STUB_FIELD("Num3", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 4 key");
    LUASF_STUB_FIELD("Num4", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 5 key");
    LUASF_STUB_FIELD("Num5", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 6 key");
    LUASF_STUB_FIELD("Num6", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 7 key");
    LUASF_STUB_FIELD("Num7", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 8 key");
    LUASF_STUB_FIELD("Num8", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The 9 key");
    LUASF_STUB_FIELD("Num9", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Escape key");
    LUASF_STUB_FIELD("Escape", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The left Control key");
    LUASF_STUB_FIELD("LControl", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The left Shift key");
    LUASF_STUB_FIELD("LShift", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The left Alt key");
    LUASF_STUB_FIELD("LAlt", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The left OS specific key: window (Windows and Linux), apple (macOS), ...");
    LUASF_STUB_FIELD("LSystem", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The right Control key");
    LUASF_STUB_FIELD("RControl", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The right Shift key");
    LUASF_STUB_FIELD("RShift", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The right Alt key");
    LUASF_STUB_FIELD("RAlt", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The right OS specific key: window (Windows and Linux), apple (macOS), ...");
    LUASF_STUB_FIELD("RSystem", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Menu key");
    LUASF_STUB_FIELD("Menu", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The [ key");
    LUASF_STUB_FIELD("LBracket", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The ] key");
    LUASF_STUB_FIELD("RBracket", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The ; key");
    LUASF_STUB_FIELD("Semicolon", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The , key");
    LUASF_STUB_FIELD("Comma", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The . key");
    LUASF_STUB_FIELD("Period", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The ' key");
    LUASF_STUB_FIELD("Apostrophe", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The / key");
    LUASF_STUB_FIELD("Slash", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The \\ key");
    LUASF_STUB_FIELD("Backslash", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The ` key");
    LUASF_STUB_FIELD("Grave", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The = key");
    LUASF_STUB_FIELD("Equal", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The - key (hyphen)");
    LUASF_STUB_FIELD("Hyphen", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Space key");
    LUASF_STUB_FIELD("Space", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Enter/Return keys");
    LUASF_STUB_FIELD("Enter", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Backspace key");
    LUASF_STUB_FIELD("Backspace", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Tabulation key");
    LUASF_STUB_FIELD("Tab", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Page up key");
    LUASF_STUB_FIELD("PageUp", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Page down key");
    LUASF_STUB_FIELD("PageDown", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The End key");
    LUASF_STUB_FIELD("End", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Home key");
    LUASF_STUB_FIELD("Home", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Insert key");
    LUASF_STUB_FIELD("Insert", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Delete key");
    LUASF_STUB_FIELD("Delete", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The + key");
    LUASF_STUB_FIELD("Add", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The - key (minus, usually from numpad)");
    LUASF_STUB_FIELD("Subtract", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The * key");
    LUASF_STUB_FIELD("Multiply", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The / key");
    LUASF_STUB_FIELD("Divide", "sf.Keyboard.Key");
    LUASF_STUB_DOC("Left arrow");
    LUASF_STUB_FIELD("Left", "sf.Keyboard.Key");
    LUASF_STUB_DOC("Right arrow");
    LUASF_STUB_FIELD("Right", "sf.Keyboard.Key");
    LUASF_STUB_DOC("Up arrow");
    LUASF_STUB_FIELD("Up", "sf.Keyboard.Key");
    LUASF_STUB_DOC("Down arrow");
    LUASF_STUB_FIELD("Down", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 0 key");
    LUASF_STUB_FIELD("Numpad0", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 1 key");
    LUASF_STUB_FIELD("Numpad1", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 2 key");
    LUASF_STUB_FIELD("Numpad2", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 3 key");
    LUASF_STUB_FIELD("Numpad3", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 4 key");
    LUASF_STUB_FIELD("Numpad4", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 5 key");
    LUASF_STUB_FIELD("Numpad5", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 6 key");
    LUASF_STUB_FIELD("Numpad6", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 7 key");
    LUASF_STUB_FIELD("Numpad7", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 8 key");
    LUASF_STUB_FIELD("Numpad8", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The numpad 9 key");
    LUASF_STUB_FIELD("Numpad9", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F1 key");
    LUASF_STUB_FIELD("F1", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F2 key");
    LUASF_STUB_FIELD("F2", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F3 key");
    LUASF_STUB_FIELD("F3", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F4 key");
    LUASF_STUB_FIELD("F4", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F5 key");
    LUASF_STUB_FIELD("F5", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F6 key");
    LUASF_STUB_FIELD("F6", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F7 key");
    LUASF_STUB_FIELD("F7", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F8 key");
    LUASF_STUB_FIELD("F8", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F9 key");
    LUASF_STUB_FIELD("F9", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F10 key");
    LUASF_STUB_FIELD("F10", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F11 key");
    LUASF_STUB_FIELD("F11", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F12 key");
    LUASF_STUB_FIELD("F12", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F13 key");
    LUASF_STUB_FIELD("F13", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F14 key");
    LUASF_STUB_FIELD("F14", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The F15 key");
    LUASF_STUB_FIELD("F15", "sf.Keyboard.Key");
    LUASF_STUB_DOC("The Pause key");
    LUASF_STUB_FIELD("Pause", "sf.Keyboard.Key");
    sf_Keyboard.new_enum("Key",
        "Unknown", sf::Keyboard::Key::Unknown,
        "A", sf::Keyboard::Key::A,
        "B", sf::Keyboard::Key::B,
        "C", sf::Keyboard::Key::C,
        "D", sf::Keyboard::Key::D,
        "E", sf::Keyboard::Key::E,
        "F", sf::Keyboard::Key::F,
        "G", sf::Keyboard::Key::G,
        "H", sf::Keyboard::Key::H,
        "I", sf::Keyboard::Key::I,
        "J", sf::Keyboard::Key::J,
        "K", sf::Keyboard::Key::K,
        "L", sf::Keyboard::Key::L,
        "M", sf::Keyboard::Key::M,
        "N", sf::Keyboard::Key::N,
        "O", sf::Keyboard::Key::O,
        "P", sf::Keyboard::Key::P,
        "Q", sf::Keyboard::Key::Q,
        "R", sf::Keyboard::Key::R,
        "S", sf::Keyboard::Key::S,
        "T", sf::Keyboard::Key::T,
        "U", sf::Keyboard::Key::U,
        "V", sf::Keyboard::Key::V,
        "W", sf::Keyboard::Key::W,
        "X", sf::Keyboard::Key::X,
        "Y", sf::Keyboard::Key::Y,
        "Z", sf::Keyboard::Key::Z,
        "Num0", sf::Keyboard::Key::Num0,
        "Num1", sf::Keyboard::Key::Num1,
        "Num2", sf::Keyboard::Key::Num2,
        "Num3", sf::Keyboard::Key::Num3,
        "Num4", sf::Keyboard::Key::Num4,
        "Num5", sf::Keyboard::Key::Num5,
        "Num6", sf::Keyboard::Key::Num6,
        "Num7", sf::Keyboard::Key::Num7,
        "Num8", sf::Keyboard::Key::Num8,
        "Num9", sf::Keyboard::Key::Num9,
        "Escape", sf::Keyboard::Key::Escape,
        "LControl", sf::Keyboard::Key::LControl,
        "LShift", sf::Keyboard::Key::LShift,
        "LAlt", sf::Keyboard::Key::LAlt,
        "LSystem", sf::Keyboard::Key::LSystem,
        "RControl", sf::Keyboard::Key::RControl,
        "RShift", sf::Keyboard::Key::RShift,
        "RAlt", sf::Keyboard::Key::RAlt,
        "RSystem", sf::Keyboard::Key::RSystem,
        "Menu", sf::Keyboard::Key::Menu,
        "LBracket", sf::Keyboard::Key::LBracket,
        "RBracket", sf::Keyboard::Key::RBracket,
        "Semicolon", sf::Keyboard::Key::Semicolon,
        "Comma", sf::Keyboard::Key::Comma,
        "Period", sf::Keyboard::Key::Period,
        "Apostrophe", sf::Keyboard::Key::Apostrophe,
        "Slash", sf::Keyboard::Key::Slash,
        "Backslash", sf::Keyboard::Key::Backslash,
        "Grave", sf::Keyboard::Key::Grave,
        "Equal", sf::Keyboard::Key::Equal,
        "Hyphen", sf::Keyboard::Key::Hyphen,
        "Space", sf::Keyboard::Key::Space,
        "Enter", sf::Keyboard::Key::Enter,
        "Backspace", sf::Keyboard::Key::Backspace,
        "Tab", sf::Keyboard::Key::Tab,
        "PageUp", sf::Keyboard::Key::PageUp,
        "PageDown", sf::Keyboard::Key::PageDown,
        "End", sf::Keyboard::Key::End,
        "Home", sf::Keyboard::Key::Home,
        "Insert", sf::Keyboard::Key::Insert,
        "Delete", sf::Keyboard::Key::Delete,
        "Add", sf::Keyboard::Key::Add,
        "Subtract", sf::Keyboard::Key::Subtract,
        "Multiply", sf::Keyboard::Key::Multiply,
        "Divide", sf::Keyboard::Key::Divide,
        "Left", sf::Keyboard::Key::Left,
        "Right", sf::Keyboard::Key::Right,
        "Up", sf::Keyboard::Key::Up,
        "Down", sf::Keyboard::Key::Down,
        "Numpad0", sf::Keyboard::Key::Numpad0,
        "Numpad1", sf::Keyboard::Key::Numpad1,
        "Numpad2", sf::Keyboard::Key::Numpad2,
        "Numpad3", sf::Keyboard::Key::Numpad3,
        "Numpad4", sf::Keyboard::Key::Numpad4,
        "Numpad5", sf::Keyboard::Key::Numpad5,
        "Numpad6", sf::Keyboard::Key::Numpad6,
        "Numpad7", sf::Keyboard::Key::Numpad7,
        "Numpad8", sf::Keyboard::Key::Numpad8,
        "Numpad9", sf::Keyboard::Key::Numpad9,
        "F1", sf::Keyboard::Key::F1,
        "F2", sf::Keyboard::Key::F2,
        "F3", sf::Keyboard::Key::F3,
        "F4", sf::Keyboard::Key::F4,
        "F5", sf::Keyboard::Key::F5,
        "F6", sf::Keyboard::Key::F6,
        "F7", sf::Keyboard::Key::F7,
        "F8", sf::Keyboard::Key::F8,
        "F9", sf::Keyboard::Key::F9,
        "F10", sf::Keyboard::Key::F10,
        "F11", sf::Keyboard::Key::F11,
        "F12", sf::Keyboard::Key::F12,
        "F13", sf::Keyboard::Key::F13,
        "F14", sf::Keyboard::Key::F14,
        "F15", sf::Keyboard::Key::F15,
        "Pause", sf::Keyboard::Key::Pause
    );
    LUASF_STUB_DOC("\\brief The total number of keyboard keys, ignoring `Key::Unknown`");
    LUASF_STUB_VALUE("sf.Keyboard", "KeyCount", "integer");
    sf_Keyboard["KeyCount"] = sf::Keyboard::KeyCount;
    LUASF_STUB_DOC("\\brief Scancodes\n\nThe enumerators are bound to a physical key and do not depend on\nthe keyboard layout used by the operating system. Usually, the AT-101\nkeyboard can be used as reference for the physical position of the keys.");
    LUASF_STUB_CLASS("sf.Keyboard.Scan");
    LUASF_STUB_DOC("Represents any scancode not present in this enum");
    LUASF_STUB_FIELD("Unknown", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard a and A key");
    LUASF_STUB_FIELD("A", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard b and B key");
    LUASF_STUB_FIELD("B", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard c and C key");
    LUASF_STUB_FIELD("C", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard d and D key");
    LUASF_STUB_FIELD("D", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard e and E key");
    LUASF_STUB_FIELD("E", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard f and F key");
    LUASF_STUB_FIELD("F", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard g and G key");
    LUASF_STUB_FIELD("G", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard h and H key");
    LUASF_STUB_FIELD("H", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard i and I key");
    LUASF_STUB_FIELD("I", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard j and J key");
    LUASF_STUB_FIELD("J", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard k and K key");
    LUASF_STUB_FIELD("K", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard l and L key");
    LUASF_STUB_FIELD("L", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard m and M key");
    LUASF_STUB_FIELD("M", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard n and N key");
    LUASF_STUB_FIELD("N", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard o and O key");
    LUASF_STUB_FIELD("O", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard p and P key");
    LUASF_STUB_FIELD("P", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard q and Q key");
    LUASF_STUB_FIELD("Q", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard r and R key");
    LUASF_STUB_FIELD("R", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard s and S key");
    LUASF_STUB_FIELD("S", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard t and T key");
    LUASF_STUB_FIELD("T", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard u and U key");
    LUASF_STUB_FIELD("U", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard v and V key");
    LUASF_STUB_FIELD("V", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard w and W key");
    LUASF_STUB_FIELD("W", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard x and X key");
    LUASF_STUB_FIELD("X", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard y and Y key");
    LUASF_STUB_FIELD("Y", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard z and Z key");
    LUASF_STUB_FIELD("Z", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 1 and ! key");
    LUASF_STUB_FIELD("Num1", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 2 and @ key");
    LUASF_STUB_FIELD("Num2", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 3 and # key");
    LUASF_STUB_FIELD("Num3", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 4 and $ key");
    LUASF_STUB_FIELD("Num4", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 5 and % key");
    LUASF_STUB_FIELD("Num5", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 6 and ^ key");
    LUASF_STUB_FIELD("Num6", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 7 and & key");
    LUASF_STUB_FIELD("Num7", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 8 and * key");
    LUASF_STUB_FIELD("Num8", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 9 and ) key");
    LUASF_STUB_FIELD("Num9", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard 0 and ) key");
    LUASF_STUB_FIELD("Num0", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Enter/Return key");
    LUASF_STUB_FIELD("Enter", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Escape key");
    LUASF_STUB_FIELD("Escape", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Backspace key");
    LUASF_STUB_FIELD("Backspace", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Tab key");
    LUASF_STUB_FIELD("Tab", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Space key");
    LUASF_STUB_FIELD("Space", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard - and _ key");
    LUASF_STUB_FIELD("Hyphen", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard = and +");
    LUASF_STUB_FIELD("Equal", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard [ and { key");
    LUASF_STUB_FIELD("LBracket", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard ] and } key");
    LUASF_STUB_FIELD("RBracket", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard \\ and | key OR various keys for Non-US keyboards");
    LUASF_STUB_FIELD("Backslash", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard ; and : key");
    LUASF_STUB_FIELD("Semicolon", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard ' and \" key");
    LUASF_STUB_FIELD("Apostrophe", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard ` and ~ key");
    LUASF_STUB_FIELD("Grave", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard , and < key");
    LUASF_STUB_FIELD("Comma", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard . and > key");
    LUASF_STUB_FIELD("Period", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard / and ? key");
    LUASF_STUB_FIELD("Slash", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F1 key");
    LUASF_STUB_FIELD("F1", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F2 key");
    LUASF_STUB_FIELD("F2", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F3 key");
    LUASF_STUB_FIELD("F3", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F4 key");
    LUASF_STUB_FIELD("F4", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F5 key");
    LUASF_STUB_FIELD("F5", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F6 key");
    LUASF_STUB_FIELD("F6", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F7 key");
    LUASF_STUB_FIELD("F7", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F8 key");
    LUASF_STUB_FIELD("F8", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F9 key");
    LUASF_STUB_FIELD("F9", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F10 key");
    LUASF_STUB_FIELD("F10", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F11 key");
    LUASF_STUB_FIELD("F11", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F12 key");
    LUASF_STUB_FIELD("F12", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F13 key");
    LUASF_STUB_FIELD("F13", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F14 key");
    LUASF_STUB_FIELD("F14", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F15 key");
    LUASF_STUB_FIELD("F15", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F16 key");
    LUASF_STUB_FIELD("F16", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F17 key");
    LUASF_STUB_FIELD("F17", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F18 key");
    LUASF_STUB_FIELD("F18", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F19 key");
    LUASF_STUB_FIELD("F19", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F20 key");
    LUASF_STUB_FIELD("F20", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F21 key");
    LUASF_STUB_FIELD("F21", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F22 key");
    LUASF_STUB_FIELD("F22", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F23 key");
    LUASF_STUB_FIELD("F23", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard F24 key");
    LUASF_STUB_FIELD("F24", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Caps %Lock key");
    LUASF_STUB_FIELD("CapsLock", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Print Screen key");
    LUASF_STUB_FIELD("PrintScreen", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Scroll %Lock key");
    LUASF_STUB_FIELD("ScrollLock", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Pause key");
    LUASF_STUB_FIELD("Pause", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Insert key");
    LUASF_STUB_FIELD("Insert", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Home key");
    LUASF_STUB_FIELD("Home", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Page Up key");
    LUASF_STUB_FIELD("PageUp", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Delete Forward key");
    LUASF_STUB_FIELD("Delete", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard End key");
    LUASF_STUB_FIELD("End", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Page Down key");
    LUASF_STUB_FIELD("PageDown", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Right Arrow key");
    LUASF_STUB_FIELD("Right", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Left Arrow key");
    LUASF_STUB_FIELD("Left", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Down Arrow key");
    LUASF_STUB_FIELD("Down", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Up Arrow key");
    LUASF_STUB_FIELD("Up", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad Num %Lock and Clear key");
    LUASF_STUB_FIELD("NumLock", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad / key");
    LUASF_STUB_FIELD("NumpadDivide", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad * key");
    LUASF_STUB_FIELD("NumpadMultiply", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad - key");
    LUASF_STUB_FIELD("NumpadMinus", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad + key");
    LUASF_STUB_FIELD("NumpadPlus", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("keypad = key");
    LUASF_STUB_FIELD("NumpadEqual", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad Enter/Return key");
    LUASF_STUB_FIELD("NumpadEnter", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad . and Delete key");
    LUASF_STUB_FIELD("NumpadDecimal", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 1 and End key");
    LUASF_STUB_FIELD("Numpad1", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 2 and Down Arrow key");
    LUASF_STUB_FIELD("Numpad2", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 3 and Page Down key");
    LUASF_STUB_FIELD("Numpad3", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 4 and Left Arrow key");
    LUASF_STUB_FIELD("Numpad4", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 5 key");
    LUASF_STUB_FIELD("Numpad5", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 6 and Right Arrow key");
    LUASF_STUB_FIELD("Numpad6", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 7 and Home key");
    LUASF_STUB_FIELD("Numpad7", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 8 and Up Arrow key");
    LUASF_STUB_FIELD("Numpad8", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 9 and Page Up key");
    LUASF_STUB_FIELD("Numpad9", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keypad 0 and Insert key");
    LUASF_STUB_FIELD("Numpad0", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Non-US \\ and | key");
    LUASF_STUB_FIELD("NonUsBackslash", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Application key");
    LUASF_STUB_FIELD("Application", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Execute key");
    LUASF_STUB_FIELD("Execute", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Mode Change key");
    LUASF_STUB_FIELD("ModeChange", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Help key");
    LUASF_STUB_FIELD("Help", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Menu key");
    LUASF_STUB_FIELD("Menu", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Select key");
    LUASF_STUB_FIELD("Select", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Redo key");
    LUASF_STUB_FIELD("Redo", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Undo key");
    LUASF_STUB_FIELD("Undo", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Cut key");
    LUASF_STUB_FIELD("Cut", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Copy key");
    LUASF_STUB_FIELD("Copy", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Paste key");
    LUASF_STUB_FIELD("Paste", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Volume Mute key");
    LUASF_STUB_FIELD("VolumeMute", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Volume Up key");
    LUASF_STUB_FIELD("VolumeUp", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Volume Down key");
    LUASF_STUB_FIELD("VolumeDown", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Media Play Pause key");
    LUASF_STUB_FIELD("MediaPlayPause", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Media Stop key");
    LUASF_STUB_FIELD("MediaStop", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Media Next Track key");
    LUASF_STUB_FIELD("MediaNextTrack", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Media Previous Track key");
    LUASF_STUB_FIELD("MediaPreviousTrack", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Left Control key");
    LUASF_STUB_FIELD("LControl", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Left Shift key");
    LUASF_STUB_FIELD("LShift", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Left Alt key");
    LUASF_STUB_FIELD("LAlt", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Left System key");
    LUASF_STUB_FIELD("LSystem", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Right Control key");
    LUASF_STUB_FIELD("RControl", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Right Shift key");
    LUASF_STUB_FIELD("RShift", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Right Alt key");
    LUASF_STUB_FIELD("RAlt", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Right System key");
    LUASF_STUB_FIELD("RSystem", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Back key");
    LUASF_STUB_FIELD("Back", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Forward key");
    LUASF_STUB_FIELD("Forward", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Refresh key");
    LUASF_STUB_FIELD("Refresh", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Stop key");
    LUASF_STUB_FIELD("Stop", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Search key");
    LUASF_STUB_FIELD("Search", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Favorites key");
    LUASF_STUB_FIELD("Favorites", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Home Page key");
    LUASF_STUB_FIELD("HomePage", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Launch Application 1 key");
    LUASF_STUB_FIELD("LaunchApplication1", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Launch Application 2 key");
    LUASF_STUB_FIELD("LaunchApplication2", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Launch Mail key");
    LUASF_STUB_FIELD("LaunchMail", "sf.Keyboard.Scan");
    LUASF_STUB_DOC("Keyboard Launch Media Select key");
    LUASF_STUB_FIELD("LaunchMediaSelect", "sf.Keyboard.Scan");
    sf_Keyboard.new_enum("Scan",
        "Unknown", sf::Keyboard::Scan::Unknown,
        "A", sf::Keyboard::Scan::A,
        "B", sf::Keyboard::Scan::B,
        "C", sf::Keyboard::Scan::C,
        "D", sf::Keyboard::Scan::D,
        "E", sf::Keyboard::Scan::E,
        "F", sf::Keyboard::Scan::F,
        "G", sf::Keyboard::Scan::G,
        "H", sf::Keyboard::Scan::H,
        "I", sf::Keyboard::Scan::I,
        "J", sf::Keyboard::Scan::J,
        "K", sf::Keyboard::Scan::K,
        "L", sf::Keyboard::Scan::L,
        "M", sf::Keyboard::Scan::M,
        "N", sf::Keyboard::Scan::N,
        "O", sf::Keyboard::Scan::O,
        "P", sf::Keyboard::Scan::P,
        "Q", sf::Keyboard::Scan::Q,
        "R", sf::Keyboard::Scan::R,
        "S", sf::Keyboard::Scan::S,
        "T", sf::Keyboard::Scan::T,
        "U", sf::Keyboard::Scan::U,
        "V", sf::Keyboard::Scan::V,
        "W", sf::Keyboard::Scan::W,
        "X", sf::Keyboard::Scan::X,
        "Y", sf::Keyboard::Scan::Y,
        "Z", sf::Keyboard::Scan::Z,
        "Num1", sf::Keyboard::Scan::Num1,
        "Num2", sf::Keyboard::Scan::Num2,
        "Num3", sf::Keyboard::Scan::Num3,
        "Num4", sf::Keyboard::Scan::Num4,
        "Num5", sf::Keyboard::Scan::Num5,
        "Num6", sf::Keyboard::Scan::Num6,
        "Num7", sf::Keyboard::Scan::Num7,
        "Num8", sf::Keyboard::Scan::Num8,
        "Num9", sf::Keyboard::Scan::Num9,
        "Num0", sf::Keyboard::Scan::Num0,
        "Enter", sf::Keyboard::Scan::Enter,
        "Escape", sf::Keyboard::Scan::Escape,
        "Backspace", sf::Keyboard::Scan::Backspace,
        "Tab", sf::Keyboard::Scan::Tab,
        "Space", sf::Keyboard::Scan::Space,
        "Hyphen", sf::Keyboard::Scan::Hyphen,
        "Equal", sf::Keyboard::Scan::Equal,
        "LBracket", sf::Keyboard::Scan::LBracket,
        "RBracket", sf::Keyboard::Scan::RBracket,
        "Backslash", sf::Keyboard::Scan::Backslash,
        "Semicolon", sf::Keyboard::Scan::Semicolon,
        "Apostrophe", sf::Keyboard::Scan::Apostrophe,
        "Grave", sf::Keyboard::Scan::Grave,
        "Comma", sf::Keyboard::Scan::Comma,
        "Period", sf::Keyboard::Scan::Period,
        "Slash", sf::Keyboard::Scan::Slash,
        "F1", sf::Keyboard::Scan::F1,
        "F2", sf::Keyboard::Scan::F2,
        "F3", sf::Keyboard::Scan::F3,
        "F4", sf::Keyboard::Scan::F4,
        "F5", sf::Keyboard::Scan::F5,
        "F6", sf::Keyboard::Scan::F6,
        "F7", sf::Keyboard::Scan::F7,
        "F8", sf::Keyboard::Scan::F8,
        "F9", sf::Keyboard::Scan::F9,
        "F10", sf::Keyboard::Scan::F10,
        "F11", sf::Keyboard::Scan::F11,
        "F12", sf::Keyboard::Scan::F12,
        "F13", sf::Keyboard::Scan::F13,
        "F14", sf::Keyboard::Scan::F14,
        "F15", sf::Keyboard::Scan::F15,
        "F16", sf::Keyboard::Scan::F16,
        "F17", sf::Keyboard::Scan::F17,
        "F18", sf::Keyboard::Scan::F18,
        "F19", sf::Keyboard::Scan::F19,
        "F20", sf::Keyboard::Scan::F20,
        "F21", sf::Keyboard::Scan::F21,
        "F22", sf::Keyboard::Scan::F22,
        "F23", sf::Keyboard::Scan::F23,
        "F24", sf::Keyboard::Scan::F24,
        "CapsLock", sf::Keyboard::Scan::CapsLock,
        "PrintScreen", sf::Keyboard::Scan::PrintScreen,
        "ScrollLock", sf::Keyboard::Scan::ScrollLock,
        "Pause", sf::Keyboard::Scan::Pause,
        "Insert", sf::Keyboard::Scan::Insert,
        "Home", sf::Keyboard::Scan::Home,
        "PageUp", sf::Keyboard::Scan::PageUp,
        "Delete", sf::Keyboard::Scan::Delete,
        "End", sf::Keyboard::Scan::End,
        "PageDown", sf::Keyboard::Scan::PageDown,
        "Right", sf::Keyboard::Scan::Right,
        "Left", sf::Keyboard::Scan::Left,
        "Down", sf::Keyboard::Scan::Down,
        "Up", sf::Keyboard::Scan::Up,
        "NumLock", sf::Keyboard::Scan::NumLock,
        "NumpadDivide", sf::Keyboard::Scan::NumpadDivide,
        "NumpadMultiply", sf::Keyboard::Scan::NumpadMultiply,
        "NumpadMinus", sf::Keyboard::Scan::NumpadMinus,
        "NumpadPlus", sf::Keyboard::Scan::NumpadPlus,
        "NumpadEqual", sf::Keyboard::Scan::NumpadEqual,
        "NumpadEnter", sf::Keyboard::Scan::NumpadEnter,
        "NumpadDecimal", sf::Keyboard::Scan::NumpadDecimal,
        "Numpad1", sf::Keyboard::Scan::Numpad1,
        "Numpad2", sf::Keyboard::Scan::Numpad2,
        "Numpad3", sf::Keyboard::Scan::Numpad3,
        "Numpad4", sf::Keyboard::Scan::Numpad4,
        "Numpad5", sf::Keyboard::Scan::Numpad5,
        "Numpad6", sf::Keyboard::Scan::Numpad6,
        "Numpad7", sf::Keyboard::Scan::Numpad7,
        "Numpad8", sf::Keyboard::Scan::Numpad8,
        "Numpad9", sf::Keyboard::Scan::Numpad9,
        "Numpad0", sf::Keyboard::Scan::Numpad0,
        "NonUsBackslash", sf::Keyboard::Scan::NonUsBackslash,
        "Application", sf::Keyboard::Scan::Application,
        "Execute", sf::Keyboard::Scan::Execute,
        "ModeChange", sf::Keyboard::Scan::ModeChange,
        "Help", sf::Keyboard::Scan::Help,
        "Menu", sf::Keyboard::Scan::Menu,
        "Select", sf::Keyboard::Scan::Select,
        "Redo", sf::Keyboard::Scan::Redo,
        "Undo", sf::Keyboard::Scan::Undo,
        "Cut", sf::Keyboard::Scan::Cut,
        "Copy", sf::Keyboard::Scan::Copy,
        "Paste", sf::Keyboard::Scan::Paste,
        "VolumeMute", sf::Keyboard::Scan::VolumeMute,
        "VolumeUp", sf::Keyboard::Scan::VolumeUp,
        "VolumeDown", sf::Keyboard::Scan::VolumeDown,
        "MediaPlayPause", sf::Keyboard::Scan::MediaPlayPause,
        "MediaStop", sf::Keyboard::Scan::MediaStop,
        "MediaNextTrack", sf::Keyboard::Scan::MediaNextTrack,
        "MediaPreviousTrack", sf::Keyboard::Scan::MediaPreviousTrack,
        "LControl", sf::Keyboard::Scan::LControl,
        "LShift", sf::Keyboard::Scan::LShift,
        "LAlt", sf::Keyboard::Scan::LAlt,
        "LSystem", sf::Keyboard::Scan::LSystem,
        "RControl", sf::Keyboard::Scan::RControl,
        "RShift", sf::Keyboard::Scan::RShift,
        "RAlt", sf::Keyboard::Scan::RAlt,
        "RSystem", sf::Keyboard::Scan::RSystem,
        "Back", sf::Keyboard::Scan::Back,
        "Forward", sf::Keyboard::Scan::Forward,
        "Refresh", sf::Keyboard::Scan::Refresh,
        "Stop", sf::Keyboard::Scan::Stop,
        "Search", sf::Keyboard::Scan::Search,
        "Favorites", sf::Keyboard::Scan::Favorites,
        "HomePage", sf::Keyboard::Scan::HomePage,
        "LaunchApplication1", sf::Keyboard::Scan::LaunchApplication1,
        "LaunchApplication2", sf::Keyboard::Scan::LaunchApplication2,
        "LaunchMail", sf::Keyboard::Scan::LaunchMail,
        "LaunchMediaSelect", sf::Keyboard::Scan::LaunchMediaSelect
    );
    LUASF_STUB_ALIAS("sf.Keyboard.Scancode", "sf.Keyboard.Scan");
    {
        const sol::object aliasValue = sf_Keyboard.raw_get<sol::object>("Scancode");
        const sol::object aliasTarget = sf_Keyboard.raw_get<sol::object>("Scan");
        if ((!aliasValue.valid() || aliasValue.get_type() == sol::type::lua_nil) &&
            aliasTarget.valid() && aliasTarget.get_type() != sol::type::lua_nil)
            sf_Keyboard.raw_set("Scancode", aliasTarget);
    }
    LUASF_STUB_DOC("\\brief The total number of scancodes, ignoring `Scan::Unknown`");
    LUASF_STUB_VALUE("sf.Keyboard", "ScancodeCount", "integer");
    sf_Keyboard["ScancodeCount"] = sf::Keyboard::ScancodeCount;
    LUASF_STUB_DOC("\\brief Check if a key is pressed\n\n\\warning On macOS you're required to grant input monitoring access for\nyour application in order for `isKeyPressed` to work.\n\n\\param key Key to check\n\n\\return `true` if the key is pressed, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Keyboard", "isKeyPressed", "fun(key: sf.Keyboard.Key): boolean");
    LUASF_STUB_OVERLOAD("sf.Keyboard", "isKeyPressed", "fun(code: sf.Keyboard.Scan): boolean");
    sf_Keyboard.set_function("isKeyPressed",
        sol::overload(
            [](sf::Keyboard::Key key) -> bool {
                return sf::Keyboard::isKeyPressed(key);
            },
            [](sf::Keyboard::Scancode code) -> bool {
                return sf::Keyboard::isKeyPressed(code);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Localize a physical key to a logical one\n\n\\param code Scancode to localize\n\n\\return The key corresponding to the scancode under the current\nkeyboard layout used by the operating system, or\n`sf::Keyboard::Key::Unknown` when the scancode cannot be mapped\nto a Key.\n\n\\see `delocalize`");
    LUASF_STUB_FUNCTION("sf.Keyboard", "localize", "fun(code: sf.Keyboard.Scan): sf.Keyboard.Key");
    sf_Keyboard.set_function("localize",
        [](sf::Keyboard::Scancode code) -> sf::Keyboard::Key {
            return sf::Keyboard::localize(code);
        }
    );
    LUASF_STUB_DOC("\\brief Identify the physical key corresponding to a logical one\n\n\\param key Key to \"delocalize\"\n\n\\return The scancode corresponding to the key under the current\nkeyboard layout used by the operating system, or\n`sf::Keyboard::Scan::Unknown` when the key cannot be mapped\nto a `sf::Keyboard::Scancode`.\n\n\\see `localize`");
    LUASF_STUB_FUNCTION("sf.Keyboard", "delocalize", "fun(key: sf.Keyboard.Key): sf.Keyboard.Scancode");
    sf_Keyboard.set_function("delocalize",
        [](sf::Keyboard::Key key) -> sf::Keyboard::Scancode {
            return sf::Keyboard::delocalize(key);
        }
    );
    LUASF_STUB_DOC("\\brief Provide a string representation for a given scancode\n\nThe returned string is a short, non-technical description of\nthe key represented with the given scancode. Most effectively\nused in user interfaces, as the description for the key takes\nthe users keyboard layout into consideration.\n\n\\warning The result is OS-dependent: for example, `sf::Keyboard::Scan::LSystem`\nis \"Left Meta\" on Linux, \"Left Windows\" on Windows and\n\"Left Command\" on macOS.\n\nThe current keyboard layout set by the operating system is used to\ninterpret the scancode: for example, `sf::Keyboard::Key::Semicolon` is\nmapped to \";\" for layout and to \"\u00e9\" for others.\n\n\\param code Scancode to check\n\n\\return The localized description of the code");
    LUASF_STUB_FUNCTION("sf.Keyboard", "getDescription", "fun(code: sf.Keyboard.Scan): string");
    sf_Keyboard.set_function("getDescription",
        [](sf::Keyboard::Scancode code) -> std::string {
            return lua_sf::to_utf8_string(sf::Keyboard::getDescription(code));
        }
    );
    LUASF_STUB_DOC("\\brief Show or hide the virtual keyboard\n\n\\warning The virtual keyboard is not supported on all\nsystems. It will typically be implemented on mobile OSes\n(Android, iOS) but not on desktop OSes (Windows, Linux, ...).\n\nIf the virtual keyboard is not available, this function does\nnothing.\n\n\\param visible `true` to show, `false` to hide");
    LUASF_STUB_FUNCTION("sf.Keyboard", "setVirtualKeyboardVisible", "fun(visible: boolean)");
    sf_Keyboard.set_function("setVirtualKeyboardVisible",
        [](bool visible) {
            sf::Keyboard::setVirtualKeyboardVisible(visible);
        }
    );
}
