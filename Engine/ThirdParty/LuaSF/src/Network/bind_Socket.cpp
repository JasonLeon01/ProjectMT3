#include "Network/bind_Socket.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Socket(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Socket = sf.new_usertype<sf::Socket>("Socket", sol::no_constructor);
    sol::table table_sf__Socket = sf["Socket"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Socket>(lua);
    LUASF_STUB_DOC("\\brief Base class for all the socket types");
    LUASF_STUB_CLASS("sf.Socket");
    LUASF_STUB_DOC("\\brief Set the blocking state of the socket\n\nIn blocking mode, calls will not return until they have\ncompleted their task. For example, a call to Receive in\nblocking mode won't return until some data was actually\nreceived.\nIn non-blocking mode, calls will always return immediately,\nusing the return code to signal whether there was data\navailable or not.\nBy default, all sockets are blocking.\n\n\\param blocking `true` to set the socket as blocking, `false` for non-blocking\n\n\\see `isBlocking`");
    LUASF_STUB_FUNCTION("sf.Socket", "setBlocking", "fun(self: sf.Socket, blocking: boolean)");
    type_sf__Socket.set_function("setBlocking",
        [](sf::Socket& self, bool blocking) {
            self.setBlocking(blocking);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the socket is in blocking or non-blocking mode\n\n\\return `true` if the socket is blocking, `false` otherwise\n\n\\see `setBlocking`");
    LUASF_STUB_FUNCTION("sf.Socket", "isBlocking", "fun(self: sf.Socket): boolean");
    type_sf__Socket.set_function("isBlocking",
        [](sf::Socket& self) -> bool {
            return self.isBlocking();
        }
    );
    LUASF_STUB_DOC("\\brief Status codes that may be returned by socket functions");
    LUASF_STUB_CLASS("sf.Socket.Status");
    LUASF_STUB_DOC("The socket has sent / received the data");
    LUASF_STUB_FIELD("Done", "sf.Socket.Status");
    LUASF_STUB_DOC("The socket is not ready to send / receive data yet");
    LUASF_STUB_FIELD("NotReady", "sf.Socket.Status");
    LUASF_STUB_DOC("The socket sent a part of the data");
    LUASF_STUB_FIELD("Partial", "sf.Socket.Status");
    LUASF_STUB_DOC("The TCP socket has been disconnected");
    LUASF_STUB_FIELD("Disconnected", "sf.Socket.Status");
    LUASF_STUB_DOC("An unexpected error happened");
    LUASF_STUB_FIELD("Error", "sf.Socket.Status");
    table_sf__Socket.new_enum("Status",
        "Done", sf::Socket::Status::Done,
        "NotReady", sf::Socket::Status::NotReady,
        "Partial", sf::Socket::Status::Partial,
        "Disconnected", sf::Socket::Status::Disconnected,
        "Error", sf::Socket::Status::Error
    );
    LUASF_STUB_DOC("Special value that tells the system to pick any available port");
    LUASF_STUB_VALUE("sf.Socket", "AnyPort", "integer");
    table_sf__Socket["AnyPort"] = sf::Socket::AnyPort;
}
