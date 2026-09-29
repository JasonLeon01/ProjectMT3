#include "Network/bind_SocketSelector.hpp"

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
    "\\brief Multiplexer that allows to read from multiple sockets",
    "\\brief Default constructor",
    "\\brief Add a new socket to the selector\n\nThe type of readiness to wait for can be specified.\nSpecifying `SocketSelector::Receive` will wait for the\nsocket to become ready to receive data from, specifying\n`SocketSelector::Send` will wait for the socket to become\nready to send data on. Specifying\n`SocketSelector::Receive | SocketSelector::Send`\nwill wait for the socket to become either ready to send\nor receive data on.\n\nAdding a socket after it has already been added will just\noverwrite the existing readiness type with the new value.\n\nThis function keeps a weak reference to the socket,\nso you have to make sure that the socket is not destroyed\nwhile it is stored in the selector.\nThis function does nothing if the socket is not valid.\n\nWhen adding a socket to the selector you can also attach\na callback along with it. The callback is called by\n`dispatchReadyCallbacks` when a socket is determined to be\nready after a call to `wait`.\n\nUsing attached callbacks instead of having to individually\ncall `isReady` on every socket after every call to `wait`\nallows for scaling up to a large number of sockets. This\nis because the overhead of checking for socket readiness\nusing `isReady` grows proportionally to the total number\nof sockets. When using callbacks calling `isReady` on\nevery socket is no longer necessary.\n\nBecause a socket can be ready for receiving, sending or\nboth, the type of readiness is passed to the attached\ncallback as a bitwise combination of\n`SocketSelector::Receive` and/or `SocketSelector::Send`\nwhen it is called by `dispatchReadyCallbacks`.\nSome systems don't support combined read and write\nnotifications. On these systems, if a socket is ready\nto be both received from and sent to the callback will be\ncalled twice, once with `SocketSelector::Receive` and once\nwith `SocketSelector::Send`.\n\nTo remove the attached callback of a socket, call `add`\nagain with an empty function.\n\nBy default, no readiness callback is attached when adding\na socket.\n\n\\param socket        Reference to the socket to add\n\\param readinessType Type of readiness to wait for, a bitwise combination of `SocketSelector::Receive` and/or `SocketSelector::Send`\n\\param readyCallback Ready callback to attach to the socket, pass an empty function to remove the ready callback\n\n\\return `true` if the socket was added successfully, `false` otherwise\n\n\\see `remove`, `clear`",
    "\\brief Remove a socket from the selector\n\nThis function doesn't destroy the socket, it simply\nremoves the reference that the selector has to it.\n\n\\param socket Reference to the socket to remove\n\n\\return `true` if the socket was removed successfully, `false` otherwise\n\n\\see `add`, `clear`",
    "\\brief Remove all the sockets stored in the selector\n\nThis function doesn't destroy any instance, it simply\nremoves all the references that the selector has to\nexternal sockets.\n\n\\see `add`, `remove`",
    "\\brief Wait until one or more sockets are ready to receive or send\n\nThis function returns as soon as at least one socket has\nsome data available to be received or data can be sent,\ndepending on how the socket was added to this selector.\nTo know which sockets are ready, use the `isReady` function.\nIf you use a timeout and no socket is ready before the timeout\nis over, the function returns `false`.\n\n\\param timeout Maximum time to wait, (use Time::Zero for infinity)\n\n\\return `true` if there are sockets ready, `false` otherwise\n\n\\see `isReady`, `dispatchReadyCallbacks`",
    "\\brief Test a socket to know if it is ready to receive or send data\n\nThis function must be used after a call to `wait`, to know\nwhich sockets are ready to receive or send data. If a socket\nis ready, a call to receive or send will never block because\nwe know that there is data available to read or we can write.\nNote that if this function returns `true` for a TcpListener,\nthis means that it is ready to accept a new connection.\n\n\\param socket        Socket to test\n\\param readinessType Type of readiness to check for, a bitwise combination of `SocketSelector::Receive` and/or `SocketSelector::Send`\n\n\\return `true` if the socket is ready to read, `false` otherwise\n\n\\see `wait`",
    "\\brief Dispatch callbacks of ready sockets\n\nAfter calling `wait` returns `true`, at least one socket\nis ready to receive or send data. Calling\n`dispatchReadyCallbacks` will call the attached ready\ncallback for every socket that is ready to either receive\nor send data. Sockets that don't have a callback attached\ncan still be individually checked using `isReady`.\n\nThe readiness state of each socket is maintained until\nthe next call to `wait`. Calling `dispatchReadyCallbacks`\nmultiple times after a single call to `wait` will run the\nexact same callbacks with the exact same passed arguments.\n\n\\see `wait`",
    "Check if sockets are ready to be received from",
    "Check if sockets are ready to be sent to",
}; }

