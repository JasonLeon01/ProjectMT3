#include "Network/bind_UdpSocket.hpp"

#include <algorithm>
#include <array>
#include <string_view>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace { constexpr std::array<std::string_view, 12> docs = {
    "\\brief Specialized socket using the UDP protocol",
    "\\brief Default constructor",
    "\\brief Set the blocking state of the socket\n\nIn blocking mode, calls will not return until they have\ncompleted their task. For example, a call to Receive in\nblocking mode won't return until some data was actually\nreceived.\nIn non-blocking mode, calls will always return immediately,\nusing the return code to signal whether there was data\navailable or not.\nBy default, all sockets are blocking.\n\n\\param blocking `true` to set the socket as blocking, `false` for non-blocking\n\n\\see `isBlocking`",
    "\\brief Tell whether the socket is in blocking or non-blocking mode\n\n\\return `true` if the socket is blocking, `false` otherwise\n\n\\see `setBlocking`",
    "\\brief Get the port to which the socket is bound locally\n\nIf the socket is not bound to a port, this function\nreturns 0.\n\n\\return Port to which the socket is bound\n\n\\see `bind`",
    "\\brief Bind the socket to a specific port\n\nBinding the socket to a port is necessary for being\nable to receive data on that port.\n\nWhen providing `sf::Socket::AnyPort` as port, the listener\nwill request an available port from the system.\nThe chosen port can be retrieved by calling `getLocalPort()`.\n\nSince the socket can only be bound to a single port at\nany given moment, if it is already bound when this\nfunction is called, it will be unbound from the previous\nport before being bound to the new one.\n\n\\param port    Port to bind the socket to\n\\param address Address of the interface to bind to\n\n\\return Status code\n\n\\see `unbind`, `getLocalPort`",
    "\\brief Unbind the socket from the local port to which it is bound\n\nThe port that the socket was previously bound to is immediately\nmade available to the operating system after this function is called.\nThis means that a subsequent call to `bind()` will be able to re-bind\nthe port if no other process has done so in the mean time.\nIf the socket is not bound to a port, this function has no effect.\n\n\\see `bind`",
    "\\brief Send raw data to a remote peer\n\nMake sure that `size` is not greater than\n`UdpSocket::MaxDatagramSize`, otherwise this function will\nfail and no data will be sent.\n\n\\param data          Pointer to the sequence of bytes to send\n\\param size          Number of bytes to send\n\\param remoteAddress Address of the receiver\n\\param remotePort    Port of the receiver to send the data to\n\n\\return Status code\n\n\\see `receive`",
    "\\brief Receive raw data from a remote peer\n\nIn blocking mode, this function will wait until some\nbytes are actually received.\nBe careful to use a buffer which is large enough for\nthe data that you intend to receive, if it is too small\nthen an error will be returned and *all* the data will\nbe lost.\n\n\\param data          Pointer to the array to fill with the received bytes\n\\param size          Maximum number of bytes that can be received\n\\param received      This variable is filled with the actual number of bytes received\n\\param remoteAddress Address of the peer that sent the data\n\\param remotePort    Port of the peer that sent the data\n\n\\return Status code\n\n\\see `send`",
    "\\brief Send a formatted packet of data to a remote peer\n\nMake sure that the packet size is not greater than\n`UdpSocket::MaxDatagramSize`, otherwise this function will\nfail and no data will be sent.\n\n\\param packet        Packet to send\n\\param remoteAddress Address of the receiver\n\\param remotePort    Port of the receiver to send the data to\n\n\\return Status code\n\n\\see `receive`",
    "\\brief Receive a formatted packet of data from a remote peer\n\nIn blocking mode, this function will wait until the whole packet\nhas been received.\n\n\\param packet        Packet to fill with the received data\n\\param remoteAddress Address of the peer that sent the data\n\\param remotePort    Port of the peer that sent the data\n\n\\return Status code\n\n\\see `send`",
    "The maximum number of bytes that can be sent in a single UDP datagram",
}; }

