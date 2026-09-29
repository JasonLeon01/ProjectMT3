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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/System/Harmony/ResourceManagerImpl.hpp>
#include <SFML/System/Harmony/ResourceStream.hpp>

#include <algorithm>
#include <limits>
#include <rawfile/raw_file.h>
#include <rawfile/raw_file_manager.h>
#include <string>

#include <cassert>
#include <cstdint>
#include <cstdio>


namespace
{
std::string normalizeRawFilePath(const std::filesystem::path& filename)
{
    if (filename.empty() || filename.is_absolute())
        return {};

    const auto            normalized = filename.lexically_normal();
    std::filesystem::path safePath;

    for (const auto& component : normalized)
    {
        if (component == "..")
            return {};

        if (component != ".")
            safePath /= component;
    }

    return safePath.generic_string();
}
} // namespace


namespace sf::priv
{
////////////////////////////////////////////////////////////
HarmonyResourceStream::~HarmonyResourceStream()
{
    close();
}


////////////////////////////////////////////////////////////
void HarmonyResourceStream::close()
{
    if (m_file)
    {
        OH_ResourceManager_CloseRawFile64(m_file);
        m_file = nullptr;
    }

    // Closing a rawfile can still access its resource manager. Release the
    // manager lease only after the file handle is closed.
    m_resourceManager.reset();
}


////////////////////////////////////////////////////////////
bool HarmonyResourceStream::open(const std::filesystem::path& filename)
{
    close();

    const auto resourcePath = normalizeRawFilePath(filename);
    if (resourcePath.empty())
        return false;

    auto        resourceManagerLease = Harmony::acquireResourceManagerLease();
    const auto* resourceManager      = static_cast<const NativeResourceManager*>(
        Harmony::getResourceManager(resourceManagerLease));
    if (!resourceManager)
        return false;

    m_file = OH_ResourceManager_OpenRawFile64(resourceManager, resourcePath.c_str());
    if (m_file)
        m_resourceManager = std::move(resourceManagerLease);
    return m_file != nullptr;
}


////////////////////////////////////////////////////////////
std::optional<std::size_t> HarmonyResourceStream::read(void* data, std::size_t size)
{
    assert(m_file && "HarmonyResourceStream::read() cannot be called when the rawfile is not initialized");

    const auto readSize = std::min(size, static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max()));
    const auto result   = OH_ResourceManager_ReadRawFile64(m_file, data, static_cast<std::int64_t>(readSize));

    if (result < 0)
        return std::nullopt;

    return static_cast<std::size_t>(result);
}


////////////////////////////////////////////////////////////
std::optional<std::size_t> HarmonyResourceStream::seek(std::size_t position)
{
    assert(m_file && "HarmonyResourceStream::seek() cannot be called when the rawfile is not initialized");

    if (position > static_cast<std::size_t>(std::numeric_limits<std::int64_t>::max()) ||
        OH_ResourceManager_SeekRawFile64(m_file, static_cast<std::int64_t>(position), SEEK_SET) != 0)
    {
        return std::nullopt;
    }

    return tell();
}


////////////////////////////////////////////////////////////
std::optional<std::size_t> HarmonyResourceStream::tell()
{
    assert(m_file && "HarmonyResourceStream::tell() cannot be called when the rawfile is not initialized");

    const auto result = OH_ResourceManager_GetRawFileOffset64(m_file);
    if (result < 0)
        return std::nullopt;

    return static_cast<std::size_t>(result);
}


////////////////////////////////////////////////////////////
std::optional<std::size_t> HarmonyResourceStream::getSize()
{
    assert(m_file && "HarmonyResourceStream::getSize() cannot be called when the rawfile is not initialized");

    const auto result = OH_ResourceManager_GetRawFileSize64(m_file);
    if (result < 0)
        return std::nullopt;

    return static_cast<std::size_t>(result);
}

} // namespace sf::priv
