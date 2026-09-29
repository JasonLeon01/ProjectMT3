#include "Network/bind_Socket.hpp"

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

namespace { constexpr std::array<std::string_view, 10> docs = {
    "\\brief Base class for all the socket types",
    "\\brief Set the blocking state of the socket\n\nIn blocking mode, calls will not return until they have\ncompleted their task. For example, a call to Receive in\nblocking mode won't return until some data was actually\nreceived.\nIn non-blocking mode, calls will always return immediately,\nusing the return code to signal whether there was data\navailable or not.\nBy default, all sockets are blocking.\n\n\\param blocking `true` to set the socket as blocking, `false` for non-blocking\n\n\\see `isBlocking`",
    "\\brief Tell whether the socket is in blocking or non-blocking mode\n\n\\return `true` if the socket is blocking, `false` otherwise\n\n\\see `setBlocking`",
    "\\brief Status codes that may be returned by socket functions",
    "The socket has sent / received the data",
    "The socket is not ready to send / receive data yet",
    "The socket sent a part of the data",
    "The TCP socket has been disconnected",
    "An unexpected error happened",
    "Special value that tells the system to pick any available port",
}; }

void bind_Socket(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Socket = lua_glue::BindClass<sf::Socket>(sf, "Socket");
    lua_glue::Table table_sf__Socket = sf["Socket"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Socket>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Socket");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Socket", "setBlocking", "fun(self: sf.Socket, blocking: boolean)");
    lua_glue::BindCallable(type_sf__Socket, "setBlocking",
        [](sf::Socket& self, bool blocking) {
            self.setBlocking(blocking);
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Socket", "isBlocking", "fun(self: sf.Socket): boolean");
    lua_glue::BindCallable(type_sf__Socket, "isBlocking",
        [](const sf::Socket& self) -> bool {
            return self.isBlocking();
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_CLASS("sf.Socket.Status");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("Done", "sf.Socket.Status");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("NotReady", "sf.Socket.Status");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("Partial", "sf.Socket.Status");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FIELD("Disconnected", "sf.Socket.Status");
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FIELD("Error", "sf.Socket.Status");
    lua_glue::BindEnum<sf::Socket::Status>(table_sf__Socket, "Status", {
        {"Done", sf::Socket::Status::Done},
        {"NotReady", sf::Socket::Status::NotReady},
        {"Partial", sf::Socket::Status::Partial},
        {"Disconnected", sf::Socket::Status::Disconnected},
        {"Error", sf::Socket::Status::Error}
    });
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_VALUE("sf.Socket", "AnyPort", "integer");
    lua_glue::BindStaticAttr<const unsigned short>(table_sf__Socket, "AnyPort", &sf::Socket::AnyPort);
}
