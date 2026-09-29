#include "Window/bind_Window.hpp"

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

namespace { constexpr std::array<std::string_view, 40> docs = {
    "\\brief Window that serves as a target for OpenGL rendering",
    "\\brief Default constructor\n\nThis constructor doesn't actually create the window,\nuse the other constructors or call `create()` to do so.",
    "\\brief Construct a new window\n\nThis constructor creates the window with the size and pixel\ndepth defined in `mode`. An optional style can be passed to\ncustomize the look and behavior of the window (borders,\ntitle bar, resizable, closable, ...). An optional state can\nbe provided. If `state` is `State::Fullscreen`, then `mode`\nmust be a valid video mode.\n\nThe last parameter is an optional structure specifying\nadvanced OpenGL context settings such as anti-aliasing,\ndepth-buffer bits, etc.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param style    %Window style, a bitwise OR combination of `sf::Style` enumerators\n\\param state    %Window state\n\\param settings Additional settings for the underlying OpenGL context",
    "\\brief Construct a new window\n\nThis constructor creates the window with the size and pixel\ndepth defined in `mode`. If `state` is `State::Fullscreen`,\nthen `mode` must be a valid video mode.\n\nThe last parameter is an optional structure specifying\nadvanced OpenGL context settings such as anti-aliasing,\ndepth-buffer bits, etc.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param state    %Window state\n\\param settings Additional settings for the underlying OpenGL context",
    "\\brief Construct the window from an existing control\n\nUse this constructor if you want to create an OpenGL\nrendering area into an already existing control.\n\nThe second parameter is an optional structure specifying\nadvanced OpenGL context settings such as anti-aliasing,\ndepth-buffer bits, etc.\n\n\\param handle   Platform-specific handle of the control\n\\param settings Additional settings for the underlying OpenGL context",
    "\\brief Create (or recreate) the window\n\nIf the window was already created, it closes it first.\nIf `state` is `State::Fullscreen`, then `mode` must be\na valid video mode.\n\n\\param mode  Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title Title of the window\n\\param style %Window style, a bitwise OR combination of `sf::Style` enumerators\n\\param state %Window state",
    "\\brief Create (or recreate) the window\n\nIf the window was already created, it closes it first.\nIf `state` is `State::Fullscreen`, then `mode` must be\na valid video mode.\n\n\\param mode  Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title Title of the window\n\\param state %Window state",
    "\\brief Create (or recreate) the window from an existing control\n\n\\param handle Platform-specific handle of the control",
    "\\brief Close the window and destroy all the attached resources\n\nAfter calling this function, the `sf::Window` instance remains\nvalid and you can call `create()` to recreate the window.\nAll other functions such as `pollEvent()` or `display()` will\nstill work (i.e. you don't have to test `isOpen()` every time),\nand will have no effect on closed windows.",
    "\\brief Tell whether or not the window is open\n\nThis function returns whether or not the window exists.\nNote that a hidden window (`setVisible(false)`) is open\n(therefore this function would return `true`).\n\n\\return `true` if the window is open, `false` if it has been closed",
    "\\brief Pop the next event from the front of the FIFO event queue, if any, and return it\n\nThis function is not blocking: if there's no pending event then\nit will return a `std::nullopt`. Note that more than one event\nmay be present in the event queue, thus you should always call\nthis function in a loop to make sure that you process every\npending event.\n\\code\nwhile (const std::optional event = window.pollEvent())\n{\n// process event...\n}\n\\endcode\n\n\\return The event, otherwise `std::nullopt` if no events are pending\n\n\\see `waitEvent`, `handleEvents`",
    "\\brief Wait for an event and return it\n\nThis function is blocking: if there's no pending event then\nit will wait until an event is received or until the provided\ntimeout elapses. Only if an error or a timeout occurs the\nreturned event will be `std::nullopt`.\nThis function is typically used when you have a thread that is\ndedicated to events handling: you want to make this thread sleep\nas long as no new event is received.\n\\code\nwhile (const std::optional event = window.waitEvent())\n{\n// process event...\n}\n\\endcode\n\n\\param timeout Maximum time to wait (`Time::Zero` for infinite)\n\n\\return The event, otherwise `std::nullopt` on timeout or if window was closed\n\n\\see `pollEvent`, `handleEvents`",
    "\\brief Get the position of the window\n\n\\return Position of the window, in pixels\n\n\\see `setPosition`",
    "\\brief Change the position of the window on screen\n\nThis function only works for top-level windows\n(i.e. it will be ignored for windows created from\nthe handle of a child window/control).\n\n\\param position New position, in pixels\n\n\\see `getPosition`",
    "\\brief Get the size of the rendering region of the window\n\nThe size doesn't include the titlebar and borders\nof the window.\n\n\\return Size in pixels\n\n\\see `setSize`",
    "\\brief Change the size of the rendering region of the window\n\n\\param size New size, in pixels\n\n\\see `getSize`",
    "\\brief Set the minimum window rendering region size\n\nPass `std::nullopt` to unset the minimum size\n\n\\param minimumSize New minimum size, in pixels",
    "\\brief Set the maximum window rendering region size\n\nPass `std::nullopt` to unset the maximum size\n\n\\param maximumSize New maximum size, in pixels",
    "\\brief Change the title of the window\n\n\\param title New title\n\n\\see `setIcon`",
    "\\brief Change the window's icon\n\n`pixels` must be an array of `size` pixels\nin 32-bits RGBA format.\n\nThe OS default icon is used by default.\n\n\\param size   Icon's width and height, in pixels\n\\param pixels Pointer to the array of pixels in memory. The\npixels are copied, so you need not keep the\nsource alive after calling this function.\n\n\\see `setTitle`",
    "\\brief Show or hide the window\n\nThe window is shown by default.\n\n\\param visible `true` to show the window, `false` to hide it",
    "\\brief Show or hide the mouse cursor\n\nThe mouse cursor is visible by default.\n\n\\warning On Windows, this function needs to be called from the\nthread that created the window.\n\n\\param visible `true` to show the mouse cursor, `false` to hide it",
    "\\brief Grab or release the mouse cursor\n\nIf set, grabs the mouse cursor inside this window's client\narea so it may no longer be moved outside its bounds.\nNote that grabbing is only active while the window has\nfocus.\n\n\\param grabbed `true` to enable, `false` to disable",
    "\\brief Set the displayed cursor to a native system cursor\n\nUpon window creation, the arrow cursor is used by default.\n\n\\warning The cursor must not be destroyed while in use by\nthe window.\n\n\\warning Features related to Cursor are not supported on\niOS and Android.\n\n\\param cursor Native system cursor type to display\n\n\\see `sf::Cursor::createFromSystem`, `sf::Cursor::createFromPixels`",
    "\\brief Enable or disable automatic key-repeat\n\nIf key repeat is enabled, you will receive repeated\nKeyPressed events while keeping a key pressed. If it is disabled,\nyou will only get a single event when the key is pressed.\n\nKey repeat is enabled by default.\n\n\\param enabled `true` to enable, `false` to disable",
    "\\brief Change the joystick threshold\n\nThe joystick threshold is the value below which\nno JoystickMoved event will be generated.\n\nThe threshold value is 0.1 by default.\n\n\\param threshold New threshold, in the range [0, 100]",
    "\\brief Request the current window to be made the active\nforeground window\n\nAt any given time, only one window may have the input focus\nto receive input events such as keystrokes or mouse events.\nIf a window requests focus, it only hints to the operating\nsystem, that it would like to be focused. The operating system\nis free to deny the request.\nThis is not to be confused with `setActive()`.\n\n\\see `hasFocus`",
    "\\brief Check whether the window has the input focus\n\nAt any given time, only one window may have the input focus\nto receive input events such as keystrokes or most mouse\nevents.\n\n\\return `true` if window has focus, `false` otherwise\n\\see `requestFocus`",
    "\\brief Get the OS-specific handle of the window\n\nThe type of the returned handle is `sf::WindowHandle`,\nwhich is a type alias to the handle type defined by the OS.\nYou shouldn't need to use this function, unless you have\nvery specific stuff to implement that SFML doesn't support,\nor implement a temporary workaround until a bug is fixed.\n\n\\return System handle of the window",
    "\\brief Create (or recreate) the window\n\nIf the window was already created, it closes it first.\nIf `state` is `State::Fullscreen`, then `mode` must be\na valid video mode.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param style    %Window style, a bitwise OR combination of `sf::Style` enumerators\n\\param state    %Window state",
    "\\brief Create (or recreate) the window\n\nIf the window was already created, it closes it first.\nIf `state` is `State::Fullscreen`, then `mode` must be\na valid video mode.\n\nThe last parameter is a structure specifying advanced OpenGL\ncontext settings such as anti-aliasing, depth-buffer bits, etc.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param style    %Window style, a bitwise OR combination of `sf::Style` enumerators\n\\param state    %Window state\n\\param settings Additional settings for the underlying OpenGL context",
    "\\brief Create (or recreate) the window\n\nIf the window was already created, it closes it first.\nIf `state` is `State::Fullscreen`, then `mode` must be\na valid video mode.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param state    %Window state",
    "\\brief Create (or recreate) the window\n\nIf the window was already created, it closes it first.\nIf `state` is `State::Fullscreen`, then `mode` must be\na valid video mode.\n\nThe last parameter is a structure specifying advanced OpenGL\ncontext settings such as anti-aliasing, depth-buffer bits, etc.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param state    %Window state\n\\param settings Additional settings for the underlying OpenGL context",
    "\\brief Create (or recreate) the window from an existing control\n\nUse this function if you want to create an OpenGL\nrendering area into an already existing control.\nIf the window was already created, it closes it first.\n\n\\param handle   Platform-specific handle of the control",
    "\\brief Create (or recreate) the window from an existing control\n\nUse this function if you want to create an OpenGL\nrendering area into an already existing control.\nIf the window was already created, it closes it first.\n\nThe second parameter is an optional structure specifying\nadvanced OpenGL context settings such as anti-aliasing,\ndepth-buffer bits, etc.\n\n\\param handle   Platform-specific handle of the control\n\\param settings Additional settings for the underlying OpenGL context",
    "\\brief Get the settings of the OpenGL context of the window\n\nNote that these settings may be different from what was\npassed to the constructor or the `create()` function,\nif one or more settings were not supported. In this case,\nSFML chose the closest match.\n\n\\return Structure containing the OpenGL context settings",
    "\\brief Enable or disable vertical synchronization\n\nActivating vertical synchronization will limit the number\nof frames displayed to the refresh rate of the monitor.\nThis can avoid some visual artifacts, and limit the framerate\nto a good value (but not constant across different computers).\n\nVertical synchronization is disabled by default.\n\n\\param enabled `true` to enable v-sync, `false` to deactivate it",
    "\\brief Limit the framerate to a maximum fixed frequency\n\nIf a limit is set, the window will use a small delay after\neach call to `display()` to ensure that the current frame\nlasted long enough to match the framerate limit.\nSFML will try to match the given limit as much as it can,\nbut since it internally uses `sf::sleep`, whose precision\ndepends on the underlying OS, the results may be a little\nimprecise as well (for example, you can get 65 FPS when\nrequesting 60).\n\n\\param limit Framerate limit, in frames per seconds (use 0 to disable limit)",
    "\\brief Activate or deactivate the window as the current target\nfor OpenGL rendering\n\nA window is active only on the current thread, if you want to\nmake it active on another thread you have to deactivate it\non the previous thread first if it was active.\nOnly one window can be active on a thread at a time, thus\nthe window previously active (if any) automatically gets deactivated.\nThis is not to be confused with `requestFocus()`.\n\n\\param active `true` to activate, `false` to deactivate\n\n\\return `true` if operation was successful, `false` otherwise",
    "\\brief Display on screen what has been rendered to the window so far\n\nThis function is typically called after all OpenGL rendering\nhas been done for the current frame, in order to show\nit on screen.",
}; }

