#include "Graphics/bind_RenderWindow.hpp"

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

namespace { constexpr std::array<std::string_view, 65> docs = {
    "\\brief Window that can serve as a target for 2D drawing",
    "\\brief Default constructor\n\nThis constructor doesn't actually create the window,\nuse the other constructors or call `create()` to do so.",
    "\\brief Construct a new window\n\nThis constructor creates the window with the size and pixel\ndepth defined in `mode`. An optional style can be passed to\ncustomize the look and behavior of the window (borders,\ntitle bar, resizable, closable, ...).\n\nThe last parameter is an optional structure specifying\nadvanced OpenGL context settings such as anti-aliasing,\ndepth-buffer bits, etc. You shouldn't care about these\nparameters for a regular usage of the graphics module.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param style    %Window style, a bitwise OR combination of `sf::Style` enumerators\n\\param state    %Window state\n\\param settings Additional settings for the underlying OpenGL context",
    "\\brief Construct a new window\n\nThis constructor creates the window with the size and pixel\ndepth defined in `mode`. If `state` is `State::Fullscreen`,\nthen `mode` must be a valid video mode.\n\nThe last parameter is an optional structure specifying\nadvanced OpenGL context settings such as anti-aliasing,\ndepth-buffer bits, etc.\n\n\\param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)\n\\param title    Title of the window\n\\param state    %Window state\n\\param settings Additional settings for the underlying OpenGL context",
    "\\brief Construct the window from an existing control\n\nUse this constructor if you want to create an SFML\nrendering area into an already existing control.\n\nThe second parameter is an optional structure specifying\nadvanced OpenGL context settings such as anti-aliasing,\ndepth-buffer bits, etc. You shouldn't care about these\nparameters for a regular usage of the graphics module.\n\n\\param handle   Platform-specific handle of the control (\\a HWND on\nWindows, \\a %Window on Linux/FreeBSD, \\a NSWindow on macOS)\n\\param settings Additional settings for the underlying OpenGL context",
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
    "\\brief Clear the entire target with a single color\n\nThis function is usually called once every frame,\nto clear the previous contents of the target.\n\n\\param color Fill color to use to clear the render target",
    "\\brief Clear the stencil buffer to a specific value\n\nThe specified value is truncated to the bit width of\nthe current stencil buffer.\n\n\\param stencilValue Stencil value to clear to",
    "\\brief Clear the entire target with a single color and stencil value\n\nThe specified stencil value is truncated to the bit\nwidth of the current stencil buffer.\n\n\\param color        Fill color to use to clear the render target\n\\param stencilValue Stencil value to clear to",
    "\\brief Change the current active view\n\nThe view is like a 2D camera, it controls which part of\nthe 2D scene is visible, and how it is viewed in the\nrender target.\nThe new view will affect everything that is drawn, until\nanother view is set.\nThe render target keeps its own copy of the view object,\nso it is not necessary to keep the original one alive\nafter calling this function.\nTo restore the original view of the target, you can pass\nthe result of `getDefaultView()` to this function.\n\n\\param view New view to use\n\n\\see `getView`, `getDefaultView`",
    "\\brief Get the view currently in use in the render target\n\n\\return The view object that is currently used\n\n\\see `setView`, `getDefaultView`",
    "\\brief Get the default view of the render target\n\nThe default view has the initial size of the render target,\nand never changes after the target has been created.\n\n\\return The default view of the render target\n\n\\see `setView`, `getView`",
    "\\brief Get the viewport of a view, applied to this render target\n\nThe viewport is defined in the view as a ratio, this function\nsimply applies this ratio to the current dimensions of the\nrender target to calculate the pixels rectangle that the viewport\nactually covers in the target.\n\n\\param view The view for which we want to compute the viewport\n\n\\return Viewport rectangle, expressed in pixels",
    "\\brief Get the scissor rectangle of a view, applied to this render target\n\nThe scissor rectangle is defined in the view as a ratio. This\nfunction simply applies this ratio to the current dimensions\nof the render target to calculate the pixels rectangle\nthat the scissor rectangle actually covers in the target.\n\n\\param view The view for which we want to compute the scissor rectangle\n\n\\return Scissor rectangle, expressed in pixels",
    "\\brief Convert a point from target coordinates to world\ncoordinates, using the current view\n\nThis function is an overload of the mapPixelToCoords\nfunction that implicitly uses the current view.\nIt is equivalent to:\n\\code\ntarget.mapPixelToCoords(point, target.getView());\n\\endcode\n\n\\param point Pixel to convert\n\n\\return The converted point, in \"world\" coordinates\n\n\\see `mapCoordsToPixel`",
    "\\brief Convert a point from target coordinates to world coordinates\n\nThis function finds the 2D position that matches the\ngiven pixel of the render target. In other words, it does\nthe inverse of what the graphics card does, to find the\ninitial position of a rendered pixel.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (10, 50) in your render target may map to the point\n(150, 75) in your 2D world -- if the view is translated by (140, 25).\n\nFor render-windows, this function is typically used to find\nwhich point (or object) is located below the mouse cursor.\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Pixel to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in \"world\" units\n\n\\see `mapCoordsToPixel`",
    "\\brief Convert a point from world coordinates to target\ncoordinates, using the current view\n\nThis function is an overload of the `mapCoordsToPixel`\nfunction that implicitly uses the current view.\nIt is equivalent to:\n\\code\ntarget.mapCoordsToPixel(point, target.getView());\n\\endcode\n\n\\param point Point to convert\n\n\\return The converted point, in target coordinates (pixels)\n\n\\see `mapPixelToCoords`",
    "\\brief Convert a point from world coordinates to target coordinates\n\nThis function finds the pixel of the render target that matches\nthe given 2D point. In other words, it goes through the same process\nas the graphics card, to compute the final position of a rendered point.\n\nInitially, both coordinate systems (world units and target pixels)\nmatch perfectly. But if you define a custom view or resize your\nrender target, this assertion is not `true` anymore, i.e. a point\nlocated at (150, 75) in your 2D world may map to the pixel\n(10, 50) of your render target -- if the view is translated by (140, 25).\n\nThis version uses a custom view for calculations, see the other\noverload of the function if you want to use the current view of the\nrender target.\n\n\\param point Point to convert\n\\param view The view to use for converting the point\n\n\\return The converted point, in target coordinates (pixels)\n\n\\see `mapPixelToCoords`",
    "\\brief Draw a drawable object to the render target\n\n\\param drawable Object to draw\n\\param states   Render states to use for drawing",
    "\\brief Draw primitives defined by an array of vertices\n\n\\param vertices    Pointer to the vertices\n\\param vertexCount Number of vertices in the array\n\\param type        Type of primitives to draw\n\\param states      Render states to use for drawing",
    "\\brief Draw primitives defined by a vertex buffer\n\n\\param vertexBuffer Vertex buffer\n\\param states       Render states to use for drawing",
    "\\brief Draw primitives defined by a vertex buffer\n\n\\param vertexBuffer Vertex buffer\n\\param firstVertex  Index of the first vertex to render\n\\param vertexCount  Number of vertices to render\n\\param states       Render states to use for drawing",
    "\\brief Return the size of the rendering region of the target\n\n\\return Size in pixels",
    "\\brief Tell if the render target will use sRGB encoding when drawing on it\n\n\\return `true` if the render target use sRGB encoding, `false` otherwise",
    "\\brief Activate or deactivate the render target for rendering\n\nThis function makes the render target's context current for\nfuture OpenGL rendering operations (so you shouldn't care\nabout it if you're not doing direct OpenGL stuff).\nA render target's context is active only on the current thread,\nif you want to make it active on another thread you have\nto deactivate it on the previous thread first if it was active.\nOnly one context can be current in a thread, so if you\nwant to draw OpenGL geometry to another render target\ndon't forget to activate it again. Activating a render\ntarget will automatically deactivate the previously active\ncontext (if any).\n\n\\param active `true` to activate, `false` to deactivate\n\n\\return `true` if operation was successful, `false` otherwise",
    "\\brief Save the OpenGL render states modified by SFML\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering. Combined with popGLStates,\nit ensures that:\n\\li SFML's internal states are not messed up by your OpenGL code\n\\li your OpenGL states are not modified by a call to a SFML function\n\nMore specifically, it must be used around code that\ncalls `draw` functions. Example:\n\\code\n// OpenGL code here...\nwindow.pushGLStates();\nwindow.draw(...);\nwindow.draw(...);\nwindow.popGLStates();\n// OpenGL code here...\n\\endcode\n\nNote that this function is quite expensive: it saves the\nprogram, textures, vertex attributes and the other OpenGL\nstates that SFML drawing can modify. State outside this set\nis deliberately not covered.\nIt is provided for convenience, but the best results will\nbe achieved if you handle OpenGL states yourself (because\nyou know which states have really changed, and need to be\nsaved and restored). Take a look at the resetGLStates\nfunction if you do so.\n\n\\see `popGLStates`",
    "\\brief Restore the previously saved OpenGL render states\n\nSee the description of `pushGLStates` to get a detailed\ndescription of these functions.\n\n\\see `pushGLStates`",
    "\\brief Reset the internal OpenGL states so that the target is ready for drawing\n\nThis function can be used when you mix SFML drawing\nand direct OpenGL rendering, if you choose not to use\n`pushGLStates`/`popGLStates`. It makes sure that all OpenGL\nstates needed by SFML are set, so that subsequent `draw()`\ncalls will work as expected.\n\nExample:\n\\code\n// OpenGL code here...\nwindow.resetGLStates();\nwindow.draw(...);\nwindow.draw(...);\n// OpenGL code here...\n\\endcode",
    "\\brief Get the size of the rendering region of the window\n\nThe size doesn't include the titlebar and borders\nof the window.\n\n\\return Size in pixels",
    "\\brief Change the window's icon\n\nThe OS default icon is used by default.\n\n\\param icon Image to use as the icon. The image is copied,\nso you need not keep the source alive after\ncalling this function.",
    "\\brief Tell if the window will use sRGB encoding when drawing on it\n\nYou can request sRGB encoding for a window by having the sRgbCapable flag set in the `ContextSettings`\n\n\\return `true` if the window use sRGB encoding, `false` otherwise",
}; }

