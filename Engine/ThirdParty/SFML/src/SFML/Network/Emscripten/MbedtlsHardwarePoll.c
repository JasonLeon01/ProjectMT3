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

#include <emscripten.h>
#include <limits.h>
#include <mbedtls/entropy.h>
#include <stddef.h>

// JavaScript bodies must not be formatted as C++.
// clang-format off
EM_JS(int, sfmlFillHardwareRandom, (unsigned char* output, int length), {
    try
    {
        const cryptoObject = globalThis.crypto;
        if (!cryptoObject || typeof cryptoObject.getRandomValues !== 'function')
            return 0;
        const bytes = HEAPU8.subarray(output, output + length);
        const chunk = 65536;
        for (let offset = 0; offset < length; offset += chunk)
            cryptoObject.getRandomValues(bytes.subarray(offset, Math.min(length, offset + chunk)));
        return 1;
    }
    catch (error)
    {
        return 0;
    }
});
// clang-format on

int mbedtls_hardware_poll(void* data, unsigned char* output, size_t len, size_t* olen)
{
    (void)data;

    if (!olen)
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;

    *olen = 0;
    if (len == 0)
        return 0;

    if (!output || len > INT_MAX || !sfmlFillHardwareRandom(output, (int)len))
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;

    *olen = len;
    return 0;
}