void bind_Window(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    auto type_sf__Window = lua_glue::BindClass<sf::Window>(sf, "Window");
    lua_glue::BindBase<sf::Window, sf::WindowBase>(type_sf__Window);
    lua_glue::Table table_sf__Window = sf["Window"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Window>(lua);
    lua_glue::Table native_bases_sf__Window = lua.create_table();
    native_bases_sf__Window.add(lua["sf"]["WindowBase"].get<lua_glue::Table>());
    table_sf__Window.raw_set("__nativeBases", native_bases_sf__Window);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Window", "sf.WindowBase");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Window", "new", "fun(mode: sf.VideoMode, title: string, style?: integer, state?: sf.State, settings?: sf.ContextSettings): sf.Window");
    LUASF_STUB_OVERLOAD("sf.Window", "new", "fun(mode: sf.VideoMode, title: string, state: sf.State, settings?: sf.ContextSettings): sf.Window");
    LUASF_STUB_OVERLOAD("sf.Window", "new", "fun(handle: sf.WindowHandle, settings?: sf.ContextSettings): sf.Window");
    LUASF_STUB_OVERLOAD("sf.Window", "new", "fun(): sf.Window");
    lua_glue::BindCallable(type_sf__Window, "new",
        [](sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::Window>(mode, lua_sf::to_sf_string(title), style.value(), state, settings);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned int>(Style::Default);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::State>(sf::State::Windowed);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::ContextSettings{ };
        }}},
        docs[2]
    );
    lua_glue::BindCallable(type_sf__Window, "new",
        [](sf::VideoMode mode, std::string title, sf::State state, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::Window>(mode, lua_sf::to_sf_string(title), state, settings);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::ContextSettings{ };
        }}},
        docs[3]
    );
    lua_glue::BindCallable(type_sf__Window, "new",
        [](const lua_sf::WindowHandle& handle, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::Window>(handle.getHandle(), settings);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::ContextSettings{ };
        }}},
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Window, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Window>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FUNCTION("sf.Window", "create", "fun(self: sf.Window, mode: sf.VideoMode, title: string, style: integer, state: sf.State, settings: sf.ContextSettings)");
    LUASF_STUB_OVERLOAD("sf.Window", "create", "fun(self: sf.Window, mode: sf.VideoMode, title: string, style?: integer, state?: sf.State)");
    LUASF_STUB_OVERLOAD("sf.Window", "create", "fun(self: sf.Window, mode: sf.VideoMode, title: string, state: sf.State, settings: sf.ContextSettings)");
    LUASF_STUB_OVERLOAD("sf.Window", "create", "fun(self: sf.Window, mode: sf.VideoMode, title: string, state: sf.State)");
    LUASF_STUB_OVERLOAD("sf.Window", "create", "fun(self: sf.Window, handle: sf.WindowHandle, settings: sf.ContextSettings)");
    LUASF_STUB_OVERLOAD("sf.Window", "create", "fun(self: sf.Window, handle: sf.WindowHandle)");
    lua_glue::BindCallable(type_sf__Window, "create",
        [](sf::Window& self, sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state, const sf::ContextSettings& settings) {
            self.create(mode, lua_sf::to_sf_string(title), style.value(), state, settings);
        },
        docs[30]
    );
    lua_glue::BindCallable(type_sf__Window, "create",
        [](sf::Window& self, sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state) {
            self.create(mode, lua_sf::to_sf_string(title), style.value(), state);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned int>(Style::Default);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::State>(sf::State::Windowed);
        }}},
        docs[29]
    );
    lua_glue::BindCallable(type_sf__Window, "create",
        [](sf::Window& self, sf::VideoMode mode, std::string title, sf::State state, const sf::ContextSettings& settings) {
            self.create(mode, lua_sf::to_sf_string(title), state, settings);
        },
        docs[32]
    );
    lua_glue::BindCallable(type_sf__Window, "create",
        [](sf::Window& self, sf::VideoMode mode, std::string title, sf::State state) {
            self.create(mode, lua_sf::to_sf_string(title), state);
        },
        docs[31]
    );
    lua_glue::BindCallable(type_sf__Window, "create",
        [](sf::Window& self, const lua_sf::WindowHandle& handle, const sf::ContextSettings& settings) {
            self.create(handle.getHandle(), settings);
        },
        docs[34]
    );
    lua_glue::BindCallable(type_sf__Window, "create",
        [](sf::Window& self, const lua_sf::WindowHandle& handle) {
            self.create(handle.getHandle());
        },
        docs[33]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Window", "close", "fun(self: sf.Window)");
    lua_glue::BindCallable(type_sf__Window, "close",
        [](sf::Window& self) {
            self.close();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Window", "isOpen", "fun(self: sf.Window): boolean");
    lua_glue::BindCallable(type_sf__Window, "isOpen",
        [](const sf::Window& self) -> bool {
            return static_cast<const sf::WindowBase&>(self).isOpen();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Window", "pollEvent", "fun(self: sf.Window): sf.Event|nil");
    lua_glue::BindCallable(type_sf__Window, "pollEvent",
        [lua](sf::Window& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, static_cast<sf::WindowBase&>(self).pollEvent());
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Window", "waitEvent", "fun(self: sf.Window, timeout?: sf.Time): sf.Event|nil");
    lua_glue::BindCallable(type_sf__Window, "waitEvent",
        [lua](sf::Window& self, sf::Time timeout) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, static_cast<sf::WindowBase&>(self).waitEvent(timeout));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Time>(sf::Time::Zero);
        }}},
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Window", "getPosition", "fun(self: sf.Window): sf.Vector2i");
    lua_glue::BindCallable(type_sf__Window, "getPosition",
        [](const sf::Window& self) -> sf::Vector2i {
            return static_cast<const sf::WindowBase&>(self).getPosition();
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Window", "setPosition", "fun(self: sf.Window, position: sf.Vector2i)");
    lua_glue::BindCallable(type_sf__Window, "setPosition",
        [](sf::Window& self, sf::Vector2i position) {
            static_cast<sf::WindowBase&>(self).setPosition(position);
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Window", "getSize", "fun(self: sf.Window): sf.Vector2u");
    lua_glue::BindCallable(type_sf__Window, "getSize",
        [](const sf::Window& self) -> sf::Vector2u {
            return static_cast<const sf::WindowBase&>(self).getSize();
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Window", "setSize", "fun(self: sf.Window, size: sf.Vector2u)");
    lua_glue::BindCallable(type_sf__Window, "setSize",
        [](sf::Window& self, sf::Vector2u size) {
            static_cast<sf::WindowBase&>(self).setSize(size);
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Window", "setMinimumSize", "fun(self: sf.Window, minimumSize: sf.Vector2u|nil)");
    lua_glue::BindCallable(type_sf__Window, "setMinimumSize",
        [](sf::Window& self, lua_glue::Object minimumSize) {
            auto minimumSize_optional = lua_sf::optional_from_object<sf::Vector2u>(minimumSize);
            static_cast<sf::WindowBase&>(self).setMinimumSize(minimumSize_optional);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Window", "setMaximumSize", "fun(self: sf.Window, maximumSize: sf.Vector2u|nil)");
    lua_glue::BindCallable(type_sf__Window, "setMaximumSize",
        [](sf::Window& self, lua_glue::Object maximumSize) {
            auto maximumSize_optional = lua_sf::optional_from_object<sf::Vector2u>(maximumSize);
            static_cast<sf::WindowBase&>(self).setMaximumSize(maximumSize_optional);
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.Window", "setTitle", "fun(self: sf.Window, title: string)");
    lua_glue::BindCallable(type_sf__Window, "setTitle",
        [](sf::Window& self, std::string title) {
            static_cast<sf::WindowBase&>(self).setTitle(lua_sf::to_sf_string(title));
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Window", "setIcon", "fun(self: sf.Window, size: sf.Vector2u, pixels: any)");
    lua_glue::BindCallable(type_sf__Window, "setIcon",
        [](sf::Window& self, sf::Vector2u size, lua_glue::Object pixels) {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            static_cast<sf::WindowBase&>(self).setIcon(size, pixels_buffer.data());
        },
        docs[19]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Window", "setVisible", "fun(self: sf.Window, visible: boolean)");
    lua_glue::BindCallable(type_sf__Window, "setVisible",
        [](sf::Window& self, bool visible) {
            static_cast<sf::WindowBase&>(self).setVisible(visible);
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Window", "setMouseCursorVisible", "fun(self: sf.Window, visible: boolean)");
    lua_glue::BindCallable(type_sf__Window, "setMouseCursorVisible",
        [](sf::Window& self, bool visible) {
            static_cast<sf::WindowBase&>(self).setMouseCursorVisible(visible);
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Window", "setMouseCursorGrabbed", "fun(self: sf.Window, grabbed: boolean)");
    lua_glue::BindCallable(type_sf__Window, "setMouseCursorGrabbed",
        [](sf::Window& self, bool grabbed) {
            static_cast<sf::WindowBase&>(self).setMouseCursorGrabbed(grabbed);
        },
        docs[22]
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.Window", "setMouseCursor", "fun(self: sf.Window, cursor: sf.Cursor)");
    lua_glue::BindCallable(type_sf__Window, "setMouseCursor",
        [](sf::Window& self, const sf::Cursor& cursor) {
            static_cast<sf::WindowBase&>(self).setMouseCursor(cursor);
        },
        docs[23]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.Window", "setKeyRepeatEnabled", "fun(self: sf.Window, enabled: boolean)");
    lua_glue::BindCallable(type_sf__Window, "setKeyRepeatEnabled",
        [](sf::Window& self, bool enabled) {
            static_cast<sf::WindowBase&>(self).setKeyRepeatEnabled(enabled);
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.Window", "setJoystickThreshold", "fun(self: sf.Window, threshold: number)");
    lua_glue::BindCallable(type_sf__Window, "setJoystickThreshold",
        [](sf::Window& self, float threshold) {
            static_cast<sf::WindowBase&>(self).setJoystickThreshold(threshold);
        },
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.Window", "requestFocus", "fun(self: sf.Window)");
    lua_glue::BindCallable(type_sf__Window, "requestFocus",
        [](sf::Window& self) {
            static_cast<sf::WindowBase&>(self).requestFocus();
        },
        docs[26]
    );
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FUNCTION("sf.Window", "hasFocus", "fun(self: sf.Window): boolean");
    lua_glue::BindCallable(type_sf__Window, "hasFocus",
        [](const sf::Window& self) -> bool {
            return static_cast<const sf::WindowBase&>(self).hasFocus();
        },
        docs[27]
    );
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FUNCTION("sf.Window", "getNativeHandle", "fun(self: sf.Window): sf.WindowHandle");
    lua_glue::BindCallable(type_sf__Window, "getNativeHandle",
        [](const sf::Window& self) -> lua_sf::WindowHandle {
            return lua_sf::WindowHandle::fromNative(static_cast<const sf::WindowBase&>(self).getNativeHandle());
        },
        docs[28]
    );
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FUNCTION("sf.Window", "getSettings", "fun(self: sf.Window): sf.ContextSettings");
    lua_glue::BindCallable(type_sf__Window, "getSettings",
        [](const sf::Window& self) {
            return std::cref(self.getSettings());
        },
        docs[35],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FUNCTION("sf.Window", "setVerticalSyncEnabled", "fun(self: sf.Window, enabled: boolean)");
    lua_glue::BindCallable(type_sf__Window, "setVerticalSyncEnabled",
        [](sf::Window& self, bool enabled) {
            self.setVerticalSyncEnabled(enabled);
        },
        docs[36]
    );
    LUASF_STUB_DOC(docs[37]);
    LUASF_STUB_FUNCTION("sf.Window", "setFramerateLimit", "fun(self: sf.Window, limit: integer)");
    lua_glue::BindCallable(type_sf__Window, "setFramerateLimit",
        [](sf::Window& self, lua_sf::LuaIntegral<unsigned int> limit) {
            self.setFramerateLimit(limit.value());
        },
        docs[37]
    );
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FUNCTION("sf.Window", "setActive", "fun(self: sf.Window, active?: boolean): boolean");
    lua_glue::BindCallable(type_sf__Window, "setActive",
        [](const sf::Window& self, bool active) -> bool {
            return self.setActive(active);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(true);
        }}},
        docs[38]
    );
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FUNCTION("sf.Window", "display", "fun(self: sf.Window)");
    lua_glue::BindCallable(type_sf__Window, "display",
        [](sf::Window& self) {
            self.display();
        },
        docs[39]
    );
    // Skipped sf::Window::createVulkanSurface(const VkInstance &, VkSurfaceKHR &, const VkAllocationCallbacks *): unsupported parameter type const VkInstance&.
}
