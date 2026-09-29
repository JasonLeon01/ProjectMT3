#include "Network/bind_Dns.hpp"

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

namespace { constexpr std::array<std::string_view, 14> docs = {
    "\\brief A DNS MX record",
    "Host willing to act as mail exchange",
    "Preference of this record among others, lower values are preferred",
    "\\brief A DNS SRV record",
    "The domain name of the target host",
    "The port on the target host of the service",
    "Server selection mechanism, larger weights should be given a proportionately higher probability of being selected",
    "The priority of the target host, a client must attempt to contact the target host with the lowest-numbered priority it can reach",
    "\\brief Resolve a hostname into a list of IP addresses\n\n\\param hostname Hostname to resolve\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of IP addresses the given hostname resolves to, `std::nullopt` is returned if name resolution fails, an empty list is returned if the hostname could not be resolved to any address",
    "\\brief Query NS records for a hostname\n\n\\param hostname Hostname to query NS records for\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of NS record strings, an empty list is returned if there are no NS records for the hostname",
    "\\brief Query MX records for a hostname\n\n\\param hostname Hostname to query MX records for\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of MX records, an empty list is returned if there are no MX records for the hostname",
    "\\brief Query SRV records for a hostname\n\n\\param hostname Hostname to query SRV records for\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of SRV records, an empty list is returned if there are no SRV records for the hostname",
    "\\brief Query TXT records for a hostname\n\n\\param hostname Hostname to query TXT records for\n\\param servers  The list of servers to query, if empty use the default servers\n\\param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever\n\n\\return List of TXT record string lists, an empty list is returned if there are no TXT records for the hostname",
    "\\brief Get the computer's public address via DNS\n\nThe public address is the address of the computer from the\npoint of view of the internet, i.e. something like 89.54.1.169\nor 2600:1901:0:13e0::1 as opposed to a private or local address\nlike 192.168.1.56 or fe80::1234:5678:9abc.\nIt is necessary for communication with hosts outside of the\nlocal network.\n\nThe only way to reliably get the public address is to send\ndata to a host on the internet and see what the origin\naddress is; as a consequence, this function depends on both\nyour network connection and the server, and may be very slow.\nYou should try to use it as little as possible. Because this\nfunction depends on the network connection and on a distant\nserver, you can specify a time limit if you don't want your\nprogram to get stuck waiting in case there is a problem; this\nlimit is deactivated by default.\n\nThis function makes use of DNS queries get the public address.\n\n\\param timeout Maximum time to wait, `std::nullopt` to wait forever\n\\param type    The type of public address to get\n\n\\return Public IP address of the computer on success, `std::nullopt` otherwise",
}; }

