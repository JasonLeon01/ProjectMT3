#include "Network/bind_Ftp.hpp"

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

namespace { constexpr std::array<std::string_view, 76> docs = {
    "\\brief A FTP client\n\n\\deprecated Use `sf::Sftp` if possible.",
    "\\brief Default constructor",
    "\\brief Connect to the specified FTP server\n\nThe port has a default value of 21, which is the standard\nport used by the FTP protocol. You shouldn't use a different\nvalue, unless you really know what you do.\nThis function tries to connect to the server so it may take\na while to complete, especially if the server is not\nreachable. To avoid blocking your application for too long,\nyou can use a timeout. The default value, `Time::Zero`, means that the\nsystem timeout will be used (which is usually pretty long).\n\n\\param server  Name or address of the FTP server to connect to\n\\param port    Port used for the connection\n\\param timeout Maximum time to wait\n\n\\return Server response to the request\n\n\\see `disconnect`",
    "\\brief Close the connection with the server\n\n\\return Server response to the request\n\n\\see `connect`",
    "\\brief Log in using an anonymous account\n\nLogging in is mandatory after connecting to the server.\nUsers that are not logged in cannot perform any operation.\n\n\\return Server response to the request",
    "\\brief Log in using a username and a password\n\nLogging in is mandatory after connecting to the server.\nUsers that are not logged in cannot perform any operation.\n\n\\param name     User name\n\\param password Password\n\n\\return Server response to the request",
    "\\brief Send a null command to keep the connection alive\n\nThis command is useful because the server may close the\nconnection automatically if no command is sent.\n\n\\return Server response to the request",
    "\\brief Get the current working directory\n\nThe working directory is the root path for subsequent\noperations involving directories and/or filenames.\n\n\\return Server response to the request\n\n\\see `getDirectoryListing`, `changeDirectory`, `parentDirectory`",
    "\\brief Get the contents of the given directory\n\nThis function retrieves the sub-directories and files\ncontained in the given directory. It is not recursive.\nThe `directory` parameter is relative to the current\nworking directory.\n\n\\param directory Directory to list\n\n\\return Server response to the request\n\n\\see `getWorkingDirectory`, `changeDirectory`, `parentDirectory`",
    "\\brief Change the current working directory\n\nThe new directory must be relative to the current one.\n\n\\param directory New working directory\n\n\\return Server response to the request\n\n\\see `getWorkingDirectory`, `getDirectoryListing`, `parentDirectory`",
    "\\brief Go to the parent directory of the current one\n\n\\return Server response to the request\n\n\\see `getWorkingDirectory`, `getDirectoryListing`, `changeDirectory`",
    "\\brief Create a new directory\n\nThe new directory is created as a child of the current\nworking directory.\n\n\\param name Name of the directory to create\n\n\\return Server response to the request\n\n\\see `deleteDirectory`",
    "\\brief Remove an existing directory\n\nThe directory to remove must be relative to the\ncurrent working directory.\nUse this function with caution, the directory will\nbe removed permanently!\n\n\\param name Name of the directory to remove\n\n\\return Server response to the request\n\n\\see `createDirectory`",
    "\\brief Rename an existing file\n\nThe file names must be relative to the current working\ndirectory.\n\n\\param file    File to rename\n\\param newName New name of the file\n\n\\return Server response to the request\n\n\\see `deleteFile`",
    "\\brief Remove an existing file\n\nThe file name must be relative to the current working\ndirectory.\nUse this function with caution, the file will be\nremoved permanently!\n\n\\param name File to remove\n\n\\return Server response to the request\n\n\\see `renameFile`",
    "\\brief Download a file from the server\n\nThe file name of the distant file is relative to the\ncurrent working directory of the server, and the local\ndestination path is relative to the current directory\nof your application.\nIf a file with the same file name as the distant file\nalready exists in the local destination path, it will\nbe overwritten.\n\n\\param remoteFile File name of the distant file to download\n\\param localPath  The directory in which to put the file on the local computer\n\\param mode       Transfer mode\n\n\\return Server response to the request\n\n\\see `upload`",
    "\\brief Upload a file to the server\n\nThe name of the local file is relative to the current\nworking directory of your application, and the\nremote path is relative to the current directory of the\nFTP server.\n\nThe append parameter controls whether the remote file is\nappended to or overwritten if it already exists.\n\n\\param localFile  Path of the local file to upload\n\\param remotePath The directory in which to put the file on the server\n\\param mode       Transfer mode\n\\param append     Pass `true` to append to or `false` to overwrite the remote file if it already exists\n\n\\return Server response to the request\n\n\\see `download`",
    "\\brief Send a command to the FTP server\n\nWhile the most often used commands are provided as member\nfunctions in the `sf::Ftp` class, this method can be used\nto send any FTP command to the server. If the command\nrequires one or more parameters, they can be specified\nin `parameter`. If the server returns information, you\ncan extract it from the response using `Response::getMessage()`.\n\n\\param command   Command to send\n\\param parameter Command parameter\n\n\\return Server response to the request",
    "\\brief Enumeration of transfer modes",
    "Binary mode (file is transferred as a sequence of bytes)",
    "Text mode using ASCII encoding",
    "Text mode using EBCDIC encoding",
    "\\brief FTP response",
    "\\brief Default constructor\n\nThis constructor is used by the FTP client to build\nthe response.\n\n\\param code    Response status code\n\\param message Response message",
    "\\brief Check if the status code means a success\n\nThis function is defined for convenience, it is\nequivalent to testing if the status code is < 400.\n\n\\return `true` if the status is a success, `false` if it is a failure",
    "\\brief Get the status code of the response\n\n\\return Status code",
    "\\brief Get the full message contained in the response\n\n\\return The response message",
    "\\brief Status codes possibly returned by a FTP response",
    "Restart marker reply",
    "Service ready in N minutes",
    "Data connection already opened, transfer starting",
    "File status ok, about to open data connection",
    "Command ok",
    "Command not implemented",
    "System status, or system help reply",
    "Directory status",
    "File status",
    "Help message",
    "NAME system type, where NAME is an official system name from the list in the Assigned Numbers document",
    "Service ready for new user",
    "Service closing control connection",
    "Data connection open, no transfer in progress",
    "Closing data connection, requested file action successful",
    "Entering passive mode",
    "User logged in, proceed. Logged out if appropriate",
    "Requested file action ok",
    "PATHNAME created",
    "User name ok, need password",
    "Need account for login",
    "Requested file action pending further information",
    "Service not available, closing control connection",
    "Can't open data connection",
    "Connection closed, transfer aborted",
    "Requested file action not taken",
    "Requested action aborted, local error in processing",
    "Requested action not taken; insufficient storage space in system, file unavailable",
    "Syntax error, command unrecognized",
    "Syntax error in parameters or arguments",
    "Bad sequence of commands",
    "Command not implemented for that parameter",
    "Not logged in",
    "Need account for storing files",
    "Requested action not taken, file unavailable",
    "Requested action aborted, page type unknown",
    "Requested file action aborted, exceeded storage allocation",
    "Requested action not taken, file name not allowed",
    "Not part of the FTP standard, generated by SFML when a received response cannot be parsed",
    "Not part of the FTP standard, generated by SFML when the low-level socket connection with the server fails",
    "Not part of the FTP standard, generated by SFML when the low-level socket connection is unexpectedly closed",
    "Not part of the FTP standard, generated by SFML when a local file cannot be read or written",
    "\\brief Specialization of FTP response returning a directory",
    "\\brief Default constructor\n\n\\param response Source response",
    "\\brief Get the directory returned in the response\n\n\\return Directory name",
    "\\brief Specialization of FTP response returning a\nfile name listing",
    "\\brief Default constructor\n\n\\param response  Source response\n\\param data      Data containing the raw listing",
    "\\brief Return the array of directory/file names\n\n\\return Array containing the requested listing",
}; }

