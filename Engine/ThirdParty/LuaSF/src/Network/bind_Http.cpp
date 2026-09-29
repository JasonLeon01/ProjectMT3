#include "Network/bind_Http.hpp"

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

namespace { constexpr std::array<std::string_view, 48> docs = {
    "\\brief A HTTP client",
    "\\brief Default constructor",
    "\\brief Construct the HTTP client with the target host\n\nThis is equivalent to calling `setHost(host, port)`.\nThe port has a default value of 0, which means that the\nHTTP client will use the right port according to the\nprotocol used (80 for HTTP). You should leave it like\nthis unless you really need a port other than the\nstandard one, or use an unknown protocol.\n\n\\param host        Web server to connect to\n\\param port        Port to use for the connection\n\\param addressType Address type to use for the connection, `std::nullopt` to specify no preference",
    "\\brief Set the target host\n\nThis function just stores the host address and port, it\ndoesn't actually connect to it until you send a request.\nIt does however try to resolve the address.\nThe port has a default value of 0, which means that the\nHTTP client will use the right port according to the\nprotocol used (80 for HTTP). You should leave it like\nthis unless you really need a port other than the\nstandard one, or use an unknown protocol.\n\n\\param host        Web server to connect to\n\\param port        Port to use for the connection\n\\param addressType Address type to use for the connection, `std::nullopt` to specify no preference\n\n\\return `true` if the host has been resolved and is valid, `false` otherwise",
    "\\brief Send a HTTP request and return the server's response.\n\nYou must have a valid host before sending a request (see `setHost`).\nAny missing mandatory header field in the request will be added\nwith an appropriate value.\nWarning: this function waits for the server's response and may\nnot return instantly; use a thread if you don't want to block your\napplication, or use a timeout to limit the time to wait. A value\nof `Time::Zero` means that the client will use the system default timeout\n(which is usually pretty long).\n\n\\param request      Request to send\n\\param timeout      Maximum time to wait\n\\param verifyServer Verify the server if using HTTPS\n\n\\return Server's response",
    "\\brief HTTP request",
    "\\brief Default constructor\n\nThis constructor creates a GET request, with the root\nURI (\"/\") and an empty body.\n\n\\param uri    Target URI\n\\param method Method to use for the request\n\\param body   Content of the request's body",
    "\\brief Set the value of a field\n\nThe field is created if it doesn't exist. The name of\nthe field is case-insensitive.\nBy default, a request doesn't contain any field (but the\nmandatory fields are added later by the HTTP client when\nsending the request).\n\n\\param field Name of the field to set\n\\param value Value of the field",
    "\\brief Set the request method\n\nSee the Method enumeration for a complete list of all\nthe available methods.\nThe method is `Http::Request::Method::Get` by default.\n\n\\param method Method to use for the request",
    "\\brief Set the requested URI\n\nThe URI is the resource (usually a web page or a file)\nthat you want to get or post.\nThe URI is \"/\" (the root page) by default.\n\n\\param uri URI to request, relative to the host",
    "\\brief Set the HTTP version for the request\n\nThe HTTP version is 1.0 by default.\n\n\\param major Major HTTP version number\n\\param minor Minor HTTP version number",
    "\\brief Set the body of the request\n\nThe body of a request is optional and only makes sense\nfor POST requests. It is ignored for all other methods.\nThe body is empty by default.\n\n\\param body Content of the body",
    "\\brief Enumerate the available HTTP methods for a request",
    "Request in get mode, standard method to retrieve a page",
    "Request in post mode, usually to send data to a page",
    "Request a page's header only",
    "Request in put mode, useful for a REST API",
    "Request in delete mode, useful for a REST API",
    "\\brief HTTP response",
    "\\brief Get the value of a field\n\nIf the field `field` is not found in the response header,\nthe empty string is returned. This function uses\ncase-insensitive comparisons.\n\n\\param field Name of the field to get\n\n\\return Value of the field, or empty string if not found",
    "\\brief Get the response status code\n\nThe status code should be the first thing to be checked\nafter receiving a response, it defines whether it is a\nsuccess, a failure or anything else (see the Status\nenumeration).\n\n\\return Status code of the response",
    "\\brief Get the major HTTP version number of the response\n\n\\return Major HTTP version number\n\n\\see `getMinorHttpVersion`",
    "\\brief Get the minor HTTP version number of the response\n\n\\return Minor HTTP version number\n\n\\see `getMajorHttpVersion`",
    "\\brief Get the body of the response\n\nThe body of a response may contain:\n\\li the requested page (for GET requests)\n\\li a response from the server (for POST requests)\n\\li nothing (for HEAD requests)\n\\li an error message (in case of an error)\n\n\\return The response body",
    "\\brief Enumerate all the valid status codes for a response",
    "Most common code returned when operation was successful",
    "The resource has successfully been created",
    "The request has been accepted, but will be processed later by the server",
    "The server didn't send any data in return",
    "The server informs the client that it should clear the view (form) that caused the request to be sent",
    "The server has sent a part of the resource, as a response to a partial GET request",
    "The requested page can be accessed from several locations",
    "The requested page has permanently moved to a new location",
    "The requested page has temporarily moved to a new location",
    "For conditional requests, means the requested page hasn't changed and doesn't need to be refreshed",
    "The server couldn't understand the request (syntax error)",
    "The requested page needs an authentication to be accessed",
    "The requested page cannot be accessed at all, even with authentication",
    "The requested page doesn't exist",
    "The server can't satisfy the partial GET request (with a \"Range\" header field)",
    "The server encountered an unexpected error",
    "The server doesn't implement a requested feature",
    "The gateway server has received an error from the source server",
    "The server is temporarily unavailable (overloaded, in maintenance, ...)",
    "The gateway server couldn't receive a response from the source server",
    "The server doesn't support the requested HTTP version",
    "Response is not a valid HTTP one",
    "Connection with server failed",
}; }

