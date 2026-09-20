#include "Graphics/bind_RenderWindow.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_RenderWindow(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__RenderWindow = sf.new_usertype<sf::RenderWindow>("RenderWindow",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Window, sf::WindowBase, sf::RenderTarget>()
    );
    sol::table table_sf__RenderWindow = sf["RenderWindow"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::RenderWindow>(lua);
    sol::table native_bases_sf__RenderWindow = lua.create_table();
    native_bases_sf__RenderWindow.add(lua["sf"]["Window"].get<sol::table>());
    native_bases_sf__RenderWindow.add(lua["sf"]["RenderTarget"].get<sol::table>());
    table_sf__RenderWindow.raw_set("__nativeBases", native_bases_sf__RenderWindow);
    LUASF_STUB_DOC("\\brief Window that can serve as a target for 2D drawing");
    LUASF_STUB_CLASS("sf.RenderWindow", "sf.Window, sf.WindowBase, sf.RenderTarget");
    LUASF_STUB_DOC("\\brief Construct a new window\n\nThis constructor creates the window with the size and pixel\ndepth defined in `mode`. An optional style can be passed to\ncustomize the look and behavior of the window (borders,\ntitle bar, resizable, closable, ...).\n\nThe last parameter is an optional structure specifying\nadvanced OpenGL context settings such as anti-aliasing,\ndepth-buffer bits, etc. You shouldn't care about these\nparameters for a regular usage of the graphics module.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param style    %Window style, a bitwise OR combination of `sf::Style` enumerators\n\\param state    %Window state\n\\param settings Additional settings for the underlying OpenGL context");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "new", "fun(mode: sf.VideoMode, title: string, style: integer, state: sf.State, settings: sf.ContextSettings): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(mode: sf.VideoMode, title: string, style: integer, state: sf.State): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(mode: sf.VideoMode, title: string, state: sf.State, settings: sf.ContextSettings): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(mode: sf.VideoMode, title: string, style: integer): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(mode: sf.VideoMode, title: string, state: sf.State): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(mode: sf.VideoMode, title: string): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(handle: sf.WindowHandle, settings: sf.ContextSettings): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(handle: sf.WindowHandle): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(): sf.RenderWindow");
    type_sf__RenderWindow.set_function("new", sol::factories(
        [](sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(mode, lua_sf::to_sf_string(title), style.value(), state, settings);
        },
        [](sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(mode, lua_sf::to_sf_string(title), style.value(), state);
        },
        [](sf::VideoMode mode, std::string title, sf::State state, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(mode, lua_sf::to_sf_string(title), state, settings);
        },
        [](sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(mode, lua_sf::to_sf_string(title), style.value());
        },
        [](sf::VideoMode mode, std::string title, sf::State state) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(mode, lua_sf::to_sf_string(title), state);
        },
        [](sf::VideoMode mode, std::string title) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(mode, lua_sf::to_sf_string(title));
        },
        [](const lua_sf::WindowHandle& handle, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(handle.native(), settings);
        },
        [](const lua_sf::WindowHandle& handle) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(handle.native());
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>();
        }
    ));
    LUASF_STUB_DOC("\\brief Create (or recreate) the window\n\nIf the window was already created, it closes it first.\nIf `state` is `State::Fullscreen`, then `mode` must be\na valid video mode.\n\nThe last parameter is a structure specifying advanced OpenGL\ncontext settings such as anti-aliasing, depth-buffer bits, etc.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param style    %Window style, a bitwise OR combination of `sf::Style` enumerators\n\\param state    %Window state\n\\param settings Additional settings for the underlying OpenGL context");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, style: integer, state: sf.State, settings: sf.ContextSettings)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, style: integer, state: sf.State)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, state: sf.State, settings: sf.ContextSettings)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, style: integer)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, state: sf.State)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, handle: sf.WindowHandle, settings: sf.ContextSettings)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, handle: sf.WindowHandle)");
    type_sf__RenderWindow.set_function("create",
        sol::overload(
            [](sf::RenderWindow& self, sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state, const sf::ContextSettings& settings) {
                static_cast<sf::Window&>(self).create(mode, lua_sf::to_sf_string(title), style.value(), state, settings);
            },
            [](sf::RenderWindow& self, sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state) {
                static_cast<sf::WindowBase&>(self).create(mode, lua_sf::to_sf_string(title), style.value(), state);
            },
            [](sf::RenderWindow& self, sf::VideoMode mode, std::string title, sf::State state, const sf::ContextSettings& settings) {
                static_cast<sf::Window&>(self).create(mode, lua_sf::to_sf_string(title), state, settings);
            },
            [](sf::RenderWindow& self, sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style) {
                static_cast<sf::WindowBase&>(self).create(mode, lua_sf::to_sf_string(title), style.value());
            },
            [](sf::RenderWindow& self, sf::VideoMode mode, std::string title, sf::State state) {
                static_cast<sf::WindowBase&>(self).create(mode, lua_sf::to_sf_string(title), state);
            },
            [](sf::RenderWindow& self, sf::VideoMode mode, std::string title) {
                static_cast<sf::WindowBase&>(self).create(mode, lua_sf::to_sf_string(title));
            },
            [](sf::RenderWindow& self, const lua_sf::WindowHandle& handle, const sf::ContextSettings& settings) {
                static_cast<sf::Window&>(self).create(handle.native(), settings);
            },
            [](sf::RenderWindow& self, const lua_sf::WindowHandle& handle) {
                static_cast<sf::WindowBase&>(self).create(handle.native());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Close the window and destroy all the attached resources\n\nAfter calling this function, the `sf::Window` instance remains\nvalid and you can call `create()` to recreate the window.\nAll other functions such as `pollEvent()` or `display()` will\nstill work (i.e. you don't have to test `isOpen()` every time),\nand will have no effect on closed windows.");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "close", "fun(self: sf.RenderWindow)");
    type_sf__RenderWindow.set_function("close",
        [](sf::RenderWindow& self) {
            static_cast<sf::WindowBase&>(self).close();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether or not the window is open\n\nThis function returns whether or not the window exists.\nNote that a hidden window (`setVisible(false)`) is open\n(therefore this function would return `true`).\n\n\\return `true` if the window is open, `false` if it has been closed");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "isOpen", "fun(self: sf.RenderWindow): boolean");
    type_sf__RenderWindow.set_function("isOpen",
        [](sf::RenderWindow& self) -> bool {
            return static_cast<sf::WindowBase&>(self).isOpen();
        }
    );
    LUASF_STUB_DOC("\\brief Pop the next event from the front of the FIFO event queue, if any, and return it\n\nThis function is not blocking: if there's no pending event then\nit will return a `std::nullopt`. Note that more than one event\nmay be present in the event queue, thus you should always call\nthis function in a loop to make sure that you process every\npending event.\n\\code\nwhile (const std::optional event = window.pollEvent())\n{\n// process event...\n}\n\\endcode\n\n\\return The event, otherwise `std::nullopt` if no events are pending\n\n\\see `waitEvent`, `handleEvents`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "pollEvent", "fun(self: sf.RenderWindow): sf.Event|nil");
    type_sf__RenderWindow.set_function("pollEvent",
        [lua](sf::RenderWindow& self) -> sol::object {
            return lua_sf::optional_to_object(lua, static_cast<sf::WindowBase&>(self).pollEvent());
        }
    );
    LUASF_STUB_DOC("\\brief Wait for an event and return it\n\nThis function is blocking: if there's no pending event then\nit will wait until an event is received or until the provided\ntimeout elapses. Only if an error or a timeout occurs the\nreturned event will be `std::nullopt`.\nThis function is typically used when you have a thread that is\ndedicated to events handling: you want to make this thread sleep\nas long as no new event is received.\n\\code\nwhile (const std::optional event = window.waitEvent())\n{\n// process event...\n}\n\\endcode\n\n\\param timeout Maximum time to wait (`Time::Zero` for infinite)\n\n\\return The event, otherwise `std::nullopt` on timeout or if window was closed\n\n\\see `pollEvent`, `handleEvents`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "waitEvent", "fun(self: sf.RenderWindow, timeout: sf.Time): sf.Event|nil");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "waitEvent", "fun(self: sf.RenderWindow): sf.Event|nil");
    type_sf__RenderWindow.set_function("waitEvent",
        sol::overload(
            [lua](sf::RenderWindow& self, sf::Time timeout) -> sol::object {
                return lua_sf::optional_to_object(lua, static_cast<sf::WindowBase&>(self).waitEvent(timeout));
            },
            [lua](sf::RenderWindow& self) -> sol::object {
                return lua_sf::optional_to_object(lua, static_cast<sf::WindowBase&>(self).waitEvent());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the position of the window\n\n\\return Position of the window, in pixels\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getPosition", "fun(self: sf.RenderWindow): sf.Vector2i");
    type_sf__RenderWindow.set_function("getPosition",
        [](sf::RenderWindow& self) -> sf::Vector2i {
            return static_cast<sf::WindowBase&>(self).getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief Change the position of the window on screen\n\nThis function only works for top-level windows\n(i.e. it will be ignored for windows created from\nthe handle of a child window/control).\n\n\\param position New position, in pixels\n\n\\see `getPosition`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setPosition", "fun(self: sf.RenderWindow, position: sf.Vector2i)");
    type_sf__RenderWindow.set_function("setPosition",
        [](sf::RenderWindow& self, sf::Vector2i position) {
            static_cast<sf::WindowBase&>(self).setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief Get the size of the rendering region of the window\n\nThe size doesn't include the titlebar and borders\nof the window.\n\n\\return Size in pixels");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getSize", "fun(self: sf.RenderWindow): sf.Vector2u");
    type_sf__RenderWindow.set_function("getSize",
        [](sf::RenderWindow& self) -> sf::Vector2u {
            return self.getSize();
        }
    );
    LUASF_STUB_DOC("\\brief Change the size of the rendering region of the window\n\n\\param size New size, in pixels\n\n\\see `getSize`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setSize", "fun(self: sf.RenderWindow, size: sf.Vector2u)");
    type_sf__RenderWindow.set_function("setSize",
        [](sf::RenderWindow& self, sf::Vector2u size) {
            static_cast<sf::WindowBase&>(self).setSize(size);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum window rendering region size\n\nPass `std::nullopt` to unset the minimum size\n\n\\param minimumSize New minimum size, in pixels");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMinimumSize", "fun(self: sf.RenderWindow, minimumSize: sf.Vector2u|nil)");
    type_sf__RenderWindow.set_function("setMinimumSize",
        [](sf::RenderWindow& self, sol::object minimumSize) {
            auto minimumSize_optional = lua_sf::optional_from_object<sf::Vector2u>(minimumSize);
            static_cast<sf::WindowBase&>(self).setMinimumSize(minimumSize_optional);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum window rendering region size\n\nPass `std::nullopt` to unset the maximum size\n\n\\param maximumSize New maximum size, in pixels");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMaximumSize", "fun(self: sf.RenderWindow, maximumSize: sf.Vector2u|nil)");
    type_sf__RenderWindow.set_function("setMaximumSize",
        [](sf::RenderWindow& self, sol::object maximumSize) {
            auto maximumSize_optional = lua_sf::optional_from_object<sf::Vector2u>(maximumSize);
            static_cast<sf::WindowBase&>(self).setMaximumSize(maximumSize_optional);
        }
    );
    LUASF_STUB_DOC("\\brief Change the title of the window\n\n\\param title New title\n\n\\see `setIcon`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setTitle", "fun(self: sf.RenderWindow, title: string)");
    type_sf__RenderWindow.set_function("setTitle",
        [](sf::RenderWindow& self, std::string title) {
            static_cast<sf::WindowBase&>(self).setTitle(lua_sf::to_sf_string(title));
        }
    );
    LUASF_STUB_DOC("\\brief Change the window's icon\n\nThe OS default icon is used by default.\n\n\\param icon Image to use as the icon. The image is copied,\nso you need not keep the source alive after\ncalling this function.");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setIcon", "fun(self: sf.RenderWindow, icon: sf.Image)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "setIcon", "fun(self: sf.RenderWindow, size: sf.Vector2u, pixels: any)");
    type_sf__RenderWindow.set_function("setIcon",
        sol::overload(
            [](sf::RenderWindow& self, const sf::Image& icon) {
                self.setIcon(icon);
            },
            [](sf::RenderWindow& self, sf::Vector2u size, sol::object pixels) {
                auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
                static_cast<sf::WindowBase&>(self).setIcon(size, pixels_buffer.data());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Show or hide the window\n\nThe window is shown by default.\n\n\\param visible `true` to show the window, `false` to hide it");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setVisible", "fun(self: sf.RenderWindow, visible: boolean)");
    type_sf__RenderWindow.set_function("setVisible",
        [](sf::RenderWindow& self, bool visible) {
            static_cast<sf::WindowBase&>(self).setVisible(visible);
        }
    );
    LUASF_STUB_DOC("\\brief Show or hide the mouse cursor\n\nThe mouse cursor is visible by default.\n\n\\warning On Windows, this function needs to be called from the\nthread that created the window.\n\n\\param visible `true` to show the mouse cursor, `false` to hide it");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMouseCursorVisible", "fun(self: sf.RenderWindow, visible: boolean)");
    type_sf__RenderWindow.set_function("setMouseCursorVisible",
        [](sf::RenderWindow& self, bool visible) {
            static_cast<sf::WindowBase&>(self).setMouseCursorVisible(visible);
        }
    );
    LUASF_STUB_DOC("\\brief Grab or release the mouse cursor\n\nIf set, grabs the mouse cursor inside this window's client\narea so it may no longer be moved outside its bounds.\nNote that grabbing is only active while the window has\nfocus.\n\n\\param grabbed `true` to enable, `false` to disable");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMouseCursorGrabbed", "fun(self: sf.RenderWindow, grabbed: boolean)");
    type_sf__RenderWindow.set_function("setMouseCursorGrabbed",
        [](sf::RenderWindow& self, bool grabbed) {
            static_cast<sf::WindowBase&>(self).setMouseCursorGrabbed(grabbed);
        }
    );
    LUASF_STUB_DOC("\\brief Set the displayed cursor to a native system cursor\n\nUpon window creation, the arrow cursor is used by default.\n\n\\warning The cursor must not be destroyed while in use by\nthe window.\n\n\\warning Features related to Cursor are not supported on\niOS and Android.\n\n\\param cursor Native system cursor type to display\n\n\\see `sf::Cursor::createFromSystem`, `sf::Cursor::createFromPixels`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMouseCursor", "fun(self: sf.RenderWindow, cursor: sf.Cursor)");
    type_sf__RenderWindow.set_function("setMouseCursor",
        [](sf::RenderWindow& self, const sf::Cursor& cursor) {
            static_cast<sf::WindowBase&>(self).setMouseCursor(cursor);
        }
    );
    LUASF_STUB_DOC("\\brief Enable or disable automatic key-repeat\n\nIf key repeat is enabled, you will receive repeated\nKeyPressed events while keeping a key pressed. If it is disabled,\nyou will only get a single event when the key is pressed.\n\nKey repeat is enabled by default.\n\n\\param enabled `true` to enable, `false` to disable");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setKeyRepeatEnabled", "fun(self: sf.RenderWindow, enabled: boolean)");
    type_sf__RenderWindow.set_function("setKeyRepeatEnabled",
        [](sf::RenderWindow& self, bool enabled) {
            static_cast<sf::WindowBase&>(self).setKeyRepeatEnabled(enabled);
        }
    );
    LUASF_STUB_DOC("\\brief Change the joystick threshold\n\nThe joystick threshold is the value below which\nno JoystickMoved event will be generated.\n\nThe threshold value is 0.1 by default.\n\n\\param threshold New threshold, in the range [0, 100]");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setJoystickThreshold", "fun(self: sf.RenderWindow, threshold: number)");
    type_sf__RenderWindow.set_function("setJoystickThreshold",
        [](sf::RenderWindow& self, float threshold) {
            static_cast<sf::WindowBase&>(self).setJoystickThreshold(threshold);
        }
    );
    LUASF_STUB_DOC("\\brief Request the current window to be made the active\nforeground window\n\nAt any given time, only one window may have the input focus\nto receive input events such as keystrokes or mouse events.\nIf a window requests focus, it only hints to the operating\nsystem, that it would like to be focused. The operating system\nis free to deny the request.\nThis is not to be confused with `setActive()`.\n\n\\see `hasFocus`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "requestFocus", "fun(self: sf.RenderWindow)");
    type_sf__RenderWindow.set_function("requestFocus",
        [](sf::RenderWindow& self) {
            static_cast<sf::WindowBase&>(self).requestFocus();
        }
    );
    LUASF_STUB_DOC("\\brief Check whether the window has the input focus\n\nAt any given time, only one window may have the input focus\nto receive input events such as keystrokes or most mouse\nevents.\n\n\\return `true` if window has focus, `false` otherwise\n\\see `requestFocus`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "hasFocus", "fun(self: sf.RenderWindow): boolean");
    type_sf__RenderWindow.set_function("hasFocus",
        [](sf::RenderWindow& self) -> bool {
            return static_cast<sf::WindowBase&>(self).hasFocus();
        }
    );
    LUASF_STUB_DOC("\\brief Get the OS-specific handle of the window\n\nThe type of the returned handle is `sf::WindowHandle`,\nwhich is a type alias to the handle type defined by the OS.\nYou shouldn't need to use this function, unless you have\nvery specific stuff to implement that SFML doesn't support,\nor implement a temporary workaround until a bug is fixed.\n\n\\return System handle of the window");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getNativeHandle", "fun(self: sf.RenderWindow): sf.WindowHandle");
    type_sf__RenderWindow.set_function("getNativeHandle",
        [](sf::RenderWindow& self) -> lua_sf::WindowHandle {
            return lua_sf::WindowHandle::fromNative(static_cast<sf::WindowBase&>(self).getNativeHandle());
        }
    );
    LUASF_STUB_DOC("\\brief Get the settings of the OpenGL context of the window\n\nNote that these settings may be different from what was\npassed to the constructor or the `create()` function,\nif one or more settings were not supported. In this case,\nSFML chose the closest match.\n\n\\return Structure containing the OpenGL context settings");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getSettings", "fun(self: sf.RenderWindow): sf.ContextSettings");
    type_sf__RenderWindow.set_function("getSettings",
        sol::policies(
            [](sf::RenderWindow& self) {
                return std::cref(static_cast<sf::Window&>(self).getSettings());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Enable or disable vertical synchronization\n\nActivating vertical synchronization will limit the number\nof frames displayed to the refresh rate of the monitor.\nThis can avoid some visual artifacts, and limit the framerate\nto a good value (but not constant across different computers).\n\nVertical synchronization is disabled by default.\n\n\\param enabled `true` to enable v-sync, `false` to deactivate it");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setVerticalSyncEnabled", "fun(self: sf.RenderWindow, enabled: boolean)");
    type_sf__RenderWindow.set_function("setVerticalSyncEnabled",
        [](sf::RenderWindow& self, bool enabled) {
            static_cast<sf::Window&>(self).setVerticalSyncEnabled(enabled);
        }
    );
    LUASF_STUB_DOC("\\brief Limit the framerate to a maximum fixed frequency\n\nIf a limit is set, the window will use a small delay after\neach call to `display()` to ensure that the current frame\nlasted long enough to match the framerate limit.\nSFML will try to match the given limit as much as it can,\nbut since it internally uses `sf::sleep`, whose precision\ndepends on the underlying OS, the results may be a little\nimprecise as well (for example, you can get 65 FPS when\nrequesting 60).\n\n\\param limit Framerate limit, in frames per seconds (use 0 to disable limit)");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setFramerateLimit", "fun(self: sf.RenderWindow, limit: integer)");
    type_sf__RenderWindow.set_function("setFramerateLimit",
        [](sf::RenderWindow& self, lua_sf::LuaIntegral<unsigned int> limit) {
            static_cast<sf::Window&>(self).setFramerateLimit(limit.value());
        }
    );
    LUASF_STUB_DOC("\\brief Activate or deactivate the window as the current target\nfor OpenGL rendering\n\nA window is active only on the current thread, if you want to\nmake it active on another thread you have to deactivate it\non the previous thread first if it was active.\nOnly one window can be active on a thread at a time, thus\nthe window previously active (if any) automatically gets deactivated.\nThis is not to be confused with `requestFocus()`.\n\n\\param active `true` to activate, `false` to deactivate\n\n\\return `true` if operation was successful, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setActive", "fun(self: sf.RenderWindow, active: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "setActive", "fun(self: sf.RenderWindow): boolean");
    type_sf__RenderWindow.set_function("setActive",
        sol::overload(
            [](sf::RenderWindow& self, bool active) -> bool {
                return self.setActive(active);
            },
            [](sf::RenderWindow& self) -> bool {
                return self.setActive();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Display on screen what has been rendered to the window so far\n\nThis function is typically called after all OpenGL rendering\nhas been done for the current frame, in order to show\nit on screen.");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "display", "fun(self: sf.RenderWindow)");
    type_sf__RenderWindow.set_function("display",
        [](sf::RenderWindow& self) {
            static_cast<sf::Window&>(self).display();
        }
    );
    LUASF_STUB_DOC("\\brief Clear the entire target with a single color and stencil value\n\nThe specified stencil value is truncated to the bit\nwidth of the current stencil buffer.\n\n\\param color        Fill color to use to clear the render target\n\\param stencilValue Stencil value to clear to");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "clear", "fun(self: sf.RenderWindow, color: sf.Color, stencilValue: sf.StencilValue)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "clear", "fun(self: sf.RenderWindow, color: sf.Color)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "clear", "fun(self: sf.RenderWindow)");
    type_sf__RenderWindow.set_function("clear",
        sol::overload(
            [](sf::RenderWindow& self, sf::Color color, sf::StencilValue stencilValue) {
                static_cast<sf::RenderTarget&>(self).clear(color, stencilValue);
            },
            [](sf::RenderWindow& self, sf::Color color) {
                static_cast<sf::RenderTarget&>(self).clear(color);
            },
            [](sf::RenderWindow& self) {
                static_cast<sf::RenderTarget&>(self).clear();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Clear the stencil buffer to a specific value\n\nThe specified value is truncated to the bit width of\nthe current stencil buffer.\n\n\\param stencilValue Stencil value to clear to");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "clearStencil", "fun(self: sf.RenderWindow, stencilValue: sf.StencilValue)");
    type_sf__RenderWindow.set_function("clearStencil",
        [](sf::RenderWindow& self, sf::StencilValue stencilValue) {
            static_cast<sf::RenderTarget&>(self).clearStencil(stencilValue);
        }
    );
    LUASF_STUB_DOC("\\brief Change the current active view\n\nThe view is like a 2D camera, it controls which part of\nthe 2D scene is visible, and how it is viewed in the\nrender target.\nThe new view will affect everything that is drawn, until\nanother view is set.\nThe render target keeps its own copy of the view object,\nso it is not necessary to keep the original one alive\nafter calling this function.\nTo restore the original view of the target, you can pass\nthe result of `getDefaultView()` to this function.\n\n\\param view New view to use\n\n\\see `getView`, `getDefaultView`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setView", "fun(self: sf.RenderWindow, view: sf.View)");
    type_sf__RenderWindow.set_function("setView",
        [](sf::RenderWindow& self, const sf::View& view) {
            static_cast<sf::RenderTarget&>(self).setView(view);
        }
    );
    LUASF_STUB_DOC("\\brief Get the view currently in use in the render target\n\n\\return The view object that is currently used\n\n\\see `setView`, `getDefaultView`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getView", "fun(self: sf.RenderWindow): sf.View");
    type_sf__RenderWindow.set_function("getView",
        sol::policies(
            [](sf::RenderWindow& self) {
                return std::cref(static_cast<sf::RenderTarget&>(self).getView());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the default view of the render target\n\nThe default view has the initial size of the render target,\nand never changes after the target has been created.\n\n\\return The default view of the render target\n\n\\see `setView`, `getView`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getDefaultView", "fun(self: sf.RenderWindow): sf.View");
    type_sf__RenderWindow.set_function("getDefaultView",
        sol::policies(
            [](sf::RenderWindow& self) {
                return std::cref(static_cast<sf::RenderTarget&>(self).getDefaultView());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the viewport of a view, applied to this render target\n\nThe viewport is defined in the view as a ratio, this function\nsimply applies this ratio to the current dimensions of the\nrender target to calculate the pixels rectangle that the viewport\nactually covers in the target.\n\n\\param view The view for which we want to compute the viewport\n\n\\return Viewport rectangle, expressed in pixels");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getViewport", "fun(self: sf.RenderWindow, view: sf.View): sf.IntRect");
    type_sf__RenderWindow.set_function("getViewport",
        [](sf::RenderWindow& self, const sf::View& view) -> sf::IntRect {
            return static_cast<sf::RenderTarget&>(self).getViewport(view);
        }
    );
    LUASF_STUB_DOC("\\brief Get the scissor rectangle of a view, applied to this render target\n\nThe scissor rectangle is defined in the view as a ratio. This\nfunction simply applies this ratio to the current dimensions\nof the render target to calculate the pixels rectangle\nthat the scissor rectangle actually covers in the target.\n\n\\param view The view for which we want to compute the scissor rectangle\n\n\\return Scissor rectangle, expressed in pixels");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getScissor", "fun(self: sf.RenderWindow, view: sf.View): sf.IntRect");
    type_sf__RenderWindow.set_function("getScissor",
        [](sf::RenderWindow& self, const sf::View& view) -> sf::IntRect {
            return static_cast<sf::RenderTarget&>(self).getScissor(view);
        }
    );
    LUASF_STUB_DOC("\\brief Convert a point from target coordinates to world coordinates\n\nThis function finds the 2D position that matches the\ngiven pixel of the render target. In other words, it does\nthe inverse of what the graphics card does, to find the\ninitial position of a rendered pixel.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (10, 50) in your render target may map to the point\n(150, 75) in your 2D world -- if the view is translated by (140, 25).\n\nFor render-windows, this function is typically used to find\nwhich point (or object) is located below the mouse cursor.\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Pixel to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in \"world\" units\n\n\\see `mapCoordsToPixel`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "mapPixelToCoords", "fun(self: sf.RenderWindow, point: sf.Vector2i, view: sf.View): sf.Vector2f");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "mapPixelToCoords", "fun(self: sf.RenderWindow, point: sf.Vector2i): sf.Vector2f");
    type_sf__RenderWindow.set_function("mapPixelToCoords",
        sol::overload(
            [](sf::RenderWindow& self, sf::Vector2i point, const sf::View& view) -> sf::Vector2f {
                return static_cast<sf::RenderTarget&>(self).mapPixelToCoords(point, view);
            },
            [](sf::RenderWindow& self, sf::Vector2i point) -> sf::Vector2f {
                return static_cast<sf::RenderTarget&>(self).mapPixelToCoords(point);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Convert a point from world coordinates to target coordinates\n\nThis function finds the pixel of the render target that matches\nthe given 2D point. In other words, it goes through the same process\nas the graphics card, to compute the final position of a rendered point.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (150, 75) in your 2D world may map to the pixel\n(10, 50) of your render target -- if the view is translated by (140, 25).\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Point to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in target coordinates (pixels)\n\n\\see `mapPixelToCoords`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "mapCoordsToPixel", "fun(self: sf.RenderWindow, point: sf.Vector2f, view: sf.View): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "mapCoordsToPixel", "fun(self: sf.RenderWindow, point: sf.Vector2f): sf.Vector2i");
    type_sf__RenderWindow.set_function("mapCoordsToPixel",
        sol::overload(
            [](sf::RenderWindow& self, sf::Vector2f point, const sf::View& view) -> sf::Vector2i {
                return static_cast<sf::RenderTarget&>(self).mapCoordsToPixel(point, view);
            },
            [](sf::RenderWindow& self, sf::Vector2f point) -> sf::Vector2i {
                return static_cast<sf::RenderTarget&>(self).mapCoordsToPixel(point);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Draw primitives defined by a vertex buffer\n\n\\param vertexBuffer Vertex buffer\n\\param firstVertex  Index of the first vertex to render\n\\param vertexCount  Number of vertices to render\n\\param states       Render states to use for drawing");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, drawable: sf.Drawable, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, vertexBuffer: sf.VertexBuffer, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, drawable: sf.Drawable)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, vertexBuffer: sf.VertexBuffer)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, vertices: any, type: sf.PrimitiveType, states: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, vertices: any, type: sf.PrimitiveType)");
    type_sf__RenderWindow.set_function("draw",
        sol::overload(
            [](sf::RenderWindow& self, const sf::VertexBuffer& vertexBuffer, lua_sf::LuaIntegral<std::size_t> firstVertex, lua_sf::LuaIntegral<std::size_t> vertexCount, const sf::RenderStates& states) {
                static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, firstVertex.value(), vertexCount.value(), states);
            },
            [](sf::RenderWindow& self, const sf::VertexBuffer& vertexBuffer, lua_sf::LuaIntegral<std::size_t> firstVertex, lua_sf::LuaIntegral<std::size_t> vertexCount) {
                static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, firstVertex.value(), vertexCount.value());
            },
            [](sf::RenderWindow& self, const sf::Drawable& drawable, const sf::RenderStates& states) {
                static_cast<sf::RenderTarget&>(self).draw(drawable, states);
            },
            [](sf::RenderWindow& self, const sf::VertexBuffer& vertexBuffer, const sf::RenderStates& states) {
                static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, states);
            },
            [](sf::RenderWindow& self, const sf::Drawable& drawable) {
                static_cast<sf::RenderTarget&>(self).draw(drawable);
            },
            [](sf::RenderWindow& self, const sf::VertexBuffer& vertexBuffer) {
                static_cast<sf::RenderTarget&>(self).draw(vertexBuffer);
            },
            [](sf::RenderWindow& self, sol::object vertices, sf::PrimitiveType type, const sf::RenderStates& states) {
                auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
                static_cast<sf::RenderTarget&>(self).draw(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), type, states);
            },
            [](sf::RenderWindow& self, sol::object vertices, sf::PrimitiveType type) {
                auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
                static_cast<sf::RenderTarget&>(self).draw(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), type);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Tell if the window will use sRGB encoding when drawing on it\n\nYou can request sRGB encoding for a window by having the sRgbCapable flag set in the `ContextSettings`\n\n\\return `true` if the window use sRGB encoding, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "isSrgb", "fun(self: sf.RenderWindow): boolean");
    type_sf__RenderWindow.set_function("isSrgb",
        [](sf::RenderWindow& self) -> bool {
            return self.isSrgb();
        }
    );
    LUASF_STUB_DOC("\\brief Save the OpenGL render states modified by SFML\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering. Combined with popGLStates,\nit ensures that:\n\\li SFML's internal states are not messed up by your OpenGL code\n\\li your OpenGL states are not modified by a call to a SFML function\n\nMore specifically, it must be used around code that\ncalls `draw` functions. Example:\n\\code\n// OpenGL code here...\nwindow.pushGLStates();\nwindow.draw(...);\nwindow.draw(...);\nwindow.popGLStates();\n// OpenGL code here...\n\\endcode\n\nNote that this function is quite expensive: it saves the\nprogram, textures, vertex attributes and the other OpenGL\nstates that SFML drawing can modify. State outside this set\nis deliberately not covered.\nIt is provided for convenience, but the best results will\nbe achieved if you handle OpenGL states yourself (because\nyou know which states have really changed, and need to be\nsaved and restored). Take a look at the resetGLStates\nfunction if you do so.\n\n\\see `popGLStates`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "pushGLStates", "fun(self: sf.RenderWindow)");
    type_sf__RenderWindow.set_function("pushGLStates",
        [](sf::RenderWindow& self) {
            static_cast<sf::RenderTarget&>(self).pushGLStates();
        }
    );
    LUASF_STUB_DOC("\\brief Restore the previously saved OpenGL render states\n\nSee the description of `pushGLStates` to get a detailed\ndescription of these functions.\n\n\\see `pushGLStates`");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "popGLStates", "fun(self: sf.RenderWindow)");
    type_sf__RenderWindow.set_function("popGLStates",
        [](sf::RenderWindow& self) {
            static_cast<sf::RenderTarget&>(self).popGLStates();
        }
    );
    LUASF_STUB_DOC("\\brief Reset the internal OpenGL states so that the target is ready for drawing\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering, if you choose not to use\n`pushGLStates`/`popGLStates`. It makes sure that all OpenGL\nstates needed by SFML are set, so that subsequent `draw()`\ncalls will work as expected.\n\nExample:\n\\code\n// OpenGL code here...\nwindow.resetGLStates();\nwindow.draw(...);\nwindow.draw(...);\n// OpenGL code here...\n\\endcode");
    LUASF_STUB_FUNCTION("sf.RenderWindow", "resetGLStates", "fun(self: sf.RenderWindow)");
    type_sf__RenderWindow.set_function("resetGLStates",
        [](sf::RenderWindow& self) {
            static_cast<sf::RenderTarget&>(self).resetGLStates();
        }
    );
    // Skipped sf::RenderWindow::createVulkanSurface(const VkInstance &, VkSurfaceKHR &, const VkAllocationCallbacks *): unsupported parameter type const VkInstance&.
    // Skipped sf::RenderWindow::createVulkanSurface(const VkInstance &, VkSurfaceKHR &, const VkAllocationCallbacks *): unsupported parameter type const VkInstance&.
}
