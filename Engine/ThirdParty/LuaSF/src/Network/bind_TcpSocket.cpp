#include "Network/bind_TcpSocket.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_TcpSocket(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__TcpSocket = sf.new_usertype<sf::TcpSocket>("TcpSocket",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Socket>()
    );
    sol::table table_sf__TcpSocket = sf["TcpSocket"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::TcpSocket>(lua);
    sol::table native_bases_sf__TcpSocket = lua.create_table();
    native_bases_sf__TcpSocket.add(lua["sf"]["Socket"].get<sol::table>());
    table_sf__TcpSocket.raw_set("__nativeBases", native_bases_sf__TcpSocket);
    LUASF_STUB_DOC("\\brief Specialized socket using the TCP protocol");
    LUASF_STUB_CLASS("sf.TcpSocket", "sf.Socket");
    LUASF_STUB_DOC("\\brief Default constructor");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "new", "fun(): sf.TcpSocket");
    type_sf__TcpSocket.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::TcpSocket>();
        }
    ));
    LUASF_STUB_DOC("\\brief Set the blocking state of the socket\n\nIn blocking mode, calls will not return until they have\ncompleted their task. For example, a call to Receive in\nblocking mode won't return until some data was actually\nreceived.\nIn non-blocking mode, calls will always return immediately,\nusing the return code to signal whether there was data\navailable or not.\nBy default, all sockets are blocking.\n\n\\param blocking `true` to set the socket as blocking, `false` for non-blocking\n\n\\see `isBlocking`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "setBlocking", "fun(self: sf.TcpSocket, blocking: boolean)");
    type_sf__TcpSocket.set_function("setBlocking",
        [](sf::TcpSocket& self, bool blocking) {
            static_cast<sf::Socket&>(self).setBlocking(blocking);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the socket is in blocking or non-blocking mode\n\n\\return `true` if the socket is blocking, `false` otherwise\n\n\\see `setBlocking`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "isBlocking", "fun(self: sf.TcpSocket): boolean");
    type_sf__TcpSocket.set_function("isBlocking",
        [](sf::TcpSocket& self) -> bool {
            return static_cast<sf::Socket&>(self).isBlocking();
        }
    );
    LUASF_STUB_DOC("\\brief Get the port to which the socket is bound locally\n\nIf the socket is not connected, this function returns 0.\n\n\\return Port to which the socket is bound\n\n\\see `connect`, `getRemotePort`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "getLocalPort", "fun(self: sf.TcpSocket): integer");
    type_sf__TcpSocket.set_function("getLocalPort",
        [](sf::TcpSocket& self) -> unsigned short {
            return self.getLocalPort();
        }
    );
    LUASF_STUB_DOC("\\brief Get the address of the connected peer\n\nIf the socket is not connected, this function returns\nan unset optional.\n\n\\return Address of the remote peer\n\n\\see `getRemotePort`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "getRemoteAddress", "fun(self: sf.TcpSocket): sf.IpAddress|nil");
    type_sf__TcpSocket.set_function("getRemoteAddress",
        [lua](sf::TcpSocket& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.getRemoteAddress());
        }
    );
    LUASF_STUB_DOC("\\brief Get the port of the connected peer to which\nthe socket is connected\n\nIf the socket is not connected, this function returns 0.\n\n\\return Remote port to which the socket is connected\n\n\\see `getRemoteAddress`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "getRemotePort", "fun(self: sf.TcpSocket): integer");
    type_sf__TcpSocket.set_function("getRemotePort",
        [](sf::TcpSocket& self) -> unsigned short {
            return self.getRemotePort();
        }
    );
    LUASF_STUB_DOC("\\brief Connect the socket to a remote peer\n\nIn blocking mode, this function may take a while, especially\nif the remote peer is not reachable. The last parameter allows\nyou to stop trying to connect after a given timeout.\nIf the socket is already connected, the connection is\nforcibly disconnected before attempting to connect again.\n\n\\param remoteAddress Address of the remote peer\n\\param remotePort    Port of the remote peer\n\\param timeout       Optional maximum time to wait\n\n\\return Status code\n\n\\see `disconnect`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "connect", "fun(self: sf.TcpSocket, remoteAddress: sf.IpAddress, remotePort: integer, timeout: sf.Time): sf.Socket.Status");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "connect", "fun(self: sf.TcpSocket, remoteAddress: sf.IpAddress, remotePort: integer): sf.Socket.Status");
    type_sf__TcpSocket.set_function("connect",
        sol::overload(
            [](sf::TcpSocket& self, sf::IpAddress remoteAddress, lua_sf::LuaIntegral<unsigned short> remotePort, sf::Time timeout) -> sf::Socket::Status {
                return self.connect(remoteAddress, remotePort.value(), timeout);
            },
            [](sf::TcpSocket& self, sf::IpAddress remoteAddress, lua_sf::LuaIntegral<unsigned short> remotePort) -> sf::Socket::Status {
                return self.connect(remoteAddress, remotePort.value());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Disconnect the socket from its remote peer\n\nThis function gracefully closes the connection. If the\nsocket is not connected, this function has no effect.\n\n\\see `connect`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "disconnect", "fun(self: sf.TcpSocket)");
    type_sf__TcpSocket.set_function("disconnect",
        [](sf::TcpSocket& self) {
            self.disconnect();
        }
    );
    LUASF_STUB_DOC("\\brief Set up transport layer security as a client\n\nOnce the TCP connection is connected, transport layer\nsecurity can be set up.\n\nAll the necessary cryptographic initialization will\nbe performed when this function is called.\n\nIf this function is called before the TCP connection is\nconnected, it will return `TlsStatus::NotConnected` and\nmust be called again once the TCP connection is connected.\n\nIf this function started TLS setup but could not finish\nit within this call e.g. because this socket was set to\nnon-blocking, it will return `TlsStatus::HandshakeStarted`\nand this function will have to be called repeatedly until\n`TlsStatus::HandshakeComplete` is returned. If this socket\nis blocking, `TlsStatus::HandshakeComplete` should be\nreturned within the same function call if TLS setup was\nsuccessful.\n\nIf `TlsStatus::Error` is returned, something went wrong\nwith TLS setup and the connection must be reconnected and\nTLS setup reattempted after it is connected again.\n\nIf verification is enabled, this function verifies the peer\nusing the system provided certificate store. If the peer\ndoes not have a certificate that was signed by a certificate\nauthority i.e. a self-signed certificate, the entire certificate\nchain can be provided using the alternative overload.\n\nServers that host multiple services under different names\nneed to know which of those services we want to connect\nto in order to reply with the correct certificate chain.\nServer name indication (SNI) is used for this purpose. The\nhostname provided to this function is sent to the server\nif it supports SNI in order for it to return the corresponding\ncertificate chain. The hostname is then used to verify the\ncertificate chain that was returned by the server. If the\nserver does not support SNI or only serves a single\ncertificate chain, the hostname will only be used for\nverification.\n\n\\param hostname   Hostname of the remote peer, used for verification\n\\param verifyPeer `true` to enable peer verification, `false` to disable it\n\n\\return TLS status code\n\n\\see `setupTlsServer`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "setupTlsClient", "fun(self: sf.TcpSocket, hostname: string, verifyPeer: boolean): sf.TcpSocket.TlsStatus");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "setupTlsClient", "fun(self: sf.TcpSocket, hostname: string, certificateChainData: string): sf.TcpSocket.TlsStatus");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "setupTlsClient", "fun(self: sf.TcpSocket, hostname: string): sf.TcpSocket.TlsStatus");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "setupTlsClient", "fun(self: sf.TcpSocket, hostname: string, certificateChainData: any): sf.TcpSocket.TlsStatus");
    type_sf__TcpSocket.set_function("setupTlsClient",
        sol::overload(
            [](sf::TcpSocket& self, std::string hostname, bool verifyPeer) -> sf::TcpSocket::TlsStatus {
                return self.setupTlsClient(lua_sf::to_sf_string(hostname), verifyPeer);
            },
            [](sf::TcpSocket& self, std::string hostname, std::string certificateChainData) -> sf::TcpSocket::TlsStatus {
                return self.setupTlsClient(lua_sf::to_sf_string(hostname), certificateChainData.c_str());
            },
            [](sf::TcpSocket& self, std::string hostname) -> sf::TcpSocket::TlsStatus {
                return self.setupTlsClient(lua_sf::to_sf_string(hostname));
            },
            [](sf::TcpSocket& self, std::string hostname, sol::object certificateChainData) -> sf::TcpSocket::TlsStatus {
                auto certificateChainData_buffer = lua_sf::array_from_object<std::byte>(certificateChainData);
                return self.setupTlsClient(lua_sf::to_sf_string(hostname), certificateChainData_buffer.data(), static_cast<std::size_t>(certificateChainData_buffer.size()));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Set up transport layer security as a server\n\nOnce the TCP connection is connected, transport layer\nsecurity can be set up.\n\nAll the necessary cryptographic initialization will\nbe performed when this function is called.\n\nIf this function is called before the TCP connection is\nconnected, it will return `TlsStatus::NotConnected` and\nmust be called again once the TCP connection is connected.\n\nIf this function started TLS setup but could not finish\nit within this call e.g. because this socket was set to\nnon-blocking, it will return `TlsStatus::HandshakeStarted`\nand this function will have to be called repeatedly until\n`TlsStatus::HandshakeComplete` is returned. If this socket\nis blocking, `TlsStatus::HandshakeComplete` should be\nreturned within the same function call if TLS setup was\nsuccessful.\n\nIf `TlsStatus::Error` is returned, something went wrong\nwith TLS setup and the connection must be disconnected.\nThe client must reconnect and reattempt TLS setup again.\n\nAs a server, a certificate chain as well as a private key\nmust be provided.\n\nThe certificate and private key data should be provided in\nPEM format.\n\nIf the private key is secured by a password, the password\nmust be provided.\n\n\\param certificateChainData   Certificate chain data in PEM encoding\n\\param privateKeyData         Private key data in PEM encoding\n\\param privateKeyPasswordData Private key password if required\n\n\\return TLS status code\n\n\\see `setupTlsClient`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "setupTlsServer", "fun(self: sf.TcpSocket, certificateChainData: string, privateKeyData: string, privateKeyPasswordData: string): sf.TcpSocket.TlsStatus");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "setupTlsServer", "fun(self: sf.TcpSocket, certificateChainData: string, privateKeyData: string): sf.TcpSocket.TlsStatus");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "setupTlsServer", "fun(self: sf.TcpSocket, certificateChainData: any, privateKeyData: any, privateKeyPasswordData: any): sf.TcpSocket.TlsStatus");
    type_sf__TcpSocket.set_function("setupTlsServer",
        sol::overload(
            [](sf::TcpSocket& self, std::string certificateChainData, std::string privateKeyData, std::string privateKeyPasswordData) -> sf::TcpSocket::TlsStatus {
                return self.setupTlsServer(certificateChainData, privateKeyData, privateKeyPasswordData);
            },
            [](sf::TcpSocket& self, std::string certificateChainData, std::string privateKeyData) -> sf::TcpSocket::TlsStatus {
                return self.setupTlsServer(certificateChainData, privateKeyData);
            },
            [](sf::TcpSocket& self, sol::object certificateChainData, sol::object privateKeyData, sol::object privateKeyPasswordData) -> sf::TcpSocket::TlsStatus {
                auto certificateChainData_buffer = lua_sf::array_from_object<std::byte>(certificateChainData);
                auto privateKeyData_buffer = lua_sf::array_from_object<std::byte>(privateKeyData);
                auto privateKeyPasswordData_buffer = lua_sf::array_from_object<std::byte>(privateKeyPasswordData);
                return self.setupTlsServer(certificateChainData_buffer.data(), static_cast<std::size_t>(certificateChainData_buffer.size()), privateKeyData_buffer.data(), static_cast<std::size_t>(privateKeyData_buffer.size()), privateKeyPasswordData_buffer.data(), static_cast<std::size_t>(privateKeyPasswordData_buffer.size()));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the name of the TLS ciphersuite currently in use\n\n\\return TLS ciphersuite currently in use or `std::nullopt` if TLS is not set up\n\n\\see `setupTlsClient`, `setupTlsServer`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "getCurrentCiphersuiteName", "fun(self: sf.TcpSocket): string|nil");
    type_sf__TcpSocket.set_function("getCurrentCiphersuiteName",
        [lua](sf::TcpSocket& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.getCurrentCiphersuiteName());
        }
    );
    LUASF_STUB_DOC("\\brief Send a formatted packet of data to the remote peer\n\nIn non-blocking mode, if this function returns `sf::Socket::Status::Partial`,\nyou \\em must retry sending the same unmodified packet before sending\nanything else in order to guarantee the packet arrives at the remote\npeer uncorrupted.\nThis function will fail if the socket is not connected.\n\n\\param packet Packet to send\n\n\\return Status code\n\n\\see `receive`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "send", "fun(self: sf.TcpSocket, packet: sf.Packet): sf.Socket.Status");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "send", "fun(self: sf.TcpSocket, data: any): sf.Socket.Status, any");
    type_sf__TcpSocket.set_function("send",
        sol::overload(
            [](sf::TcpSocket& self, sf::Packet& packet) -> sf::Socket::Status {
                return self.send(packet);
            },
            [](sf::TcpSocket& self, sol::object data) {
                auto data_buffer = lua_sf::array_from_object<std::byte>(data);
                std::size_t sent{};
                auto result = self.send(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), sent);
                return std::make_tuple(result, std::move(sent));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Receive raw data from the remote peer\n\nIn blocking mode, this function will wait until some\nbytes are actually received.\nThis function will fail if the socket is not connected.\n\n\\param data     Pointer to the array to fill with the received bytes\n\\param size     Maximum number of bytes that can be received\n\\param received This variable is filled with the actual number of bytes received\n\n\\return Status code\n\n\\see `send`");
    LUASF_STUB_FUNCTION("sf.TcpSocket", "receive", "fun(self: sf.TcpSocket, size: integer): sf.Socket.Status, any, any");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "receive", "fun(self: sf.TcpSocket): sf.Socket.Status, any");
    type_sf__TcpSocket.set_function("receive",
        sol::overload(
            [](sf::TcpSocket& self, std::size_t size) {
                std::vector<std::byte> data_buffer(size);
                std::size_t received{};
                auto result = self.receive(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), received);
                const auto data_buffer_written = static_cast<std::size_t>(received);
                if (data_buffer_written < data_buffer.size())
                    data_buffer.resize(data_buffer_written);
                return std::make_tuple(result, sol::as_table(data_buffer), std::move(received));
            },
            [](sf::TcpSocket& self) {
                sf::Packet packet{};
                auto result = self.receive(packet);
                return std::make_tuple(result, std::move(packet));
            }
        )
    );
    LUASF_STUB_DOC("\\brief TLS status codes that may be returned by TLS setup");
    LUASF_STUB_CLASS("sf.TcpSocket.TlsStatus");
    LUASF_STUB_DOC("TCP connection not yet connected");
    LUASF_STUB_FIELD("NotConnected", "sf.TcpSocket.TlsStatus");
    LUASF_STUB_DOC("TLS handshake has been started");
    LUASF_STUB_FIELD("HandshakeStarted", "sf.TcpSocket.TlsStatus");
    LUASF_STUB_DOC("TLS handshake is complete, stream is encrypted");
    LUASF_STUB_FIELD("HandshakeComplete", "sf.TcpSocket.TlsStatus");
    LUASF_STUB_DOC("An unexpected error happened");
    LUASF_STUB_FIELD("Error", "sf.TcpSocket.TlsStatus");
    table_sf__TcpSocket.new_enum("TlsStatus",
        "NotConnected", sf::TcpSocket::TlsStatus::NotConnected,
        "HandshakeStarted", sf::TcpSocket::TlsStatus::HandshakeStarted,
        "HandshakeComplete", sf::TcpSocket::TlsStatus::HandshakeComplete,
        "Error", sf::TcpSocket::TlsStatus::Error
    );
}
