#include "Network/bind_TcpSocket.hpp"

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

namespace { constexpr std::array<std::string_view, 26> docs = {
    "\\brief Specialized socket using the TCP protocol",
    "\\brief Default constructor",
    "\\brief Set the blocking state of the socket\n\nIn blocking mode, calls will not return until they have\ncompleted their task. For example, a call to Receive in\nblocking mode won't return until some data was actually\nreceived.\nIn non-blocking mode, calls will always return immediately,\nusing the return code to signal whether there was data\navailable or not.\nBy default, all sockets are blocking.\n\n\\param blocking `true` to set the socket as blocking, `false` for non-blocking\n\n\\see `isBlocking`",
    "\\brief Tell whether the socket is in blocking or non-blocking mode\n\n\\return `true` if the socket is blocking, `false` otherwise\n\n\\see `setBlocking`",
    "\\brief Get the port to which the socket is bound locally\n\nIf the socket is not connected, this function returns 0.\n\n\\return Port to which the socket is bound\n\n\\see `connect`, `getRemotePort`",
    "\\brief Get the address of the connected peer\n\nIf the socket is not connected, this function returns\nan unset optional.\n\n\\return Address of the remote peer\n\n\\see `getRemotePort`",
    "\\brief Get the port of the connected peer to which\nthe socket is connected\n\nIf the socket is not connected, this function returns 0.\n\n\\return Remote port to which the socket is connected\n\n\\see `getRemoteAddress`",
    "\\brief Connect the socket to a remote peer\n\nIn blocking mode, this function may take a while, especially\nif the remote peer is not reachable. The last parameter allows\nyou to stop trying to connect after a given timeout.\nIf the socket is already connected, the connection is\nforcibly disconnected before attempting to connect again.\n\n\\param remoteAddress Address of the remote peer\n\\param remotePort    Port of the remote peer\n\\param timeout       Optional maximum time to wait\n\n\\return Status code\n\n\\see `disconnect`",
    "\\brief Disconnect the socket from its remote peer\n\nThis function gracefully closes the connection. If the\nsocket is not connected, this function has no effect.\n\n\\see `connect`",
    "\\brief Set up transport layer security as a client\n\nOnce the TCP connection is connected, transport layer\nsecurity can be set up.\n\nAll the necessary cryptographic initialization will\nbe performed when this function is called.\n\nIf this function is called before the TCP connection is\nconnected, it will return `TlsStatus::NotConnected` and\nmust be called again once the TCP connection is connected.\n\nIf this function started TLS setup but could not finish\nit within this call e.g. because this socket was set to\nnon-blocking, it will return `TlsStatus::HandshakeStarted`\nand this function will have to be called repeatedly until\n`TlsStatus::HandshakeComplete` is returned. If this socket\nis blocking, `TlsStatus::HandshakeComplete` should be\nreturned within the same function call if TLS setup was\nsuccessful.\n\nIf `TlsStatus::Error` is returned, something went wrong\nwith TLS setup and the connection must be reconnected and\nTLS setup reattempted after it is connected again.\n\nIf verification is enabled, this function verifies the peer\nusing the system provided certificate store. If the peer\ndoes not have a certificate that was signed by a certificate\nauthority i.e. a self-signed certificate, the entire certificate\nchain can be provided using the alternative overload.\n\nServers that host multiple services under different names\nneed to know which of those services we want to connect\nto in order to reply with the correct certificate chain.\nServer name indication (SNI) is used for this purpose. The\nhostname provided to this function is sent to the server\nif it supports SNI in order for it to return the corresponding\ncertificate chain. The hostname is then used to verify the\ncertificate chain that was returned by the server. If the\nserver does not support SNI or only serves a single\ncertificate chain, the hostname will only be used for\nverification.\n\n\\param hostname   Hostname of the remote peer, used for verification\n\\param verifyPeer `true` to enable peer verification, `false` to disable it\n\n\\return TLS status code\n\n\\see `setupTlsServer`",
    "\\brief Set up transport layer security as a client\n\nOnce the TCP connection is connected, transport layer\nsecurity can be set up.\n\nAll the necessary cryptographic initialization will\nbe performed when this function is called.\n\nIf this function is called before the TCP connection is\nconnected, it will return `TlsStatus::NotConnected` and\nmust be called again once the TCP connection is connected.\n\nIf this function started TLS setup but could not finish\nit within this call e.g. because this socket was set to\nnon-blocking, it will return `TlsStatus::HandshakeStarted`\nand this function will have to be called repeatedly until\n`TlsStatus::HandshakeComplete` is returned. If this socket\nis blocking, `TlsStatus::HandshakeComplete` should be\nreturned within the same function call if TLS setup was\nsuccessful.\n\nIf `TlsStatus::Error` is returned, something went wrong\nwith TLS setup and the connection must be reconnected and\nTLS setup reattempted after it is connected again.\n\nServers that host multiple services under different names\nneed to know which of those services we want to connect\nto in order to reply with the correct certificate chain.\nServer name indication (SNI) is used for this purpose. The\nhostname provided to this function is sent to the server\nif it supports SNI in order for it to return the corresponding\ncertificate chain. The hostname is then used to verify the\ncertificate chain that was returned by the server. If the\nserver does not support SNI or only serves a single\ncertificate chain, the hostname will only be used for\nverification.\n\nWhen calling this overload, the certificate chain to verify\nthe host with has to be provided. Verification is always\nenabled when calling this overload.\n\nThe certificate data should be provided in PEM format.\n\nThis overload is provided to prevent a const char* argument\nresulting in the `bool verifyPeer` overload being called\ninstead of the `std::string_view` overload.\n\n\\param hostname             Hostname of the remote peer, used for verification\n\\param certificateChainData Null terminated string containing certificate chain data in PEM encoding\n\n\\return TLS status code\n\n\\see `setupTlsServer`",
    "\\brief Set up transport layer security as a client\n\nOnce the TCP connection is connected, transport layer\nsecurity can be set up.\n\nAll the necessary cryptographic initialization will\nbe performed when this function is called.\n\nIf this function is called before the TCP connection is\nconnected, it will return `TlsStatus::NotConnected` and\nmust be called again once the TCP connection is connected.\n\nIf this function started TLS setup but could not finish\nit within this call e.g. because this socket was set to\nnon-blocking, it will return `TlsStatus::HandshakeStarted`\nand this function will have to be called repeatedly until\n`TlsStatus::HandshakeComplete` is returned. If this socket\nis blocking, `TlsStatus::HandshakeComplete` should be\nreturned within the same function call if TLS setup was\nsuccessful.\n\nIf `TlsStatus::Error` is returned, something went wrong\nwith TLS setup and the connection must be reconnected and\nTLS setup reattempted after it is connected again.\n\nServers that host multiple services under different names\nneed to know which of those services we want to connect\nto in order to reply with the correct certificate chain.\nServer name indication (SNI) is used for this purpose. The\nhostname provided to this function is sent to the server\nif it supports SNI in order for it to return the corresponding\ncertificate chain. The hostname is then used to verify the\ncertificate chain that was returned by the server. If the\nserver does not support SNI or only serves a single\ncertificate chain, the hostname will only be used for\nverification.\n\nWhen calling this overload, the certificate chain to verify\nthe host with has to be provided. Verification is always\nenabled when calling this overload.\n\nThe certificate data can be provided in PEM or DER format.\n\n\\param hostname             Hostname of the remote peer, used for verification\n\\param certificateChainData Certificate chain data in PEM or DER encoding\n\\param certificateChainSize Size of the certificate chain data\n\n\\return TLS status code\n\n\\see `setupTlsServer`",
    "\\brief Set up transport layer security as a client\n\nOnce the TCP connection is connected, transport layer\nsecurity can be set up.\n\nAll the necessary cryptographic initialization will\nbe performed when this function is called.\n\nIf this function is called before the TCP connection is\nconnected, it will return `TlsStatus::NotConnected` and\nmust be called again once the TCP connection is connected.\n\nIf this function started TLS setup but could not finish\nit within this call e.g. because this socket was set to\nnon-blocking, it will return `TlsStatus::HandshakeStarted`\nand this function will have to be called repeatedly until\n`TlsStatus::HandshakeComplete` is returned. If this socket\nis blocking, `TlsStatus::HandshakeComplete` should be\nreturned within the same function call if TLS setup was\nsuccessful.\n\nIf `TlsStatus::Error` is returned, something went wrong\nwith TLS setup and the connection must be reconnected and\nTLS setup reattempted after it is connected again.\n\nServers that host multiple services under different names\nneed to know which of those services we want to connect\nto in order to reply with the correct certificate chain.\nServer name indication (SNI) is used for this purpose. The\nhostname provided to this function is sent to the server\nif it supports SNI in order for it to return the corresponding\ncertificate chain. The hostname is then used to verify the\ncertificate chain that was returned by the server. If the\nserver does not support SNI or only serves a single\ncertificate chain, the hostname will only be used for\nverification.\n\nWhen calling this overload, the certificate chain to verify\nthe host with has to be provided. Verification is always\nenabled when calling this overload.\n\nThe certificate data should be provided in PEM format.\n\n\\param hostname             Hostname of the remote peer, used for verification\n\\param certificateChainData Certificate chain data in PEM encoding\n\n\\return TLS status code\n\n\\see `setupTlsServer`",
    "\\brief Set up transport layer security as a server\n\nOnce the TCP connection is connected, transport layer\nsecurity can be set up.\n\nAll the necessary cryptographic initialization will\nbe performed when this function is called.\n\nIf this function is called before the TCP connection is\nconnected, it will return `TlsStatus::NotConnected` and\nmust be called again once the TCP connection is connected.\n\nIf this function started TLS setup but could not finish\nit within this call e.g. because this socket was set to\nnon-blocking, it will return `TlsStatus::HandshakeStarted`\nand this function will have to be called repeatedly until\n`TlsStatus::HandshakeComplete` is returned. If this socket\nis blocking, `TlsStatus::HandshakeComplete` should be\nreturned within the same function call if TLS setup was\nsuccessful.\n\nIf `TlsStatus::Error` is returned, something went wrong\nwith TLS setup and the connection must be disconnected.\nThe client must reconnect and reattempt TLS setup again.\n\nAs a server, a certificate chain as well as a private key\nmust be provided.\n\nThe certificate and private key data can be provided in\nPEM or DER format.\n\nIf the private key is secured by a password, the password\nmust be provided.\n\n\\param certificateChainData   Certificate chain data in PEM or DER encoding\n\\param certificateChainSize   Size of the certificate chain data\n\\param privateKeyData         Private key data in PEM or DER encoding\n\\param privateKeySize         Size of the private key data\n\\param privateKeyPasswordData Private key password data\n\\param privateKeyPasswordSize Size of the private key password data\n\n\\return TLS status code\n\n\\see `setupTlsClient`",
    "\\brief Set up transport layer security as a server\n\nOnce the TCP connection is connected, transport layer\nsecurity can be set up.\n\nAll the necessary cryptographic initialization will\nbe performed when this function is called.\n\nIf this function is called before the TCP connection is\nconnected, it will return `TlsStatus::NotConnected` and\nmust be called again once the TCP connection is connected.\n\nIf this function started TLS setup but could not finish\nit within this call e.g. because this socket was set to\nnon-blocking, it will return `TlsStatus::HandshakeStarted`\nand this function will have to be called repeatedly until\n`TlsStatus::HandshakeComplete` is returned. If this socket\nis blocking, `TlsStatus::HandshakeComplete` should be\nreturned within the same function call if TLS setup was\nsuccessful.\n\nIf `TlsStatus::Error` is returned, something went wrong\nwith TLS setup and the connection must be disconnected.\nThe client must reconnect and reattempt TLS setup again.\n\nAs a server, a certificate chain as well as a private key\nmust be provided.\n\nThe certificate and private key data should be provided in\nPEM format.\n\nIf the private key is secured by a password, the password\nmust be provided.\n\n\\param certificateChainData   Certificate chain data in PEM encoding\n\\param privateKeyData         Private key data in PEM encoding\n\\param privateKeyPasswordData Private key password if required\n\n\\return TLS status code\n\n\\see `setupTlsClient`",
    "\\brief Get the name of the TLS ciphersuite currently in use\n\n\\return TLS ciphersuite currently in use or `std::nullopt` if TLS is not set up\n\n\\see `setupTlsClient`, `setupTlsServer`",
    "\\brief Send raw data to the remote peer\n\nTo be able to handle partial sends over non-blocking\nsockets, use the `send(const void*, std::size_t, std::size_t&)`\noverload instead.\nThis function will fail if the socket is not connected.\n\n\\param data Pointer to the sequence of bytes to send\n\\param size Number of bytes to send\n\n\\return Status code\n\n\\see `receive`",
    "\\brief Send raw data to the remote peer\n\nThis function will fail if the socket is not connected.\n\n\\param data Pointer to the sequence of bytes to send\n\\param size Number of bytes to send\n\\param sent The number of bytes sent will be written here\n\n\\return Status code\n\n\\see `receive`",
    "\\brief Receive raw data from the remote peer\n\nIn blocking mode, this function will wait until some\nbytes are actually received.\nThis function will fail if the socket is not connected.\n\n\\param data     Pointer to the array to fill with the received bytes\n\\param size     Maximum number of bytes that can be received\n\\param received This variable is filled with the actual number of bytes received\n\n\\return Status code\n\n\\see `send`",
    "\\brief Send a formatted packet of data to the remote peer\n\nIn non-blocking mode, if this function returns `sf::Socket::Status::Partial`,\nyou \\em must retry sending the same unmodified packet before sending\nanything else in order to guarantee the packet arrives at the remote\npeer uncorrupted.\nThis function will fail if the socket is not connected.\n\n\\param packet Packet to send\n\n\\return Status code\n\n\\see `receive`",
    "\\brief Receive a formatted packet of data from the remote peer\n\nIn blocking mode, this function will wait until the whole packet\nhas been received.\nThis function will fail if the socket is not connected.\n\n\\param packet Packet to fill with the received data\n\n\\return Status code\n\n\\see `send`",
    "\\brief TLS status codes that may be returned by TLS setup",
    "TCP connection not yet connected",
    "TLS handshake has been started",
    "TLS handshake is complete, stream is encrypted",
    "An unexpected error happened",
}; }

