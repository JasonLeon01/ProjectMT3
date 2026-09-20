#include "Network/bind_TcpListener.hpp"

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

namespace { constexpr std::array<std::string_view, 8> docs = {
    "\\brief Socket that listens to new TCP connections",
    "\\brief Default constructor",
    "\\brief Set the blocking state of the socket\n\nIn blocking mode, calls will not return until they have\ncompleted their task. For example, a call to Receive in\nblocking mode won't return until some data was actually\nreceived.\nIn non-blocking mode, calls will always return immediately,\nusing the return code to signal whether there was data\navailable or not.\nBy default, all sockets are blocking.\n\n\\param blocking `true` to set the socket as blocking, `false` for non-blocking\n\n\\see `isBlocking`",
    "\\brief Tell whether the socket is in blocking or non-blocking mode\n\n\\return `true` if the socket is blocking, `false` otherwise\n\n\\see `setBlocking`",
    "\\brief Get the port to which the socket is bound locally\n\nIf the socket is not listening to a port, this function\nreturns 0.\n\n\\return Port to which the socket is bound\n\n\\see `listen`",
    "\\brief Start listening for incoming connection attempts\n\nThis function makes the socket start listening on the\nspecified port, waiting for incoming connection attempts.\n\nIf the socket is already listening on a port when this\nfunction is called, it will stop listening on the old\nport before starting to listen on the new port.\n\nWhen providing `sf::Socket::AnyPort` as port, the listener\nwill request an available port from the system.\nThe chosen port can be retrieved by calling `getLocalPort()`.\n\n\\param port    Port to listen on for incoming connection attempts\n\\param address Address of the interface to listen on\n\n\\return Status code\n\n\\see `accept`, `close`",
    "\\brief Stop listening and close the socket\n\nThis function gracefully stops the listener. If the\nsocket is not listening, this function has no effect.\n\n\\see `listen`",
    "\\brief Accept a new connection\n\nIf the socket is in blocking mode, this function will\nnot return until a connection is actually received.\n\n\\param socket Socket that will hold the new connection\n\n\\return Status code\n\n\\see `listen`",
}; }

void bind_TcpListener(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__TcpListener = lua_glue::BindClass<sf::TcpListener>(sf, "TcpListener");
    lua_glue::BindBase<sf::TcpListener, sf::Socket>(type_sf__TcpListener);
    lua_glue::Table table_sf__TcpListener = sf["TcpListener"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::TcpListener>(lua);
    lua_glue::Table native_bases_sf__TcpListener = lua.create_table();
    native_bases_sf__TcpListener.add(lua["sf"]["Socket"].get<lua_glue::Table>());
    table_sf__TcpListener.raw_set("__nativeBases", native_bases_sf__TcpListener);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.TcpListener", "sf.Socket");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.TcpListener", "new", "fun(): sf.TcpListener");
    lua_glue::BindCallable(type_sf__TcpListener, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::TcpListener>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.TcpListener", "setBlocking", "fun(self: sf.TcpListener, blocking: boolean)");
    lua_glue::BindCallable(type_sf__TcpListener, "setBlocking",
        [](sf::TcpListener& self, bool blocking) {
            static_cast<sf::Socket&>(self).setBlocking(blocking);
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.TcpListener", "isBlocking", "fun(self: sf.TcpListener): boolean");
    lua_glue::BindCallable(type_sf__TcpListener, "isBlocking",
        [](const sf::TcpListener& self) -> bool {
            return static_cast<const sf::Socket&>(self).isBlocking();
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.TcpListener", "getLocalPort", "fun(self: sf.TcpListener): integer");
    lua_glue::BindCallable(type_sf__TcpListener, "getLocalPort",
        [](const sf::TcpListener& self) -> unsigned short {
            return self.getLocalPort();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.TcpListener", "listen", "fun(self: sf.TcpListener, port: integer, address?: sf.IpAddress): sf.Socket.Status");
    lua_glue::BindCallable(type_sf__TcpListener, "listen",
        [](sf::TcpListener& self, lua_sf::LuaIntegral<unsigned short> port, sf::IpAddress address) -> sf::Socket::Status {
            return self.listen(port.value(), address);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::IpAddress>(sf::IpAddress::Any);
        }}},
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.TcpListener", "close", "fun(self: sf.TcpListener)");
    lua_glue::BindCallable(type_sf__TcpListener, "close",
        [](sf::TcpListener& self) {
            self.close();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.TcpListener", "accept", "fun(self: sf.TcpListener): sf.Socket.Status, any");
    lua_glue::BindCallable(type_sf__TcpListener, "accept",
        [](sf::TcpListener& self) {
            sf::TcpSocket socket{};
            auto result = self.accept(socket);
            return std::make_tuple(result, std::move(socket));
        },
        docs[7]
    );
}
