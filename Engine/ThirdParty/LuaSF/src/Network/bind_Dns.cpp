#include "Network/bind_Dns.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Dns(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    sol::table sf_Dns = sf["Dns"].get_or_create<sol::table>();
    auto type_sf__Dns__MxRecord = sf_Dns.new_usertype<sf::Dns::MxRecord>("MxRecord", sol::no_constructor);
    sol::table table_sf__Dns__MxRecord = sf_Dns["MxRecord"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Dns::MxRecord>(lua);
    LUASF_STUB_DOC("\\brief A DNS MX record");
    LUASF_STUB_CLASS("sf.Dns.MxRecord");
    LUASF_STUB_DOC("Host willing to act as mail exchange");
    LUASF_STUB_FIELD("exchange", "string");
    LUASF_STUB_DOC("Preference of this record among others, lower values are preferred");
    LUASF_STUB_FIELD("preference", "integer");
    LUASF_STUB_FUNCTION("sf.Dns.MxRecord", "new", "fun(): sf.Dns.MxRecord");
    type_sf__Dns__MxRecord.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Dns::MxRecord>();
        }
    ));
    type_sf__Dns__MxRecord.set("exchange", sol::property(
        [](sf::Dns::MxRecord& self) -> std::string {
            return lua_sf::to_utf8_string(self.exchange);
        },
        [](sf::Dns::MxRecord& self, std::string value) {
            self.exchange = lua_sf::to_sf_string(value);
        }
    ));
    type_sf__Dns__MxRecord.set("preference", sol::property(
        [](sf::Dns::MxRecord& self) {
            return self.preference;
        },
        [](sf::Dns::MxRecord& self, lua_sf::LuaIntegral<std::uint16_t> value) {
            self.preference = value.value();
        }
    ));
    auto type_sf__Dns__SrvRecord = sf_Dns.new_usertype<sf::Dns::SrvRecord>("SrvRecord", sol::no_constructor);
    sol::table table_sf__Dns__SrvRecord = sf_Dns["SrvRecord"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Dns::SrvRecord>(lua);
    LUASF_STUB_DOC("\\brief A DNS SRV record");
    LUASF_STUB_CLASS("sf.Dns.SrvRecord");
    LUASF_STUB_DOC("The domain name of the target host");
    LUASF_STUB_FIELD("target", "string");
    LUASF_STUB_DOC("The port on the target host of the service");
    LUASF_STUB_FIELD("port", "integer");
    LUASF_STUB_DOC("Server selection mechanism, larger weights should be given a proportionately higher probability of being selected");
    LUASF_STUB_FIELD("weight", "integer");
    LUASF_STUB_DOC("The priority of the target host, a client must attempt to contact the target host with the lowest-numbered priority it can reach");
    LUASF_STUB_FIELD("priority", "integer");
    LUASF_STUB_FUNCTION("sf.Dns.SrvRecord", "new", "fun(): sf.Dns.SrvRecord");
    type_sf__Dns__SrvRecord.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Dns::SrvRecord>();
        }
    ));
    type_sf__Dns__SrvRecord.set("target", sol::property(
        [](sf::Dns::SrvRecord& self) -> std::string {
            return lua_sf::to_utf8_string(self.target);
        },
        [](sf::Dns::SrvRecord& self, std::string value) {
            self.target = lua_sf::to_sf_string(value);
        }
    ));
    type_sf__Dns__SrvRecord.set("port", sol::property(
        [](sf::Dns::SrvRecord& self) {
            return self.port;
        },
        [](sf::Dns::SrvRecord& self, lua_sf::LuaIntegral<std::uint16_t> value) {
            self.port = value.value();
        }
    ));
    type_sf__Dns__SrvRecord.set("weight", sol::property(
        [](sf::Dns::SrvRecord& self) {
            return self.weight;
        },
        [](sf::Dns::SrvRecord& self, lua_sf::LuaIntegral<std::uint16_t> value) {
            self.weight = value.value();
        }
    ));
    type_sf__Dns__SrvRecord.set("priority", sol::property(
        [](sf::Dns::SrvRecord& self) {
            return self.priority;
        },
        [](sf::Dns::SrvRecord& self, lua_sf::LuaIntegral<std::uint16_t> value) {
            self.priority = value.value();
        }
    ));
    LUASF_STUB_DOC("\\brief Resolve a hostname into a list of IP addresses\n\n\\param hostname Hostname to resolve\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of IP addresses the given hostname resolves to, `std::nullopt` is returned if name resolution fails, an empty list is returned if the hostname could not be resolved to any address");
    LUASF_STUB_FUNCTION("sf.Dns", "resolve", "fun(hostname: string): sf.IpAddress[]|nil");
    LUASF_STUB_OVERLOAD("sf.Dns", "resolve", "fun(hostname: string, servers: sf.IpAddress[]): sf.IpAddress[]|nil");
    LUASF_STUB_OVERLOAD("sf.Dns", "resolve", "fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): sf.IpAddress[]|nil");
    sf_Dns.set_function("resolve",
        sol::overload(
            [lua](std::string hostname) -> sol::object {
                return lua_sf::optional_to_object(lua, sf::Dns::resolve(lua_sf::to_sf_string(hostname)));
            },
            [lua](std::string hostname, sol::table servers) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                return lua_sf::optional_to_object(lua, sf::Dns::resolve(lua_sf::to_sf_string(hostname), servers_vector));
            },
            [lua](std::string hostname, sol::table servers, sol::object timeout) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
                return lua_sf::optional_to_object(lua, sf::Dns::resolve(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Query NS records for a hostname\n\n\\param hostname Hostname to query NS records for\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of NS record strings, an empty list is returned if there are no NS records for the hostname");
    LUASF_STUB_FUNCTION("sf.Dns", "queryNs", "fun(hostname: string): string[]");
    LUASF_STUB_OVERLOAD("sf.Dns", "queryNs", "fun(hostname: string, servers: sf.IpAddress[]): string[]");
    LUASF_STUB_OVERLOAD("sf.Dns", "queryNs", "fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): string[]");
    sf_Dns.set_function("queryNs",
        sol::overload(
            [lua](std::string hostname) -> sol::object {
                return lua_sf::vector_to_object(lua, sf::Dns::queryNs(lua_sf::to_sf_string(hostname)));
            },
            [lua](std::string hostname, sol::table servers) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                return lua_sf::vector_to_object(lua, sf::Dns::queryNs(lua_sf::to_sf_string(hostname), servers_vector));
            },
            [lua](std::string hostname, sol::table servers, sol::object timeout) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
                return lua_sf::vector_to_object(lua, sf::Dns::queryNs(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Query MX records for a hostname\n\n\\param hostname Hostname to query MX records for\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of MX records, an empty list is returned if there are no MX records for the hostname");
    LUASF_STUB_FUNCTION("sf.Dns", "queryMx", "fun(hostname: string): sf.Dns.MxRecord[]");
    LUASF_STUB_OVERLOAD("sf.Dns", "queryMx", "fun(hostname: string, servers: sf.IpAddress[]): sf.Dns.MxRecord[]");
    LUASF_STUB_OVERLOAD("sf.Dns", "queryMx", "fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): sf.Dns.MxRecord[]");
    sf_Dns.set_function("queryMx",
        sol::overload(
            [lua](std::string hostname) -> sol::object {
                return lua_sf::vector_to_object(lua, sf::Dns::queryMx(lua_sf::to_sf_string(hostname)));
            },
            [lua](std::string hostname, sol::table servers) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                return lua_sf::vector_to_object(lua, sf::Dns::queryMx(lua_sf::to_sf_string(hostname), servers_vector));
            },
            [lua](std::string hostname, sol::table servers, sol::object timeout) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
                return lua_sf::vector_to_object(lua, sf::Dns::queryMx(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Query SRV records for a hostname\n\n\\param hostname Hostname to query SRV records for\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of SRV records, an empty list is returned if there are no SRV records for the hostname");
    LUASF_STUB_FUNCTION("sf.Dns", "querySrv", "fun(hostname: string): sf.Dns.SrvRecord[]");
    LUASF_STUB_OVERLOAD("sf.Dns", "querySrv", "fun(hostname: string, servers: sf.IpAddress[]): sf.Dns.SrvRecord[]");
    LUASF_STUB_OVERLOAD("sf.Dns", "querySrv", "fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): sf.Dns.SrvRecord[]");
    sf_Dns.set_function("querySrv",
        sol::overload(
            [lua](std::string hostname) -> sol::object {
                return lua_sf::vector_to_object(lua, sf::Dns::querySrv(lua_sf::to_sf_string(hostname)));
            },
            [lua](std::string hostname, sol::table servers) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                return lua_sf::vector_to_object(lua, sf::Dns::querySrv(lua_sf::to_sf_string(hostname), servers_vector));
            },
            [lua](std::string hostname, sol::table servers, sol::object timeout) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
                return lua_sf::vector_to_object(lua, sf::Dns::querySrv(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Query TXT records for a hostname\n\n\\param hostname Hostname to query TXT records for\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of TXT record string lists, an empty list is returned if there are no TXT records for the hostname");
    LUASF_STUB_FUNCTION("sf.Dns", "queryTxt", "fun(hostname: string): string[][]");
    LUASF_STUB_OVERLOAD("sf.Dns", "queryTxt", "fun(hostname: string, servers: sf.IpAddress[]): string[][]");
    LUASF_STUB_OVERLOAD("sf.Dns", "queryTxt", "fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): string[][]");
    sf_Dns.set_function("queryTxt",
        sol::overload(
            [lua](std::string hostname) -> sol::object {
                return lua_sf::vector_to_object(lua, sf::Dns::queryTxt(lua_sf::to_sf_string(hostname)));
            },
            [lua](std::string hostname, sol::table servers) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                return lua_sf::vector_to_object(lua, sf::Dns::queryTxt(lua_sf::to_sf_string(hostname), servers_vector));
            },
            [lua](std::string hostname, sol::table servers, sol::object timeout) -> sol::object {
                auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
                auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
                return lua_sf::vector_to_object(lua, sf::Dns::queryTxt(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the computer's public address via DNS\n\nThe public address is the address of the computer from the\npoint of view of the internet, i.e. something like 89.54.1.169\nor 2600:1901:0:13e0::1 as opposed to a private or local address\nlike 192.168.1.56 or fe80::1234:5678:9abc.\nIt is necessary for communication with hosts outside of the\nlocal network.\n\nThe only way to reliably get the public address is to send\ndata to a host on the internet and see what the origin\naddress is; as a consequence, this function depends on both\nyour network connection and the server, and may be very slow.\nYou should try to use it as little as possible. Because this\nfunction depends on the network connection and on a distant\nserver, you can specify a time limit if you don't want your\nprogram to get stuck waiting in case there is a problem; this\nlimit is deactivated by default.\n\nThis function makes use of DNS queries get the public address.\n\n\\param timeout Maximum time to wait, `std::nullopt` to wait forever\n\\param type    The type of public address to get\n\n\\return Public IP address of the computer on success, `std::nullopt` otherwise");
    LUASF_STUB_FUNCTION("sf.Dns", "getPublicAddress", "fun(): sf.IpAddress|nil");
    LUASF_STUB_OVERLOAD("sf.Dns", "getPublicAddress", "fun(timeout: sf.Time|nil, type: sf.IpAddress.Type): sf.IpAddress|nil");
    LUASF_STUB_OVERLOAD("sf.Dns", "getPublicAddress", "fun(timeout: sf.Time|nil): sf.IpAddress|nil");
    sf_Dns.set_function("getPublicAddress",
        sol::overload(
            [lua]() -> sol::object {
                return lua_sf::optional_to_object(lua, sf::Dns::getPublicAddress());
            },
            [lua](sol::object timeout, sf::IpAddress::Type type) -> sol::object {
                auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
                return lua_sf::optional_to_object(lua, sf::Dns::getPublicAddress(timeout_optional, type));
            },
            [lua](sol::object timeout) -> sol::object {
                auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
                return lua_sf::optional_to_object(lua, sf::Dns::getPublicAddress(timeout_optional));
            }
        )
    );
}