void bind_Http(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Http = lua_glue::BindClass<sf::Http>(sf, "Http");
    lua_glue::Table table_sf__Http = sf["Http"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Http>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Http");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Http", "new", "fun(): sf.Http");
    LUASF_STUB_OVERLOAD("sf.Http", "new", "fun(host: string, port?: integer, addressType?: sf.IpAddress.Type|nil): sf.Http");
    lua_glue::BindCallable(type_sf__Http, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Http>();
        },
        docs[1]
    );
    lua_glue::BindCallable(type_sf__Http, "new",
        [](std::string host, lua_sf::LuaIntegral<unsigned short> port, lua_glue::Object addressType) {
            auto addressType_optional = lua_sf::optional_from_object<sf::IpAddress::Type>(addressType);
            return lua_sf::makeLuaSharedObject<sf::Http>(host, port.value(), addressType_optional);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned short>(0);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::optional_to_object(lua, static_cast<std::optional<sf::IpAddress::Type>>(std::nullopt));
        }}},
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Http", "setHost", "fun(self: sf.Http, host: string, port?: integer, addressType?: sf.IpAddress.Type|nil): boolean");
    lua_glue::BindCallable(type_sf__Http, "setHost",
        [](sf::Http& self, std::string host, lua_sf::LuaIntegral<unsigned short> port, lua_glue::Object addressType) -> bool {
            auto addressType_optional = lua_sf::optional_from_object<sf::IpAddress::Type>(addressType);
            return self.setHost(host, port.value(), addressType_optional);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned short>(0);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::optional_to_object(lua, static_cast<std::optional<sf::IpAddress::Type>>(std::nullopt));
        }}},
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Http", "sendRequest", "fun(self: sf.Http, request: sf.Http.Request, timeout?: sf.Time, verifyServer?: boolean): sf.Http.Response");
    lua_glue::BindCallable(type_sf__Http, "sendRequest",
        [](const sf::Http& self, const sf::Http::Request& request, sf::Time timeout, bool verifyServer) -> sf::Http::Response {
            return self.sendRequest(request, timeout, verifyServer);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Time>(sf::Time::Zero);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(true);
        }}},
        docs[4]
    );
    auto type_sf__Http__Request = lua_glue::BindClass<sf::Http::Request>(table_sf__Http, "Request");
    lua_glue::Table table_sf__Http__Request = table_sf__Http["Request"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Http::Request>(lua);
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_CLASS("sf.Http.Request");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Http.Request", "new", "fun(uri?: string, method?: sf.Http.Request.Method, body?: string): sf.Http.Request");
    lua_glue::BindCallable(type_sf__Http__Request, "new",
        [](std::string uri, sf::Http::Request::Method method, std::string body) {
            return lua_sf::makeLuaSharedObject<sf::Http::Request>(uri, method, body);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::string(static_cast<std::string>("/"));
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Http::Request::Method>(sf::Http::Request::Method::Get);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::string(static_cast<std::string>(""));
        }}},
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Http.Request", "setField", "fun(self: sf.Http.Request, field: string, value: string)");
    lua_glue::BindCallable(type_sf__Http__Request, "setField",
        [](sf::Http::Request& self, std::string field, std::string value) {
            self.setField(field, value);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Http.Request", "setMethod", "fun(self: sf.Http.Request, method: sf.Http.Request.Method)");
    lua_glue::BindCallable(type_sf__Http__Request, "setMethod",
        [](sf::Http::Request& self, sf::Http::Request::Method method) {
            self.setMethod(method);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Http.Request", "setUri", "fun(self: sf.Http.Request, uri: string)");
    lua_glue::BindCallable(type_sf__Http__Request, "setUri",
        [](sf::Http::Request& self, std::string uri) {
            self.setUri(uri);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Http.Request", "setHttpVersion", "fun(self: sf.Http.Request, major: integer, minor: integer)");
    lua_glue::BindCallable(type_sf__Http__Request, "setHttpVersion",
        [](sf::Http::Request& self, lua_sf::LuaIntegral<unsigned int> major, lua_sf::LuaIntegral<unsigned int> minor) {
            self.setHttpVersion(major.value(), minor.value());
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Http.Request", "setBody", "fun(self: sf.Http.Request, body: string)");
    lua_glue::BindCallable(type_sf__Http__Request, "setBody",
        [](sf::Http::Request& self, std::string body) {
            self.setBody(body);
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_CLASS("sf.Http.Request.Method");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FIELD("Get", "sf.Http.Request.Method");
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FIELD("Post", "sf.Http.Request.Method");
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FIELD("Head", "sf.Http.Request.Method");
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FIELD("Put", "sf.Http.Request.Method");
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FIELD("Delete", "sf.Http.Request.Method");
    lua_glue::BindEnum<sf::Http::Request::Method>(table_sf__Http__Request, "Method", {
        {"Get", sf::Http::Request::Method::Get},
        {"Post", sf::Http::Request::Method::Post},
        {"Head", sf::Http::Request::Method::Head},
        {"Put", sf::Http::Request::Method::Put},
        {"Delete", sf::Http::Request::Method::Delete}
    });
    auto type_sf__Http__Response = lua_glue::BindClass<sf::Http::Response>(table_sf__Http, "Response");
    lua_glue::Table table_sf__Http__Response = table_sf__Http["Response"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Http::Response>(lua);
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_CLASS("sf.Http.Response");
    LUASF_STUB_FUNCTION("sf.Http.Response", "new", "fun(): sf.Http.Response");
    lua_glue::BindCallable(type_sf__Http__Response, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Http::Response>();
        }
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Http.Response", "getField", "fun(self: sf.Http.Response, field: string): string");
    lua_glue::BindCallable(type_sf__Http__Response, "getField",
        [](const sf::Http::Response& self, std::string field) -> std::string {
            return std::string(self.getField(field));
        },
        docs[19],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Http.Response", "getStatus", "fun(self: sf.Http.Response): sf.Http.Response.Status");
    lua_glue::BindCallable(type_sf__Http__Response, "getStatus",
        [](const sf::Http::Response& self) -> sf::Http::Response::Status {
            return self.getStatus();
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Http.Response", "getMajorHttpVersion", "fun(self: sf.Http.Response): integer");
    lua_glue::BindCallable(type_sf__Http__Response, "getMajorHttpVersion",
        [](const sf::Http::Response& self) -> unsigned int {
            return self.getMajorHttpVersion();
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Http.Response", "getMinorHttpVersion", "fun(self: sf.Http.Response): integer");
    lua_glue::BindCallable(type_sf__Http__Response, "getMinorHttpVersion",
        [](const sf::Http::Response& self) -> unsigned int {
            return self.getMinorHttpVersion();
        },
        docs[22]
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.Http.Response", "getBody", "fun(self: sf.Http.Response): string");
    lua_glue::BindCallable(type_sf__Http__Response, "getBody",
        [](const sf::Http::Response& self) -> std::string {
            return std::string(self.getBody());
        },
        docs[23],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_CLASS("sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FIELD("Ok", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FIELD("Created", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FIELD("Accepted", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FIELD("NoContent", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_FIELD("ResetContent", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FIELD("PartialContent", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FIELD("MultipleChoices", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FIELD("MovedPermanently", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FIELD("MovedTemporarily", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FIELD("NotModified", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FIELD("BadRequest", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FIELD("Unauthorized", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[37]);
    LUASF_STUB_FIELD("Forbidden", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FIELD("NotFound", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FIELD("RangeNotSatisfiable", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[40]);
    LUASF_STUB_FIELD("InternalServerError", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[41]);
    LUASF_STUB_FIELD("NotImplemented", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[42]);
    LUASF_STUB_FIELD("BadGateway", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[43]);
    LUASF_STUB_FIELD("ServiceNotAvailable", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[44]);
    LUASF_STUB_FIELD("GatewayTimeout", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[45]);
    LUASF_STUB_FIELD("VersionNotSupported", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[46]);
    LUASF_STUB_FIELD("InvalidResponse", "sf.Http.Response.Status");
    LUASF_STUB_DOC(docs[47]);
    LUASF_STUB_FIELD("ConnectionFailed", "sf.Http.Response.Status");
    lua_glue::BindEnum<sf::Http::Response::Status>(table_sf__Http__Response, "Status", {
        {"Ok", sf::Http::Response::Status::Ok},
        {"Created", sf::Http::Response::Status::Created},
        {"Accepted", sf::Http::Response::Status::Accepted},
        {"NoContent", sf::Http::Response::Status::NoContent},
        {"ResetContent", sf::Http::Response::Status::ResetContent},
        {"PartialContent", sf::Http::Response::Status::PartialContent},
        {"MultipleChoices", sf::Http::Response::Status::MultipleChoices},
        {"MovedPermanently", sf::Http::Response::Status::MovedPermanently},
        {"MovedTemporarily", sf::Http::Response::Status::MovedTemporarily},
        {"NotModified", sf::Http::Response::Status::NotModified},
        {"BadRequest", sf::Http::Response::Status::BadRequest},
        {"Unauthorized", sf::Http::Response::Status::Unauthorized},
        {"Forbidden", sf::Http::Response::Status::Forbidden},
        {"NotFound", sf::Http::Response::Status::NotFound},
        {"RangeNotSatisfiable", sf::Http::Response::Status::RangeNotSatisfiable},
        {"InternalServerError", sf::Http::Response::Status::InternalServerError},
        {"NotImplemented", sf::Http::Response::Status::NotImplemented},
        {"BadGateway", sf::Http::Response::Status::BadGateway},
        {"ServiceNotAvailable", sf::Http::Response::Status::ServiceNotAvailable},
        {"GatewayTimeout", sf::Http::Response::Status::GatewayTimeout},
        {"VersionNotSupported", sf::Http::Response::Status::VersionNotSupported},
        {"InvalidResponse", sf::Http::Response::Status::InvalidResponse},
        {"ConnectionFailed", sf::Http::Response::Status::ConnectionFailed}
    });
}
