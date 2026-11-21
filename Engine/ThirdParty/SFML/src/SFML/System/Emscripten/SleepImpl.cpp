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

#include <SFML/System/Emscripten/Browser.hpp>

#include <algorithm>
#include <emscripten.h>
#include <limits>

// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_ASYNC_JS(void, sfmlWaitForBrowserFrame, (), {
    await new Promise((resolve) => {
        if (document.hidden)
        {
            setTimeout(resolve, 16);
            return;
        }

        let frame = null;
        let settled = false;
        const finish = () => {
            if (settled)
                return;
            settled = true;
            document.removeEventListener('visibilitychange', onVisibilityChange);
            if (frame !== null)
                cancelAnimationFrame(frame);
            resolve();
        };
        const onVisibilityChange = () => {
            if (document.hidden)
                finish();
        };

        document.addEventListener('visibilitychange', onVisibilityChange);
        frame = requestAnimationFrame(finish);
        onVisibilityChange();
    });
});
// clang-format on

namespace sf::priv
{
void sleepImpl(Time duration)
{
    const auto microseconds = std::max<std::int64_t>(duration.asMicroseconds(), 0);
    const auto milliseconds = microseconds / 1000 + (microseconds % 1000 != 0 ? 1 : 0);
    emscripten_sleep(
        static_cast<unsigned int>(std::clamp<std::int64_t>(milliseconds, 0, std::numeric_limits<unsigned int>::max())));
}
void waitForBrowserFrame()
{
    sfmlWaitForBrowserFrame();
}
} // namespace sf::priv
