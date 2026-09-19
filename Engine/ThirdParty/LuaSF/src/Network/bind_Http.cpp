#include "Network/bind_Http.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Http(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Http = sf.new_usertype<sf::Http>("Http", sol::no_constructor);
    sol::table table_sf__Http = sf["Http"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Http>(lua);
    LUASF_STUB_DOC("\\brief A HTTP client");
    LUASF_STUB_CLASS("sf.Http");
    LUASF_STUB_DOC("\\brief Construct the HTTP client with the target host\n\nThis is equivalent to calling `setHost(host, port)`.\nThe port has a default value of 0, which means that the\nHTTP client will use the right port according to the\nprotocol used (80 for HTTP). You should leave it like\nthis unless you really need a port other than the\nstandard one, or use an unknown protocol.\n\n\\param host        Web server to connect to\n\\param port        Port to use for the connection\n\\param addressType Address type to use for the connection, `std::nullopt` to specify no preference");
    LUASF_STUB_FUNCTION("sf.Http", "new", "fun(host: string, port: integer): sf.Http");
    LUASF_STUB_OVERLOAD("sf.Http", "new", "fun(host: string): sf.Http");
    LUASF_STUB_OVERLOAD("sf.Http", "new", "fun(): sf.Http");
    LUASF_STUB_OVERLOAD("sf.Http", "new", "fun(host: string, port: integer, addressType: sf.IpAddress.Type|nil): sf.Http");
    type_sf__Http.set_function("new", sol::factories(
        [](std::string host, lua_sf::LuaIntegral<unsigned short> port) {
            return lua_sf::makeLuaSharedObject<sf::Http>(host, port.value());
        },
        [](std::string host) {
            return lua_sf::makeLuaSharedObject<sf::Http>(host);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::Http>();
        },
        [](std::string host, lua_sf::LuaIntegral<unsigned short> port, sol::object addressType) {
            auto addressType_optional = lua_sf::optional_from_object<sf::IpAddress::Type>(addressType);
            return lua_sf::makeLuaSharedObject<sf::Http>(host, port.value(), addressType_optional);
        }
    ));
    LUASF_STUB_DOC("\\brief Set the target host\n\nThis function just stores the host address and port, it\ndoesn't actually connect to it until you send a request.\nIt does however try to resolve the address.\nThe port has a default value of 0, which means that the\nHTTP client will use the right port according to the\nprotocol used (80 for HTTP). You should leave it like\nthis unless you really need a port other than the\nstandard one, or use an unknown protocol.\n\n\\param host        Web server to connect to\n\\param port        Port to use for the connection\n\\param addressType Address type to use for the connection, `std::nullopt` to specify no preference\n\n\\return `true` if the host has been resolved and is valid, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Http", "setHost", "fun(self: sf.Http, host: string, port: integer): boolean");
    LUASF_STUB_OVERLOAD("sf.Http", "setHost", "fun(self: sf.Http, host: string): boolean");
    LUASF_STUB_OVERLOAD("sf.Http", "setHost", "fun(self: sf.Http, host: string, port: integer, addressType: sf.IpAddress.Type|nil): boolean");
    type_sf__Http.set_function("setHost",
        sol::overload(
            [](sf::Http& self, std::string host, lua_sf::LuaIntegral<unsigned short> port) -> bool {
                return self.setHost(host, port.value());
            },
            [](sf::Http& self, std::string host) -> bool {
                return self.setHost(host);
            },
            [](sf::Http& self, std::string host, lua_sf::LuaIntegral<unsigned short> port, sol::object addressType) -> bool {
                auto addressType_optional = lua_sf::optional_from_object<sf::IpAddress::Type>(addressType);
                return self.setHost(host, port.value(), addressType_optional);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Send a HTTP request and return the server's response.\n\nYou must have a valid host before sending a request (see `setHost`).\nAny missing mandatory header field in the request will be added\nwith an appropriate value.\nWarning: this function waits for the server's response and may\nnot return instantly; use a thread if you don't want to block your\napplication, or use a timeout to limit the time to wait. A value\nof `Time::Zero` means that the client will use the system default timeout\n(which is usually pretty long).\n\n\\param request      Request to send\n\\param timeout      Maximum time to wait\n\\param verifyServer Verify the server if using HTTPS\n\n\\return Server's response");
    LUASF_STUB_FUNCTION("sf.Http", "sendRequest", "fun(self: sf.Http, request: sf.Http.Request, timeout: sf.Time, verifyServer: boolean): sf.Http.Response");
    LUASF_STUB_OVERLOAD("sf.Http", "sendRequest", "fun(self: sf.Http, request: sf.Http.Request, timeout: sf.Time): sf.Http.Response");
    LUASF_STUB_OVERLOAD("sf.Http", "sendRequest", "fun(self: sf.Http, request: sf.Http.Request): sf.Http.Response");
    type_sf__Http.set_function("sendRequest",
        sol::overload(
            [](sf::Http& self, const sf::Http::Request& request, sf::Time timeout, bool verifyServer) -> sf::Http::Response {
                return self.sendRequest(request, timeout, verifyServer);
            },
            [](sf::Http& self, const sf::Http::Request& request, sf::Time timeout) -> sf::Http::Response {
                return self.sendRequest(request, timeout);
            },
            [](sf::Http& self, const sf::Http::Request& request) -> sf::Http::Response {
                return self.sendRequest(request);
            }
        )
    );
    auto type_sf__Http__Request = table_sf__Http.new_usertype<sf::Http::Request>("Request", sol::no_constructor);
    sol::table table_sf__Http__Request = table_sf__Http["Request"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Http::Request>(lua);
    LUASF_STUB_DOC("\\brief HTTP request");
    LUASF_STUB_CLASS("sf.Http.Request");
    LUASF_STUB_DOC("\\brief Default constructor\n\nThis constructor creates a GET request, with the root\nURI (\"/\") and an empty body.\n\n\\param uri    Target URI\n\\param method Method to use for the request\n\\param body   Content of the request's body");
    LUASF_STUB_FUNCTION("sf.Http.Request", "new", "fun(uri: string, method: sf.Http.Request.Method, body: string): sf.Http.Request");
    LUASF_STUB_OVERLOAD("sf.Http.Request", "new", "fun(uri: string, method: sf.Http.Request.Method): sf.Http.Request");
    LUASF_STUB_OVERLOAD("sf.Http.Request", "new", "fun(uri: string): sf.Http.Request");
    LUASF_STUB_OVERLOAD("sf.Http.Request", "new", "fun(): sf.Http.Request");
    type_sf__Http__Request.set_function("new", sol::factories(
        [](std::string uri, sf::Http::Request::Method method, std::string body) {
            return lua_sf::makeLuaSharedObject<sf::Http::Request>(uri, method, body);
        },
        [](std::string uri, sf::Http::Request::Method method) {
            return lua_sf::makeLuaSharedObject<sf::Http::Request>(uri, method);
        },
        [](std::string uri) {
            return lua_sf::makeLuaSharedObject<sf::Http::Request>(uri);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::Http::Request>();
        }
    ));
    LUASF_STUB_DOC("\\brief Set the value of a field\n\nThe field is created if it doesn't exist. The name of\nthe field is case-insensitive.\nBy default, a request doesn't contain any field (but the\nmandatory fields are added later by the HTTP client when\nsending the request).\n\n\\param field Name of the field to set\n\\param value Value of the field");
    LUASF_STUB_FUNCTION("sf.Http.Request", "setField", "fun(self: sf.Http.Request, field: string, value: string)");
    type_sf__Http__Request.set_function("setField",
        [](sf::Http::Request& self, std::string field, std::string value) {
            self.setField(field, value);
        }
    );
    LUASF_STUB_DOC("\\brief Set the request method\n\nSee the Method enumeration for a complete list of all\nthe available methods.\nThe method is `Http::Request::Method::Get` by default.\n\n\\param method Method to use for the request");
    LUASF_STUB_FUNCTION("sf.Http.Request", "setMethod", "fun(self: sf.Http.Request, method: sf.Http.Request.Method)");
    type_sf__Http__Request.set_function("setMethod",
        [](sf::Http::Request& self, sf::Http::Request::Method method) {
            self.setMethod(method);
        }
    );
    LUASF_STUB_DOC("\\brief Set the requested URI\n\nThe URI is the resource (usually a web page or a file)\nthat you want to get or post.\nThe URI is \"/\" (the root page) by default.\n\n\\param uri URI to request, relative to the host");
    LUASF_STUB_FUNCTION("sf.Http.Request", "setUri", "fun(self: sf.Http.Request, uri: string)");
    type_sf__Http__Request.set_function("setUri",
        [](sf::Http::Request& self, std::string uri) {
            self.setUri(uri);
        }
    );
    LUASF_STUB_DOC("\\brief Set the HTTP version for the request\n\nThe HTTP version is 1.0 by default.\n\n\\param major Major HTTP version number\n\\param minor Minor HTTP version number");
    LUASF_STUB_FUNCTION("sf.Http.Request", "setHttpVersion", "fun(self: sf.Http.Request, major: integer, minor: integer)");
    type_sf__Http__Request.set_function("setHttpVersion",
        [](sf::Http::Request& self, lua_sf::LuaIntegral<unsigned int> major, lua_sf::LuaIntegral<unsigned int> minor) {
            self.setHttpVersion(major.value(), minor.value());
        }
    );
    LUASF_STUB_DOC("\\brief Set the body of the request\n\nThe body of a request is optional and only makes sense\nfor POST requests. It is ignored for all other methods.\nThe body is empty by default.\n\n\\param body Content of the body");
    LUASF_STUB_FUNCTION("sf.Http.Request", "setBody", "fun(self: sf.Http.Request, body: string)");
    type_sf__Http__Request.set_function("setBody",
        [](sf::Http::Request& self, std::string body) {
            self.setBody(body);
        }
    );
    LUASF_STUB_DOC("\\brief Enumerate the available HTTP methods for a request");
    LUASF_STUB_CLASS("sf.Http.Request.Method");
    LUASF_STUB_DOC("Request in get mode, standard method to retrieve a page");
    LUASF_STUB_FIELD("Get", "sf.Http.Request.Method");
    LUASF_STUB_DOC("Request in post mode, usually to send data to a page");
    LUASF_STUB_FIELD("Post", "sf.Http.Request.Method");
    LUASF_STUB_DOC("Request a page's header only");
    LUASF_STUB_FIELD("Head", "sf.Http.Request.Method");
    LUASF_STUB_DOC("Request in put mode, useful for a REST API");
    LUASF_STUB_FIELD("Put", "sf.Http.Request.Method");
    LUASF_STUB_DOC("Request in delete mode, useful for a REST API");
    LUASF_STUB_FIELD("Delete", "sf.Http.Request.Method");
    table_sf__Http__Request.new_enum("Method",
        "Get", sf::Http::Request::Method::Get,
        "Post", sf::Http::Request::Method::Post,
        "Head", sf::Http::Request::Method::Head,
        "Put", sf::Http::Request::Method::Put,
        "Delete", sf::Http::Request::Method::Delete
    );
    auto type_sf__Http__Response = table_sf__Http.new_usertype<sf::Http::Response>("Response", sol::no_constructor);
    sol::table table_sf__Http__Response = table_sf__Http["Response"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Http::Response>(lua);
    LUASF_STUB_DOC("\\brief HTTP response");
    LUASF_STUB_CLASS("sf.Http.Response");
    LUASF_STUB_FUNCTION("sf.Http.Response", "new", "fun(): sf.Http.Response");
    type_sf__Http__Response.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Http::Response>();
        }
    ));
    LUASF_STUB_DOC("\\brief Get the value of a field\n\nIf the field `field` is not found in the response header,\nthe empty string is returned. This function uses\ncase-insensitive comparisons.\n\n\\param field Name of the field to get\n\n\\return Value of the field, or empty string if not found");
    LUASF_STUB_FUNCTION("sf.Http.Response", "getField", "fun(self: sf.Http.Response, field: string): string");
    type_sf__Http__Response.set_function("getField",
        sol::policies(
            [](sf::Http::Response& self, std::string field) -> std::string {
                return std::string(self.getField(field));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the response status code\n\nThe status code should be the first thing to be checked\nafter receiving a response, it defines whether it is a\nsuccess, a failure or anything else (see the Status\nenumeration).\n\n\\return Status code of the response");
    LUASF_STUB_FUNCTION("sf.Http.Response", "getStatus", "fun(self: sf.Http.Response): sf.Http.Response.Status");
    type_sf__Http__Response.set_function("getStatus",
        [](sf::Http::Response& self) -> sf::Http::Response::Status {
            return self.getStatus();
        }
    );
    LUASF_STUB_DOC("\\brief Get the major HTTP version number of the response\n\n\\return Major HTTP version number\n\n\\see `getMinorHttpVersion`");
    LUASF_STUB_FUNCTION("sf.Http.Response", "getMajorHttpVersion", "fun(self: sf.Http.Response): integer");
    type_sf__Http__Response.set_function("getMajorHttpVersion",
        [](sf::Http::Response& self) -> unsigned int {
            return self.getMajorHttpVersion();
        }
    );
    LUASF_STUB_DOC("\\brief Get the minor HTTP version number of the response\n\n\\return Minor HTTP version number\n\n\\see `getMajorHttpVersion`");
    LUASF_STUB_FUNCTION("sf.Http.Response", "getMinorHttpVersion", "fun(self: sf.Http.Response): integer");
    type_sf__Http__Response.set_function("getMinorHttpVersion",
        [](sf::Http::Response& self) -> unsigned int {
            return self.getMinorHttpVersion();
        }
    );
    LUASF_STUB_DOC("\\brief Get the body of the response\n\nThe body of a response may contain:\n\\li the requested page (for GET requests)\n\\li a response from the server (for POST requests)\n\\li nothing (for HEAD requests)\n\\li an error message (in case of an error)\n\n\\return The response body");
    LUASF_STUB_FUNCTION("sf.Http.Response", "getBody", "fun(self: sf.Http.Response): string");
    type_sf__Http__Response.set_function("getBody",
        sol::policies(
            [](sf::Http::Response& self) -> std::string {
                return std::string(self.getBody());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Enumerate all the valid status codes for a response");
    LUASF_STUB_CLASS("sf.Http.Response.Status");
    LUASF_STUB_DOC("Most common code returned when operation was successful");
    LUASF_STUB_FIELD("Ok", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The resource has successfully been created");
    LUASF_STUB_FIELD("Created", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The request has been accepted, but will be processed later by the server");
    LUASF_STUB_FIELD("Accepted", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The server didn't send any data in return");
    LUASF_STUB_FIELD("NoContent", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The server informs the client that it should clear the view (form) that caused the request to be sent");
    LUASF_STUB_FIELD("ResetContent", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The server has sent a part of the resource, as a response to a partial GET request");
    LUASF_STUB_FIELD("PartialContent", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The requested page can be accessed from several locations");
    LUASF_STUB_FIELD("MultipleChoices", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The requested page has permanently moved to a new location");
    LUASF_STUB_FIELD("MovedPermanently", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The requested page has temporarily moved to a new location");
    LUASF_STUB_FIELD("MovedTemporarily", "sf.Http.Response.Status");
    LUASF_STUB_DOC("For conditional requests, means the requested page hasn't changed and doesn't need to be refreshed");
    LUASF_STUB_FIELD("NotModified", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The server couldn't understand the request (syntax error)");
    LUASF_STUB_FIELD("BadRequest", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The requested page needs an authentication to be accessed");
    LUASF_STUB_FIELD("Unauthorized", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The requested page cannot be accessed at all, even with authentication");
    LUASF_STUB_FIELD("Forbidden", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The requested page doesn't exist");
    LUASF_STUB_FIELD("NotFound", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The server can't satisfy the partial GET request (with a \"Range\" header field)");
    LUASF_STUB_FIELD("RangeNotSatisfiable", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The server encountered an unexpected error");
    LUASF_STUB_FIELD("InternalServerError", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The server doesn't implement a requested feature");
    LUASF_STUB_FIELD("NotImplemented", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The gateway server has received an error from the source server");
    LUASF_STUB_FIELD("BadGateway", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The server is temporarily unavailable (overloaded, in maintenance, ...)");
    LUASF_STUB_FIELD("ServiceNotAvailable", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The gateway server couldn't receive a response from the source server");
    LUASF_STUB_FIELD("GatewayTimeout", "sf.Http.Response.Status");
    LUASF_STUB_DOC("The server doesn't support the requested HTTP version");
    LUASF_STUB_FIELD("VersionNotSupported", "sf.Http.Response.Status");
    LUASF_STUB_DOC("Response is not a valid HTTP one");
    LUASF_STUB_FIELD("InvalidResponse", "sf.Http.Response.Status");
    LUASF_STUB_DOC("Connection with server failed");
    LUASF_STUB_FIELD("ConnectionFailed", "sf.Http.Response.Status");
    table_sf__Http__Response.new_enum("Status",
        "Ok", sf::Http::Response::Status::Ok,
        "Created", sf::Http::Response::Status::Created,
        "Accepted", sf::Http::Response::Status::Accepted,
        "NoContent", sf::Http::Response::Status::NoContent,
        "ResetContent", sf::Http::Response::Status::ResetContent,
        "PartialContent", sf::Http::Response::Status::PartialContent,
        "MultipleChoices", sf::Http::Response::Status::MultipleChoices,
        "MovedPermanently", sf::Http::Response::Status::MovedPermanently,
        "MovedTemporarily", sf::Http::Response::Status::MovedTemporarily,
        "NotModified", sf::Http::Response::Status::NotModified,
        "BadRequest", sf::Http::Response::Status::BadRequest,
        "Unauthorized", sf::Http::Response::Status::Unauthorized,
        "Forbidden", sf::Http::Response::Status::Forbidden,
        "NotFound", sf::Http::Response::Status::NotFound,
        "RangeNotSatisfiable", sf::Http::Response::Status::RangeNotSatisfiable,
        "InternalServerError", sf::Http::Response::Status::InternalServerError,
        "NotImplemented", sf::Http::Response::Status::NotImplemented,
        "BadGateway", sf::Http::Response::Status::BadGateway,
        "ServiceNotAvailable", sf::Http::Response::Status::ServiceNotAvailable,
        "GatewayTimeout", sf::Http::Response::Status::GatewayTimeout,
        "VersionNotSupported", sf::Http::Response::Status::VersionNotSupported,
        "InvalidResponse", sf::Http::Response::Status::InvalidResponse,
        "ConnectionFailed", sf::Http::Response::Status::ConnectionFailed
    );
}