void bind_UdpSocket(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__UdpSocket = lua_glue::BindClass<sf::UdpSocket>(sf, "UdpSocket");
    lua_glue::BindBase<sf::UdpSocket, sf::Socket>(type_sf__UdpSocket);
    lua_glue::Table table_sf__UdpSocket = sf["UdpSocket"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::UdpSocket>(lua);
    lua_glue::Table native_bases_sf__UdpSocket = lua.create_table();
    native_bases_sf__UdpSocket.add(lua["sf"]["Socket"].get<lua_glue::Table>());
    table_sf__UdpSocket.raw_set("__nativeBases", native_bases_sf__UdpSocket);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.UdpSocket", "sf.Socket");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.UdpSocket", "new", "fun(): sf.UdpSocket");
    lua_glue::BindCallable(type_sf__UdpSocket, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::UdpSocket>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.UdpSocket", "setBlocking", "fun(self: sf.UdpSocket, blocking: boolean)");
    lua_glue::BindCallable(type_sf__UdpSocket, "setBlocking",
        [](sf::UdpSocket& self, bool blocking) {
            static_cast<sf::Socket&>(self).setBlocking(blocking);
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.UdpSocket", "isBlocking", "fun(self: sf.UdpSocket): boolean");
    lua_glue::BindCallable(type_sf__UdpSocket, "isBlocking",
        [](const sf::UdpSocket& self) -> bool {
            return static_cast<const sf::Socket&>(self).isBlocking();
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.UdpSocket", "getLocalPort", "fun(self: sf.UdpSocket): integer");
    lua_glue::BindCallable(type_sf__UdpSocket, "getLocalPort",
        [](const sf::UdpSocket& self) -> unsigned short {
            return self.getLocalPort();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.UdpSocket", "bind", "fun(self: sf.UdpSocket, port: integer, address?: sf.IpAddress): sf.Socket.Status");
    lua_glue::BindCallable(type_sf__UdpSocket, "bind",
        [](sf::UdpSocket& self, lua_sf::LuaIntegral<unsigned short> port, sf::IpAddress address) -> sf::Socket::Status {
            return self.bind(port.value(), address);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::IpAddress>(sf::IpAddress::Any);
        }}},
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.UdpSocket", "unbind", "fun(self: sf.UdpSocket)");
    lua_glue::BindCallable(type_sf__UdpSocket, "unbind",
        [](sf::UdpSocket& self) {
            self.unbind();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.UdpSocket", "send", "fun(self: sf.UdpSocket, packet: sf.Packet, remoteAddress: sf.IpAddress, remotePort: integer): sf.Socket.Status");
    LUASF_STUB_OVERLOAD("sf.UdpSocket", "send", "fun(self: sf.UdpSocket, data: any, remoteAddress: sf.IpAddress, remotePort: integer): sf.Socket.Status");
    lua_glue::BindCallable(type_sf__UdpSocket, "send",
        [](sf::UdpSocket& self, sf::Packet& packet, sf::IpAddress remoteAddress, lua_sf::LuaIntegral<unsigned short> remotePort) -> sf::Socket::Status {
            return self.send(packet, remoteAddress, remotePort.value());
        },
        docs[9]
    );
    lua_glue::BindCallable(type_sf__UdpSocket, "send",
        [](sf::UdpSocket& self, lua_glue::Object data, sf::IpAddress remoteAddress, lua_sf::LuaIntegral<unsigned short> remotePort) -> sf::Socket::Status {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return self.send(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), remoteAddress, remotePort.value());
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.UdpSocket", "receive", "fun(self: sf.UdpSocket, size: integer): sf.Socket.Status, any, any, any, any");
    LUASF_STUB_OVERLOAD("sf.UdpSocket", "receive", "fun(self: sf.UdpSocket): sf.Socket.Status, any, any, any");
    lua_glue::BindCallable(type_sf__UdpSocket, "receive",
        [](sf::UdpSocket& self, std::size_t size) {
            std::vector<std::byte> data_buffer(size);
            std::size_t received{};
            std::optional<sf::IpAddress> remoteAddress{};
            unsigned short remotePort{};
            auto result = self.receive(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), received, remoteAddress, remotePort);
            const auto data_buffer_written = static_cast<std::size_t>(received);
            if (data_buffer_written < data_buffer.size())
                data_buffer.resize(data_buffer_written);
            return std::make_tuple(result, lua_glue::AsTable(data_buffer), std::move(received), std::move(remoteAddress), std::move(remotePort));
        },
        docs[8]
    );
    lua_glue::BindCallable(type_sf__UdpSocket, "receive",
        [](sf::UdpSocket& self) {
            sf::Packet packet{};
            std::optional<sf::IpAddress> remoteAddress{};
            unsigned short remotePort{};
            auto result = self.receive(packet, remoteAddress, remotePort);
            return std::make_tuple(result, std::move(packet), std::move(remoteAddress), std::move(remotePort));
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_VALUE("sf.UdpSocket", "MaxDatagramSize", "integer");
    lua_glue::BindStaticAttr<const std::size_t>(table_sf__UdpSocket, "MaxDatagramSize", &sf::UdpSocket::MaxDatagramSize);
}