void bind_TcpSocket(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__TcpSocket = lua_glue::BindClass<sf::TcpSocket>(sf, "TcpSocket");
    lua_glue::BindBase<sf::TcpSocket, sf::Socket>(type_sf__TcpSocket);
    lua_glue::Table table_sf__TcpSocket = sf["TcpSocket"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::TcpSocket>(lua);
    lua_glue::Table native_bases_sf__TcpSocket = lua.create_table();
    native_bases_sf__TcpSocket.add(lua["sf"]["Socket"].get<lua_glue::Table>());
    table_sf__TcpSocket.raw_set("__nativeBases", native_bases_sf__TcpSocket);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.TcpSocket", "sf.Socket");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "new", "fun(): sf.TcpSocket");
    lua_glue::BindCallable(type_sf__TcpSocket, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::TcpSocket>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "setBlocking", "fun(self: sf.TcpSocket, blocking: boolean)");
    lua_glue::BindCallable(type_sf__TcpSocket, "setBlocking",
        [](sf::TcpSocket& self, bool blocking) {
            static_cast<sf::Socket&>(self).setBlocking(blocking);
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "isBlocking", "fun(self: sf.TcpSocket): boolean");
    lua_glue::BindCallable(type_sf__TcpSocket, "isBlocking",
        [](const sf::TcpSocket& self) -> bool {
            return static_cast<const sf::Socket&>(self).isBlocking();
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "getLocalPort", "fun(self: sf.TcpSocket): integer");
    lua_glue::BindCallable(type_sf__TcpSocket, "getLocalPort",
        [](const sf::TcpSocket& self) -> unsigned short {
            return self.getLocalPort();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "getRemoteAddress", "fun(self: sf.TcpSocket): sf.IpAddress|nil");
    lua_glue::BindCallable(type_sf__TcpSocket, "getRemoteAddress",
        [lua](const sf::TcpSocket& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.getRemoteAddress());
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "getRemotePort", "fun(self: sf.TcpSocket): integer");
    lua_glue::BindCallable(type_sf__TcpSocket, "getRemotePort",
        [](const sf::TcpSocket& self) -> unsigned short {
            return self.getRemotePort();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "connect", "fun(self: sf.TcpSocket, remoteAddress: sf.IpAddress, remotePort: integer, timeout?: sf.Time): sf.Socket.Status");
    lua_glue::BindCallable(type_sf__TcpSocket, "connect",
        [](sf::TcpSocket& self, sf::IpAddress remoteAddress, lua_sf::LuaIntegral<unsigned short> remotePort, sf::Time timeout) -> sf::Socket::Status {
            return self.connect(remoteAddress, remotePort.value(), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Time>(sf::Time::Zero);
        }}},
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "disconnect", "fun(self: sf.TcpSocket)");
    lua_glue::BindCallable(type_sf__TcpSocket, "disconnect",
        [](sf::TcpSocket& self) {
            self.disconnect();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "setupTlsClient", "fun(self: sf.TcpSocket, hostname: string, verifyPeer?: boolean): sf.TcpSocket.TlsStatus");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "setupTlsClient", "fun(self: sf.TcpSocket, hostname: string, certificateChainData: string): sf.TcpSocket.TlsStatus");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "setupTlsClient", "fun(self: sf.TcpSocket, hostname: string, certificateChainData: any): sf.TcpSocket.TlsStatus");
    lua_glue::BindCallable(type_sf__TcpSocket, "setupTlsClient",
        [](sf::TcpSocket& self, std::string hostname, bool verifyPeer) -> sf::TcpSocket::TlsStatus {
            return self.setupTlsClient(lua_sf::to_sf_string(hostname), verifyPeer);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(true);
        }}},
        docs[9]
    );
    lua_glue::BindCallable(type_sf__TcpSocket, "setupTlsClient",
        [](sf::TcpSocket& self, std::string hostname, std::string certificateChainData) -> sf::TcpSocket::TlsStatus {
            return self.setupTlsClient(lua_sf::to_sf_string(hostname), certificateChainData.c_str());
        },
        docs[10]
    );
    lua_glue::BindCallable(type_sf__TcpSocket, "setupTlsClient",
        [](sf::TcpSocket& self, std::string hostname, lua_glue::Object certificateChainData) -> sf::TcpSocket::TlsStatus {
            auto certificateChainData_buffer = lua_sf::array_from_object<std::byte>(certificateChainData);
            return self.setupTlsClient(lua_sf::to_sf_string(hostname), certificateChainData_buffer.data(), static_cast<std::size_t>(certificateChainData_buffer.size()));
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "setupTlsServer", "fun(self: sf.TcpSocket, certificateChainData: string, privateKeyData: string, privateKeyPasswordData?: string): sf.TcpSocket.TlsStatus");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "setupTlsServer", "fun(self: sf.TcpSocket, certificateChainData: any, privateKeyData: any, privateKeyPasswordData: any): sf.TcpSocket.TlsStatus");
    lua_glue::BindCallable(type_sf__TcpSocket, "setupTlsServer",
        [](sf::TcpSocket& self, std::string certificateChainData, std::string privateKeyData, std::string privateKeyPasswordData) -> sf::TcpSocket::TlsStatus {
            return self.setupTlsServer(certificateChainData, privateKeyData, privateKeyPasswordData);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::string(std::string_view{ });
        }}},
        docs[14]
    );
    lua_glue::BindCallable(type_sf__TcpSocket, "setupTlsServer",
        [](sf::TcpSocket& self, lua_glue::Object certificateChainData, lua_glue::Object privateKeyData, lua_glue::Object privateKeyPasswordData) -> sf::TcpSocket::TlsStatus {
            auto certificateChainData_buffer = lua_sf::array_from_object<std::byte>(certificateChainData);
            auto privateKeyData_buffer = lua_sf::array_from_object<std::byte>(privateKeyData);
            auto privateKeyPasswordData_buffer = lua_sf::array_from_object<std::byte>(privateKeyPasswordData);
            return self.setupTlsServer(certificateChainData_buffer.data(), static_cast<std::size_t>(certificateChainData_buffer.size()), privateKeyData_buffer.data(), static_cast<std::size_t>(privateKeyData_buffer.size()), privateKeyPasswordData_buffer.data(), static_cast<std::size_t>(privateKeyPasswordData_buffer.size()));
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "getCurrentCiphersuiteName", "fun(self: sf.TcpSocket): string|nil");
    lua_glue::BindCallable(type_sf__TcpSocket, "getCurrentCiphersuiteName",
        [lua](const sf::TcpSocket& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.getCurrentCiphersuiteName());
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "send", "fun(self: sf.TcpSocket, packet: sf.Packet): sf.Socket.Status");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "send", "fun(self: sf.TcpSocket, data: any): sf.Socket.Status, any");
    lua_glue::BindCallable(type_sf__TcpSocket, "send",
        [](sf::TcpSocket& self, sf::Packet& packet) -> sf::Socket::Status {
            return self.send(packet);
        },
        docs[19]
    );
    lua_glue::BindCallable(type_sf__TcpSocket, "send",
        [](sf::TcpSocket& self, lua_glue::Object data) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            std::size_t sent{};
            auto result = self.send(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), sent);
            return std::make_tuple(result, std::move(sent));
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.TcpSocket", "receive", "fun(self: sf.TcpSocket, size: integer): sf.Socket.Status, any, any");
    LUASF_STUB_OVERLOAD("sf.TcpSocket", "receive", "fun(self: sf.TcpSocket): sf.Socket.Status, any");
    lua_glue::BindCallable(type_sf__TcpSocket, "receive",
        [](sf::TcpSocket& self, std::size_t size) {
            std::vector<std::byte> data_buffer(size);
            std::size_t received{};
            auto result = self.receive(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()), received);
            const auto data_buffer_written = static_cast<std::size_t>(received);
            if (data_buffer_written < data_buffer.size())
                data_buffer.resize(data_buffer_written);
            return std::make_tuple(result, lua_glue::AsTable(data_buffer), std::move(received));
        },
        docs[18]
    );
    lua_glue::BindCallable(type_sf__TcpSocket, "receive",
        [](sf::TcpSocket& self) {
            sf::Packet packet{};
            auto result = self.receive(packet);
            return std::make_tuple(result, std::move(packet));
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_CLASS("sf.TcpSocket.TlsStatus");
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FIELD("NotConnected", "sf.TcpSocket.TlsStatus");
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FIELD("HandshakeStarted", "sf.TcpSocket.TlsStatus");
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FIELD("HandshakeComplete", "sf.TcpSocket.TlsStatus");
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FIELD("Error", "sf.TcpSocket.TlsStatus");
    lua_glue::BindEnum<sf::TcpSocket::TlsStatus>(table_sf__TcpSocket, "TlsStatus", {
        {"NotConnected", sf::TcpSocket::TlsStatus::NotConnected},
        {"HandshakeStarted", sf::TcpSocket::TlsStatus::HandshakeStarted},
        {"HandshakeComplete", sf::TcpSocket::TlsStatus::HandshakeComplete},
        {"Error", sf::TcpSocket::TlsStatus::Error}
    });
}
