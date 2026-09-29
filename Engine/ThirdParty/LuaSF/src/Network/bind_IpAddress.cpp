#include "Network/bind_IpAddress.hpp"

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

namespace { constexpr std::array<std::string_view, 25> docs = {
    "\\brief Encapsulate an IPv4 network address",
    "\\brief Construct an IPv4 address from 4 bytes\n\nCalling `IpAddress(a, b, c, d)` is equivalent to calling\n`IpAddress::resolve(\"a.b.c.d\")`, but safer as it doesn't\nhave to parse a string to get the address components.\n\n\\param byte0 First byte of the address\n\\param byte1 Second byte of the address\n\\param byte2 Third byte of the address\n\\param byte3 Fourth byte of the address",
    "\\brief Construct an IPv4 address from a 32-bit integer\n\nThis constructor uses the internal representation of\nthe address directly. It should be used for optimization\npurposes, and only if you got that representation from\n`IpAddress::toInteger()`.\n\n\\param address 4 bytes of the address packed into a 32-bit integer\n\n\\see `toInteger`",
    "\\brief Construct an IPv6 address from 16 bytes\n\n\\param bytes Array of 16 bytes containing the address",
    "\\brief Construct the address from a null-terminated string view\n\n\\deprecated Use `sf::Dns::resolve()` instead.\n\nHere \\a address can be either a decimal address\n(ex: \"192.168.1.56\") or a network name (ex: \"localhost\").\n\nThis function will only resolve to an IPv4 address.\nUse Dns::resolve() to resolve to IPv6 addresses as well.\n\n\\param address IP address or network name\n\n\\return Address if provided argument was valid, otherwise `std::nullopt`",
    "\\brief Try to construct an address from its string representation\n\nThe string should contain either a valid representation of an\nIPv4 address in dotted-decimal notation or a valid representation\nof an IPv6 address in internet standard notation.\n\nExamples:\n- 192.168.1.56\n- FEDC:BA98:7654:3210:FEDC:BA98:7654:3210\n- fedc:ba98:7654:3210:fedc:ba98:7654:3210\n- 1080:0:0:0:8:800:200C:417A\n- 1080::8:800:200C:417A\n- FF01::101\n- ::1\n- ::\n- 0:0:0:0:0:0:13.1.68.3\n- ::13.1.68.3\n- 0:0:0:0:0:FFFF:129.144.52.38\n- ::FFFF:129.144.52.38\n\n\\param address String representation of the address\n\n\\return Address if provided argument was a valid string represenation of an IP address, otherwise `std::nullopt`\n\n\\see `toString`",
    "\\brief Get a string representation of the address\n\nThe returned string is the decimal representation of the\nIP address (like \"192.168.1.56\" or \"FF01::101\"), even if\nit was constructed from a host name.\n\n\\return String representation of the address\n\n\\see `fromString`, `toInteger`",
    "\\brief Get an integer representation of the address\n\nThis function can only be called if this is an IPv4\naddress. Check with isV4() before calling this function.\n\nThe returned number is the internal representation of the\naddress, and should be used for optimization purposes only\n(like sending the address through a socket).\nThe integer produced by this function can then be converted\nback to a `sf::IpAddress` with the proper constructor.\n\n\\return 32-bits unsigned integer representation of the address\n\n\\see `toString`",
    "\\brief Get an array of bytes representing the address\n\nThis function can only be called if this is an IPv6\naddress. Check with isV6() before calling this function.\n\nThe returned array is the internal representation of the\naddress, and should be used for optimization purposes only\n(like sending the address through a socket).\nThe array produced by this function can then be converted\nback to a `sf::IpAddress` with the proper constructor.\n\n\\return 16-byte array representation of the address\n\n\\see `toString`",
    "\\brief Get the type of this IP address\n\n\\return The type of this IP address (IPv4 or IPv6)",
    "\\brief Check if this IP address is an IPv4 address\n\nEquivalent to getType() == Type::IPv4\n\n\\return true if this is an IPv4 address, false otherwise",
    "\\brief Check if this IP address is an IPv6 address\n\nEquivalent to getType() == Type::IPv6\n\n\\return true if this is an IPv6 address, false otherwise",
    "\\brief Get the computer's local address\n\nThe local address is the address of the computer from the\nLAN point of view, i.e. something like 192.168.1.56. It is\nmeaningful only for communications over the local network.\nUnlike getPublicAddress, this function is fast and may be\nused safely anywhere.\n\n\\param type Type of local address\n\n\\return Local IP address of the computer on success, `std::nullopt` otherwise\n\n\\see `getPublicAddress`",
    "\\brief Get the computer's public address\n\nThe public address is the address of the computer from the\npoint of view of the internet, i.e. something like 89.54.1.169\nor 2600:1901:0:13e0::1 as opposed to a private or local address\nlike 192.168.1.56 or fe80::1234:5678:9abc.\nIt is necessary for communication with hosts outside of the\nlocal network.\n\nThe only way to reliably get the public address is to send\ndata to a host on the internet and see what the origin\naddress is; as a consequence, this function depends on both\nyour network connection and the server, and may be very slow.\nYou should try to use it as little as possible. Because this\nfunction depends on the network connection and on a distant\nserver, you can specify a time limit if you don't want your\nprogram to get stuck waiting in case there is a problem; this\nlimit is deactivated by default.\n\nIf tamper resistance is required, setting `secure` to `true`\nwill make use of verified HTTPS connections to get the address.\n\n\\param timeout Maximum time to wait\n\\param type    The type of public address to get, `std::nullopt` to specify no preference\n\\param secure  true to retrieve the public address via a secure HTTPS connection, false to retrieve via DNS or an insecure connection\n\n\\return Public IP address of the computer on success, `std::nullopt` otherwise\n\n\\see `getLocalAddress`",
    "\\brief Type of IP address",
    "IPv4 address",
    "IPv6 address",
    "The same as AnyV4",
    "The same as LocalHostV4",
    "The same as BroadcastV4",
    "Value representing any IPv4 address (0.0.0.0)",
    "The \"localhost\" IPv4 address (for connecting a computer to itself locally)",
    "The \"broadcast\" IPv4 address (for sending UDP messages to everyone on a local network)",
    "Value representing any IPv6 address (::)",
    "The \"localhost\" IPv6 address (for connecting a computer to itself locally)",
}; }

