#include <SFML/Network/UdpSocket.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>

TEST_CASE("[Network] sf::UdpSocket")
{
    SECTION("Type traits")
    {
        STATIC_CHECK(!std::is_copy_constructible_v<sf::UdpSocket>);
        STATIC_CHECK(!std::is_copy_assignable_v<sf::UdpSocket>);
        STATIC_CHECK(std::is_nothrow_move_constructible_v<sf::UdpSocket>);
        STATIC_CHECK(std::is_nothrow_move_assignable_v<sf::UdpSocket>);
    }

    SECTION("Constants")
    {
        STATIC_CHECK(sf::UdpSocket::MaxDatagramSize == 65507);
    }

    SECTION("Construction")
    {
        const sf::UdpSocket udpSocket;
        CHECK(udpSocket.getLocalPort() == 0);
    }

    SECTION("bind()/unbind()")
    {
        SECTION("IPv4")
        {
            sf::UdpSocket udpSocket;
            CHECK(udpSocket.bind(sf::Socket::AnyPort, sf::IpAddress::Broadcast) == sf::Socket::Status::Error);
            CHECK(udpSocket.bind(sf::Socket::AnyPort) == sf::Socket::Status::Done);
#if defined(SFML_SYSTEM_EMSCRIPTEN)
            // The browser cannot occupy a host port. AnyPort stays 0; an explicit port is recorded.
            CHECK(udpSocket.getLocalPort() == 0);
            CHECK(udpSocket.bind(41000) == sf::Socket::Status::Done);
            CHECK(udpSocket.getLocalPort() == 41000);
#else
            CHECK(udpSocket.getLocalPort() != 0);
#endif

            udpSocket.unbind();
            CHECK(udpSocket.getLocalPort() == 0);
        }

        SECTION("IPv6")
        {
            sf::UdpSocket udpSocket;
            CHECK(udpSocket.bind(sf::Socket::AnyPort, sf::IpAddress::AnyV6) == sf::Socket::Status::Done);
#if defined(SFML_SYSTEM_EMSCRIPTEN)
            CHECK(udpSocket.getLocalPort() == 0);
#else
            CHECK(udpSocket.getLocalPort() != 0);
#endif

            udpSocket.unbind();
            CHECK(udpSocket.getLocalPort() == 0);
        }
    }
}