void bind_RenderWindow(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__RenderWindow = lua_glue::BindClass<sf::RenderWindow>(sf, "RenderWindow");
    lua_glue::BindBase<sf::RenderWindow, sf::Window>(type_sf__RenderWindow);
    lua_glue::BindBase<sf::RenderWindow, sf::WindowBase>(type_sf__RenderWindow);
    lua_glue::BindBase<sf::RenderWindow, sf::RenderTarget>(type_sf__RenderWindow);
    lua_glue::Table table_sf__RenderWindow = sf["RenderWindow"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::RenderWindow>(lua);
    lua_glue::Table native_bases_sf__RenderWindow = lua.create_table();
    native_bases_sf__RenderWindow.add(lua["sf"]["Window"].get<lua_glue::Table>());
    native_bases_sf__RenderWindow.add(lua["sf"]["RenderTarget"].get<lua_glue::Table>());
    table_sf__RenderWindow.raw_set("__nativeBases", native_bases_sf__RenderWindow);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.RenderWindow", "sf.Window, sf.WindowBase, sf.RenderTarget");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "new", "fun(mode: sf.VideoMode, title: string, style?: integer, state?: sf.State, settings?: sf.ContextSettings): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(mode: sf.VideoMode, title: string, state: sf.State, settings?: sf.ContextSettings): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(handle: sf.WindowHandle, settings?: sf.ContextSettings): sf.RenderWindow");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "new", "fun(): sf.RenderWindow");
    lua_glue::BindCallable(type_sf__RenderWindow, "new",
        [](sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(mode, lua_sf::to_sf_string(title), style.value(), state, settings);
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
    lua_glue::BindCallable(type_sf__RenderWindow, "new",
        [](sf::VideoMode mode, std::string title, sf::State state, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(mode, lua_sf::to_sf_string(title), state, settings);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::ContextSettings{ };
        }}},
        docs[3]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "new",
        [](const lua_sf::WindowHandle& handle, const sf::ContextSettings& settings) {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>(handle.getHandle(), settings);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::ContextSettings{ };
        }}},
        docs[4]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::RenderWindow>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, style: integer, state: sf.State, settings: sf.ContextSettings)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, style?: integer, state?: sf.State)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, state: sf.State, settings: sf.ContextSettings)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, state: sf.State)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, handle: sf.WindowHandle, settings: sf.ContextSettings)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "create", "fun(self: sf.RenderWindow, handle: sf.WindowHandle)");
    lua_glue::BindCallable(type_sf__RenderWindow, "create",
        [](sf::RenderWindow& self, sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state, const sf::ContextSettings& settings) {
            static_cast<sf::Window&>(self).create(mode, lua_sf::to_sf_string(title), style.value(), state, settings);
        },
        docs[30]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "create",
        [](sf::RenderWindow& self, sf::VideoMode mode, std::string title, lua_sf::LuaIntegral<std::uint32_t> style, sf::State state) {
            static_cast<sf::WindowBase&>(self).create(mode, lua_sf::to_sf_string(title), style.value(), state);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned int>(Style::Default);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::State>(sf::State::Windowed);
        }}},
        docs[5]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "create",
        [](sf::RenderWindow& self, sf::VideoMode mode, std::string title, sf::State state, const sf::ContextSettings& settings) {
            static_cast<sf::Window&>(self).create(mode, lua_sf::to_sf_string(title), state, settings);
        },
        docs[32]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "create",
        [](sf::RenderWindow& self, sf::VideoMode mode, std::string title, sf::State state) {
            static_cast<sf::WindowBase&>(self).create(mode, lua_sf::to_sf_string(title), state);
        },
        docs[6]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "create",
        [](sf::RenderWindow& self, const lua_sf::WindowHandle& handle, const sf::ContextSettings& settings) {
            static_cast<sf::Window&>(self).create(handle.getHandle(), settings);
        },
        docs[34]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "create",
        [](sf::RenderWindow& self, const lua_sf::WindowHandle& handle) {
            static_cast<sf::WindowBase&>(self).create(handle.getHandle());
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "close", "fun(self: sf.RenderWindow)");
    lua_glue::BindCallable(type_sf__RenderWindow, "close",
        [](sf::RenderWindow& self) {
            static_cast<sf::WindowBase&>(self).close();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "isOpen", "fun(self: sf.RenderWindow): boolean");
    lua_glue::BindCallable(type_sf__RenderWindow, "isOpen",
        [](const sf::RenderWindow& self) -> bool {
            return static_cast<const sf::WindowBase&>(self).isOpen();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "pollEvent", "fun(self: sf.RenderWindow): sf.Event|nil");
    lua_glue::BindCallable(type_sf__RenderWindow, "pollEvent",
        [lua](sf::RenderWindow& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, static_cast<sf::WindowBase&>(self).pollEvent());
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "waitEvent", "fun(self: sf.RenderWindow, timeout?: sf.Time): sf.Event|nil");
    lua_glue::BindCallable(type_sf__RenderWindow, "waitEvent",
        [lua](sf::RenderWindow& self, sf::Time timeout) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, static_cast<sf::WindowBase&>(self).waitEvent(timeout));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Time>(sf::Time::Zero);
        }}},
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getPosition", "fun(self: sf.RenderWindow): sf.Vector2i");
    lua_glue::BindCallable(type_sf__RenderWindow, "getPosition",
        [](const sf::RenderWindow& self) -> sf::Vector2i {
            return static_cast<const sf::WindowBase&>(self).getPosition();
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setPosition", "fun(self: sf.RenderWindow, position: sf.Vector2i)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setPosition",
        [](sf::RenderWindow& self, sf::Vector2i position) {
            static_cast<sf::WindowBase&>(self).setPosition(position);
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[62]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getSize", "fun(self: sf.RenderWindow): sf.Vector2u");
    lua_glue::BindCallable(type_sf__RenderWindow, "getSize",
        [](const sf::RenderWindow& self) -> sf::Vector2u {
            return self.getSize();
        },
        docs[62]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setSize", "fun(self: sf.RenderWindow, size: sf.Vector2u)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setSize",
        [](sf::RenderWindow& self, sf::Vector2u size) {
            static_cast<sf::WindowBase&>(self).setSize(size);
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMinimumSize", "fun(self: sf.RenderWindow, minimumSize: sf.Vector2u|nil)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setMinimumSize",
        [](sf::RenderWindow& self, lua_glue::Object minimumSize) {
            auto minimumSize_optional = lua_sf::optional_from_object<sf::Vector2u>(minimumSize);
            static_cast<sf::WindowBase&>(self).setMinimumSize(minimumSize_optional);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMaximumSize", "fun(self: sf.RenderWindow, maximumSize: sf.Vector2u|nil)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setMaximumSize",
        [](sf::RenderWindow& self, lua_glue::Object maximumSize) {
            auto maximumSize_optional = lua_sf::optional_from_object<sf::Vector2u>(maximumSize);
            static_cast<sf::WindowBase&>(self).setMaximumSize(maximumSize_optional);
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setTitle", "fun(self: sf.RenderWindow, title: string)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setTitle",
        [](sf::RenderWindow& self, std::string title) {
            static_cast<sf::WindowBase&>(self).setTitle(lua_sf::to_sf_string(title));
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[63]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setIcon", "fun(self: sf.RenderWindow, icon: sf.Image)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "setIcon", "fun(self: sf.RenderWindow, size: sf.Vector2u, pixels: any)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setIcon",
        [](sf::RenderWindow& self, const sf::Image& icon) {
            self.setIcon(icon);
        },
        docs[63]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "setIcon",
        [](sf::RenderWindow& self, sf::Vector2u size, lua_glue::Object pixels) {
            auto pixels_buffer = lua_sf::array_from_object<std::uint8_t>(pixels);
            static_cast<sf::WindowBase&>(self).setIcon(size, pixels_buffer.data());
        },
        docs[19]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setVisible", "fun(self: sf.RenderWindow, visible: boolean)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setVisible",
        [](sf::RenderWindow& self, bool visible) {
            static_cast<sf::WindowBase&>(self).setVisible(visible);
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMouseCursorVisible", "fun(self: sf.RenderWindow, visible: boolean)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setMouseCursorVisible",
        [](sf::RenderWindow& self, bool visible) {
            static_cast<sf::WindowBase&>(self).setMouseCursorVisible(visible);
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMouseCursorGrabbed", "fun(self: sf.RenderWindow, grabbed: boolean)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setMouseCursorGrabbed",
        [](sf::RenderWindow& self, bool grabbed) {
            static_cast<sf::WindowBase&>(self).setMouseCursorGrabbed(grabbed);
        },
        docs[22]
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setMouseCursor", "fun(self: sf.RenderWindow, cursor: sf.Cursor)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setMouseCursor",
        [](sf::RenderWindow& self, const sf::Cursor& cursor) {
            static_cast<sf::WindowBase&>(self).setMouseCursor(cursor);
        },
        docs[23]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setKeyRepeatEnabled", "fun(self: sf.RenderWindow, enabled: boolean)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setKeyRepeatEnabled",
        [](sf::RenderWindow& self, bool enabled) {
            static_cast<sf::WindowBase&>(self).setKeyRepeatEnabled(enabled);
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setJoystickThreshold", "fun(self: sf.RenderWindow, threshold: number)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setJoystickThreshold",
        [](sf::RenderWindow& self, float threshold) {
            static_cast<sf::WindowBase&>(self).setJoystickThreshold(threshold);
        },
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "requestFocus", "fun(self: sf.RenderWindow)");
    lua_glue::BindCallable(type_sf__RenderWindow, "requestFocus",
        [](sf::RenderWindow& self) {
            static_cast<sf::WindowBase&>(self).requestFocus();
        },
        docs[26]
    );
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "hasFocus", "fun(self: sf.RenderWindow): boolean");
    lua_glue::BindCallable(type_sf__RenderWindow, "hasFocus",
        [](const sf::RenderWindow& self) -> bool {
            return static_cast<const sf::WindowBase&>(self).hasFocus();
        },
        docs[27]
    );
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getNativeHandle", "fun(self: sf.RenderWindow): sf.WindowHandle");
    lua_glue::BindCallable(type_sf__RenderWindow, "getNativeHandle",
        [](const sf::RenderWindow& self) -> lua_sf::WindowHandle {
            return lua_sf::WindowHandle::fromNative(static_cast<const sf::WindowBase&>(self).getNativeHandle());
        },
        docs[28]
    );
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getSettings", "fun(self: sf.RenderWindow): sf.ContextSettings");
    lua_glue::BindCallable(type_sf__RenderWindow, "getSettings",
        [](const sf::RenderWindow& self) {
            return std::cref(static_cast<const sf::Window&>(self).getSettings());
        },
        docs[35],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setVerticalSyncEnabled", "fun(self: sf.RenderWindow, enabled: boolean)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setVerticalSyncEnabled",
        [](sf::RenderWindow& self, bool enabled) {
            static_cast<sf::Window&>(self).setVerticalSyncEnabled(enabled);
        },
        docs[36]
    );
    LUASF_STUB_DOC(docs[37]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setFramerateLimit", "fun(self: sf.RenderWindow, limit: integer)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setFramerateLimit",
        [](sf::RenderWindow& self, lua_sf::LuaIntegral<unsigned int> limit) {
            static_cast<sf::Window&>(self).setFramerateLimit(limit.value());
        },
        docs[37]
    );
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setActive", "fun(self: sf.RenderWindow, active?: boolean): boolean");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "setActive", "fun(self: sf.RenderWindow, active?: boolean): boolean");
    lua_glue::BindCallable(type_sf__RenderWindow, "setActive",
        [](const sf::RenderWindow& self, bool active) -> bool {
            return static_cast<const sf::Window&>(self).setActive(active);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(true);
        }}},
        docs[38]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "setActive",
        [](sf::RenderWindow& self, bool active) -> bool {
            return self.setActive(active);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(true);
        }}},
        docs[38]
    );
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "display", "fun(self: sf.RenderWindow)");
    lua_glue::BindCallable(type_sf__RenderWindow, "display",
        [](sf::RenderWindow& self) {
            static_cast<sf::Window&>(self).display();
        },
        docs[39]
    );
    LUASF_STUB_DOC(docs[42]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "clear", "fun(self: sf.RenderWindow, color: sf.Color, stencilValue: sf.StencilValue)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "clear", "fun(self: sf.RenderWindow, color?: sf.Color)");
    lua_glue::BindCallable(type_sf__RenderWindow, "clear",
        [](sf::RenderWindow& self, sf::Color color, sf::StencilValue stencilValue) {
            static_cast<sf::RenderTarget&>(self).clear(color, stencilValue);
        },
        docs[42]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "clear",
        [](sf::RenderWindow& self, sf::Color color) {
            static_cast<sf::RenderTarget&>(self).clear(color);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Color>(sf::Color::Black);
        }}},
        docs[40]
    );
    LUASF_STUB_DOC(docs[41]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "clearStencil", "fun(self: sf.RenderWindow, stencilValue: sf.StencilValue)");
    lua_glue::BindCallable(type_sf__RenderWindow, "clearStencil",
        [](sf::RenderWindow& self, sf::StencilValue stencilValue) {
            static_cast<sf::RenderTarget&>(self).clearStencil(stencilValue);
        },
        docs[41]
    );
    LUASF_STUB_DOC(docs[43]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "setView", "fun(self: sf.RenderWindow, view: sf.View)");
    lua_glue::BindCallable(type_sf__RenderWindow, "setView",
        [](sf::RenderWindow& self, const sf::View& view) {
            static_cast<sf::RenderTarget&>(self).setView(view);
        },
        docs[43]
    );
    LUASF_STUB_DOC(docs[44]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getView", "fun(self: sf.RenderWindow): sf.View");
    lua_glue::BindCallable(type_sf__RenderWindow, "getView",
        [](const sf::RenderWindow& self) {
            return std::cref(static_cast<const sf::RenderTarget&>(self).getView());
        },
        docs[44],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[45]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getDefaultView", "fun(self: sf.RenderWindow): sf.View");
    lua_glue::BindCallable(type_sf__RenderWindow, "getDefaultView",
        [](const sf::RenderWindow& self) {
            return std::cref(static_cast<const sf::RenderTarget&>(self).getDefaultView());
        },
        docs[45],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[46]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getViewport", "fun(self: sf.RenderWindow, view: sf.View): sf.IntRect");
    lua_glue::BindCallable(type_sf__RenderWindow, "getViewport",
        [](const sf::RenderWindow& self, const sf::View& view) -> sf::IntRect {
            return static_cast<const sf::RenderTarget&>(self).getViewport(view);
        },
        docs[46]
    );
    LUASF_STUB_DOC(docs[47]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "getScissor", "fun(self: sf.RenderWindow, view: sf.View): sf.IntRect");
    lua_glue::BindCallable(type_sf__RenderWindow, "getScissor",
        [](const sf::RenderWindow& self, const sf::View& view) -> sf::IntRect {
            return static_cast<const sf::RenderTarget&>(self).getScissor(view);
        },
        docs[47]
    );
    LUASF_STUB_DOC(docs[49]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "mapPixelToCoords", "fun(self: sf.RenderWindow, point: sf.Vector2i, view: sf.View): sf.Vector2f");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "mapPixelToCoords", "fun(self: sf.RenderWindow, point: sf.Vector2i): sf.Vector2f");
    lua_glue::BindCallable(type_sf__RenderWindow, "mapPixelToCoords",
        [](const sf::RenderWindow& self, sf::Vector2i point, const sf::View& view) -> sf::Vector2f {
            return static_cast<const sf::RenderTarget&>(self).mapPixelToCoords(point, view);
        },
        docs[49]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "mapPixelToCoords",
        [](const sf::RenderWindow& self, sf::Vector2i point) -> sf::Vector2f {
            return static_cast<const sf::RenderTarget&>(self).mapPixelToCoords(point);
        },
        docs[48]
    );
    LUASF_STUB_DOC(docs[51]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "mapCoordsToPixel", "fun(self: sf.RenderWindow, point: sf.Vector2f, view: sf.View): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "mapCoordsToPixel", "fun(self: sf.RenderWindow, point: sf.Vector2f): sf.Vector2i");
    lua_glue::BindCallable(type_sf__RenderWindow, "mapCoordsToPixel",
        [](const sf::RenderWindow& self, sf::Vector2f point, const sf::View& view) -> sf::Vector2i {
            return static_cast<const sf::RenderTarget&>(self).mapCoordsToPixel(point, view);
        },
        docs[51]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "mapCoordsToPixel",
        [](const sf::RenderWindow& self, sf::Vector2f point) -> sf::Vector2i {
            return static_cast<const sf::RenderTarget&>(self).mapCoordsToPixel(point);
        },
        docs[50]
    );
    LUASF_STUB_DOC(docs[55]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer, states?: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, drawable: sf.Drawable, states?: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, vertexBuffer: sf.VertexBuffer, states?: sf.RenderStates)");
    LUASF_STUB_OVERLOAD("sf.RenderWindow", "draw", "fun(self: sf.RenderWindow, vertices: any, type: sf.PrimitiveType, states?: sf.RenderStates)");
    lua_glue::BindCallable(type_sf__RenderWindow, "draw",
        [](sf::RenderWindow& self, const sf::VertexBuffer& vertexBuffer, lua_sf::LuaIntegral<std::size_t> firstVertex, lua_sf::LuaIntegral<std::size_t> vertexCount, const sf::RenderStates& states) {
            static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, firstVertex.value(), vertexCount.value(), states);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::RenderStates>(sf::RenderStates::Default);
        }}},
        docs[55]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "draw",
        [](sf::RenderWindow& self, const sf::Drawable& drawable, const sf::RenderStates& states) {
            static_cast<sf::RenderTarget&>(self).draw(drawable, states);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::RenderStates>(sf::RenderStates::Default);
        }}},
        docs[52]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "draw",
        [](sf::RenderWindow& self, const sf::VertexBuffer& vertexBuffer, const sf::RenderStates& states) {
            static_cast<sf::RenderTarget&>(self).draw(vertexBuffer, states);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::RenderStates>(sf::RenderStates::Default);
        }}},
        docs[54]
    );
    lua_glue::BindCallable(type_sf__RenderWindow, "draw",
        [](sf::RenderWindow& self, lua_glue::Object vertices, sf::PrimitiveType type, const sf::RenderStates& states) {
            auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
            static_cast<sf::RenderTarget&>(self).draw(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), type, states);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::RenderStates>(sf::RenderStates::Default);
        }}},
        docs[53]
    );
    LUASF_STUB_DOC(docs[64]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "isSrgb", "fun(self: sf.RenderWindow): boolean");
    lua_glue::BindCallable(type_sf__RenderWindow, "isSrgb",
        [](const sf::RenderWindow& self) -> bool {
            return self.isSrgb();
        },
        docs[64]
    );
    LUASF_STUB_DOC(docs[59]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "pushGLStates", "fun(self: sf.RenderWindow)");
    lua_glue::BindCallable(type_sf__RenderWindow, "pushGLStates",
        [](sf::RenderWindow& self) {
            static_cast<sf::RenderTarget&>(self).pushGLStates();
        },
        docs[59]
    );
    LUASF_STUB_DOC(docs[60]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "popGLStates", "fun(self: sf.RenderWindow)");
    lua_glue::BindCallable(type_sf__RenderWindow, "popGLStates",
        [](sf::RenderWindow& self) {
            static_cast<sf::RenderTarget&>(self).popGLStates();
        },
        docs[60]
    );
    LUASF_STUB_DOC(docs[61]);
    LUASF_STUB_FUNCTION("sf.RenderWindow", "resetGLStates", "fun(self: sf.RenderWindow)");
    lua_glue::BindCallable(type_sf__RenderWindow, "resetGLStates",
        [](sf::RenderWindow& self) {
            static_cast<sf::RenderTarget&>(self).resetGLStates();
        },
        docs[61]
    );
    // Skipped sf::RenderWindow::createVulkanSurface(const VkInstance &, VkSurfaceKHR &, const VkAllocationCallbacks *): unsupported parameter type const VkInstance&.
}
