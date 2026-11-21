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
#include <SFML/Network/SocketHandle.hpp>

#include <optional>
#include <string>
#include <vector>

#include <cstddef>
#include <cstdint>


namespace sf::priv
{
////////////////////////////////////////////////////////////
/// \brief Log a web limitation once, then let the caller return the existing failure
///
////////////////////////////////////////////////////////////
void webSkip(const char* message);

[[nodiscard]] SocketHandle webCreateTcp();
[[nodiscard]] SocketHandle webCreateUdp();
void                       webClose(SocketHandle handle);

// Status values match sf::Socket::Status: 0 Done, 1 NotReady, 2 Partial, 3 Disconnected, 4 Error.
// webTcpSecure uses 2 for TlsStatus::NotConnected and 1 for HandshakeStarted.
[[nodiscard]] int webTcpConnect(SocketHandle handle, const char* host, unsigned short port, bool secure, bool wait, int timeoutMs);
[[nodiscard]] int  webTcpSecure(SocketHandle handle, const char* host, bool wait, int timeoutMs);
[[nodiscard]] int  webTcpSend(SocketHandle handle, const void* data, std::size_t size, std::size_t& sent, bool wait);
[[nodiscard]] int  webTcpReceive(SocketHandle handle, void* data, std::size_t size, std::size_t& received, bool wait);
[[nodiscard]] bool webTcpIsOpen(SocketHandle handle);

[[nodiscard]] int            webUdpBind(SocketHandle handle, unsigned short port);
[[nodiscard]] unsigned short webUdpLocalPort(SocketHandle handle);
[[nodiscard]] int webUdpSend(SocketHandle handle, const void* data, std::size_t size, const char* host, unsigned short port, bool wait);
[[nodiscard]] int webUdpReceive(
    SocketHandle    handle,
    void*           data,
    std::size_t     size,
    std::size_t&    received,
    std::string&    host,
    unsigned short& port,
    bool            wait);

[[nodiscard]] bool webIsReadable(SocketHandle handle);
[[nodiscard]] bool webIsWritable(SocketHandle handle);
[[nodiscard]] int  webWait(const int* handles, const int* masks, int count, int timeoutMs);

[[nodiscard]] bool webFetch(const std::string& url,
                            const std::string& method,
                            const std::string& headers,
                            const std::string* body,
                            int                timeoutMs,
                            std::string&       rawResponse);

[[nodiscard]] std::optional<std::vector<std::uint8_t>> webDoh(
    const std::string&  url,
    const std::uint8_t* query,
    std::size_t         querySize,
    int                 timeoutMs);
} // namespace sf::priv