void bind_IpAddress(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__IpAddress = lua_glue::BindStruct<sf::IpAddress>(sf, "IpAddress");
    lua_glue::Table table_sf__IpAddress = sf["IpAddress"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.IpAddress");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "new", "fun(byte0: integer, byte1: integer, byte2: integer, byte3: integer): sf.IpAddress");
    LUASF_STUB_OVERLOAD("sf.IpAddress", "new", "fun(address: integer): sf.IpAddress");
    LUASF_STUB_OVERLOAD("sf.IpAddress", "new", "fun(bytes: any): sf.IpAddress");
    lua_glue::BindCallable(type_sf__IpAddress, "new",
        [](lua_sf::LuaIntegral<std::uint8_t> byte0, lua_sf::LuaIntegral<std::uint8_t> byte1, lua_sf::LuaIntegral<std::uint8_t> byte2, lua_sf::LuaIntegral<std::uint8_t> byte3) {
            return sf::IpAddress{byte0.value(), byte1.value(), byte2.value(), byte3.value()};
        },
        docs[1]
    );
    lua_glue::BindCallable(type_sf__IpAddress, "new",
        [](lua_sf::LuaIntegral<std::uint32_t> address) {
            return sf::IpAddress{address.value()};
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__IpAddress, "new",
        [](std::array<std::uint8_t, 16> bytes) {
            return sf::IpAddress{bytes};
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "resolve", "fun(address: string): sf.IpAddress|nil");
    lua_glue::BindCallable(type_sf__IpAddress, "resolve",
        [lua](std::string address) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, sf::IpAddress::resolve(address));
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "fromString", "fun(address: string): sf.IpAddress|nil");
    lua_glue::BindCallable(type_sf__IpAddress, "fromString",
        [lua](std::string address) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, sf::IpAddress::fromString(address));
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "toString", "fun(self: sf.IpAddress): string");
    lua_glue::BindCallable(type_sf__IpAddress, "toString",
        [](const sf::IpAddress& self) -> std::string {
            return std::string(self.toString());
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "toInteger", "fun(self: sf.IpAddress): integer");
    lua_glue::BindCallable(type_sf__IpAddress, "toInteger",
        [](const sf::IpAddress& self) -> std::uint32_t {
            return self.toInteger();
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "toBytes", "fun(self: sf.IpAddress): any");
    lua_glue::BindCallable(type_sf__IpAddress, "toBytes",
        [](const sf::IpAddress& self) -> std::array<std::uint8_t, 16> {
            return self.toBytes();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "getType", "fun(self: sf.IpAddress): sf.IpAddress.Type");
    lua_glue::BindCallable(type_sf__IpAddress, "getType",
        [](const sf::IpAddress& self) -> sf::IpAddress::Type {
            return self.getType();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "isV4", "fun(self: sf.IpAddress): boolean");
    lua_glue::BindCallable(type_sf__IpAddress, "isV4",
        [](const sf::IpAddress& self) -> bool {
            return self.isV4();
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "isV6", "fun(self: sf.IpAddress): boolean");
    lua_glue::BindCallable(type_sf__IpAddress, "isV6",
        [](const sf::IpAddress& self) -> bool {
            return self.isV6();
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "getLocalAddress", "fun(type?: sf.IpAddress.Type): sf.IpAddress|nil");
    lua_glue::BindCallable(type_sf__IpAddress, "getLocalAddress",
        [lua](sf::IpAddress::Type type) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, sf::IpAddress::getLocalAddress(type));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::IpAddress::Type>(sf::IpAddress::Type::IpV4);
        }}},
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.IpAddress", "getPublicAddress", "fun(timeout?: sf.Time, type?: sf.IpAddress.Type|nil, secure?: boolean): sf.IpAddress|nil");
    lua_glue::BindCallable(type_sf__IpAddress, "getPublicAddress",
        [lua](sf::Time timeout, lua_glue::Object type, bool secure) -> lua_glue::Object {
            auto type_optional = lua_sf::optional_from_object<sf::IpAddress::Type>(type);
            return lua_sf::optional_to_object(lua, sf::IpAddress::getPublicAddress(timeout, type_optional, secure));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Time>(sf::Time::Zero);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::optional_to_object(lua, static_cast<std::optional<sf::IpAddress::Type>>(sf::IpAddress::Type::IpV4));
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_CLASS("sf.IpAddress.Type");
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FIELD("IpV4", "sf.IpAddress.Type");
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FIELD("IpV6", "sf.IpAddress.Type");
    lua_glue::BindEnum<sf::IpAddress::Type>(table_sf__IpAddress, "Type", {
        {"IpV4", sf::IpAddress::Type::IpV4},
        {"IpV6", sf::IpAddress::Type::IpV6}
    });
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_VALUE("sf.IpAddress", "Any", "sf.IpAddress");
    lua_glue::BindStaticAttr<const sf::IpAddress>(table_sf__IpAddress, "Any", &sf::IpAddress::Any);
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_VALUE("sf.IpAddress", "LocalHost", "sf.IpAddress");
    lua_glue::BindStaticAttr<const sf::IpAddress>(table_sf__IpAddress, "LocalHost", &sf::IpAddress::LocalHost);
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_VALUE("sf.IpAddress", "Broadcast", "sf.IpAddress");
    lua_glue::BindStaticAttr<const sf::IpAddress>(table_sf__IpAddress, "Broadcast", &sf::IpAddress::Broadcast);
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_VALUE("sf.IpAddress", "AnyV4", "sf.IpAddress");
    lua_glue::BindStaticAttr<const sf::IpAddress>(table_sf__IpAddress, "AnyV4", &sf::IpAddress::AnyV4);
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_VALUE("sf.IpAddress", "LocalHostV4", "sf.IpAddress");
    lua_glue::BindStaticAttr<const sf::IpAddress>(table_sf__IpAddress, "LocalHostV4", &sf::IpAddress::LocalHostV4);
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_VALUE("sf.IpAddress", "BroadcastV4", "sf.IpAddress");
    lua_glue::BindStaticAttr<const sf::IpAddress>(table_sf__IpAddress, "BroadcastV4", &sf::IpAddress::BroadcastV4);
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_VALUE("sf.IpAddress", "AnyV6", "sf.IpAddress");
    lua_glue::BindStaticAttr<const sf::IpAddress>(table_sf__IpAddress, "AnyV6", &sf::IpAddress::AnyV6);
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_VALUE("sf.IpAddress", "LocalHostV6", "sf.IpAddress");
    lua_glue::BindStaticAttr<const sf::IpAddress>(table_sf__IpAddress, "LocalHostV6", &sf::IpAddress::LocalHostV6);
    LUASF_STUB_FUNCTION("sf.IpAddress", "copy", "fun(self: sf.IpAddress): sf.IpAddress");
    LUASF_STUB_FUNCTION("sf.IpAddress", "deepcopy", "fun(self: sf.IpAddress): sf.IpAddress");
}