void bind_SocketSelector(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__SocketSelector = lua_glue::BindClass<sf::SocketSelector>(sf, "SocketSelector");
    lua_glue::Table table_sf__SocketSelector = sf["SocketSelector"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::SocketSelector>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.SocketSelector");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.SocketSelector", "new", "fun(): sf.SocketSelector");
    lua_glue::BindCallable(type_sf__SocketSelector, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::SocketSelector>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.SocketSelector", "add", "fun(self: sf.SocketSelector, socket: sf.Socket, readinessType?: integer, readyCallback?: fun(arg1: sf.SocketSelector.ReadinessType)): boolean");
    lua_glue::BindCallable(type_sf__SocketSelector, "add",
        [](sf::SocketSelector& self, const sf::Socket& socket, sf::SocketSelector::ReadinessType readinessType, lua_glue::Object readyCallback) -> bool {
            return self.add(socket, readinessType, lua_sf::callback::from_object<std::function<void(sf::SocketSelector::ReadinessType)>, lua_sf::callback::GenericCallbackCodec>(readyCallback, lua_sf::callback::CallbackOptions{"sf::SocketSelector::add.readyCallback", false}));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned int>(sf::SocketSelector::Receive);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::function<void (unsigned int)>{ };
        }}},
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.SocketSelector", "remove", "fun(self: sf.SocketSelector, socket: sf.Socket): boolean");
    lua_glue::BindCallable(type_sf__SocketSelector, "remove",
        [](sf::SocketSelector& self, const sf::Socket& socket) -> bool {
            return self.remove(socket);
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.SocketSelector", "clear", "fun(self: sf.SocketSelector)");
    lua_glue::BindCallable(type_sf__SocketSelector, "clear",
        [](sf::SocketSelector& self) {
            self.clear();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.SocketSelector", "wait", "fun(self: sf.SocketSelector, timeout?: sf.Time): boolean");
    lua_glue::BindCallable(type_sf__SocketSelector, "wait",
        [](sf::SocketSelector& self, sf::Time timeout) -> bool {
            return self.wait(timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Time>(sf::Time::Zero);
        }}},
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.SocketSelector", "isReady", "fun(self: sf.SocketSelector, socket: sf.Socket, readinessType?: integer): boolean");
    lua_glue::BindCallable(type_sf__SocketSelector, "isReady",
        [](const sf::SocketSelector& self, const sf::Socket& socket, sf::SocketSelector::ReadinessType readinessType) -> bool {
            return self.isReady(socket, readinessType);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned int>(sf::SocketSelector::Receive);
        }}},
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.SocketSelector", "dispatchReadyCallbacks", "fun(self: sf.SocketSelector)");
    lua_glue::BindCallable(type_sf__SocketSelector, "dispatchReadyCallbacks",
        [](sf::SocketSelector& self) {
            self.dispatchReadyCallbacks();
        },
        docs[7]
    );
    LUASF_STUB_ALIAS("sf.SocketSelector.ReadinessType", "sf.SocketSelector.unsigned int");
    {
        const lua_glue::Object aliasValue = table_sf__SocketSelector.raw_get<lua_glue::Object>("ReadinessType");
        const lua_glue::Object aliasTarget = table_sf__SocketSelector.raw_get<lua_glue::Object>("unsigned int");
        if ((!aliasValue.valid() || aliasValue.get_type() == lua_glue::Type::Nil) &&
            aliasTarget.valid() && aliasTarget.get_type() != lua_glue::Type::Nil)
            table_sf__SocketSelector.raw_set("ReadinessType", aliasTarget);
    }
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_VALUE("sf.SocketSelector", "Receive", "sf.SocketSelector.ReadinessType");
    table_sf__SocketSelector.raw_set("Receive", static_cast<sf::SocketSelector::ReadinessType>(sf::SocketSelector::Receive));
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_VALUE("sf.SocketSelector", "Send", "sf.SocketSelector.ReadinessType");
    table_sf__SocketSelector.raw_set("Send", static_cast<sf::SocketSelector::ReadinessType>(sf::SocketSelector::Send));
}
