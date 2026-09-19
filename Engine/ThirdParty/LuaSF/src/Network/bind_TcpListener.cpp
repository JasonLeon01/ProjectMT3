#include "Network/bind_TcpListener.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_TcpListener(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__TcpListener = sf.new_usertype<sf::TcpListener>("TcpListener",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Socket>()
    );
    sol::table table_sf__TcpListener = sf["TcpListener"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::TcpListener>(lua);
    sol::table native_bases_sf__TcpListener = lua.create_table();
    native_bases_sf__TcpListener.add(lua["sf"]["Socket"].get<sol::table>());
    table_sf__TcpListener.raw_set("__nativeBases", native_bases_sf__TcpListener);
    LUASF_STUB_DOC("\\brief Socket that listens to new TCP connections");
    LUASF_STUB_CLASS("sf.TcpListener", "sf.Socket");
    LUASF_STUB_DOC("\\brief Default constructor");
    LUASF_STUB_FUNCTION("sf.TcpListener", "new", "fun(): sf.TcpListener");
    type_sf__TcpListener.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::TcpListener>();
        }
    ));
    LUASF_STUB_DOC("\\brief Set the blocking state of the socket\n\nIn blocking mode, calls will not return until they have\ncompleted their task. For example, a call to Receive in\nblocking mode won't return until some data was actually\nreceived.\nIn non-blocking mode, calls will always return immediately,\nusing the return code to signal whether there was data\navailable or not.\nBy default, all sockets are blocking.\n\n\\param blocking `true` to set the socket as blocking, `false` for non-blocking\n\n\\see `isBlocking`");
    LUASF_STUB_FUNCTION("sf.TcpListener", "setBlocking", "fun(self: sf.TcpListener, blocking: boolean)");
    type_sf__TcpListener.set_function("setBlocking",
        [](sf::TcpListener& self, bool blocking) {
            static_cast<sf::Socket&>(self).setBlocking(blocking);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the socket is in blocking or non-blocking mode\n\n\\return `true` if the socket is blocking, `false` otherwise\n\n\\see `setBlocking`");
    LUASF_STUB_FUNCTION("sf.TcpListener", "isBlocking", "fun(self: sf.TcpListener): boolean");
    type_sf__TcpListener.set_function("isBlocking",
        [](sf::TcpListener& self) -> bool {
            return static_cast<sf::Socket&>(self).isBlocking();
        }
    );
    LUASF_STUB_DOC("\\brief Get the port to which the socket is bound locally\n\nIf the socket is not listening to a port, this function\nreturns 0.\n\n\\return Port to which the socket is bound\n\n\\see `listen`");
    LUASF_STUB_FUNCTION("sf.TcpListener", "getLocalPort", "fun(self: sf.TcpListener): integer");
    type_sf__TcpListener.set_function("getLocalPort",
        [](sf::TcpListener& self) -> unsigned short {
            return self.getLocalPort();
        }
    );
    LUASF_STUB_DOC("\\brief Start listening for incoming connection attempts\n\nThis function makes the socket start listening on the\nspecified port, waiting for incoming connection attempts.\n\nIf the socket is already listening on a port when this\nfunction is called, it will stop listening on the old\nport before starting to listen on the new port.\n\nWhen providing `sf::Socket::AnyPort` as port, the listener\nwill request an available port from the system.\nThe chosen port can be retrieved by calling `getLocalPort()`.\n\n\\param port    Port to listen on for incoming connection attempts\n\\param address Address of the interface to listen on\n\n\\return Status code\n\n\\see `accept`, `close`");
    LUASF_STUB_FUNCTION("sf.TcpListener", "listen", "fun(self: sf.TcpListener, port: integer, address: sf.IpAddress): sf.Socket.Status");
    LUASF_STUB_OVERLOAD("sf.TcpListener", "listen", "fun(self: sf.TcpListener, port: integer): sf.Socket.Status");
    type_sf__TcpListener.set_function("listen",
        sol::overload(
            [](sf::TcpListener& self, lua_sf::LuaIntegral<unsigned short> port, sf::IpAddress address) -> sf::Socket::Status {
                return self.listen(port.value(), address);
            },
            [](sf::TcpListener& self, lua_sf::LuaIntegral<unsigned short> port) -> sf::Socket::Status {
                return self.listen(port.value());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Stop listening and close the socket\n\nThis function gracefully stops the listener. If the\nsocket is not listening, this function has no effect.\n\n\\see `listen`");
    LUASF_STUB_FUNCTION("sf.TcpListener", "close", "fun(self: sf.TcpListener)");
    type_sf__TcpListener.set_function("close",
        [](sf::TcpListener& self) {
            self.close();
        }
    );
    LUASF_STUB_DOC("\\brief Accept a new connection\n\nIf the socket is in blocking mode, this function will\nnot return until a connection is actually received.\n\n\\param socket Socket that will hold the new connection\n\n\\return Status code\n\n\\see `listen`");
    LUASF_STUB_FUNCTION("sf.TcpListener", "accept", "fun(self: sf.TcpListener): sf.Socket.Status, any");
    type_sf__TcpListener.set_function("accept",
        [](sf::TcpListener& self) {
            sf::TcpSocket socket{};
            auto result = self.accept(socket);
            return std::make_tuple(result, std::move(socket));
        }
    );
}
