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
#include <SFML/Network/Backend.hpp>
#include <SFML/Network/Dns.hpp>
#include <SFML/Network/SocketImpl.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <array>
#include <ostream>

#include <cstddef>
#include <cstdint>


namespace sf::priv
{
////////////////////////////////////////////////////////////
SocketHandle socketInvalid()
{
    return SocketImpl::invalidSocket();
}


////////////////////////////////////////////////////////////
SocketHandle socketCreate(bool tcp, IpAddress::Type addressType)
{
    return socket(addressType == IpAddress::Type::IpV4 ? PF_INET : PF_INET6, tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
}


////////////////////////////////////////////////////////////
void socketConfigure(SocketHandle handle, bool tcp)
{
    if (tcp)
    {
        // Disable the Nagle algorithm (i.e. removes buffering of TCP packets)
        int yes = 1;
        if (setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1)
        {
            err() << "Failed to set socket option \"TCP_NODELAY\" ; "
                  << "all your TCP packets will be buffered" << std::endl;
        }

// On macOS, disable the SIGPIPE signal on disconnection
#ifdef SFML_SYSTEM_MACOS
        if (setsockopt(handle, SOL_SOCKET, SO_NOSIGPIPE, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1)
        {
            err() << "Failed to set socket option \"SO_NOSIGPIPE\"" << std::endl;
        }
#endif
    }
    else
    {
        // Enable broadcast by default for UDP sockets
        int yes = 1;
        if (setsockopt(handle, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1)
        {
            err() << "Failed to enable broadcast on UDP socket" << std::endl;
        }
    }
}


////////////////////////////////////////////////////////////
void socketClose(SocketHandle handle)
{
    SocketImpl::close(handle);
}


////////////////////////////////////////////////////////////
void socketSetBlocking(SocketHandle handle, bool blocking)
{
    SocketImpl::setBlocking(handle, blocking);
}


////////////////////////////////////////////////////////////
bool httpPrepareHost(const std::string& hostname, std::optional<IpAddress::Type>, std::vector<IpAddress>& hosts)
{
    hosts = Dns::resolve(hostname).value_or(std::vector<IpAddress>{});
    return !hosts.empty();
}


////////////////////////////////////////////////////////////
std::optional<std::string> httpSend(const HttpMessage& message, Time timeout, bool verifyServer)
{
    auto hosts = message.hosts;

    if (message.addressType == IpAddress::Type::IpV4)
        hosts.erase(std::remove_if(hosts.begin(), hosts.end(), [](const auto& host) { return host.isV6(); }), hosts.end());

    if (message.addressType == IpAddress::Type::IpV6)
        hosts.erase(std::remove_if(hosts.begin(), hosts.end(), [](const auto& host) { return host.isV4(); }), hosts.end());

    for (const auto& host : message.hosts)
    {
        if (message.addressType.has_value() && (host.getType() != message.addressType))
            continue;

        TcpSocket connection;

        // Connect the socket to the host
        if (connection.connect(host, message.port, timeout) == Socket::Status::Done)
        {
            if (message.https &&
                (connection.setupTlsClient(message.hostName, verifyServer) != TcpSocket::TlsStatus::HandshakeComplete))
                continue;

            // Convert the request to string and send it through the connected socket
            const std::string& requestStr = message.prepared;

            if (!requestStr.empty())
            {
                // Send it through the socket
                if (connection.send(requestStr.c_str(), requestStr.size()) == Socket::Status::Done)
                {
                    // Wait for the server's response
                    std::string            receivedStr;
                    std::size_t            size = 0;
                    std::array<char, 1024> buffer{};

                    // When the HTTPS connection makes use of TLS 1.3 new session ticket
                    // messages can be received by the client from the server at any time
                    // When these messages are received the receive function will return Socket::Status::Partial
                    // In this case We just continue to call receive until actual payload
                    // data is available, the connection is closed or an error occurs
                    auto result = connection.receive(buffer.data(), buffer.size(), size);

                    while ((result == Socket::Status::Done) || (result == Socket::Status::Partial))
                    {
                        // Only append payload data when it has been completely received
                        if (result == Socket::Status::Done)
                            receivedStr.append(buffer.data(), buffer.data() + size);

                        result = connection.receive(buffer.data(), buffer.size(), size);
                    }

                    // If we received any data (even if it doesn't contain a HTTP 200 response)
                    // we successfully found a host that we can connect to under the hostname
                    // Stop attempting to connect to further addresses in this case
                    if (!receivedStr.empty())
                    {
                        connection.disconnect();
                        return receivedStr;
                    }
                }
            }

            // Close the connection
            connection.disconnect();
        }
    }

    return std::nullopt;
}


////////////////////////////////////////////////////////////
std::optional<IpAddress> localAddress(IpAddress::Type type)
{
    // The method here is to connect a UDP socket to a public ip,
    // and get the local socket address with the getsockname function.
    // UDP connection will not send anything to the network, so this function won't cause any overhead.

    if (type == IpAddress::Type::IpV4)
    {
        // Create the socket
        const SocketHandle sock = socket(PF_INET, SOCK_DGRAM, 0);
        if (sock == SocketImpl::invalidSocket())
        {
            err() << "Failed to retrieve local address (invalid socket)" << std::endl;
            return std::nullopt;
        }

        // Connect the socket to a public ip (here 1.1.1.1) on any
        // port. This will give the local address of the network interface
        // used for default routing which is usually what we want.
        sockaddr_in address = SocketImpl::createAddress(0x01010101, 9);
        if (connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1)
        {
            SocketImpl::close(sock);

            err() << "Failed to retrieve local address (socket connection failure)" << std::endl;
            return std::nullopt;
        }

        // Get the local address of the socket connection
        SocketImpl::AddrLength size = sizeof(address);
        if (getsockname(sock, reinterpret_cast<sockaddr*>(&address), &size) == -1)
        {
            SocketImpl::close(sock);

            err() << "Failed to retrieve local address (socket local address retrieval failure)" << std::endl;
            return std::nullopt;
        }

        // Close the socket
        SocketImpl::close(sock);

        // Finally build the IP address
        return IpAddress(ntohl(address.sin_addr.s_addr));
    }

    // If we get to this point the user wants to get the local IPv6 address

    // Create the socket
    const SocketHandle sock = socket(PF_INET6, SOCK_DGRAM, 0);
    if (sock == SocketImpl::invalidSocket())
    {
        err() << "Failed to retrieve local address (invalid socket)" << std::endl;
        return std::nullopt;
    }

    // Connect the socket to a public ip (here 2a00::1) on any
    // port. This will give the local address of the network interface
    // used for default routing which is usually what we want.
    sockaddr_in6 address = SocketImpl::
        createAddress(std::array<std::uint8_t,
                                 16>{0x2a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
                      9);
    if (connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1)
    {
        SocketImpl::close(sock);

        err() << "Failed to retrieve local address (socket connection failure)" << std::endl;
        return std::nullopt;
    }

    // Get the local address of the socket connection
    SocketImpl::AddrLength size = sizeof(address);
    if (getsockname(sock, reinterpret_cast<sockaddr*>(&address), &size) == -1)
    {
        SocketImpl::close(sock);

        err() << "Failed to retrieve local address (socket local address retrieval failure)" << std::endl;
        return std::nullopt;
    }

    // Close the socket
    SocketImpl::close(sock);

    // Finally build the IP address
    return IpAddress(
        {address.sin6_addr.s6_addr[0],
         address.sin6_addr.s6_addr[1],
         address.sin6_addr.s6_addr[2],
         address.sin6_addr.s6_addr[3],
         address.sin6_addr.s6_addr[4],
         address.sin6_addr.s6_addr[5],
         address.sin6_addr.s6_addr[6],
         address.sin6_addr.s6_addr[7],
         address.sin6_addr.s6_addr[8],
         address.sin6_addr.s6_addr[9],
         address.sin6_addr.s6_addr[10],
         address.sin6_addr.s6_addr[11],
         address.sin6_addr.s6_addr[12],
         address.sin6_addr.s6_addr[13],
         address.sin6_addr.s6_addr[14],
         address.sin6_addr.s6_addr[15]});
}
} // namespace sf::priv
