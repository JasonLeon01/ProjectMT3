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

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/VideoMode.hpp>

#include <algorithm>
#include <functional>
#include <list>
#include <mutex>
#include <utility>
#include <vector>


namespace sf::priv::Harmony
{
////////////////////////////////////////////////////////////
/// Retrieve an immutable snapshot of the current fullscreen modes.
///
/// Harmony's surface dimensions are lifecycle state rather than a fixed
/// monitor capability list. Immutable snapshots preserve the lifetime and
/// contents of every reference previously returned by the process.
////////////////////////////////////////////////////////////
[[nodiscard]] inline const std::vector<VideoMode>& refreshFullscreenModes(std::vector<VideoMode> modes)
{
    std::sort(modes.begin(), modes.end(), std::greater<>());

    // Equal lifecycle states reuse one snapshot, so repeated frame queries and
    // ordinary portrait/landscape transitions do not continually grow the
    // cache. A distinct snapshot cannot be reclaimed safely because the public
    // API returns a const reference with no way to observe when it is released.
    struct Cache
    {
        std::mutex                        mutex;
        std::list<std::vector<VideoMode>> snapshots;
    };

    static Cache          cache;
    const std::lock_guard lock(cache.mutex);
    if (const auto snapshot = std::find(cache.snapshots.cbegin(), cache.snapshots.cend(), modes);
        snapshot != cache.snapshots.cend())
        return *snapshot;

    cache.snapshots.push_back(std::move(modes));
    return cache.snapshots.back();
}

} // namespace sf::priv::Harmony