void bind_Dns(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    lua_glue::Table sf_Dns = sf["Dns"].get_or_create<lua_glue::Table>();
    auto type_sf__Dns__MxRecord = lua_glue::BindClass<sf::Dns::MxRecord>(sf_Dns, "MxRecord");
    lua_glue::Table table_sf__Dns__MxRecord = sf_Dns["MxRecord"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Dns::MxRecord>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Dns.MxRecord");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("exchange", "string");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("preference", "integer");
    LUASF_STUB_FUNCTION("sf.Dns.MxRecord", "new", "fun(): sf.Dns.MxRecord");
    lua_glue::BindCallable(type_sf__Dns__MxRecord, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Dns::MxRecord>();
        }
    );
    lua_glue::BindProperty(type_sf__Dns__MxRecord, "exchange",
        [](const sf::Dns::MxRecord& self) -> std::string {
            return lua_sf::to_utf8_string(self.exchange);
        },
        [](sf::Dns::MxRecord& self, std::string value) {
            self.exchange = lua_sf::to_sf_string(value);
        }
    );
    lua_glue::BindProperty(type_sf__Dns__MxRecord, "preference",
        [](const sf::Dns::MxRecord& self) {
            return self.preference;
        },
        [](sf::Dns::MxRecord& self, lua_sf::LuaIntegral<std::uint16_t> value) {
            self.preference = value.value();
        }
    );
    auto type_sf__Dns__SrvRecord = lua_glue::BindClass<sf::Dns::SrvRecord>(sf_Dns, "SrvRecord");
    lua_glue::Table table_sf__Dns__SrvRecord = sf_Dns["SrvRecord"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Dns::SrvRecord>(lua);
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_CLASS("sf.Dns.SrvRecord");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("target", "string");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("port", "integer");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("weight", "integer");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FIELD("priority", "integer");
    LUASF_STUB_FUNCTION("sf.Dns.SrvRecord", "new", "fun(): sf.Dns.SrvRecord");
    lua_glue::BindCallable(type_sf__Dns__SrvRecord, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Dns::SrvRecord>();
        }
    );
    lua_glue::BindProperty(type_sf__Dns__SrvRecord, "target",
        [](const sf::Dns::SrvRecord& self) -> std::string {
            return lua_sf::to_utf8_string(self.target);
        },
        [](sf::Dns::SrvRecord& self, std::string value) {
            self.target = lua_sf::to_sf_string(value);
        }
    );
    lua_glue::BindProperty(type_sf__Dns__SrvRecord, "port",
        [](const sf::Dns::SrvRecord& self) {
            return self.port;
        },
        [](sf::Dns::SrvRecord& self, lua_sf::LuaIntegral<std::uint16_t> value) {
            self.port = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Dns__SrvRecord, "weight",
        [](const sf::Dns::SrvRecord& self) {
            return self.weight;
        },
        [](sf::Dns::SrvRecord& self, lua_sf::LuaIntegral<std::uint16_t> value) {
            self.weight = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Dns__SrvRecord, "priority",
        [](const sf::Dns::SrvRecord& self) {
            return self.priority;
        },
        [](sf::Dns::SrvRecord& self, lua_sf::LuaIntegral<std::uint16_t> value) {
            self.priority = value.value();
        }
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Dns", "resolve", "fun(hostname: string, servers?: sf.IpAddress[], timeout?: sf.Time|nil): sf.IpAddress[]|nil");
    lua_glue::BindCallable(sf_Dns, "resolve",
        [lua](std::string hostname, lua_glue::Table servers, lua_glue::Object timeout) -> lua_glue::Object {
            auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
            auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
            return lua_sf::optional_to_object(lua, sf::Dns::resolve(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::vector_to_object(lua, std::vector<sf::IpAddress>{ });
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::optional_to_object(lua, static_cast<std::optional<sf::Time>>(sf::seconds(1)));
        }}},
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Dns", "queryNs", "fun(hostname: string, servers?: sf.IpAddress[], timeout?: sf.Time|nil): string[]");
    lua_glue::BindCallable(sf_Dns, "queryNs",
        [lua](std::string hostname, lua_glue::Table servers, lua_glue::Object timeout) -> lua_glue::Object {
            auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
            auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
            return lua_sf::vector_to_object(lua, sf::Dns::queryNs(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::vector_to_object(lua, std::vector<sf::IpAddress>{ });
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::optional_to_object(lua, static_cast<std::optional<sf::Time>>(sf::seconds(1)));
        }}},
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Dns", "queryMx", "fun(hostname: string, servers?: sf.IpAddress[], timeout?: sf.Time|nil): sf.Dns.MxRecord[]");
    lua_glue::BindCallable(sf_Dns, "queryMx",
        [lua](std::string hostname, lua_glue::Table servers, lua_glue::Object timeout) -> lua_glue::Object {
            auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
            auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
            return lua_sf::vector_to_object(lua, sf::Dns::queryMx(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::vector_to_object(lua, std::vector<sf::IpAddress>{ });
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::optional_to_object(lua, static_cast<std::optional<sf::Time>>(sf::seconds(1)));
        }}},
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Dns", "querySrv", "fun(hostname: string, servers?: sf.IpAddress[], timeout?: sf.Time|nil): sf.Dns.SrvRecord[]");
    lua_glue::BindCallable(sf_Dns, "querySrv",
        [lua](std::string hostname, lua_glue::Table servers, lua_glue::Object timeout) -> lua_glue::Object {
            auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
            auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
            return lua_sf::vector_to_object(lua, sf::Dns::querySrv(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::vector_to_object(lua, std::vector<sf::IpAddress>{ });
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::optional_to_object(lua, static_cast<std::optional<sf::Time>>(sf::seconds(1)));
        }}},
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Dns", "queryTxt", "fun(hostname: string, servers?: sf.IpAddress[], timeout?: sf.Time|nil): string[][]");
    lua_glue::BindCallable(sf_Dns, "queryTxt",
        [lua](std::string hostname, lua_glue::Table servers, lua_glue::Object timeout) -> lua_glue::Object {
            auto servers_vector = lua_sf::array_from_object<sf::IpAddress>(servers);
            auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
            return lua_sf::vector_to_object(lua, sf::Dns::queryTxt(lua_sf::to_sf_string(hostname), servers_vector, timeout_optional));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::vector_to_object(lua, std::vector<sf::IpAddress>{ });
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::optional_to_object(lua, static_cast<std::optional<sf::Time>>(sf::seconds(1)));
        }}},
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Dns", "getPublicAddress", "fun(timeout?: sf.Time|nil, type?: sf.IpAddress.Type): sf.IpAddress|nil");
    lua_glue::BindCallable(sf_Dns, "getPublicAddress",
        [lua](lua_glue::Object timeout, sf::IpAddress::Type type) -> lua_glue::Object {
            auto timeout_optional = lua_sf::optional_from_object<sf::Time>(timeout);
            return lua_sf::optional_to_object(lua, sf::Dns::getPublicAddress(timeout_optional, type));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::optional_to_object(lua, static_cast<std::optional<sf::Time>>(std::nullopt));
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::IpAddress::Type>(IpAddress::Type::IpV4);
        }}},
        docs[13]
    );
}