void bind_Ftp(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Ftp = lua_glue::BindClass<sf::Ftp>(sf, "Ftp");
    lua_glue::Table table_sf__Ftp = sf["Ftp"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Ftp>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Ftp");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Ftp", "new", "fun(): sf.Ftp");
    lua_glue::BindCallable(type_sf__Ftp, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Ftp>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Ftp", "connect", "fun(self: sf.Ftp, server: sf.IpAddress, port?: integer, timeout?: sf.Time): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "connect",
        [](sf::Ftp& self, sf::IpAddress server, lua_sf::LuaIntegral<unsigned short> port, sf::Time timeout) -> sf::Ftp::Response {
            return self.connect(server, port.value(), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned short>(21);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Time>(sf::Time::Zero);
        }}},
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Ftp", "disconnect", "fun(self: sf.Ftp): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "disconnect",
        [](sf::Ftp& self) -> sf::Ftp::Response {
            return self.disconnect();
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Ftp", "login", "fun(self: sf.Ftp, name: string, password: string): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp", "login", "fun(self: sf.Ftp): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "login",
        [](sf::Ftp& self, std::string name, std::string password) -> sf::Ftp::Response {
            return self.login(name, password);
        },
        docs[5]
    );
    lua_glue::BindCallable(type_sf__Ftp, "login",
        [](sf::Ftp& self) -> sf::Ftp::Response {
            return self.login();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Ftp", "keepAlive", "fun(self: sf.Ftp): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "keepAlive",
        [](sf::Ftp& self) -> sf::Ftp::Response {
            return self.keepAlive();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Ftp", "getWorkingDirectory", "fun(self: sf.Ftp): sf.Ftp.DirectoryResponse");
    lua_glue::BindCallable(type_sf__Ftp, "getWorkingDirectory",
        [](sf::Ftp& self) -> sf::Ftp::DirectoryResponse {
            return self.getWorkingDirectory();
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Ftp", "getDirectoryListing", "fun(self: sf.Ftp, directory?: string): sf.Ftp.ListingResponse");
    lua_glue::BindCallable(type_sf__Ftp, "getDirectoryListing",
        [](sf::Ftp& self, std::string directory) -> sf::Ftp::ListingResponse {
            return self.getDirectoryListing(directory);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::string(static_cast<std::string>(""));
        }}},
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Ftp", "changeDirectory", "fun(self: sf.Ftp, directory: string): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "changeDirectory",
        [](sf::Ftp& self, std::string directory) -> sf::Ftp::Response {
            return self.changeDirectory(directory);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Ftp", "parentDirectory", "fun(self: sf.Ftp): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "parentDirectory",
        [](sf::Ftp& self) -> sf::Ftp::Response {
            return self.parentDirectory();
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Ftp", "createDirectory", "fun(self: sf.Ftp, name: string): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "createDirectory",
        [](sf::Ftp& self, std::string name) -> sf::Ftp::Response {
            return self.createDirectory(name);
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Ftp", "deleteDirectory", "fun(self: sf.Ftp, name: string): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "deleteDirectory",
        [](sf::Ftp& self, std::string name) -> sf::Ftp::Response {
            return self.deleteDirectory(name);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Ftp", "renameFile", "fun(self: sf.Ftp, file: string, newName: string): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "renameFile",
        [](sf::Ftp& self, std::string file, std::string newName) -> sf::Ftp::Response {
            return self.renameFile(std::filesystem::path(file), std::filesystem::path(newName));
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Ftp", "deleteFile", "fun(self: sf.Ftp, name: string): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "deleteFile",
        [](sf::Ftp& self, std::string name) -> sf::Ftp::Response {
            return self.deleteFile(std::filesystem::path(name));
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Ftp", "download", "fun(self: sf.Ftp, remoteFile: string, localPath: string, mode?: sf.Ftp.TransferMode): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "download",
        [](sf::Ftp& self, std::string remoteFile, std::string localPath, sf::Ftp::TransferMode mode) -> sf::Ftp::Response {
            return self.download(std::filesystem::path(remoteFile), std::filesystem::path(localPath), mode);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Ftp::TransferMode>(sf::Ftp::TransferMode::Binary);
        }}},
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Ftp", "upload", "fun(self: sf.Ftp, localFile: string, remotePath: string, mode?: sf.Ftp.TransferMode, append?: boolean): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "upload",
        [](sf::Ftp& self, std::string localFile, std::string remotePath, sf::Ftp::TransferMode mode, bool append) -> sf::Ftp::Response {
            return self.upload(std::filesystem::path(localFile), std::filesystem::path(remotePath), mode, append);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Ftp::TransferMode>(sf::Ftp::TransferMode::Binary);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Ftp", "sendCommand", "fun(self: sf.Ftp, command: string, parameter?: string): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp, "sendCommand",
        [](sf::Ftp& self, std::string command, std::string parameter) -> sf::Ftp::Response {
            return self.sendCommand(command, parameter);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::string(static_cast<std::string>(""));
        }}},
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_CLASS("sf.Ftp.TransferMode");
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FIELD("Binary", "sf.Ftp.TransferMode");
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FIELD("Ascii", "sf.Ftp.TransferMode");
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FIELD("Ebcdic", "sf.Ftp.TransferMode");
    lua_glue::BindEnum<sf::Ftp::TransferMode>(table_sf__Ftp, "TransferMode", {
        {"Binary", sf::Ftp::TransferMode::Binary},
        {"Ascii", sf::Ftp::TransferMode::Ascii},
        {"Ebcdic", sf::Ftp::TransferMode::Ebcdic}
    });
    auto type_sf__Ftp__Response = lua_glue::BindClass<sf::Ftp::Response>(table_sf__Ftp, "Response");
    lua_glue::Table table_sf__Ftp__Response = table_sf__Ftp["Response"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Ftp::Response>(lua);
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_CLASS("sf.Ftp.Response");
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.Ftp.Response", "new", "fun(code?: sf.Ftp.Response.Status, message?: string): sf.Ftp.Response");
    lua_glue::BindCallable(type_sf__Ftp__Response, "new",
        [](sf::Ftp::Response::Status code, std::string message) {
            return lua_sf::makeLuaSharedObject<sf::Ftp::Response>(code, message);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::Ftp::Response::Status>(sf::Ftp::Response::Status::InvalidResponse);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::string(static_cast<std::string>(""));
        }}},
        docs[23]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.Ftp.Response", "isOk", "fun(self: sf.Ftp.Response): boolean");
    lua_glue::BindCallable(type_sf__Ftp__Response, "isOk",
        [](const sf::Ftp::Response& self) -> bool {
            return self.isOk();
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.Ftp.Response", "getStatus", "fun(self: sf.Ftp.Response): sf.Ftp.Response.Status");
    lua_glue::BindCallable(type_sf__Ftp__Response, "getStatus",
        [](const sf::Ftp::Response& self) -> sf::Ftp::Response::Status {
            return self.getStatus();
        },
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.Ftp.Response", "getMessage", "fun(self: sf.Ftp.Response): string");
    lua_glue::BindCallable(type_sf__Ftp__Response, "getMessage",
        [](const sf::Ftp::Response& self) -> std::string {
            return std::string(self.getMessage());
        },
        docs[26],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_CLASS("sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FIELD("RestartMarkerReply", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_FIELD("ServiceReadySoon", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FIELD("DataConnectionAlreadyOpened", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FIELD("OpeningDataConnection", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FIELD("Ok", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FIELD("PointlessCommand", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FIELD("SystemStatus", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FIELD("DirectoryStatus", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FIELD("FileStatus", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[37]);
    LUASF_STUB_FIELD("HelpMessage", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FIELD("SystemType", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FIELD("ServiceReady", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[40]);
    LUASF_STUB_FIELD("ClosingConnection", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[41]);
    LUASF_STUB_FIELD("DataConnectionOpened", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[42]);
    LUASF_STUB_FIELD("ClosingDataConnection", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[43]);
    LUASF_STUB_FIELD("EnteringPassiveMode", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[44]);
    LUASF_STUB_FIELD("LoggedIn", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[45]);
    LUASF_STUB_FIELD("FileActionOk", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[46]);
    LUASF_STUB_FIELD("DirectoryOk", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[47]);
    LUASF_STUB_FIELD("NeedPassword", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[48]);
    LUASF_STUB_FIELD("NeedAccountToLogIn", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[49]);
    LUASF_STUB_FIELD("NeedInformation", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[50]);
    LUASF_STUB_FIELD("ServiceUnavailable", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[51]);
    LUASF_STUB_FIELD("DataConnectionUnavailable", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[52]);
    LUASF_STUB_FIELD("TransferAborted", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[53]);
    LUASF_STUB_FIELD("FileActionAborted", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[54]);
    LUASF_STUB_FIELD("LocalError", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[55]);
    LUASF_STUB_FIELD("InsufficientStorageSpace", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[56]);
    LUASF_STUB_FIELD("CommandUnknown", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[57]);
    LUASF_STUB_FIELD("ParametersUnknown", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FIELD("CommandNotImplemented", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[58]);
    LUASF_STUB_FIELD("BadCommandSequence", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[59]);
    LUASF_STUB_FIELD("ParameterNotImplemented", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[60]);
    LUASF_STUB_FIELD("NotLoggedIn", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[61]);
    LUASF_STUB_FIELD("NeedAccountToStore", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[62]);
    LUASF_STUB_FIELD("FileUnavailable", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[63]);
    LUASF_STUB_FIELD("PageTypeUnknown", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[64]);
    LUASF_STUB_FIELD("NotEnoughMemory", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[65]);
    LUASF_STUB_FIELD("FilenameNotAllowed", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[66]);
    LUASF_STUB_FIELD("InvalidResponse", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[67]);
    LUASF_STUB_FIELD("ConnectionFailed", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[68]);
    LUASF_STUB_FIELD("ConnectionClosed", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC(docs[69]);
    LUASF_STUB_FIELD("InvalidFile", "sf.Ftp.Response.Status");
    lua_glue::BindEnum<sf::Ftp::Response::Status>(table_sf__Ftp__Response, "Status", {
        {"RestartMarkerReply", sf::Ftp::Response::Status::RestartMarkerReply},
        {"ServiceReadySoon", sf::Ftp::Response::Status::ServiceReadySoon},
        {"DataConnectionAlreadyOpened", sf::Ftp::Response::Status::DataConnectionAlreadyOpened},
        {"OpeningDataConnection", sf::Ftp::Response::Status::OpeningDataConnection},
        {"Ok", sf::Ftp::Response::Status::Ok},
        {"PointlessCommand", sf::Ftp::Response::Status::PointlessCommand},
        {"SystemStatus", sf::Ftp::Response::Status::SystemStatus},
        {"DirectoryStatus", sf::Ftp::Response::Status::DirectoryStatus},
        {"FileStatus", sf::Ftp::Response::Status::FileStatus},
        {"HelpMessage", sf::Ftp::Response::Status::HelpMessage},
        {"SystemType", sf::Ftp::Response::Status::SystemType},
        {"ServiceReady", sf::Ftp::Response::Status::ServiceReady},
        {"ClosingConnection", sf::Ftp::Response::Status::ClosingConnection},
        {"DataConnectionOpened", sf::Ftp::Response::Status::DataConnectionOpened},
        {"ClosingDataConnection", sf::Ftp::Response::Status::ClosingDataConnection},
        {"EnteringPassiveMode", sf::Ftp::Response::Status::EnteringPassiveMode},
        {"LoggedIn", sf::Ftp::Response::Status::LoggedIn},
        {"FileActionOk", sf::Ftp::Response::Status::FileActionOk},
        {"DirectoryOk", sf::Ftp::Response::Status::DirectoryOk},
        {"NeedPassword", sf::Ftp::Response::Status::NeedPassword},
        {"NeedAccountToLogIn", sf::Ftp::Response::Status::NeedAccountToLogIn},
        {"NeedInformation", sf::Ftp::Response::Status::NeedInformation},
        {"ServiceUnavailable", sf::Ftp::Response::Status::ServiceUnavailable},
        {"DataConnectionUnavailable", sf::Ftp::Response::Status::DataConnectionUnavailable},
        {"TransferAborted", sf::Ftp::Response::Status::TransferAborted},
        {"FileActionAborted", sf::Ftp::Response::Status::FileActionAborted},
        {"LocalError", sf::Ftp::Response::Status::LocalError},
        {"InsufficientStorageSpace", sf::Ftp::Response::Status::InsufficientStorageSpace},
        {"CommandUnknown", sf::Ftp::Response::Status::CommandUnknown},
        {"ParametersUnknown", sf::Ftp::Response::Status::ParametersUnknown},
        {"CommandNotImplemented", sf::Ftp::Response::Status::CommandNotImplemented},
        {"BadCommandSequence", sf::Ftp::Response::Status::BadCommandSequence},
        {"ParameterNotImplemented", sf::Ftp::Response::Status::ParameterNotImplemented},
        {"NotLoggedIn", sf::Ftp::Response::Status::NotLoggedIn},
        {"NeedAccountToStore", sf::Ftp::Response::Status::NeedAccountToStore},
        {"FileUnavailable", sf::Ftp::Response::Status::FileUnavailable},
        {"PageTypeUnknown", sf::Ftp::Response::Status::PageTypeUnknown},
        {"NotEnoughMemory", sf::Ftp::Response::Status::NotEnoughMemory},
        {"FilenameNotAllowed", sf::Ftp::Response::Status::FilenameNotAllowed},
        {"InvalidResponse", sf::Ftp::Response::Status::InvalidResponse},
        {"ConnectionFailed", sf::Ftp::Response::Status::ConnectionFailed},
        {"ConnectionClosed", sf::Ftp::Response::Status::ConnectionClosed},
        {"InvalidFile", sf::Ftp::Response::Status::InvalidFile}
    });
    auto type_sf__Ftp__DirectoryResponse = lua_glue::BindClass<sf::Ftp::DirectoryResponse>(table_sf__Ftp, "DirectoryResponse");
    lua_glue::BindBase<sf::Ftp::DirectoryResponse, sf::Ftp::Response>(type_sf__Ftp__DirectoryResponse);
    lua_glue::Table table_sf__Ftp__DirectoryResponse = table_sf__Ftp["DirectoryResponse"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Ftp::DirectoryResponse>(lua);
    lua_glue::Table native_bases_sf__Ftp__DirectoryResponse = lua.create_table();
    native_bases_sf__Ftp__DirectoryResponse.add(lua["sf"]["Ftp"]["Response"].get<lua_glue::Table>());
    table_sf__Ftp__DirectoryResponse.raw_set("__nativeBases", native_bases_sf__Ftp__DirectoryResponse);
    LUASF_STUB_DOC(docs[70]);
    LUASF_STUB_CLASS("sf.Ftp.DirectoryResponse", "sf.Ftp.Response");
    LUASF_STUB_DOC(docs[71]);
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "new", "fun(response: sf.Ftp.Response): sf.Ftp.DirectoryResponse");
    lua_glue::BindCallable(type_sf__Ftp__DirectoryResponse, "new",
        [](const sf::Ftp::Response& response) {
            return lua_sf::makeLuaSharedObject<sf::Ftp::DirectoryResponse>(response);
        },
        docs[71]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "isOk", "fun(self: sf.Ftp.DirectoryResponse): boolean");
    lua_glue::BindCallable(type_sf__Ftp__DirectoryResponse, "isOk",
        [](const sf::Ftp::DirectoryResponse& self) -> bool {
            return static_cast<const sf::Ftp::Response&>(self).isOk();
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "getStatus", "fun(self: sf.Ftp.DirectoryResponse): sf.Ftp.Response.Status");
    lua_glue::BindCallable(type_sf__Ftp__DirectoryResponse, "getStatus",
        [](const sf::Ftp::DirectoryResponse& self) -> sf::Ftp::Response::Status {
            return static_cast<const sf::Ftp::Response&>(self).getStatus();
        },
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "getMessage", "fun(self: sf.Ftp.DirectoryResponse): string");
    lua_glue::BindCallable(type_sf__Ftp__DirectoryResponse, "getMessage",
        [](const sf::Ftp::DirectoryResponse& self) -> std::string {
            return std::string(static_cast<const sf::Ftp::Response&>(self).getMessage());
        },
        docs[26],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[72]);
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "getDirectory", "fun(self: sf.Ftp.DirectoryResponse): string");
    lua_glue::BindCallable(type_sf__Ftp__DirectoryResponse, "getDirectory",
        [](const sf::Ftp::DirectoryResponse& self) -> std::string {
            return (self.getDirectory()).string();
        },
        docs[72],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    auto type_sf__Ftp__ListingResponse = lua_glue::BindClass<sf::Ftp::ListingResponse>(table_sf__Ftp, "ListingResponse");
    lua_glue::BindBase<sf::Ftp::ListingResponse, sf::Ftp::Response>(type_sf__Ftp__ListingResponse);
    lua_glue::Table table_sf__Ftp__ListingResponse = table_sf__Ftp["ListingResponse"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Ftp::ListingResponse>(lua);
    lua_glue::Table native_bases_sf__Ftp__ListingResponse = lua.create_table();
    native_bases_sf__Ftp__ListingResponse.add(lua["sf"]["Ftp"]["Response"].get<lua_glue::Table>());
    table_sf__Ftp__ListingResponse.raw_set("__nativeBases", native_bases_sf__Ftp__ListingResponse);
    LUASF_STUB_DOC(docs[73]);
    LUASF_STUB_CLASS("sf.Ftp.ListingResponse", "sf.Ftp.Response");
    LUASF_STUB_DOC(docs[74]);
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "new", "fun(response: sf.Ftp.Response, data: string): sf.Ftp.ListingResponse");
    lua_glue::BindCallable(type_sf__Ftp__ListingResponse, "new",
        [](const sf::Ftp::Response& response, std::string data) {
            return lua_sf::makeLuaSharedObject<sf::Ftp::ListingResponse>(response, data);
        },
        docs[74]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "isOk", "fun(self: sf.Ftp.ListingResponse): boolean");
    lua_glue::BindCallable(type_sf__Ftp__ListingResponse, "isOk",
        [](const sf::Ftp::ListingResponse& self) -> bool {
            return static_cast<const sf::Ftp::Response&>(self).isOk();
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "getStatus", "fun(self: sf.Ftp.ListingResponse): sf.Ftp.Response.Status");
    lua_glue::BindCallable(type_sf__Ftp__ListingResponse, "getStatus",
        [](const sf::Ftp::ListingResponse& self) -> sf::Ftp::Response::Status {
            return static_cast<const sf::Ftp::Response&>(self).getStatus();
        },
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "getMessage", "fun(self: sf.Ftp.ListingResponse): string");
    lua_glue::BindCallable(type_sf__Ftp__ListingResponse, "getMessage",
        [](const sf::Ftp::ListingResponse& self) -> std::string {
            return std::string(static_cast<const sf::Ftp::Response&>(self).getMessage());
        },
        docs[26],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[75]);
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "getListing", "fun(self: sf.Ftp.ListingResponse): string[]");
    lua_glue::BindCallable(type_sf__Ftp__ListingResponse, "getListing",
        [lua](const sf::Ftp::ListingResponse& self) -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, self.getListing());
        },
        docs[75],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
}
