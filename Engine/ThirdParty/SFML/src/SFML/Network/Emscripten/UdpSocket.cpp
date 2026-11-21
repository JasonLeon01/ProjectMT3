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
#include <SFML/Network/Emscripten/WebTransport.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/UdpSocket.hpp>

#include <SFML/System/Err.hpp>

#include <ostream>
#include <string>


namespace
{
[[nodiscard]] sf::Socket::Status udpStatus(int code)
{
    switch (code)
    {
        case 0:
            return sf::Socket::Status::Done;
        case 1:
            return sf::Socket::Status::NotReady;
        case 2:
            return sf::Socket::Status::Partial;
        case 3:
            return sf::Socket::Status::Disconnected;
        default:
            return sf::Socket::Status::Error;
    }
}

[[nodiscard]] bool unsupportedTarget(const sf::IpAddress& address)
{
    if ((address == sf::IpAddress::Broadcast) || (address == sf::IpAddress::BroadcastV4))
        return true;

    if (address.isV4())
    {
        const auto first = static_cast<std::uint8_t>(address.toInteger() >> 24);
        return (first >= 224) && (first <= 239);
    }

    return address.toBytes()[0] == 0xFF;
}
} // namespace


namespace sf
{
////////////////////////////////////////////////////////////
UdpSocket::UdpSocket() : Socket(Type::Udp)
{
}


////////////////////////////////////////////////////////////
unsigned short UdpSocket::getLocalPort() const
{
    return priv::webUdpLocalPort(getNativeHandle());
}


////////////////////////////////////////////////////////////
Socket::Status UdpSocket::bind(unsigned short port, IpAddress address)
{
    if (address == IpAddress::Broadcast)
        return Status::Error;

    if (getNativeHandle() >= 0)
        unbind();

    create(address.isV4() ? IpAddress::Type::IpV4 : IpAddress::Type::IpV6);
    if (getNativeHandle() < 0)
        return Status::Error;

    return udpStatus(priv::webUdpBind(getNativeHandle(), port));
}


////////////////////////////////////////////////////////////
void UdpSocket::unbind()
{
    close();
}


////////////////////////////////////////////////////////////
Socket::Status UdpSocket::send(const void* data, std::size_t size, IpAddress remoteAddress, unsigned short remotePort)
{
    if (size > MaxDatagramSize)
    {
        err() << "Cannot send data over the network "
              << "(the number of bytes to send is greater than sf::UdpSocket::MaxDatagramSize)" << std::endl;
        return Status::Error;
    }

    if (!data || (size == 0))
    {
        err() << "Cannot send data over the network (no data to send)" << std::endl;
        return Status::Error;
    }

    if (unsupportedTarget(remoteAddress))
    {
        priv::webSkip("UdpSocket::send to a broadcast or multicast address");
        return Status::Error;
    }

    if (getNativeHandle() < 0)
        create(remoteAddress.isV4() ? IpAddress::Type::IpV4 : IpAddress::Type::IpV6);
    if (getNativeHandle() < 0)
        return Status::Error;

    const std::string host = remoteAddress.toString();
    return udpStatus(priv::webUdpSend(getNativeHandle(), data, size, host.c_str(), remotePort, isBlocking()));
}


////////////////////////////////////////////////////////////
Socket::Status UdpSocket::receive(void*                     data,
                                  std::size_t               size,
                                  std::size_t&              received,
                                  std::optional<IpAddress>& remoteAddress,
                                  unsigned short&           remotePort)
{
    received      = 0;
    remoteAddress = std::nullopt;
    remotePort    = 0;

    if (!data)
    {
        err() << "Cannot receive data from the network (the destination buffer is invalid)" << std::endl;
        return Status::Error;
    }

    if (getNativeHandle() < 0)
        return Status::Error;

    std::string host; // NOLINT(misc-const-correctness) filled by webUdpReceive
    const auto  status = udpStatus(
        priv::webUdpReceive(getNativeHandle(), data, size, received, host, remotePort, isBlocking()));
    if (status != Status::Done)
    {
        remotePort = 0;
        return status;
    }

    if (const auto address = IpAddress::fromString(host); address)
        remoteAddress = *address;
    return Status::Done;
}


////////////////////////////////////////////////////////////
Socket::Status UdpSocket::send(Packet& packet, IpAddress remoteAddress, unsigned short remotePort)
{
    std::size_t size = 0;
    const void* data = packet.onSend(size);
    return send(data, size, remoteAddress, remotePort);
}


////////////////////////////////////////////////////////////
Socket::Status UdpSocket::receive(Packet& packet, std::optional<IpAddress>& remoteAddress, unsigned short& remotePort)
{
    std::size_t  received = 0;
    const Status status   = receive(m_buffer.data(), m_buffer.size(), received, remoteAddress, remotePort);
    packet.clear();
    if ((status == Status::Done) && (received > 0))
        packet.onReceive(m_buffer.data(), received);
    return status;
}
} // namespace sf
