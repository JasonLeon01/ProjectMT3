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
#include <SFML/Network/TcpSocket.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <arpa/inet.h>
#include <array>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <cstring>


namespace
{
[[nodiscard]] sf::Socket::Status tcpStatus(int code)
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

[[nodiscard]] int timeoutMilliseconds(sf::Time timeout)
{
    const auto milliseconds = timeout.asMilliseconds();
    if (milliseconds > std::numeric_limits<int>::max())
        return std::numeric_limits<int>::max();
    if (milliseconds < 0)
        return 0;
    return static_cast<int>(milliseconds);
}
} // namespace


namespace sf
{
struct TcpSocket::Impl
{
    std::optional<IpAddress> remote;
    unsigned short           port{};
};


////////////////////////////////////////////////////////////
TcpSocket::TcpSocket() : Socket(Type::Tcp), m_impl(std::make_unique<Impl>())
{
}


////////////////////////////////////////////////////////////
TcpSocket::~TcpSocket() = default;


////////////////////////////////////////////////////////////
TcpSocket::TcpSocket(TcpSocket&&) noexcept = default;


////////////////////////////////////////////////////////////
TcpSocket& TcpSocket::operator=(TcpSocket&&) noexcept = default;


////////////////////////////////////////////////////////////
unsigned short TcpSocket::getLocalPort() const
{
    return 0;
}


////////////////////////////////////////////////////////////
std::optional<IpAddress> TcpSocket::getRemoteAddress() const
{
    if (!priv::webTcpIsOpen(getNativeHandle()))
        return std::nullopt;

    return m_impl->remote;
}


////////////////////////////////////////////////////////////
unsigned short TcpSocket::getRemotePort() const
{
    if (!priv::webTcpIsOpen(getNativeHandle()))
        return 0;

    return m_impl->port;
}


////////////////////////////////////////////////////////////
Socket::Status TcpSocket::connect(IpAddress remoteAddress, unsigned short remotePort, Time timeout)
{
    disconnect();
    create(remoteAddress.isV4() ? IpAddress::Type::IpV4 : IpAddress::Type::IpV6);
    if (getNativeHandle() < 0)
        return Status::Error;

    m_impl->remote = remoteAddress;
    m_impl->port   = remotePort;

    const std::string host      = remoteAddress.toString();
    const bool        blocking  = isBlocking();
    bool              wait      = false;
    int               timeoutMs = -1;
    if (timeout <= Time::Zero)
    {
        wait = blocking;
    }
    else if (blocking)
    {
        wait      = true;
        timeoutMs = timeoutMilliseconds(timeout);
    }

    const Status status = tcpStatus(
        priv::webTcpConnect(getNativeHandle(), host.c_str(), remotePort, false, wait, timeoutMs));
    if ((status != Status::Done) && (status != Status::NotReady))
        m_impl->remote.reset();

    return status;
}


////////////////////////////////////////////////////////////
void TcpSocket::disconnect()
{
    close();
    m_pendingPacket = PendingPacket();
    m_impl->remote.reset();
    m_impl->port = 0;
}


////////////////////////////////////////////////////////////
TcpSocket::TlsStatus TcpSocket::setupTlsClient(const String& hostname, bool verifyPeer)
{
    if (!verifyPeer)
    {
        priv::webSkip("TcpSocket::setupTlsClient with certificate verification disabled");
        return TlsStatus::Error;
    }

    if ((getNativeHandle() < 0) || !m_impl->remote.has_value())
        return TlsStatus::NotConnected;

    const std::string host     = hostname.toAnsiString();
    const bool        blocking = isBlocking();
    const int         code     = priv::webTcpSecure(getNativeHandle(), host.c_str(), blocking, blocking ? -1 : 0);
    switch (code)
    {
        case 0:
            return TlsStatus::HandshakeComplete;
        case 1:
            return TlsStatus::HandshakeStarted;
        case 2:
            return TlsStatus::NotConnected;
        default:
            return TlsStatus::Error;
    }
}


////////////////////////////////////////////////////////////
TcpSocket::TlsStatus TcpSocket::setupTlsClient(const String&, const char*)
{
    priv::webSkip("TcpSocket::setupTlsClient with a caller-supplied certificate");
    return TlsStatus::Error;
}


////////////////////////////////////////////////////////////
TcpSocket::TlsStatus TcpSocket::setupTlsClient(const String&, const std::byte*, std::size_t)
{
    priv::webSkip("TcpSocket::setupTlsClient with a caller-supplied certificate");
    return TlsStatus::Error;
}


////////////////////////////////////////////////////////////
TcpSocket::TlsStatus TcpSocket::setupTlsClient(const String&, std::string_view)
{
    priv::webSkip("TcpSocket::setupTlsClient with a caller-supplied certificate");
    return TlsStatus::Error;
}


////////////////////////////////////////////////////////////
TcpSocket::TlsStatus TcpSocket::setupTlsServer(const std::byte*, std::size_t, const std::byte*, std::size_t, const std::byte*, std::size_t)
{
    priv::webSkip("TcpSocket::setupTlsServer");
    return TlsStatus::Error;
}


////////////////////////////////////////////////////////////
TcpSocket::TlsStatus TcpSocket::setupTlsServer(std::string_view, std::string_view, std::string_view)
{
    priv::webSkip("TcpSocket::setupTlsServer");
    return TlsStatus::Error;
}


////////////////////////////////////////////////////////////
std::optional<std::string> TcpSocket::getCurrentCiphersuiteName() const
{
    return std::nullopt;
}


////////////////////////////////////////////////////////////
Socket::Status TcpSocket::send(const void* data, std::size_t size)
{
    if (!isBlocking())
        err() << "Warning: Partial sends might not be handled properly." << std::endl;

    std::size_t sent = 0;
    return send(data, size, sent);
}


////////////////////////////////////////////////////////////
Socket::Status TcpSocket::send(const void* data, std::size_t size, std::size_t& sent)
{
    sent = 0;
    if (!data || (size == 0))
    {
        err() << "Cannot send data over the network (no data to send)" << std::endl;
        return Status::Error;
    }

    if (getNativeHandle() < 0)
        return Status::Disconnected;

    return tcpStatus(priv::webTcpSend(getNativeHandle(), data, size, sent, isBlocking()));
}


////////////////////////////////////////////////////////////
Socket::Status TcpSocket::receive(void* data, std::size_t size, std::size_t& received)
{
    received = 0;
    if (!data)
    {
        err() << "Cannot receive data from the network (the destination buffer is invalid)" << std::endl;
        return Status::Error;
    }

    if (getNativeHandle() < 0)
        return Status::Disconnected;

    return tcpStatus(priv::webTcpReceive(getNativeHandle(), data, size, received, isBlocking()));
}


////////////////////////////////////////////////////////////
Socket::Status TcpSocket::send(Packet& packet)
{
    std::size_t size = 0;
    const void* data = packet.onSend(size);

    const auto packetSize = htonl(static_cast<std::uint32_t>(size));
    m_blockToSendBuffer.resize(sizeof(packetSize) + size);
    std::memcpy(m_blockToSendBuffer.data(), &packetSize, sizeof(packetSize));
    if (size > 0)
        std::memcpy(m_blockToSendBuffer.data() + sizeof(packetSize), data, size);

    std::size_t  sent   = 0;
    const Status status = send(m_blockToSendBuffer.data() + packet.m_sendPos,
                               m_blockToSendBuffer.size() - packet.m_sendPos,
                               sent);
    if (status == Status::Partial)
        packet.m_sendPos += sent;
    else if (status == Status::Done)
        packet.m_sendPos = 0;

    return status;
}


////////////////////////////////////////////////////////////
Socket::Status TcpSocket::receive(Packet& packet)
{
    packet.clear();

    std::uint32_t packetSize = 0;
    std::size_t   received   = 0;
    if (m_pendingPacket.sizeReceived < sizeof(m_pendingPacket.size))
    {
        while (m_pendingPacket.sizeReceived < sizeof(m_pendingPacket.size))
        {
            char*        data   = reinterpret_cast<char*>(&m_pendingPacket.size) + m_pendingPacket.sizeReceived;
            const Status status = receive(data, sizeof(m_pendingPacket.size) - m_pendingPacket.sizeReceived, received);
            m_pendingPacket.sizeReceived += received;
            if (status != Status::Done)
                return status;
        }

        packetSize = ntohl(m_pendingPacket.size);
    }
    else
    {
        packetSize = ntohl(m_pendingPacket.size);
    }

    std::array<char, 1024> buffer{};
    while (m_pendingPacket.data.size() < packetSize)
    {
        const std::size_t sizeToGet = std::min(packetSize - m_pendingPacket.data.size(), buffer.size());
        const Status      status    = receive(buffer.data(), sizeToGet, received);
        if (status != Status::Done)
            return status;

        if (received > 0)
        {
            m_pendingPacket.data.resize(m_pendingPacket.data.size() + received);
            std::byte* begin = m_pendingPacket.data.data() + m_pendingPacket.data.size() - received;
            std::memcpy(begin, buffer.data(), received);
        }
    }

    if (!m_pendingPacket.data.empty())
        packet.onReceive(m_pendingPacket.data.data(), m_pendingPacket.data.size());

    m_pendingPacket = PendingPacket();
    return Status::Done;
}
} // namespace sf
