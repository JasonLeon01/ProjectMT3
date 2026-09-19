#include "Network/bind_IpAddress.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_IpAddress(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__IpAddress = sf.new_usertype<sf::IpAddress>("IpAddress", sol::no_constructor);
    sol::table table_sf__IpAddress = sf["IpAddress"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::IpAddress>(lua);
    LUASF_STUB_DOC("\\brief Encapsulate an IPv4 network address");
    LUASF_STUB_CLASS("sf.IpAddress");
    LUASF_STUB_DOC("\\brief Construct an IPv4 address from 4 bytes\n\nCalling `IpAddress(a, b, c, d)` is equivalent to calling\n`IpAddress::resolve(\"a.b.c.d\")`, but safer as it doesn't\nhave to parse a string to get the address components.\n\n\\param byte0 First byte of the address\n\\param byte1 Second byte of the address\n\\param byte2 Third byte of the address\n\\param byte3 Fourth byte of the address");
    LUASF_STUB_FUNCTION("sf.IpAddress", "new", "fun(byte0: integer, byte1: integer, byte2: integer, byte3: integer): sf.IpAddress");
    LUASF_STUB_OVERLOAD("sf.IpAddress", "new", "fun(address: integer): sf.IpAddress");
    LUASF_STUB_OVERLOAD("sf.IpAddress", "new", "fun(bytes: any): sf.IpAddress");
    type_sf__IpAddress.set_function("new", sol::factories(
        [](lua_sf::LuaIntegral<std::uint8_t> byte0, lua_sf::LuaIntegral<std::uint8_t> byte1, lua_sf::LuaIntegral<std::uint8_t> byte2, lua_sf::LuaIntegral<std::uint8_t> byte3) {
            return lua_sf::makeLuaSharedObject<sf::IpAddress>(byte0.value(), byte1.value(), byte2.value(), byte3.value());
        },
        [](lua_sf::LuaIntegral<std::uint32_t> address) {
            return lua_sf::makeLuaSharedObject<sf::IpAddress>(address.value());
        },
        [](std::array<std::uint8_t, 16> bytes) {
            return lua_sf::makeLuaSharedObject<sf::IpAddress>(bytes);
        }
    ));
    LUASF_STUB_DOC("\\brief Construct the address from a null-terminated string view\n\n\\deprecated Use `sf::Dns::resolve()` instead.\n\nHere \\a address can be either a decimal address\n(ex: \"192.168.1.56\") or a network name (ex: \"localhost\").\n\nThis function will only resolve to an IPv4 address.\nUse Dns::resolve() to resolve to IPv6 addresses as well.\n\n\\param address IP address or network name\n\n\\return Address if provided argument was valid, otherwise `std::nullopt`");
    LUASF_STUB_FUNCTION("sf.IpAddress", "resolve", "fun(address: string): sf.IpAddress|nil");
    type_sf__IpAddress.set_function("resolve",
        [lua](std::string address) -> sol::object {
            return lua_sf::optional_to_object(lua, sf::IpAddress::resolve(address));
        }
    );
    LUASF_STUB_DOC("\\brief Try to construct an address from its string representation\n\nThe string should contain either a valid representation of an\nIPv4 address in dotted-decimal notation or a valid representation\nof an IPv6 address in internet standard notation.\n\nExamples:\n- 192.168.1.56\n- FEDC:BA98:7654:3210:FEDC:BA98:7654:3210\n- fedc:ba98:7654:3210:fedc:ba98:7654:3210\n- 1080:0:0:0:8:800:200C:417A\n- 1080::8:800:200C:417A\n- FF01::101\n- ::1\n- ::\n- 0:0:0:0:0:0:13.1.68.3\n- ::13.1.68.3\n- 0:0:0:0:0:FFFF:129.144.52.38\n- ::FFFF:129.144.52.38\n\n\\param address String representation of the address\n\n\\return Address if provided argument was a valid string represenation of an IP address, otherwise `std::nullopt`\n\n\\see `toString`");
    LUASF_STUB_FUNCTION("sf.IpAddress", "fromString", "fun(address: string): sf.IpAddress|nil");
    type_sf__IpAddress.set_function("fromString",
        [lua](std::string address) -> sol::object {
            return lua_sf::optional_to_object(lua, sf::IpAddress::fromString(address));
        }
    );
    LUASF_STUB_DOC("\\brief Get a string representation of the address\n\nThe returned string is the decimal representation of the\nIP address (like \"192.168.1.56\" or \"FF01::101\"), even if\nit was constructed from a host name.\n\n\\return String representation of the address\n\n\\see `fromString`, `toInteger`");
    LUASF_STUB_FUNCTION("sf.IpAddress", "toString", "fun(self: sf.IpAddress): string");
    type_sf__IpAddress.set_function("toString",
        [](sf::IpAddress& self) -> std::string {
            return std::string(self.toString());
        }
    );
    LUASF_STUB_DOC("\\brief Get an integer representation of the address\n\nThis function can only be called if this is an IPv4\naddress. Check with isV4() before calling this function.\n\nThe returned number is the internal representation of the\naddress, and should be used for optimization purposes only\n(like sending the address through a socket).\nThe integer produced by this function can then be converted\nback to a `sf::IpAddress` with the proper constructor.\n\n\\return 32-bits unsigned integer representation of the address\n\n\\see `toString`");
    LUASF_STUB_FUNCTION("sf.IpAddress", "toInteger", "fun(self: sf.IpAddress): integer");
    type_sf__IpAddress.set_function("toInteger",
        [](sf::IpAddress& self) -> std::uint32_t {
            return self.toInteger();
        }
    );
    LUASF_STUB_DOC("\\brief Get an array of bytes representing the address\n\nThis function can only be called if this is an IPv6\naddress. Check with isV6() before calling this function.\n\nThe returned array is the internal representation of the\naddress, and should be used for optimization purposes only\n(like sending the address through a socket).\nThe array produced by this function can then be converted\nback to a `sf::IpAddress` with the proper constructor.\n\n\\return 16-byte array representation of the address\n\n\\see `toString`");
    LUASF_STUB_FUNCTION("sf.IpAddress", "toBytes", "fun(self: sf.IpAddress): any");
    type_sf__IpAddress.set_function("toBytes",
        [](sf::IpAddress& self) -> std::array<std::uint8_t, 16> {
            return self.toBytes();
        }
    );
    LUASF_STUB_DOC("\\brief Get the type of this IP address\n\n\\return The type of this IP address (IPv4 or IPv6)");
    LUASF_STUB_FUNCTION("sf.IpAddress", "getType", "fun(self: sf.IpAddress): sf.IpAddress.Type");
    type_sf__IpAddress.set_function("getType",
        [](sf::IpAddress& self) -> sf::IpAddress::Type {
            return self.getType();
        }
    );
    LUASF_STUB_DOC("\\brief Check if this IP address is an IPv4 address\n\nEquivalent to getType() == Type::IPv4\n\n\\return true if this is an IPv4 address, false otherwise");
    LUASF_STUB_FUNCTION("sf.IpAddress", "isV4", "fun(self: sf.IpAddress): boolean");
    type_sf__IpAddress.set_function("isV4",
        [](sf::IpAddress& self) -> bool {
            return self.isV4();
        }
    );
    LUASF_STUB_DOC("\\brief Check if this IP address is an IPv6 address\n\nEquivalent to getType() == Type::IPv6\n\n\\return true if this is an IPv6 address, false otherwise");
    LUASF_STUB_FUNCTION("sf.IpAddress", "isV6", "fun(self: sf.IpAddress): boolean");
    type_sf__IpAddress.set_function("isV6",
        [](sf::IpAddress& self) -> bool {
            return self.isV6();
        }
    );
    LUASF_STUB_DOC("\\brief Get the computer's local address\n\nThe local address is the address of the computer from the\nLAN point of view, i.e. something like 192.168.1.56. It is\nmeaningful only for communications over the local network.\nUnlike getPublicAddress, this function is fast and may be\nused safely anywhere.\n\n\\param type Type of local address\n\n\\return Local IP address of the computer on success, `std::nullopt` otherwise\n\n\\see `getPublicAddress`");
    LUASF_STUB_FUNCTION("sf.IpAddress", "getLocalAddress", "fun(type: sf.IpAddress.Type): sf.IpAddress|nil");
    LUASF_STUB_OVERLOAD("sf.IpAddress", "getLocalAddress", "fun(): sf.IpAddress|nil");
    type_sf__IpAddress.set_function("getLocalAddress",
        sol::overload(
            [lua](sf::IpAddress::Type type) -> sol::object {
                return lua_sf::optional_to_object(lua, sf::IpAddress::getLocalAddress(type));
            },
            [lua]() -> sol::object {
                return lua_sf::optional_to_object(lua, sf::IpAddress::getLocalAddress());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the computer's public address\n\nThe public address is the address of the computer from the\npoint of view of the internet, i.e. something like 89.54.1.169\nor 2600:1901:0:13e0::1 as opposed to a private or local address\nlike 192.168.1.56 or fe80::1234:5678:9abc.\nIt is necessary for communication with hosts outside of the\nlocal network.\n\nThe only way to reliably get the public address is to send\ndata to a host on the internet and see what the origin\naddress is; as a consequence, this function depends on both\nyour network connection and the server, and may be very slow.\nYou should try to use it as little as possible. Because this\nfunction depends on the network connection and on a distant\nserver, you can specify a time limit if you don't want your\nprogram to get stuck waiting in case there is a problem; this\nlimit is deactivated by default.\n\nIf tamper resistance is required, setting `secure` to `true`\nwill make use of verified HTTPS connections to get the address.\n\n\\param timeout Maximum time to wait\n\\param type    The type of public address to get, `std::nullopt` to specify no preference\n\\param secure  true to retrieve the public address via a secure HTTPS connection, false to retrieve via DNS or an insecure connection\n\n\\return Public IP address of the computer on success, `std::nullopt` otherwise\n\n\\see `getLocalAddress`");
    LUASF_STUB_FUNCTION("sf.IpAddress", "getPublicAddress", "fun(timeout: sf.Time): sf.IpAddress|nil");
    LUASF_STUB_OVERLOAD("sf.IpAddress", "getPublicAddress", "fun(): sf.IpAddress|nil");
    LUASF_STUB_OVERLOAD("sf.IpAddress", "getPublicAddress", "fun(timeout: sf.Time, type: sf.IpAddress.Type|nil, secure: boolean): sf.IpAddress|nil");
    LUASF_STUB_OVERLOAD("sf.IpAddress", "getPublicAddress", "fun(timeout: sf.Time, type: sf.IpAddress.Type|nil): sf.IpAddress|nil");
    type_sf__IpAddress.set_function("getPublicAddress",
        sol::overload(
            [lua](sf::Time timeout) -> sol::object {
                return lua_sf::optional_to_object(lua, sf::IpAddress::getPublicAddress(timeout));
            },
            [lua]() -> sol::object {
                return lua_sf::optional_to_object(lua, sf::IpAddress::getPublicAddress());
            },
            [lua](sf::Time timeout, sol::object type, bool secure) -> sol::object {
                auto type_optional = lua_sf::optional_from_object<sf::IpAddress::Type>(type);
                return lua_sf::optional_to_object(lua, sf::IpAddress::getPublicAddress(timeout, type_optional, secure));
            },
            [lua](sf::Time timeout, sol::object type) -> sol::object {
                auto type_optional = lua_sf::optional_from_object<sf::IpAddress::Type>(type);
                return lua_sf::optional_to_object(lua, sf::IpAddress::getPublicAddress(timeout, type_optional));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Type of IP address");
    LUASF_STUB_CLASS("sf.IpAddress.Type");
    LUASF_STUB_DOC("IPv4 address");
    LUASF_STUB_FIELD("IpV4", "sf.IpAddress.Type");
    LUASF_STUB_DOC("IPv6 address");
    LUASF_STUB_FIELD("IpV6", "sf.IpAddress.Type");
    table_sf__IpAddress.new_enum("Type",
        "IpV4", sf::IpAddress::Type::IpV4,
        "IpV6", sf::IpAddress::Type::IpV6
    );
    LUASF_STUB_DOC("The same as AnyV4");
    LUASF_STUB_VALUE("sf.IpAddress", "Any", "sf.IpAddress");
    table_sf__IpAddress["Any"] = sf::IpAddress::Any;
    LUASF_STUB_DOC("The same as LocalHostV4");
    LUASF_STUB_VALUE("sf.IpAddress", "LocalHost", "sf.IpAddress");
    table_sf__IpAddress["LocalHost"] = sf::IpAddress::LocalHost;
    LUASF_STUB_DOC("The same as BroadcastV4");
    LUASF_STUB_VALUE("sf.IpAddress", "Broadcast", "sf.IpAddress");
    table_sf__IpAddress["Broadcast"] = sf::IpAddress::Broadcast;
    LUASF_STUB_DOC("Value representing any IPv4 address (0.0.0.0)");
    LUASF_STUB_VALUE("sf.IpAddress", "AnyV4", "sf.IpAddress");
    table_sf__IpAddress["AnyV4"] = sf::IpAddress::AnyV4;
    LUASF_STUB_DOC("The \"localhost\" IPv4 address (for connecting a computer to itself locally)");
    LUASF_STUB_VALUE("sf.IpAddress", "LocalHostV4", "sf.IpAddress");
    table_sf__IpAddress["LocalHostV4"] = sf::IpAddress::LocalHostV4;
    LUASF_STUB_DOC("The \"broadcast\" IPv4 address (for sending UDP messages to everyone on a local network)");
    LUASF_STUB_VALUE("sf.IpAddress", "BroadcastV4", "sf.IpAddress");
    table_sf__IpAddress["BroadcastV4"] = sf::IpAddress::BroadcastV4;
    LUASF_STUB_DOC("Value representing any IPv6 address (::)");
    LUASF_STUB_VALUE("sf.IpAddress", "AnyV6", "sf.IpAddress");
    table_sf__IpAddress["AnyV6"] = sf::IpAddress::AnyV6;
    LUASF_STUB_DOC("The \"localhost\" IPv6 address (for connecting a computer to itself locally)");
    LUASF_STUB_VALUE("sf.IpAddress", "LocalHostV6", "sf.IpAddress");
    table_sf__IpAddress["LocalHostV6"] = sf::IpAddress::LocalHostV6;
}
