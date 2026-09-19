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

#include <SFML/Window/Harmony/CursorImpl.hpp>

#include <limits>


namespace sf::priv
{
bool CursorImpl::loadFromPixels(const std::uint8_t* pixels, Vector2u size, Vector2u hotspot)
{
    if (!pixels || !size.x || !size.y || hotspot.x >= size.x || hotspot.y >= size.y ||
        size.x > std::numeric_limits<std::size_t>::max() / 4u / size.y)
        return false;

    m_pixels.assign(pixels, pixels + static_cast<std::size_t>(size.x) * size.y * 4u);
    m_size    = size;
    m_hotspot = hotspot;
    m_systemType.reset();
    return true;
}


bool CursorImpl::loadFromSystem(Cursor::Type type)
{
    m_pixels.clear();
    m_size       = {};
    m_hotspot    = {};
    m_systemType = type;
    return true;
}


const std::vector<std::uint8_t>& CursorImpl::getPixels() const
{
    return m_pixels;
}


Vector2u CursorImpl::getSize() const
{
    return m_size;
}


Vector2u CursorImpl::getHotspot() const
{
    return m_hotspot;
}


std::optional<Cursor::Type> CursorImpl::getSystemType() const
{
    return m_systemType;
}

} // namespace sf::priv
