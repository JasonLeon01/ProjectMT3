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
#include <SFML/Network.hpp>

#include <SFML/System/Sleep.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <string>

#include <cstdlib>
#include <cstring>


namespace
{
int fail(const char* message)
{
    std::cerr << "network web test failed: " << message << std::endl;
    return 1;
}

unsigned short portFromEnv(const char* name)
{
    const char* value = std::getenv(name);
    if (!value)
    {
        std::cerr << "missing " << name << std::endl;
        std::exit(1);
    }
    return static_cast<unsigned short>(std::atoi(value));
}

bool containsAddress(const std::optional<std::vector<sf::IpAddress>>& addresses, const char* text)
{
    if (!addresses.has_value())
        return false;

    return std::any_of(addresses->begin(),
                       addresses->end(),
                       [text](const sf::IpAddress& address) { return address.toString() == text; });
}
} // namespace


int main()
{
    const auto literal = sf::IpAddress::fromString("198.51.100.10");
    if (!literal.has_value() || (literal->toString() != "198.51.100.10"))
        return fail("address literal");
    if (sf::IpAddress::LocalHost != sf::IpAddress(127, 0, 0, 1))
        return fail("localhost constant");

    if (sf::IpAddress::getLocalAddress().has_value() || sf::IpAddress::getLocalAddress().has_value())
        return fail("local address");

    sf::Packet packet;
    packet << std::string("local");
    if (packet.getDataSize() == 0)
        return fail("packet");

    const auto httpPort = portFromEnv("SFML_TEST_HTTP_PORT");
    const auto tcpPort  = portFromEnv("SFML_TEST_TCP_PORT");
    const auto udpPort  = portFromEnv("SFML_TEST_UDP_PORT");
    const auto ftpPort  = portFromEnv("SFML_TEST_FTP_PORT");

    const auto resolved = sf::Dns::resolve("test.sfml");
    if (!containsAddress(resolved, "203.0.113.5"))
        return fail("default dns");
    if (!containsAddress(sf::Dns::resolve("localhost"), "127.0.0.1"))
        return fail("localhost dns");
    if (sf::Dns::resolve("test.sfml", {sf::IpAddress(1, 2, 3, 4)}).has_value())
        return fail("dns server list");

    const auto names = sf::Dns::queryNs("test.sfml");
    if (names.empty() || (names.front().toAnsiString() != "ns.sfml"))
        return fail("ns");
    const auto mail = sf::Dns::queryMx("test.sfml");
    if (mail.empty() || (mail.front().preference != 10) || (mail.front().exchange.toAnsiString() != "mail.sfml"))
        return fail("mx");
    const auto services = sf::Dns::querySrv("test.sfml");
    if (services.empty() || (services.front().priority != 1) || (services.front().weight != 2) ||
        (services.front().port != 9) || (services.front().target.toAnsiString() != "srv.sfml"))
        return fail("srv");
    const auto textRecords = sf::Dns::queryTxt("test.sfml");
    if (textRecords.empty() || textRecords.front().empty() || (textRecords.front().front().toAnsiString() != "sfml"))
        return fail("txt");
    if (!sf::Dns::queryNs("test.sfml", {sf::IpAddress(1, 2, 3, 4)}).empty())
        return fail("ns server list");

    sf::Http http;
    if (!http.setHost("127.0.0.1", httpPort))
        return fail("set host");

    const sf::Http::Request request("/echo");
    const auto              response = http.sendRequest(request, sf::seconds(2));
    if ((response.getStatus() != sf::Http::Response::Status::Ok) || (response.getBody() != "hello ") ||
        (response.getField("x-test") != "sfml"))
        return fail("http get");

    sf::Http::Request post("/echo", sf::Http::Request::Method::Post);
    post.setBody("world");
    const auto posted = http.sendRequest(post, sf::seconds(2));
    if ((posted.getStatus() != sf::Http::Response::Status::Ok) || (posted.getBody() != "hello world"))
        return fail("http post");

    sf::Http typed;
    if (typed.setHost("127.0.0.1", httpPort, sf::IpAddress::Type::IpV4))
        return fail("http address type");
    if (http.sendRequest(request, sf::seconds(1), false).getStatus() != sf::Http::Response::Status::ConnectionFailed)
        return fail("http verify");

    sf::TcpSocket early;
    if (early.setupTlsClient("example.com") != sf::TcpSocket::TlsStatus::NotConnected)
        return fail("tls before connect");
    if (early.setupTlsClient("example.com", false) != sf::TcpSocket::TlsStatus::Error)
        return fail("tls verify");
    if (early.setupTlsClient("example.com", "not-a-certificate") != sf::TcpSocket::TlsStatus::Error)
        return fail("tls certificate");
    if (early.setupTlsServer("certificate", "key") != sf::TcpSocket::TlsStatus::Error)
        return fail("tls server");

    sf::TcpSocket socket;
    socket.setBlocking(false);
    const auto connecting = socket.connect(sf::IpAddress::LocalHost, tcpPort);
    if ((connecting != sf::Socket::Status::Done) && (connecting != sf::Socket::Status::NotReady))
        return fail("tcp connect");

    sf::SocketSelector selector;
    auto               calls = 0;
    if (!selector.add(socket, sf::SocketSelector::Send, [&calls](sf::SocketSelector::ReadinessType) { ++calls; }))
        return fail("selector add");
    if (!selector.wait(sf::seconds(2)) || !selector.isReady(socket, sf::SocketSelector::Send))
        return fail("selector wait");
    selector.dispatchReadyCallbacks();
    if ((calls != 1) || !socket.getRemoteAddress().has_value())
        return fail("tcp open");

    socket.setBlocking(true);
    if (socket.send("ping", 4) != sf::Socket::Status::Done)
        return fail("tcp send");
    std::array<char, 4> buffer{};
    std::size_t         received = 0;
    if ((socket.receive(buffer.data(), buffer.size(), received) != sf::Socket::Status::Done) || (received != 4) ||
        (std::string(buffer.data(), 4) != "ping"))
        return fail("tcp receive");

    sf::Packet outgoing;
    outgoing << std::string("packet");
    if (socket.send(outgoing) != sf::Socket::Status::Done)
        return fail("packet send");
    sf::Packet incoming;
    if (socket.receive(incoming) != sf::Socket::Status::Done)
        return fail("packet receive");
    std::string value;
    incoming >> value;
    if (!incoming || (value != "packet"))
        return fail("packet value");

    if (socket.send("1234", 4) != sf::Socket::Status::Done)
        return fail("split send");
    std::array<char, 2> left{};
    std::array<char, 2> right{};
    std::size_t         count = 0;
    if ((socket.receive(left.data(), left.size(), count) != sf::Socket::Status::Done) || (count != 2))
        return fail("split left");
    if ((socket.receive(right.data(), right.size(), count) != sf::Socket::Status::Done) || (count != 2) ||
        ((std::string(left.data(), 2) + std::string(right.data(), 2)) != "1234"))
        return fail("split right");

    socket.setBlocking(false);
    if (socket.send("pong", 4) != sf::Socket::Status::Done)
        return fail("nonblocking send");
    std::array<char, 4> again{};
    std::size_t         got    = 0;
    auto                result = socket.receive(again.data(), again.size(), got);
    if (result == sf::Socket::Status::NotReady)
    {
        sf::SocketSelector readable;
        if (!readable.add(socket) || !readable.wait(sf::seconds(2)))
            return fail("nonblocking wait");
        result = socket.receive(again.data(), again.size(), got);
    }
    if ((result != sf::Socket::Status::Done) || (got != 4) || (std::string(again.data(), 4) != "pong"))
        return fail("nonblocking receive");

    sf::UdpSocket first;
    sf::UdpSocket second;
    if ((first.bind(41000) != sf::Socket::Status::Done) || (first.getLocalPort() != 41000))
        return fail("udp bind");
    if (second.bind(41001) != sf::Socket::Status::Done)
        return fail("udp bind peer");
    if (first.send("x", 1, sf::IpAddress::Broadcast, udpPort) != sf::Socket::Status::Error)
        return fail("udp broadcast");
    if (first.send("y", 1, sf::IpAddress(224, 0, 0, 1), udpPort) != sf::Socket::Status::Error)
        return fail("udp multicast");

    first.setBlocking(false);
    second.setBlocking(false);
    auto sent  = sf::Socket::Status::NotReady;
    auto other = sf::Socket::Status::NotReady;
    for (int attempt = 0; (attempt < 50) && ((sent != sf::Socket::Status::Done) || (other != sf::Socket::Status::Done));
         ++attempt)
    {
        if (sent != sf::Socket::Status::Done)
            sent = first.send("udp", 3, sf::IpAddress::LocalHost, udpPort);
        if (other != sf::Socket::Status::Done)
            other = second.send("ok", 2, sf::IpAddress::LocalHost, udpPort);
        if ((sent == sf::Socket::Status::Error) || (other == sf::Socket::Status::Error))
            return fail("udp send error");
        sf::sleep(sf::milliseconds(20));
    }
    if ((sent != sf::Socket::Status::Done) || (other != sf::Socket::Status::Done))
        return fail("udp send");

    second.setBlocking(true);
    std::array<char, 8>          udpBuffer{};
    std::size_t                  udpGot = 0;
    std::optional<sf::IpAddress> sender;
    unsigned short               senderPort = 0;
    if ((second.receive(udpBuffer.data(), udpBuffer.size(), udpGot, sender, senderPort) != sf::Socket::Status::Done) ||
        (udpGot != 3) || (std::string(udpBuffer.data(), 3) != "udp") || !sender.has_value() ||
        (sender->toString() != "127.0.0.1") || (senderPort != udpPort))
        return fail("udp receive");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    sf::Ftp ftp;
    if (ftp.connect(sf::IpAddress::LocalHost, ftpPort, sf::seconds(2)).getStatus() !=
        sf::Ftp::Response::Status::ServiceUnavailable)
        return fail("ftp");
#pragma GCC diagnostic pop

    sf::Sftp sftp;
    if (sftp.connect(sf::IpAddress::LocalHost, ftpPort, sf::milliseconds(800)).isOk())
        return fail("sftp");

    std::cout << "sfml-network-web-test ok" << std::endl;
    return 0;
}
