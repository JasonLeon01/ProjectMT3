#include "Network/bind_Ftp.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Ftp(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Ftp = sf.new_usertype<sf::Ftp>("Ftp", sol::no_constructor);
    sol::table table_sf__Ftp = sf["Ftp"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Ftp>(lua);
    LUASF_STUB_DOC("\\brief A FTP client\n\n\\deprecated Use `sf::Sftp` if possible.");
    LUASF_STUB_CLASS("sf.Ftp");
    LUASF_STUB_DOC("\\brief Default constructor");
    LUASF_STUB_FUNCTION("sf.Ftp", "new", "fun(): sf.Ftp");
    type_sf__Ftp.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Ftp>();
        }
    ));
    LUASF_STUB_DOC("\\brief Connect to the specified FTP server\n\nThe port has a default value of 21, which is the standard\nport used by the FTP protocol. You shouldn't use a different\nvalue, unless you really know what you do.\nThis function tries to connect to the server so it may take\na while to complete, especially if the server is not\nreachable. To avoid blocking your application for too long,\nyou can use a timeout. The default value, `Time::Zero`, means that the\nsystem timeout will be used (which is usually pretty long).\n\n\\param server  Name or address of the FTP server to connect to\n\\param port    Port used for the connection\n\\param timeout Maximum time to wait\n\n\\return Server response to the request\n\n\\see `disconnect`");
    LUASF_STUB_FUNCTION("sf.Ftp", "connect", "fun(self: sf.Ftp, server: sf.IpAddress, port: integer, timeout: sf.Time): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp", "connect", "fun(self: sf.Ftp, server: sf.IpAddress, port: integer): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp", "connect", "fun(self: sf.Ftp, server: sf.IpAddress): sf.Ftp.Response");
    type_sf__Ftp.set_function("connect",
        sol::overload(
            [](sf::Ftp& self, sf::IpAddress server, lua_sf::LuaIntegral<unsigned short> port, sf::Time timeout) -> sf::Ftp::Response {
                return self.connect(server, port.value(), timeout);
            },
            [](sf::Ftp& self, sf::IpAddress server, lua_sf::LuaIntegral<unsigned short> port) -> sf::Ftp::Response {
                return self.connect(server, port.value());
            },
            [](sf::Ftp& self, sf::IpAddress server) -> sf::Ftp::Response {
                return self.connect(server);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Close the connection with the server\n\n\\return Server response to the request\n\n\\see `connect`");
    LUASF_STUB_FUNCTION("sf.Ftp", "disconnect", "fun(self: sf.Ftp): sf.Ftp.Response");
    type_sf__Ftp.set_function("disconnect",
        [](sf::Ftp& self) -> sf::Ftp::Response {
            return self.disconnect();
        }
    );
    LUASF_STUB_DOC("\\brief Log in using a username and a password\n\nLogging in is mandatory after connecting to the server.\nUsers that are not logged in cannot perform any operation.\n\n\\param name     User name\n\\param password Password\n\n\\return Server response to the request");
    LUASF_STUB_FUNCTION("sf.Ftp", "login", "fun(self: sf.Ftp, name: string, password: string): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp", "login", "fun(self: sf.Ftp): sf.Ftp.Response");
    type_sf__Ftp.set_function("login",
        sol::overload(
            [](sf::Ftp& self, std::string name, std::string password) -> sf::Ftp::Response {
                return self.login(name, password);
            },
            [](sf::Ftp& self) -> sf::Ftp::Response {
                return self.login();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Send a null command to keep the connection alive\n\nThis command is useful because the server may close the\nconnection automatically if no command is sent.\n\n\\return Server response to the request");
    LUASF_STUB_FUNCTION("sf.Ftp", "keepAlive", "fun(self: sf.Ftp): sf.Ftp.Response");
    type_sf__Ftp.set_function("keepAlive",
        [](sf::Ftp& self) -> sf::Ftp::Response {
            return self.keepAlive();
        }
    );
    LUASF_STUB_DOC("\\brief Get the current working directory\n\nThe working directory is the root path for subsequent\noperations involving directories and/or filenames.\n\n\\return Server response to the request\n\n\\see `getDirectoryListing`, `changeDirectory`, `parentDirectory`");
    LUASF_STUB_FUNCTION("sf.Ftp", "getWorkingDirectory", "fun(self: sf.Ftp): sf.Ftp.DirectoryResponse");
    type_sf__Ftp.set_function("getWorkingDirectory",
        [](sf::Ftp& self) -> sf::Ftp::DirectoryResponse {
            return self.getWorkingDirectory();
        }
    );
    LUASF_STUB_DOC("\\brief Get the contents of the given directory\n\nThis function retrieves the sub-directories and files\ncontained in the given directory. It is not recursive.\nThe `directory` parameter is relative to the current\nworking directory.\n\n\\param directory Directory to list\n\n\\return Server response to the request\n\n\\see `getWorkingDirectory`, `changeDirectory`, `parentDirectory`");
    LUASF_STUB_FUNCTION("sf.Ftp", "getDirectoryListing", "fun(self: sf.Ftp, directory: string): sf.Ftp.ListingResponse");
    LUASF_STUB_OVERLOAD("sf.Ftp", "getDirectoryListing", "fun(self: sf.Ftp): sf.Ftp.ListingResponse");
    type_sf__Ftp.set_function("getDirectoryListing",
        sol::overload(
            [](sf::Ftp& self, std::string directory) -> sf::Ftp::ListingResponse {
                return self.getDirectoryListing(directory);
            },
            [](sf::Ftp& self) -> sf::Ftp::ListingResponse {
                return self.getDirectoryListing();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Change the current working directory\n\nThe new directory must be relative to the current one.\n\n\\param directory New working directory\n\n\\return Server response to the request\n\n\\see `getWorkingDirectory`, `getDirectoryListing`, `parentDirectory`");
    LUASF_STUB_FUNCTION("sf.Ftp", "changeDirectory", "fun(self: sf.Ftp, directory: string): sf.Ftp.Response");
    type_sf__Ftp.set_function("changeDirectory",
        [](sf::Ftp& self, std::string directory) -> sf::Ftp::Response {
            return self.changeDirectory(directory);
        }
    );
    LUASF_STUB_DOC("\\brief Go to the parent directory of the current one\n\n\\return Server response to the request\n\n\\see `getWorkingDirectory`, `getDirectoryListing`, `changeDirectory`");
    LUASF_STUB_FUNCTION("sf.Ftp", "parentDirectory", "fun(self: sf.Ftp): sf.Ftp.Response");
    type_sf__Ftp.set_function("parentDirectory",
        [](sf::Ftp& self) -> sf::Ftp::Response {
            return self.parentDirectory();
        }
    );
    LUASF_STUB_DOC("\\brief Create a new directory\n\nThe new directory is created as a child of the current\nworking directory.\n\n\\param name Name of the directory to create\n\n\\return Server response to the request\n\n\\see `deleteDirectory`");
    LUASF_STUB_FUNCTION("sf.Ftp", "createDirectory", "fun(self: sf.Ftp, name: string): sf.Ftp.Response");
    type_sf__Ftp.set_function("createDirectory",
        [](sf::Ftp& self, std::string name) -> sf::Ftp::Response {
            return self.createDirectory(name);
        }
    );
    LUASF_STUB_DOC("\\brief Remove an existing directory\n\nThe directory to remove must be relative to the\ncurrent working directory.\nUse this function with caution, the directory will\nbe removed permanently!\n\n\\param name Name of the directory to remove\n\n\\return Server response to the request\n\n\\see `createDirectory`");
    LUASF_STUB_FUNCTION("sf.Ftp", "deleteDirectory", "fun(self: sf.Ftp, name: string): sf.Ftp.Response");
    type_sf__Ftp.set_function("deleteDirectory",
        [](sf::Ftp& self, std::string name) -> sf::Ftp::Response {
            return self.deleteDirectory(name);
        }
    );
    LUASF_STUB_DOC("\\brief Rename an existing file\n\nThe file names must be relative to the current working\ndirectory.\n\n\\param file    File to rename\n\\param newName New name of the file\n\n\\return Server response to the request\n\n\\see `deleteFile`");
    LUASF_STUB_FUNCTION("sf.Ftp", "renameFile", "fun(self: sf.Ftp, file: string, newName: string): sf.Ftp.Response");
    type_sf__Ftp.set_function("renameFile",
        [](sf::Ftp& self, std::string file, std::string newName) -> sf::Ftp::Response {
            return self.renameFile(std::filesystem::path(file), std::filesystem::path(newName));
        }
    );
    LUASF_STUB_DOC("\\brief Remove an existing file\n\nThe file name must be relative to the current working\ndirectory.\nUse this function with caution, the file will be\nremoved permanently!\n\n\\param name File to remove\n\n\\return Server response to the request\n\n\\see `renameFile`");
    LUASF_STUB_FUNCTION("sf.Ftp", "deleteFile", "fun(self: sf.Ftp, name: string): sf.Ftp.Response");
    type_sf__Ftp.set_function("deleteFile",
        [](sf::Ftp& self, std::string name) -> sf::Ftp::Response {
            return self.deleteFile(std::filesystem::path(name));
        }
    );
    LUASF_STUB_DOC("\\brief Download a file from the server\n\nThe file name of the distant file is relative to the\ncurrent working directory of the server, and the local\ndestination path is relative to the current directory\nof your application.\nIf a file with the same file name as the distant file\nalready exists in the local destination path, it will\nbe overwritten.\n\n\\param remoteFile File name of the distant file to download\n\\param localPath  The directory in which to put the file on the local computer\n\\param mode       Transfer mode\n\n\\return Server response to the request\n\n\\see `upload`");
    LUASF_STUB_FUNCTION("sf.Ftp", "download", "fun(self: sf.Ftp, remoteFile: string, localPath: string, mode: sf.Ftp.TransferMode): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp", "download", "fun(self: sf.Ftp, remoteFile: string, localPath: string): sf.Ftp.Response");
    type_sf__Ftp.set_function("download",
        sol::overload(
            [](sf::Ftp& self, std::string remoteFile, std::string localPath, sf::Ftp::TransferMode mode) -> sf::Ftp::Response {
                return self.download(std::filesystem::path(remoteFile), std::filesystem::path(localPath), mode);
            },
            [](sf::Ftp& self, std::string remoteFile, std::string localPath) -> sf::Ftp::Response {
                return self.download(std::filesystem::path(remoteFile), std::filesystem::path(localPath));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Upload a file to the server\n\nThe name of the local file is relative to the current\nworking directory of your application, and the\nremote path is relative to the current directory of the\nFTP server.\n\nThe append parameter controls whether the remote file is\nappended to or overwritten if it already exists.\n\n\\param localFile  Path of the local file to upload\n\\param remotePath The directory in which to put the file on the server\n\\param mode       Transfer mode\n\\param append     Pass `true` to append to or `false` to overwrite the remote file if it already exists\n\n\\return Server response to the request\n\n\\see `download`");
    LUASF_STUB_FUNCTION("sf.Ftp", "upload", "fun(self: sf.Ftp, localFile: string, remotePath: string, mode: sf.Ftp.TransferMode, append: boolean): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp", "upload", "fun(self: sf.Ftp, localFile: string, remotePath: string, mode: sf.Ftp.TransferMode): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp", "upload", "fun(self: sf.Ftp, localFile: string, remotePath: string): sf.Ftp.Response");
    type_sf__Ftp.set_function("upload",
        sol::overload(
            [](sf::Ftp& self, std::string localFile, std::string remotePath, sf::Ftp::TransferMode mode, bool append) -> sf::Ftp::Response {
                return self.upload(std::filesystem::path(localFile), std::filesystem::path(remotePath), mode, append);
            },
            [](sf::Ftp& self, std::string localFile, std::string remotePath, sf::Ftp::TransferMode mode) -> sf::Ftp::Response {
                return self.upload(std::filesystem::path(localFile), std::filesystem::path(remotePath), mode);
            },
            [](sf::Ftp& self, std::string localFile, std::string remotePath) -> sf::Ftp::Response {
                return self.upload(std::filesystem::path(localFile), std::filesystem::path(remotePath));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Send a command to the FTP server\n\nWhile the most often used commands are provided as member\nfunctions in the `sf::Ftp` class, this method can be used\nto send any FTP command to the server. If the command\nrequires one or more parameters, they can be specified\nin `parameter`. If the server returns information, you\ncan extract it from the response using `Response::getMessage()`.\n\n\\param command   Command to send\n\\param parameter Command parameter\n\n\\return Server response to the request");
    LUASF_STUB_FUNCTION("sf.Ftp", "sendCommand", "fun(self: sf.Ftp, command: string, parameter: string): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp", "sendCommand", "fun(self: sf.Ftp, command: string): sf.Ftp.Response");
    type_sf__Ftp.set_function("sendCommand",
        sol::overload(
            [](sf::Ftp& self, std::string command, std::string parameter) -> sf::Ftp::Response {
                return self.sendCommand(command, parameter);
            },
            [](sf::Ftp& self, std::string command) -> sf::Ftp::Response {
                return self.sendCommand(command);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Enumeration of transfer modes");
    LUASF_STUB_CLASS("sf.Ftp.TransferMode");
    LUASF_STUB_DOC("Binary mode (file is transferred as a sequence of bytes)");
    LUASF_STUB_FIELD("Binary", "sf.Ftp.TransferMode");
    LUASF_STUB_DOC("Text mode using ASCII encoding");
    LUASF_STUB_FIELD("Ascii", "sf.Ftp.TransferMode");
    LUASF_STUB_DOC("Text mode using EBCDIC encoding");
    LUASF_STUB_FIELD("Ebcdic", "sf.Ftp.TransferMode");
    table_sf__Ftp.new_enum("TransferMode",
        "Binary", sf::Ftp::TransferMode::Binary,
        "Ascii", sf::Ftp::TransferMode::Ascii,
        "Ebcdic", sf::Ftp::TransferMode::Ebcdic
    );
    auto type_sf__Ftp__Response = table_sf__Ftp.new_usertype<sf::Ftp::Response>("Response", sol::no_constructor);
    sol::table table_sf__Ftp__Response = table_sf__Ftp["Response"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Ftp::Response>(lua);
    LUASF_STUB_DOC("\\brief FTP response");
    LUASF_STUB_CLASS("sf.Ftp.Response");
    LUASF_STUB_DOC("\\brief Default constructor\n\nThis constructor is used by the FTP client to build\nthe response.\n\n\\param code    Response status code\n\\param message Response message");
    LUASF_STUB_FUNCTION("sf.Ftp.Response", "new", "fun(code: sf.Ftp.Response.Status, message: string): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp.Response", "new", "fun(code: sf.Ftp.Response.Status): sf.Ftp.Response");
    LUASF_STUB_OVERLOAD("sf.Ftp.Response", "new", "fun(): sf.Ftp.Response");
    type_sf__Ftp__Response.set_function("new", sol::factories(
        [](sf::Ftp::Response::Status code, std::string message) {
            return lua_sf::makeLuaSharedObject<sf::Ftp::Response>(code, message);
        },
        [](sf::Ftp::Response::Status code) {
            return lua_sf::makeLuaSharedObject<sf::Ftp::Response>(code);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::Ftp::Response>();
        }
    ));
    LUASF_STUB_DOC("\\brief Check if the status code means a success\n\nThis function is defined for convenience, it is\nequivalent to testing if the status code is < 400.\n\n\\return `true` if the status is a success, `false` if it is a failure");
    LUASF_STUB_FUNCTION("sf.Ftp.Response", "isOk", "fun(self: sf.Ftp.Response): boolean");
    type_sf__Ftp__Response.set_function("isOk",
        [](sf::Ftp::Response& self) -> bool {
            return self.isOk();
        }
    );
    LUASF_STUB_DOC("\\brief Get the status code of the response\n\n\\return Status code");
    LUASF_STUB_FUNCTION("sf.Ftp.Response", "getStatus", "fun(self: sf.Ftp.Response): sf.Ftp.Response.Status");
    type_sf__Ftp__Response.set_function("getStatus",
        [](sf::Ftp::Response& self) -> sf::Ftp::Response::Status {
            return self.getStatus();
        }
    );
    LUASF_STUB_DOC("\\brief Get the full message contained in the response\n\n\\return The response message");
    LUASF_STUB_FUNCTION("sf.Ftp.Response", "getMessage", "fun(self: sf.Ftp.Response): string");
    type_sf__Ftp__Response.set_function("getMessage",
        sol::policies(
            [](sf::Ftp::Response& self) -> std::string {
                return std::string(self.getMessage());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Status codes possibly returned by a FTP response");
    LUASF_STUB_CLASS("sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Restart marker reply");
    LUASF_STUB_FIELD("RestartMarkerReply", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Service ready in N minutes");
    LUASF_STUB_FIELD("ServiceReadySoon", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Data connection already opened, transfer starting");
    LUASF_STUB_FIELD("DataConnectionAlreadyOpened", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("File status ok, about to open data connection");
    LUASF_STUB_FIELD("OpeningDataConnection", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Command ok");
    LUASF_STUB_FIELD("Ok", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Command not implemented");
    LUASF_STUB_FIELD("PointlessCommand", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("System status, or system help reply");
    LUASF_STUB_FIELD("SystemStatus", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Directory status");
    LUASF_STUB_FIELD("DirectoryStatus", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("File status");
    LUASF_STUB_FIELD("FileStatus", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Help message");
    LUASF_STUB_FIELD("HelpMessage", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("NAME system type, where NAME is an official system name from the list in the Assigned Numbers document");
    LUASF_STUB_FIELD("SystemType", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Service ready for new user");
    LUASF_STUB_FIELD("ServiceReady", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Service closing control connection");
    LUASF_STUB_FIELD("ClosingConnection", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Data connection open, no transfer in progress");
    LUASF_STUB_FIELD("DataConnectionOpened", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Closing data connection, requested file action successful");
    LUASF_STUB_FIELD("ClosingDataConnection", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Entering passive mode");
    LUASF_STUB_FIELD("EnteringPassiveMode", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("User logged in, proceed. Logged out if appropriate");
    LUASF_STUB_FIELD("LoggedIn", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Requested file action ok");
    LUASF_STUB_FIELD("FileActionOk", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("PATHNAME created");
    LUASF_STUB_FIELD("DirectoryOk", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("User name ok, need password");
    LUASF_STUB_FIELD("NeedPassword", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Need account for login");
    LUASF_STUB_FIELD("NeedAccountToLogIn", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Requested file action pending further information");
    LUASF_STUB_FIELD("NeedInformation", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Service not available, closing control connection");
    LUASF_STUB_FIELD("ServiceUnavailable", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Can't open data connection");
    LUASF_STUB_FIELD("DataConnectionUnavailable", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Connection closed, transfer aborted");
    LUASF_STUB_FIELD("TransferAborted", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Requested file action not taken");
    LUASF_STUB_FIELD("FileActionAborted", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Requested action aborted, local error in processing");
    LUASF_STUB_FIELD("LocalError", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Requested action not taken; insufficient storage space in system, file unavailable");
    LUASF_STUB_FIELD("InsufficientStorageSpace", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Syntax error, command unrecognized");
    LUASF_STUB_FIELD("CommandUnknown", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Syntax error in parameters or arguments");
    LUASF_STUB_FIELD("ParametersUnknown", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Command not implemented");
    LUASF_STUB_FIELD("CommandNotImplemented", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Bad sequence of commands");
    LUASF_STUB_FIELD("BadCommandSequence", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Command not implemented for that parameter");
    LUASF_STUB_FIELD("ParameterNotImplemented", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Not logged in");
    LUASF_STUB_FIELD("NotLoggedIn", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Need account for storing files");
    LUASF_STUB_FIELD("NeedAccountToStore", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Requested action not taken, file unavailable");
    LUASF_STUB_FIELD("FileUnavailable", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Requested action aborted, page type unknown");
    LUASF_STUB_FIELD("PageTypeUnknown", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Requested file action aborted, exceeded storage allocation");
    LUASF_STUB_FIELD("NotEnoughMemory", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Requested action not taken, file name not allowed");
    LUASF_STUB_FIELD("FilenameNotAllowed", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Not part of the FTP standard, generated by SFML when a received response cannot be parsed");
    LUASF_STUB_FIELD("InvalidResponse", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Not part of the FTP standard, generated by SFML when the low-level socket connection with the server fails");
    LUASF_STUB_FIELD("ConnectionFailed", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Not part of the FTP standard, generated by SFML when the low-level socket connection is unexpectedly closed");
    LUASF_STUB_FIELD("ConnectionClosed", "sf.Ftp.Response.Status");
    LUASF_STUB_DOC("Not part of the FTP standard, generated by SFML when a local file cannot be read or written");
    LUASF_STUB_FIELD("InvalidFile", "sf.Ftp.Response.Status");
    table_sf__Ftp__Response.new_enum("Status",
        "RestartMarkerReply", sf::Ftp::Response::Status::RestartMarkerReply,
        "ServiceReadySoon", sf::Ftp::Response::Status::ServiceReadySoon,
        "DataConnectionAlreadyOpened", sf::Ftp::Response::Status::DataConnectionAlreadyOpened,
        "OpeningDataConnection", sf::Ftp::Response::Status::OpeningDataConnection,
        "Ok", sf::Ftp::Response::Status::Ok,
        "PointlessCommand", sf::Ftp::Response::Status::PointlessCommand,
        "SystemStatus", sf::Ftp::Response::Status::SystemStatus,
        "DirectoryStatus", sf::Ftp::Response::Status::DirectoryStatus,
        "FileStatus", sf::Ftp::Response::Status::FileStatus,
        "HelpMessage", sf::Ftp::Response::Status::HelpMessage,
        "SystemType", sf::Ftp::Response::Status::SystemType,
        "ServiceReady", sf::Ftp::Response::Status::ServiceReady,
        "ClosingConnection", sf::Ftp::Response::Status::ClosingConnection,
        "DataConnectionOpened", sf::Ftp::Response::Status::DataConnectionOpened,
        "ClosingDataConnection", sf::Ftp::Response::Status::ClosingDataConnection,
        "EnteringPassiveMode", sf::Ftp::Response::Status::EnteringPassiveMode,
        "LoggedIn", sf::Ftp::Response::Status::LoggedIn,
        "FileActionOk", sf::Ftp::Response::Status::FileActionOk,
        "DirectoryOk", sf::Ftp::Response::Status::DirectoryOk,
        "NeedPassword", sf::Ftp::Response::Status::NeedPassword,
        "NeedAccountToLogIn", sf::Ftp::Response::Status::NeedAccountToLogIn,
        "NeedInformation", sf::Ftp::Response::Status::NeedInformation,
        "ServiceUnavailable", sf::Ftp::Response::Status::ServiceUnavailable,
        "DataConnectionUnavailable", sf::Ftp::Response::Status::DataConnectionUnavailable,
        "TransferAborted", sf::Ftp::Response::Status::TransferAborted,
        "FileActionAborted", sf::Ftp::Response::Status::FileActionAborted,
        "LocalError", sf::Ftp::Response::Status::LocalError,
        "InsufficientStorageSpace", sf::Ftp::Response::Status::InsufficientStorageSpace,
        "CommandUnknown", sf::Ftp::Response::Status::CommandUnknown,
        "ParametersUnknown", sf::Ftp::Response::Status::ParametersUnknown,
        "CommandNotImplemented", sf::Ftp::Response::Status::CommandNotImplemented,
        "BadCommandSequence", sf::Ftp::Response::Status::BadCommandSequence,
        "ParameterNotImplemented", sf::Ftp::Response::Status::ParameterNotImplemented,
        "NotLoggedIn", sf::Ftp::Response::Status::NotLoggedIn,
        "NeedAccountToStore", sf::Ftp::Response::Status::NeedAccountToStore,
        "FileUnavailable", sf::Ftp::Response::Status::FileUnavailable,
        "PageTypeUnknown", sf::Ftp::Response::Status::PageTypeUnknown,
        "NotEnoughMemory", sf::Ftp::Response::Status::NotEnoughMemory,
        "FilenameNotAllowed", sf::Ftp::Response::Status::FilenameNotAllowed,
        "InvalidResponse", sf::Ftp::Response::Status::InvalidResponse,
        "ConnectionFailed", sf::Ftp::Response::Status::ConnectionFailed,
        "ConnectionClosed", sf::Ftp::Response::Status::ConnectionClosed,
        "InvalidFile", sf::Ftp::Response::Status::InvalidFile
    );
    auto type_sf__Ftp__DirectoryResponse = table_sf__Ftp.new_usertype<sf::Ftp::DirectoryResponse>("DirectoryResponse",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Ftp::Response>()
    );
    sol::table table_sf__Ftp__DirectoryResponse = table_sf__Ftp["DirectoryResponse"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Ftp::DirectoryResponse>(lua);
    sol::table native_bases_sf__Ftp__DirectoryResponse = lua.create_table();
    native_bases_sf__Ftp__DirectoryResponse.add(lua["sf"]["Ftp"]["Response"].get<sol::table>());
    table_sf__Ftp__DirectoryResponse.raw_set("__nativeBases", native_bases_sf__Ftp__DirectoryResponse);
    LUASF_STUB_DOC("\\brief Specialization of FTP response returning a directory");
    LUASF_STUB_CLASS("sf.Ftp.DirectoryResponse", "sf.Ftp.Response");
    LUASF_STUB_DOC("\\brief Default constructor\n\n\\param response Source response");
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "new", "fun(response: sf.Ftp.Response): sf.Ftp.DirectoryResponse");
    type_sf__Ftp__DirectoryResponse.set_function("new", sol::factories(
        [](const sf::Ftp::Response& response) {
            return lua_sf::makeLuaSharedObject<sf::Ftp::DirectoryResponse>(response);
        }
    ));
    LUASF_STUB_DOC("\\brief Check if the status code means a success\n\nThis function is defined for convenience, it is\nequivalent to testing if the status code is < 400.\n\n\\return `true` if the status is a success, `false` if it is a failure");
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "isOk", "fun(self: sf.Ftp.DirectoryResponse): boolean");
    type_sf__Ftp__DirectoryResponse.set_function("isOk",
        [](sf::Ftp::DirectoryResponse& self) -> bool {
            return static_cast<sf::Ftp::Response&>(self).isOk();
        }
    );
    LUASF_STUB_DOC("\\brief Get the status code of the response\n\n\\return Status code");
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "getStatus", "fun(self: sf.Ftp.DirectoryResponse): sf.Ftp.Response.Status");
    type_sf__Ftp__DirectoryResponse.set_function("getStatus",
        [](sf::Ftp::DirectoryResponse& self) -> sf::Ftp::Response::Status {
            return static_cast<sf::Ftp::Response&>(self).getStatus();
        }
    );
    LUASF_STUB_DOC("\\brief Get the full message contained in the response\n\n\\return The response message");
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "getMessage", "fun(self: sf.Ftp.DirectoryResponse): string");
    type_sf__Ftp__DirectoryResponse.set_function("getMessage",
        sol::policies(
            [](sf::Ftp::DirectoryResponse& self) -> std::string {
                return std::string(static_cast<sf::Ftp::Response&>(self).getMessage());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the directory returned in the response\n\n\\return Directory name");
    LUASF_STUB_FUNCTION("sf.Ftp.DirectoryResponse", "getDirectory", "fun(self: sf.Ftp.DirectoryResponse): string");
    type_sf__Ftp__DirectoryResponse.set_function("getDirectory",
        sol::policies(
            [](sf::Ftp::DirectoryResponse& self) -> std::string {
                return (self.getDirectory()).string();
            },
            sol::self_dependency{}
        )
    );
    auto type_sf__Ftp__ListingResponse = table_sf__Ftp.new_usertype<sf::Ftp::ListingResponse>("ListingResponse",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Ftp::Response>()
    );
    sol::table table_sf__Ftp__ListingResponse = table_sf__Ftp["ListingResponse"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Ftp::ListingResponse>(lua);
    sol::table native_bases_sf__Ftp__ListingResponse = lua.create_table();
    native_bases_sf__Ftp__ListingResponse.add(lua["sf"]["Ftp"]["Response"].get<sol::table>());
    table_sf__Ftp__ListingResponse.raw_set("__nativeBases", native_bases_sf__Ftp__ListingResponse);
    LUASF_STUB_DOC("\\brief Specialization of FTP response returning a\nfile name listing");
    LUASF_STUB_CLASS("sf.Ftp.ListingResponse", "sf.Ftp.Response");
    LUASF_STUB_DOC("\\brief Default constructor\n\n\\param response  Source response\n\\param data      Data containing the raw listing");
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "new", "fun(response: sf.Ftp.Response, data: string): sf.Ftp.ListingResponse");
    type_sf__Ftp__ListingResponse.set_function("new", sol::factories(
        [](const sf::Ftp::Response& response, std::string data) {
            return lua_sf::makeLuaSharedObject<sf::Ftp::ListingResponse>(response, data);
        }
    ));
    LUASF_STUB_DOC("\\brief Check if the status code means a success\n\nThis function is defined for convenience, it is\nequivalent to testing if the status code is < 400.\n\n\\return `true` if the status is a success, `false` if it is a failure");
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "isOk", "fun(self: sf.Ftp.ListingResponse): boolean");
    type_sf__Ftp__ListingResponse.set_function("isOk",
        [](sf::Ftp::ListingResponse& self) -> bool {
            return static_cast<sf::Ftp::Response&>(self).isOk();
        }
    );
    LUASF_STUB_DOC("\\brief Get the status code of the response\n\n\\return Status code");
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "getStatus", "fun(self: sf.Ftp.ListingResponse): sf.Ftp.Response.Status");
    type_sf__Ftp__ListingResponse.set_function("getStatus",
        [](sf::Ftp::ListingResponse& self) -> sf::Ftp::Response::Status {
            return static_cast<sf::Ftp::Response&>(self).getStatus();
        }
    );
    LUASF_STUB_DOC("\\brief Get the full message contained in the response\n\n\\return The response message");
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "getMessage", "fun(self: sf.Ftp.ListingResponse): string");
    type_sf__Ftp__ListingResponse.set_function("getMessage",
        sol::policies(
            [](sf::Ftp::ListingResponse& self) -> std::string {
                return std::string(static_cast<sf::Ftp::Response&>(self).getMessage());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Return the array of directory/file names\n\n\\return Array containing the requested listing");
    LUASF_STUB_FUNCTION("sf.Ftp.ListingResponse", "getListing", "fun(self: sf.Ftp.ListingResponse): string[]");
    type_sf__Ftp__ListingResponse.set_function("getListing",
        sol::policies(
            [lua](sf::Ftp::ListingResponse& self) -> sol::object {
                return lua_sf::vector_to_object(lua, self.getListing());
            },
            sol::self_dependency{}
        )
    );
}
