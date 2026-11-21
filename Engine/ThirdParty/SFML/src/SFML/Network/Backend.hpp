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
#include <SFML/Network/Http.hpp>
#include <SFML/Network/SocketHandle.hpp>

#include <optional>
#include <string>
#include <utility>
#include <vector>


namespace sf::priv
{
////////////////////////////////////////////////////////////
/// \brief Transport-specific operations behind the public Network classes
///
/// A backend is a directory next to Native/ and Emscripten/. CMake compiles
/// one directory. The backend implements the functions below, plus the full
/// bodies of Dns, TcpSocket, UdpSocket, and SocketSelector.
///
/// A backend that cannot accept connections omits TcpListener.cpp and hides
/// sf::TcpListener in the public headers. Do not stub that class.
///
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// \brief Handle value that means "no socket"
///
////////////////////////////////////////////////////////////
[[nodiscard]] SocketHandle socketInvalid();

////////////////////////////////////////////////////////////
/// \brief Open a TCP or UDP socket for the given address family
///
/// \return A new handle, or socketInvalid() when creation fails
///
////////////////////////////////////////////////////////////
[[nodiscard]] SocketHandle socketCreate(bool tcp, IpAddress::Type addressType);

////////////////////////////////////////////////////////////
/// \brief Apply the options every new socket receives
///
////////////////////////////////////////////////////////////
void socketConfigure(SocketHandle handle, bool tcp);

////////////////////////////////////////////////////////////
/// \brief Release a socket handle
///
////////////////////////////////////////////////////////////
void socketClose(SocketHandle handle);

////////////////////////////////////////////////////////////
/// \brief Apply the blocking flag to an open socket
///
////////////////////////////////////////////////////////////
void socketSetBlocking(SocketHandle handle, bool blocking);

////////////////////////////////////////////////////////////
/// \brief Prepare an HTTP host using the backend's name-resolution policy
///
/// \return false when the host or requested address type cannot be used
///
////////////////////////////////////////////////////////////
[[nodiscard]] bool httpPrepareHost(const std::string&             hostname,
                                   std::optional<IpAddress::Type> addressType,
                                   std::vector<IpAddress>&        hosts);

////////////////////////////////////////////////////////////
/// \brief One HTTP request after the shared field defaults have been filled in
///
////////////////////////////////////////////////////////////
struct HttpMessage
{
    Http::Request::Method                            method{Http::Request::Method::Get};
    std::string                                      uri;
    std::string                                      body;
    std::string                                      prepared;
    std::vector<std::pair<std::string, std::string>> fields;
    std::string                                      hostName;
    unsigned short                                   port{};
    bool                                             https{};
    std::optional<IpAddress::Type>                   addressType;
    std::vector<IpAddress>                           hosts;
};

////////////////////////////////////////////////////////////
/// \brief Send one HTTP request
///
/// \return The raw HTTP response, or nullopt when no response was received
///
////////////////////////////////////////////////////////////
[[nodiscard]] std::optional<std::string> httpSend(const HttpMessage& message, Time timeout, bool verifyServer);

////////////////////////////////////////////////////////////
/// \brief Address of the local host, when the backend can discover it
///
////////////////////////////////////////////////////////////
[[nodiscard]] std::optional<IpAddress> localAddress(IpAddress::Type type);

} // namespace sf::priv
