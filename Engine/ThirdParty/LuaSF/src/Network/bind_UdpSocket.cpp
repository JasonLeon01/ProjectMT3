#include "Network/bind_UdpSocket.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_UdpSocket(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__UdpSocket = sf.new_usertype<sf::UdpSocket>("UdpSocket",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Socket>()
    );
    sol::table table_sf__UdpSocket = sf["UdpSocket"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::UdpSocket>(lua);
    sol::table native_bases_sf__UdpSocket = lua.create_table();
    native_bases_sf__UdpSocket.add(lua["sf"]["Socket"].get<sol::table>());
    table_sf__UdpSocket.raw_set("__nativeBases", native_bases_sf__UdpSocket);
    LUASF_STUB_DOC("\\brief Specialized socket using the UDP protocol");
    LUASF_STUB_CLASS("sf.UdpSocket", "sf.Socket");
    LUASF_STUB_DOC("\\brief Default constructor");
    LUASF_STUB_FUNCTION("sf.UdpSocket", "new", "fun(): sf.UdpSocket");
    type_sf__UdpSocket.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::UdpSocket>();
        }
    ));
    LUASF_STUB_DOC("\\brief Set the blocking state of the socket\n\nIn blocking mode, calls will not return until they have\ncompleted their task. For example, a call to Receive in\nblocking mode won't return until some data was actually\nreceived.\nIn non-blocking mode, calls will always return immediately,\nusing the return code to signal whether there was data\navailable or not.\nBy default, all sockets are blocking.\n\n\\param blocking `true` to set the socket as blocking, `false` for non-blocking\n\n\\see `isBlocking`");
    LUASF_STUB_FUNCTION("sf.UdpSocket", "setBlocking", "fun(self: sf.UdpSocket, blocking: boolean)");
    type_sf__UdpSocket.set_function("setBlocking",
        [](sf::UdpSocket& self, bool blocking) {
            static_cast<sf::Socket&>(self).setBlocking(blocking);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the socket is in blocking or non-blocking mode\n\n\\return `true` if the socket is blocking, `false` otherwise\n\n\\see `setBlocking`");
    LUASF_STUB_FUNCTION("sf.UdpSocket", "isBlocking", "fun(self: sf.UdpSocket): boolean");
    type_sf__UdpSocket.set_function("isBlocking",
        [](sf::UdpSocket& self) -> bool {
            return static_cast<sf::Socket&>(self).isBlocking();
        }
    );
    LUASF_STUB_DOC("\\brief Get the port to which the socket is bound locally\n\nIf the socket is not bound to a port, this function\nreturns 0.\n\n\\return Port to which the socket is bound\n\n\\see `bind`");
    LUASF_STUB_FUNCTION("sf.UdpSocket", "getLocalPort", "fun(self: sf.UdpSocket): integer");
    type_sf__UdpSocket.set_function("getLocalPort",
        [](sf::UdpSocket& self) -> unsigned short {
            return self.getLocalPort();
        }
    );
    LUASF_STUB_DOC("\\brief Bind the socket to a specific port\n\nBinding the socket to a port is necessary for being\nable to receive data on that port.\n\nWhen providing `sf::Socket::AnyPort` as port, the listener\nwill request an available port from the system.\nThe chosen port can be retrieved by calling `getLocalPort()`.\n\nSince the socket can only be bound to a single port at\nany given moment, if it is already bound when this\nfunction is called, it will be unbound from the previous\nport before being bound to the new one.\n\n\\param port    Port to bind the socket to\n\\param address Address of the interface to bind to\n\n\\return Status code\n\n\\see `unbind`, `getLocalPort`");
    LUASF_STUB_FUNCTION("sf.UdpSocket", "bind", "fun(self: sf.UdpSocket, port: integer, address: sf.IpAddress): sf.Socket.Status");
    LUASF_STUB_OVERLOAD("sf.UdpSocket", "bind", "fun(self: sf.UdpSocket, port: integer): sf.Socket.Status");
    type_sf__UdpSocket.set_function("bind",
        sol::overload(
            [](sf::UdpSocket& self, lua_sf::LuaIntegral<unsigned short> port, sf::IpAddress address) -> sf::Socket::Status {
                return self.bind(port.value(), address);
            },
            [](sf::UdpSocket& self, lua_sf::LuaIntegral<unsigned short> port) -> sf::Socket::Status {
                return self.bind(port.value());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Unbind the socket from the local port to which it is bound\n\nThe port that the socket was previously bound to is immediately\nmade available to the operating system after this function is called.\nThis means that a subsequent call to `bind()` will be able to re-bind\nthe port if no other process has done so in the mean time.\nIf the socket is not bound to a port, this function has no effect.\n\n\\see `bind`");
    LUASF_STUB_FUNCTION("sf.UdpSocket", "unbind", "fun(self: sf.UdpSocket)");
    type_sf__UdpSocket.set_function("unbind",
        [](sf::UdpSocket& self) {
            self.unbind();
        }
    );
    LUASF_STUB_DOC("\\brief Send a formatted packet of data to a remote peer\n\nMake sure that the packet size is not greater than\n`UdpSocket::MaxDatagramSize`, otherwise this function will\nfail and no data will be sent.\n\n\\param packet        Packet to send\n\\param remoteAddress Address of the receiver\n\\param remotePort    Port of the receiver to send the data to\n\n\\return Status code\n\n\\see `receive`");
    LUASF_STUB_FUNCTION("sf.UdpSocket", "send", "fun(self: sf.UdpSocket, packet: sf.Packet, remoteAddress: sf.IpAddress, remotePort: integer): sf.Socket.Status");
    LUASF_STUB_OVERLOAD("sf.UdpSocket", "send", "fun(self: sf.UdpSocket, data: any, remoteAddress: sf.IpAddress, remotePort: integer): sf.Socket.Status");
    type_sf__UdpSocket.set_function("send",
        sol::overload(
            [](sf::UdpSocket& self, sf::Packet& packet, sf::IpAddress remoteAddress, lua_sf::LuaIntegral<unsigned short> remotePort) -> sf::Socket::Status {
                return self.send(packet, remoteAddress, remotePort.value());
            },
            [](sf::UdpSocket& self, sol::object data, sf::IpAddress remoteAddress, lua_sf::LuaIntegral<unsigned short> remotePort) -> sf::Socket::Status {
                auto data_buffer = lua_sf::array_from_object<std::byte>(data);
                return self.send(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), remoteAddress, remotePort.value());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Receive raw data from a remote peer\n\nIn blocking mode, this function will wait until some\nbytes are actually received.\nBe careful to use a buffer which is large enough for\nthe data that you intend to receive, if it is too small\nthen an error will be returned and *all* the data will\nbe lost.\n\n\\param data          Pointer to the array to fill with the received bytes\n\\param size          Maximum number of bytes that can be received\n\\param received      This variable is filled with the actual number of bytes received\n\\param remoteAddress Address of the peer that sent the data\n\\param remotePort    Port of the peer that sent the data\n\n\\return Status code\n\n\\see `send`");
    LUASF_STUB_FUNCTION("sf.UdpSocket", "receive", "fun(self: sf.UdpSocket, size: integer): sf.Socket.Status, any, any, any, any");
    LUASF_STUB_OVERLOAD("sf.UdpSocket", "receive", "fun(self: sf.UdpSocket): sf.Socket.Status, any, any, any");
    type_sf__UdpSocket.set_function("receive",
        sol::overload(
            [](sf::UdpSocket& self, std::size_t size) {
                std::vector<std::byte> data_buffer(size);
                std::size_t received{};
                std::optional<sf::IpAddress> remoteAddress{};
                unsigned short remotePort{};
                auto result = self.receive(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), received, remoteAddress, remotePort);
                const auto data_buffer_written = static_cast<std::size_t>(received);
                if (data_buffer_written < data_buffer.size())
                    data_buffer.resize(data_buffer_written);
                return std::make_tuple(result, sol::as_table(data_buffer), std::move(received), std::move(remoteAddress), std::move(remotePort));
            },
            [](sf::UdpSocket& self) {
                sf::Packet packet{};
                std::optional<sf::IpAddress> remoteAddress{};
                unsigned short remotePort{};
                auto result = self.receive(packet, remoteAddress, remotePort);
                return std::make_tuple(result, std::move(packet), std::move(remoteAddress), std::move(remotePort));
            }
        )
    );
    LUASF_STUB_DOC("The maximum number of bytes that can be sent in a single UDP datagram");
    LUASF_STUB_VALUE("sf.UdpSocket", "MaxDatagramSize", "integer");
    table_sf__UdpSocket["MaxDatagramSize"] = sf::UdpSocket::MaxDatagramSize;
}
