#include "Window/bind_WindowBase.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_WindowBase(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    auto type_sf__WindowBase = sf.new_usertype<sf::WindowBase>("WindowBase", sol::no_constructor);
    sol::table table_sf__WindowBase = sf["WindowBase"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::WindowBase>(lua);
    LUASF_STUB_DOC("\\brief Window that serves as a base for other windows");
    LUASF_STUB_CLASS("sf.WindowBase");
    LUASF_STUB_DOC("\\brief Construct a new window\n\nThis constructor creates the window with the size and pixel\ndepth defined in `mode`. An optional style can be passed to\ncustomize the look and behavior of the window (borders,\ntitle bar, resizable, closable, ...). An optional state can\nbe provided. If `state` is `State::Fullscreen`, then `mode`\nmust be a valid video mode.\n\n\\param mode  Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title Title of the window\n\\param style %Window style, a bitwise OR combination of `sf::Style` enumerators\n\\param state %Window state");
    LUASF_STUB_FUNCTION("sf.WindowBase", "new", "fun(mode: sf.VideoMode, title: string, style: integer, state: sf.State): sf.WindowBase");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "new", "fun(mode: sf.VideoMode, title: string, style: integer): sf.WindowBase");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "new", "fun(mode: sf.VideoMode, title: string, state: sf.State): sf.WindowBase");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "new", "fun(mode: sf.VideoMode, title: string): sf.WindowBase");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "new", "fun(handle: sf.WindowHandle): sf.WindowBase");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "new", "fun(): sf.WindowBase");
    type_sf__WindowBase.set_function("new", sol::factories(
        [](sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state) {
            return lua_sf::makeLuaSharedObject<sf::WindowBase>(mode, lua_sf::to_sf_string(title), style.value(), state);
        },
        [](sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style) {
            return lua_sf::makeLuaSharedObject<sf::WindowBase>(mode, lua_sf::to_sf_string(title), style.value());
        },
        [](sf::VideoMode mode, std::string title, sf::State state) {
            return lua_sf::makeLuaSharedObject<sf::WindowBase>(mode, lua_sf::to_sf_string(title), state);
        },
        [](sf::VideoMode mode, std::string title) {
            return lua_sf::makeLuaSharedObject<sf::WindowBase>(mode, lua_sf::to_sf_string(title));
        },
        [](const lua_sf::WindowHandle& handle) {
            return lua_sf::makeLuaSharedObject<sf::WindowBase>(handle.getHandle());
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::WindowBase>();
        }
    ));
    LUASF_STUB_DOC("\\brief Create (or recreate) the window\n\nIf the window was already created, it closes it first.\nIf `state` is `State::Fullscreen`, then `mode` must be\na valid video mode.\n\n\\param mode  Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title Title of the window\n\\param style %Window style, a bitwise OR combination of `sf::Style` enumerators\n\\param state %Window state");
    LUASF_STUB_FUNCTION("sf.WindowBase", "create", "fun(self: sf.WindowBase, mode: sf.VideoMode, title: string, style: integer, state: sf.State)");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "create", "fun(self: sf.WindowBase, mode: sf.VideoMode, title: string, style: integer)");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "create", "fun(self: sf.WindowBase, mode: sf.VideoMode, title: string, state: sf.State)");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "create", "fun(self: sf.WindowBase, mode: sf.VideoMode, title: string)");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "create", "fun(self: sf.WindowBase, handle: sf.WindowHandle)");
    type_sf__WindowBase.set_function("create",
        sol::overload(
            [](sf::WindowBase& self, sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state) {
                self.create(mode, lua_sf::to_sf_string(title), style.value(), state);
            },
            [](sf::WindowBase& self, sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style) {
                self.create(mode, lua_sf::to_sf_string(title), style.value());
            },
            [](sf::WindowBase& self, sf::VideoMode mode, std::string title, sf::State state) {
                self.create(mode, lua_sf::to_sf_string(title), state);
            },
            [](sf::WindowBase& self, sf::VideoMode mode, std::string title) {
                self.create(mode, lua_sf::to_sf_string(title));
            },
            [](sf::WindowBase& self, const lua_sf::WindowHandle& handle) {
                self.create(handle.getHandle());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Close the window and destroy all the attached resources\n\nAfter calling this function, the `sf::Window` instance remains\nvalid and you can call `create()` to recreate the window.\nAll other functions such as `pollEvent()` or `display()` will\nstill work (i.e. you don't have to test `isOpen()` every time),\nand will have no effect on closed windows.");
    LUASF_STUB_FUNCTION("sf.WindowBase", "close", "fun(self: sf.WindowBase)");
    type_sf__WindowBase.set_function("close",
        [](sf::WindowBase& self) {
            self.close();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether or not the window is open\n\nThis function returns whether or not the window exists.\nNote that a hidden window (`setVisible(false)`) is open\n(therefore this function would return `true`).\n\n\\return `true` if the window is open, `false` if it has been closed");
    LUASF_STUB_FUNCTION("sf.WindowBase", "isOpen", "fun(self: sf.WindowBase): boolean");
    type_sf__WindowBase.set_function("isOpen",
        [](sf::WindowBase& self) -> bool {
            return self.isOpen();
        }
    );
    LUASF_STUB_DOC("\\brief Pop the next event from the front of the FIFO event queue, if any, and return it\n\nThis function is not blocking: if there's no pending event then\nit will return a `std::nullopt`. Note that more than one event\nmay be present in the event queue, thus you should always call\nthis function in a loop to make sure that you process every\npending event.\n\\code\nwhile (const std::optional event = window.pollEvent())\n{\n// process event...\n}\n\\endcode\n\n\\return The event, otherwise `std::nullopt` if no events are pending\n\n\\see `waitEvent`, `handleEvents`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "pollEvent", "fun(self: sf.WindowBase): sf.Event|nil");
    type_sf__WindowBase.set_function("pollEvent",
        [lua](sf::WindowBase& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.pollEvent());
        }
    );
    LUASF_STUB_DOC("\\brief Wait for an event and return it\n\nThis function is blocking: if there's no pending event then\nit will wait until an event is received or until the provided\ntimeout elapses. Only if an error or a timeout occurs the\nreturned event will be `std::nullopt`.\nThis function is typically used when you have a thread that is\ndedicated to events handling: you want to make this thread sleep\nas long as no new event is received.\n\\code\nwhile (const std::optional event = window.waitEvent())\n{\n// process event...\n}\n\\endcode\n\n\\param timeout Maximum time to wait (`Time::Zero` for infinite)\n\n\\return The event, otherwise `std::nullopt` on timeout or if window was closed\n\n\\see `pollEvent`, `handleEvents`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "waitEvent", "fun(self: sf.WindowBase, timeout: sf.Time): sf.Event|nil");
    LUASF_STUB_OVERLOAD("sf.WindowBase", "waitEvent", "fun(self: sf.WindowBase): sf.Event|nil");
    type_sf__WindowBase.set_function("waitEvent",
        sol::overload(
            [lua](sf::WindowBase& self, sf::Time timeout) -> sol::object {
                return lua_sf::optional_to_object(lua, self.waitEvent(timeout));
            },
            [lua](sf::WindowBase& self) -> sol::object {
                return lua_sf::optional_to_object(lua, self.waitEvent());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the position of the window\n\n\\return Position of the window, in pixels\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "getPosition", "fun(self: sf.WindowBase): sf.Vector2i");
    type_sf__WindowBase.set_function("getPosition",
        [](sf::WindowBase& self) -> sf::Vector2i {
            return self.getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief Change the position of the window on screen\n\nThis function only works for top-level windows\n(i.e. it will be ignored for windows created from\nthe handle of a child window/control).\n\n\\param position New position, in pixels\n\n\\see `getPosition`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setPosition", "fun(self: sf.WindowBase, position: sf.Vector2i)");
    type_sf__WindowBase.set_function("setPosition",
        [](sf::WindowBase& self, sf::Vector2i position) {
            self.setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief Get the size of the rendering region of the window\n\nThe size doesn't include the titlebar and borders\nof the window.\n\n\\return Size in pixels\n\n\\see `setSize`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "getSize", "fun(self: sf.WindowBase): sf.Vector2u");
    type_sf__WindowBase.set_function("getSize",
        [](sf::WindowBase& self) -> sf::Vector2u {
            return self.getSize();
        }
    );
    LUASF_STUB_DOC("\\brief Change the size of the rendering region of the window\n\n\\param size New size, in pixels\n\n\\see `getSize`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setSize", "fun(self: sf.WindowBase, size: sf.Vector2u)");
    type_sf__WindowBase.set_function("setSize",
        [](sf::WindowBase& self, sf::Vector2u size) {
            self.setSize(size);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum window rendering region size\n\nPass `std::nullopt` to unset the minimum size\n\n\\param minimumSize New minimum size, in pixels");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setMinimumSize", "fun(self: sf.WindowBase, minimumSize: sf.Vector2u|nil)");
    type_sf__WindowBase.set_function("setMinimumSize",
        [](sf::WindowBase& self, sol::object minimumSize) {
            auto minimumSize_optional = lua_sf::optional_from_object<sf::Vector2u>(minimumSize);
            self.setMinimumSize(minimumSize_optional);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum window rendering region size\n\nPass `std::nullopt` to unset the maximum size\n\n\\param maximumSize New maximum size, in pixels");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setMaximumSize", "fun(self: sf.WindowBase, maximumSize: sf.Vector2u|nil)");
    type_sf__WindowBase.set_function("setMaximumSize",
        [](sf::WindowBase& self, sol::object maximumSize) {
            auto maximumSize_optional = lua_sf::optional_from_object<sf::Vector2u>(maximumSize);
            self.setMaximumSize(maximumSize_optional);
        }
    );
    LUASF_STUB_DOC("\\brief Change the title of the window\n\n\\param title New title\n\n\\see `setIcon`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setTitle", "fun(self: sf.WindowBase, title: string)");
    type_sf__WindowBase.set_function("setTitle",
        [](sf::WindowBase& self, std::string title) {
            self.setTitle(lua_sf::to_sf_string(title));
        }
    );
    LUASF_STUB_DOC("\\brief Change the window's icon\n\n`pixels` must be an array of `size` pixels\nin 32-bits RGBA format.\n\nThe OS default icon is used by default.\n\n\\param size   Icon's width and height, in pixels\n\\param pixels Pointer to the array of pixels in memory. The\npixels are copied, so you need not keep the\nsource alive after calling this function.\n\n\\see `setTitle`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setIcon", "fun(self: sf.WindowBase, size: sf.Vector2u, pixels: any)");
    type_sf__WindowBase.set_function("setIcon",
        [](sf::WindowBase& self, sf::Vector2u size, sol::object pixels) {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            self.setIcon(size, pixels_buffer.data());
        }
    );
    LUASF_STUB_DOC("\\brief Show or hide the window\n\nThe window is shown by default.\n\n\\param visible `true` to show the window, `false` to hide it");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setVisible", "fun(self: sf.WindowBase, visible: boolean)");
    type_sf__WindowBase.set_function("setVisible",
        [](sf::WindowBase& self, bool visible) {
            self.setVisible(visible);
        }
    );
    LUASF_STUB_DOC("\\brief Show or hide the mouse cursor\n\nThe mouse cursor is visible by default.\n\n\\warning On Windows, this function needs to be called from the\nthread that created the window.\n\n\\param visible `true` to show the mouse cursor, `false` to hide it");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setMouseCursorVisible", "fun(self: sf.WindowBase, visible: boolean)");
    type_sf__WindowBase.set_function("setMouseCursorVisible",
        [](sf::WindowBase& self, bool visible) {
            self.setMouseCursorVisible(visible);
        }
    );
    LUASF_STUB_DOC("\\brief Grab or release the mouse cursor\n\nIf set, grabs the mouse cursor inside this window's client\narea so it may no longer be moved outside its bounds.\nNote that grabbing is only active while the window has\nfocus.\n\n\\param grabbed `true` to enable, `false` to disable");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setMouseCursorGrabbed", "fun(self: sf.WindowBase, grabbed: boolean)");
    type_sf__WindowBase.set_function("setMouseCursorGrabbed",
        [](sf::WindowBase& self, bool grabbed) {
            self.setMouseCursorGrabbed(grabbed);
        }
    );
    LUASF_STUB_DOC("\\brief Set the displayed cursor to a native system cursor\n\nUpon window creation, the arrow cursor is used by default.\n\n\\warning The cursor must not be destroyed while in use by\nthe window.\n\n\\warning Features related to Cursor are not supported on\niOS and Android.\n\n\\param cursor Native system cursor type to display\n\n\\see `sf::Cursor::createFromSystem`, `sf::Cursor::createFromPixels`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setMouseCursor", "fun(self: sf.WindowBase, cursor: sf.Cursor)");
    type_sf__WindowBase.set_function("setMouseCursor",
        [](sf::WindowBase& self, const sf::Cursor& cursor) {
            self.setMouseCursor(cursor);
        }
    );
    LUASF_STUB_DOC("\\brief Enable or disable automatic key-repeat\n\nIf key repeat is enabled, you will receive repeated\nKeyPressed events while keeping a key pressed. If it is disabled,\nyou will only get a single event when the key is pressed.\n\nKey repeat is enabled by default.\n\n\\param enabled `true` to enable, `false` to disable");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setKeyRepeatEnabled", "fun(self: sf.WindowBase, enabled: boolean)");
    type_sf__WindowBase.set_function("setKeyRepeatEnabled",
        [](sf::WindowBase& self, bool enabled) {
            self.setKeyRepeatEnabled(enabled);
        }
    );
    LUASF_STUB_DOC("\\brief Change the joystick threshold\n\nThe joystick threshold is the value below which\nno JoystickMoved event will be generated.\n\nThe threshold value is 0.1 by default.\n\n\\param threshold New threshold, in the range [0, 100]");
    LUASF_STUB_FUNCTION("sf.WindowBase", "setJoystickThreshold", "fun(self: sf.WindowBase, threshold: number)");
    type_sf__WindowBase.set_function("setJoystickThreshold",
        [](sf::WindowBase& self, float threshold) {
            self.setJoystickThreshold(threshold);
        }
    );
    LUASF_STUB_DOC("\\brief Request the current window to be made the active\nforeground window\n\nAt any given time, only one window may have the input focus\nto receive input events such as keystrokes or mouse events.\nIf a window requests focus, it only hints to the operating\nsystem, that it would like to be focused. The operating system\nis free to deny the request.\nThis is not to be confused with `setActive()`.\n\n\\see `hasFocus`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "requestFocus", "fun(self: sf.WindowBase)");
    type_sf__WindowBase.set_function("requestFocus",
        [](sf::WindowBase& self) {
            self.requestFocus();
        }
    );
    LUASF_STUB_DOC("\\brief Check whether the window has the input focus\n\nAt any given time, only one window may have the input focus\nto receive input events such as keystrokes or most mouse\nevents.\n\n\\return `true` if window has focus, `false` otherwise\n\\see `requestFocus`");
    LUASF_STUB_FUNCTION("sf.WindowBase", "hasFocus", "fun(self: sf.WindowBase): boolean");
    type_sf__WindowBase.set_function("hasFocus",
        [](sf::WindowBase& self) -> bool {
            return self.hasFocus();
        }
    );
    LUASF_STUB_DOC("\\brief Get the OS-specific handle of the window\n\nThe type of the returned handle is `sf::WindowHandle`,\nwhich is a type alias to the handle type defined by the OS.\nYou shouldn't need to use this function, unless you have\nvery specific stuff to implement that SFML doesn't support,\nor implement a temporary workaround until a bug is fixed.\n\n\\return System handle of the window");
    LUASF_STUB_FUNCTION("sf.WindowBase", "getNativeHandle", "fun(self: sf.WindowBase): sf.WindowHandle");
    type_sf__WindowBase.set_function("getNativeHandle",
        [](sf::WindowBase& self) -> lua_sf::WindowHandle {
            return lua_sf::WindowHandle::fromNative(self.getNativeHandle());
        }
    );
    // Skipped sf::WindowBase::createVulkanSurface(const VkInstance &, VkSurfaceKHR &, const VkAllocationCallbacks *): unsupported parameter type const VkInstance&.
    // Skipped sf::WindowBase::createVulkanSurface(const VkInstance &, VkSurfaceKHR &, const VkAllocationCallbacks *): unsupported parameter type const VkInstance&.
}
