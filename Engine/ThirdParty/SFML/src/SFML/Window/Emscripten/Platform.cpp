////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2026 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#include <SFML/Window/ClipboardImpl.hpp>
#include <SFML/Window/CursorImpl.hpp>
#include <SFML/Window/SensorImpl.hpp>
#include <SFML/Window/VideoModeImpl.hpp>

#include <SFML/System/String.hpp>

#include <emscripten.h>
#include <memory>

#include <cstdlib>

EM_JS_DEPS(sfmlClipboardDependencies, "$stringToNewUTF8,$UTF8ToString");

// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_ASYNC_JS(char*, sfmlReadClipboard, (), {
    try
    {
        return stringToNewUTF8(await navigator.clipboard.readText());
    } catch (error)
    {
        return 0;
    }
});
// clang-format on
// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_ASYNC_JS(void, sfmlWriteClipboard, (const char* text), {
    try
    {
        await navigator.clipboard.writeText(UTF8ToString(text));
    } catch (error)
    {
        console.warn('SFML clipboard access denied');
    }
});
// clang-format on

EM_JS(int, sfmlScreenWidth, (), { return Math.round(screen.width * devicePixelRatio); });
EM_JS(int, sfmlScreenHeight, (), { return Math.round(screen.height * devicePixelRatio); });

namespace sf::priv
{
void SensorImpl::initialize()
{
}
void SensorImpl::cleanup()
{
}
bool SensorImpl::isAvailable(Sensor::Type)
{
    return false;
}
bool SensorImpl::open(Sensor::Type)
{
    return false;
}
void SensorImpl::close()
{
}
Vector3f SensorImpl::update()
{
    return {};
}
void SensorImpl::setEnabled(bool)
{
}
String ClipboardImpl::getString()
{
    const std::unique_ptr<char, decltype(&std::free)> value(sfmlReadClipboard(), std::free);
    const std::string                                 text = value ? value.get() : "";
    return String::fromUtf8(text.begin(), text.end());
}
void ClipboardImpl::setString(const String& text)
{
    const auto utf8 = text.toUtf8();
    sfmlWriteClipboard(reinterpret_cast<const char*>(utf8.c_str()));
}
std::vector<VideoMode> VideoModeImpl::getFullscreenModes()
{
    return {getDesktopMode()};
}
VideoMode VideoModeImpl::getDesktopMode()
{
    const int width  = sfmlScreenWidth();
    const int height = sfmlScreenHeight();
    return VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}, 32);
}
bool CursorImpl::loadFromPixels(const std::uint8_t*, Vector2u, Vector2u)
{
    return false;
}
bool CursorImpl::loadFromSystem(Cursor::Type type)
{
    switch (type)
    {
        case Cursor::Type::Arrow:
            css = "default";
            break;
        case Cursor::Type::ArrowWait:
            css = "progress";
            break;
        case Cursor::Type::Wait:
            css = "wait";
            break;
        case Cursor::Type::Text:
            css = "text";
            break;
        case Cursor::Type::Hand:
            css = "pointer";
            break;
        case Cursor::Type::SizeHorizontal:
            css = "ew-resize";
            break;
        case Cursor::Type::SizeVertical:
            css = "ns-resize";
            break;
        case Cursor::Type::SizeTopLeftBottomRight:
            css = "nwse-resize";
            break;
        case Cursor::Type::SizeBottomLeftTopRight:
            css = "nesw-resize";
            break;
        case Cursor::Type::SizeLeft:
            css = "w-resize";
            break;
        case Cursor::Type::SizeRight:
            css = "e-resize";
            break;
        case Cursor::Type::SizeTop:
            css = "n-resize";
            break;
        case Cursor::Type::SizeBottom:
            css = "s-resize";
            break;
        case Cursor::Type::SizeTopLeft:
            css = "nw-resize";
            break;
        case Cursor::Type::SizeTopRight:
            css = "ne-resize";
            break;
        case Cursor::Type::SizeBottomLeft:
            css = "sw-resize";
            break;
        case Cursor::Type::SizeBottomRight:
            css = "se-resize";
            break;
        case Cursor::Type::SizeAll:
            css = "move";
            break;
        case Cursor::Type::Cross:
            css = "crosshair";
            break;
        case Cursor::Type::Help:
            css = "help";
            break;
        case Cursor::Type::NotAllowed:
            css = "not-allowed";
            break;
        default:
            return false;
    }
    return true;
}
} // namespace sf::priv
