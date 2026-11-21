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
#include <SFML/Network/Socket.hpp>

#include <SFML/System/Err.hpp>

#include <ostream>
#include <utility>


namespace sf
{
////////////////////////////////////////////////////////////
Socket::Socket(Type type) : m_type(type), m_socket(priv::socketInvalid())
{
}


////////////////////////////////////////////////////////////
Socket::~Socket()
{
    // Close the socket before it gets destructed
    close();
}


////////////////////////////////////////////////////////////
Socket::Socket(Socket&& socket) noexcept :
    m_type(socket.m_type),
    m_socket(std::exchange(socket.m_socket, priv::socketInvalid())),
    m_isBlocking(socket.m_isBlocking)
{
}


////////////////////////////////////////////////////////////
Socket& Socket::operator=(Socket&& socket) noexcept
{
    if (&socket == this)
        return *this;

    close();

    m_type       = socket.m_type;
    m_socket     = std::exchange(socket.m_socket, priv::socketInvalid());
    m_isBlocking = socket.m_isBlocking;
    return *this;
}


////////////////////////////////////////////////////////////
void Socket::setBlocking(bool blocking)
{
    // Apply if the socket is already created
    if (m_socket != priv::socketInvalid())
        priv::socketSetBlocking(m_socket, blocking);

    m_isBlocking = blocking;
}


////////////////////////////////////////////////////////////
bool Socket::isBlocking() const
{
    return m_isBlocking;
}


////////////////////////////////////////////////////////////
SocketHandle Socket::getNativeHandle() const
{
    return m_socket;
}


////////////////////////////////////////////////////////////
void Socket::create(IpAddress::Type addressType)
{
    // Don't create the socket if it already exists
    if (m_socket == priv::socketInvalid())
    {
        const SocketHandle handle = priv::socketCreate(m_type == Type::Tcp, addressType);
        if (handle == priv::socketInvalid())
        {
            err() << "Failed to create socket" << std::endl;
            return;
        }

        create(handle);
    }
}


////////////////////////////////////////////////////////////
void Socket::create(SocketHandle handle)
{
    // Don't create the socket if it already exists
    if (m_socket == priv::socketInvalid())
    {
        // Assign the new handle
        m_socket = handle;

        // Set the current blocking state
        setBlocking(m_isBlocking);

        priv::socketConfigure(m_socket, m_type == Type::Tcp);
    }
}


////////////////////////////////////////////////////////////
void Socket::close()
{
    // Close the socket
    if (m_socket != priv::socketInvalid())
    {
        priv::socketClose(m_socket);
        m_socket = priv::socketInvalid();
    }
}

} // namespace sf
