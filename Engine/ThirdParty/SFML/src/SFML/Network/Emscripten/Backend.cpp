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
#include <SFML/Network/Emscripten/WebTransport.hpp>

#include <SFML/System/Utils.hpp>

#include <algorithm>
#include <limits>

#include <cstdint>


namespace sf::priv
{
////////////////////////////////////////////////////////////
SocketHandle socketInvalid()
{
    return -1;
}


////////////////////////////////////////////////////////////
SocketHandle socketCreate(bool tcp, IpAddress::Type)
{
    return tcp ? webCreateTcp() : webCreateUdp();
}


////////////////////////////////////////////////////////////
void socketConfigure(SocketHandle, bool)
{
}


////////////////////////////////////////////////////////////
void socketClose(SocketHandle handle)
{
    webClose(handle);
}


////////////////////////////////////////////////////////////
void socketSetBlocking(SocketHandle, bool)
{
}


////////////////////////////////////////////////////////////
bool httpPrepareHost(const std::string& hostname, std::optional<IpAddress::Type> addressType, std::vector<IpAddress>& hosts)
{
    hosts.clear();
    if (addressType.has_value())
    {
        webSkip("Http::setHost with an address type");
        return false;
    }

    // Fetch resolves the URL itself, including hosts only visible to the browser.
    return !hostname.empty();
}


////////////////////////////////////////////////////////////
std::optional<std::string> httpSend(const HttpMessage& message, Time timeout, bool verifyServer)
{
    if (!verifyServer)
    {
        webSkip("Http::sendRequest with certificate verification disabled");
        return std::nullopt;
    }

    if (message.addressType.has_value())
    {
        webSkip("Http::sendRequest with an address type");
        return std::nullopt;
    }

    if (message.hostName.empty())
        return std::nullopt;

    const char* methodName = "GET";
    switch (message.method)
    {
        case Http::Request::Method::Get:
            break;
        case Http::Request::Method::Post:
            methodName = "POST";
            break;
        case Http::Request::Method::Head:
            methodName = "HEAD";
            break;
        case Http::Request::Method::Put:
            methodName = "PUT";
            break;
        case Http::Request::Method::Delete:
            methodName = "DELETE";
            break;
    }

    std::string headers;
    for (const auto& [name, value] : message.fields)
    {
        const auto lower = toLower(name);
        if ((lower == "host") || (lower == "connection") || (lower == "content-length") || (lower == "user-agent") ||
            (lower == "accept-encoding") || (lower == "origin") || (lower == "referer"))
            continue;

        headers += name;
        headers += ": ";
        headers += value;
        headers += "\r\n";
    }

    std::string host = message.hostName;
    if ((host.find(':') != std::string::npos) && (host.empty() || (host.front() != '[')))
        host = "[" + host + "]";

    std::string uri = message.uri.empty() ? std::string("/") : message.uri;
    if (uri.empty() || (uri.front() != '/'))
        uri.insert(uri.begin(), '/');

    const std::string url = std::string(message.https ? "https://" : "http://") + host + ":" +
                            std::to_string(message.port) + uri;
    const bool hasBody = (message.method == Http::Request::Method::Post) || (message.method == Http::Request::Method::Put) ||
                         (message.method == Http::Request::Method::Delete);
    auto timeoutMs = 0;
    if (timeout > Time::Zero)
    {
        const auto milliseconds = timeout.asMilliseconds();
        timeoutMs               = milliseconds > std::numeric_limits<int>::max()
                                      ? std::numeric_limits<int>::max()
                                      : static_cast<int>(std::max<std::int64_t>(milliseconds, 0));
    }

    std::string raw;
    if (!webFetch(url, methodName, headers, hasBody ? &message.body : nullptr, timeoutMs, raw))
        return std::nullopt;

    return raw;
}


////////////////////////////////////////////////////////////
std::optional<IpAddress> localAddress(IpAddress::Type)
{
    webSkip("IpAddress::getLocalAddress");
    return std::nullopt;
}
} // namespace sf::priv
