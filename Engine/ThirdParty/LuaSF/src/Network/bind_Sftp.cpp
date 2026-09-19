#include "Network/bind_Sftp.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Sftp(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Sftp = sf.new_usertype<sf::Sftp>("Sftp", sol::no_constructor);
    sol::table table_sf__Sftp = sf["Sftp"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sftp>(lua);
    LUASF_STUB_DOC("\\brief An SSH File Transfer Protocol (SFTP) client");
    LUASF_STUB_CLASS("sf.Sftp");
    LUASF_STUB_DOC("\\brief Default constructor");
    LUASF_STUB_FUNCTION("sf.Sftp", "new", "fun(): sf.Sftp");
    type_sf__Sftp.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Sftp>();
        }
    ));
    LUASF_STUB_DOC("\\brief Connect to the specified SFTP server\n\nThe port has a default value of 22, which is the standard\nport used by the SFTP protocol.\nThis function tries to connect to the server so it may take\na while to complete, especially if the server is not\nreachable. To avoid blocking your application for too long,\nyou can use a timeout. The default value, `Time::Zero`, means that the\nsystem timeout will be used (which is usually pretty long).\n\n\\param server  Name or address of the SFTP server to connect to\n\\param port    Port used for the connection\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of the connection attempt\n\n\\see `disconnect`");
    LUASF_STUB_FUNCTION("sf.Sftp", "connect", "fun(self: sf.Sftp, server: sf.IpAddress, port: integer, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "connect", "fun(self: sf.Sftp, server: sf.IpAddress, port: integer): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "connect", "fun(self: sf.Sftp, server: sf.IpAddress): sf.Sftp.Result");
    type_sf__Sftp.set_function("connect",
        sol::overload(
            [](sf::Sftp& self, sf::IpAddress server, lua_sf::LuaIntegral<unsigned short> port, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.connect(server, port.value(), timeout);
            },
            [](sf::Sftp& self, sf::IpAddress server, lua_sf::LuaIntegral<unsigned short> port) -> sf::Sftp::Result {
                return self.connect(server, port.value());
            },
            [](sf::Sftp& self, sf::IpAddress server) -> sf::Sftp::Result {
                return self.connect(server);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Disconnect the connection with the server\n\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of disconnecting the connection with the server\n\n\\see `connect`");
    LUASF_STUB_FUNCTION("sf.Sftp", "disconnect", "fun(self: sf.Sftp, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "disconnect", "fun(self: sf.Sftp): sf.Sftp.Result");
    type_sf__Sftp.set_function("disconnect",
        sol::overload(
            [](sf::Sftp& self, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.disconnect(timeout);
            },
            [](sf::Sftp& self) -> sf::Sftp::Result {
                return self.disconnect();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get SSH session information\n\nAfter connecting to the server and before actually\nlogging in the SSH session information of the underlying\nconnection will be available.\n\nThe session information contains among other things the\npublic key identifying the remote host and the connection\nparameters such as encryption and compression used. The\nidentifiers used follow the RFC 4253 specification.\n\nIf the session information is not available `std::nullopt`\nwill be returned.\n\nBecause SSH was developed as a parallel standard to\nSSL/TLS and automatic host certificate verification wasn't\nwidespread at the time, relying on the user to check the\nauthenticity of the host key was the typical method used\nto verify that they were connecting to the legitimate host,\nassuming the private key of the remote host was not\ncompromised.\n\nIf connection security is a high priority, examining\nthe parameters and aborting the connection if any weak\nalgorithms are used is also possible.\n\n\\return SSH session information or `std::nullopt` if it is not available");
    LUASF_STUB_FUNCTION("sf.Sftp", "getSessionInfo", "fun(self: sf.Sftp): sf.Sftp.SessionInfo|nil");
    type_sf__Sftp.set_function("getSessionInfo",
        [lua](sf::Sftp& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.getSessionInfo());
        }
    );
    LUASF_STUB_DOC("\\brief Log in using a public/private key pair\n\nLogging in is mandatory after connecting to the server.\nUsers that are not logged in cannot perform any operation.\n\nThis overload allows logging into the SFTP server using\npublic key authentication.\n\nThe public and private key data should be provided in PEM\nformat. PEM encoded data can be easily recognized by their\n`-----BEGIN ............-----` header and\n`-----END ............-----` footer.\n\nEven though it is technically possible to derive the\npublic key from the private key, due to backend\nlimitations, providing a pre-generated public key as well\nis necessary for this function to be able to succeed.\n\nIf the private key is protected by a passphrase the\npassphrase can be provided as a NULL terminated string.\nIf the private key is not protected by a passphrase the\npassphrase should be set to the empty string.\n\n\\param name                 User name\n\\param publicKeyData        Public key data\n\\param publicKeyLength      Public key data length\n\\param privateKeyData       Private key data\n\\param privateKeyLength     Private key data length\n\\param privateKeyPassphrase Private key passphrase, NULL terminated\n\\param timeout              Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of attempting to log in to the server");
    LUASF_STUB_FUNCTION("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, publicKeyData: string, publicKeyLength: integer, privateKeyData: string, privateKeyLength: integer, privateKeyPassphrase: string, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, publicKeyData: string, publicKeyLength: integer, privateKeyData: string, privateKeyLength: integer, privateKeyPassphrase: string): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, publicKeyData: string, publicKeyLength: integer, privateKeyData: string, privateKeyLength: integer): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, publicKeyData: string, privateKeyData: string, privateKeyPassphrase: string, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, publicKeyData: string, privateKeyData: string, privateKeyPassphrase: string): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, password: string, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, publicKeyData: string, privateKeyData: string): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, password: string): sf.Sftp.Result");
    type_sf__Sftp.set_function("login",
        sol::overload(
            [](sf::Sftp& self, std::string name, std::string publicKeyData, lua_sf::LuaIntegral<std::size_t> publicKeyLength, std::string privateKeyData, lua_sf::LuaIntegral<std::size_t> privateKeyLength, std::string privateKeyPassphrase, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.login(name, publicKeyData.c_str(), publicKeyLength.value(), privateKeyData.c_str(), privateKeyLength.value(), privateKeyPassphrase.c_str(), timeout);
            },
            [](sf::Sftp& self, std::string name, std::string publicKeyData, lua_sf::LuaIntegral<std::size_t> publicKeyLength, std::string privateKeyData, lua_sf::LuaIntegral<std::size_t> privateKeyLength, std::string privateKeyPassphrase) -> sf::Sftp::Result {
                return self.login(name, publicKeyData.c_str(), publicKeyLength.value(), privateKeyData.c_str(), privateKeyLength.value(), privateKeyPassphrase.c_str());
            },
            [](sf::Sftp& self, std::string name, std::string publicKeyData, lua_sf::LuaIntegral<std::size_t> publicKeyLength, std::string privateKeyData, lua_sf::LuaIntegral<std::size_t> privateKeyLength) -> sf::Sftp::Result {
                return self.login(name, publicKeyData.c_str(), publicKeyLength.value(), privateKeyData.c_str(), privateKeyLength.value());
            },
            [](sf::Sftp& self, std::string name, std::string publicKeyData, std::string privateKeyData, std::string privateKeyPassphrase, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.login(name, publicKeyData, privateKeyData, privateKeyPassphrase, timeout);
            },
            [](sf::Sftp& self, std::string name, std::string publicKeyData, std::string privateKeyData, std::string privateKeyPassphrase) -> sf::Sftp::Result {
                return self.login(name, publicKeyData, privateKeyData, privateKeyPassphrase);
            },
            [](sf::Sftp& self, std::string name, std::string password, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.login(name, password, timeout);
            },
            [](sf::Sftp& self, std::string name, std::string publicKeyData, std::string privateKeyData) -> sf::Sftp::Result {
                return self.login(name, publicKeyData, privateKeyData);
            },
            [](sf::Sftp& self, std::string name, std::string password) -> sf::Sftp::Result {
                return self.login(name, password);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Resolve a remote path into an absolute remote path\n\nPaths can contain links and other reserved path identifiers\nsuch as . and .. referring to the current directory and\nparent directory respectively.\n\nWhen determining the absolute path, which does not contain\nlinks or . or .. is necessary, this function can be used.\n\nResolving \".\" will return the absolute path to the current\nworking directory of the user after logging in to the SFTP\nserver.\n\n\\param path    Path to convert into an absolute path\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of converting the path into an absolute path\n\n\\see `getWorkingDirectory`");
    LUASF_STUB_FUNCTION("sf.Sftp", "resolvePath", "fun(self: sf.Sftp, path: string, timeout: sf.TimeoutWithPredicate): sf.Sftp.PathResult");
    LUASF_STUB_OVERLOAD("sf.Sftp", "resolvePath", "fun(self: sf.Sftp, path: string): sf.Sftp.PathResult");
    type_sf__Sftp.set_function("resolvePath",
        sol::overload(
            [](sf::Sftp& self, std::string path, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::PathResult {
                return self.resolvePath(std::filesystem::path(path), timeout);
            },
            [](sf::Sftp& self, std::string path) -> sf::Sftp::PathResult {
                return self.resolvePath(std::filesystem::path(path));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the current working directory on the server\n\nThis is an alias for calling `resolvePath(\".\")`.\n\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of getting the current working directory\n\n\\see `resolvePath`");
    LUASF_STUB_FUNCTION("sf.Sftp", "getWorkingDirectory", "fun(self: sf.Sftp, timeout: sf.TimeoutWithPredicate): sf.Sftp.PathResult");
    LUASF_STUB_OVERLOAD("sf.Sftp", "getWorkingDirectory", "fun(self: sf.Sftp): sf.Sftp.PathResult");
    type_sf__Sftp.set_function("getWorkingDirectory",
        sol::overload(
            [](sf::Sftp& self, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::PathResult {
                return self.getWorkingDirectory(timeout);
            },
            [](sf::Sftp& self) -> sf::Sftp::PathResult {
                return self.getWorkingDirectory();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the attributes of a remote file or directory\n\nDepending on whether `path` refers to a file or directory,\nthe attributes can contain e.g. the type of file, the file\nowner, group, file size, modification and access times.\n\nIf links are not to be followed, `followLinks` can be set\nto `false`. In this case the attributes of the link itself\nwill be returned.\n\n\\param path        Path to the remote file or directory whose attributes to get\n\\param followLinks `true` to follow links, `false` to return attributes of the link itself\n\\param timeout     Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of getting the attributes\n\n\\see `getDirectoryListing`");
    LUASF_STUB_FUNCTION("sf.Sftp", "getAttributes", "fun(self: sf.Sftp, path: string, followLinks: boolean, timeout: sf.TimeoutWithPredicate): sf.Sftp.AttributesResult");
    LUASF_STUB_OVERLOAD("sf.Sftp", "getAttributes", "fun(self: sf.Sftp, path: string, followLinks: boolean): sf.Sftp.AttributesResult");
    LUASF_STUB_OVERLOAD("sf.Sftp", "getAttributes", "fun(self: sf.Sftp, path: string): sf.Sftp.AttributesResult");
    type_sf__Sftp.set_function("getAttributes",
        sol::overload(
            [](sf::Sftp& self, std::string path, bool followLinks, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::AttributesResult {
                return self.getAttributes(std::filesystem::path(path), followLinks, timeout);
            },
            [](sf::Sftp& self, std::string path, bool followLinks) -> sf::Sftp::AttributesResult {
                return self.getAttributes(std::filesystem::path(path), followLinks);
            },
            [](sf::Sftp& self, std::string path) -> sf::Sftp::AttributesResult {
                return self.getAttributes(std::filesystem::path(path));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the contents of the given directory\n\nThis function retrieves the sub-directories and files\ncontained in the given directory. It is not recursive.\n\n\\param path    Path of the directory whose contents to list\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of getting the contents of the given directory\n\n\\see `getAttributes`");
    LUASF_STUB_FUNCTION("sf.Sftp", "getDirectoryListing", "fun(self: sf.Sftp, path: string, timeout: sf.TimeoutWithPredicate): sf.Sftp.ListingResult");
    LUASF_STUB_OVERLOAD("sf.Sftp", "getDirectoryListing", "fun(self: sf.Sftp, path: string): sf.Sftp.ListingResult");
    type_sf__Sftp.set_function("getDirectoryListing",
        sol::overload(
            [](sf::Sftp& self, std::string path, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::ListingResult {
                return self.getDirectoryListing(std::filesystem::path(path), timeout);
            },
            [](sf::Sftp& self, std::string path) -> sf::Sftp::ListingResult {
                return self.getDirectoryListing(std::filesystem::path(path));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Create a new directory\n\nThe new directory is created as a child of the current\nworking directory.\n\nThe default permissions value is equivalent to `rwxr-xr-x`\nor 0755 in octal notation.\n\n\\param path        Path of the directory to create\n\\param permissions Permissions of the directory to create\n\\param timeout     Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of creating the directory\n\n\\see `deleteDirectory`, `rename`");
    LUASF_STUB_FUNCTION("sf.Sftp", "createDirectory", "fun(self: sf.Sftp, path: string, permissions: any, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "createDirectory", "fun(self: sf.Sftp, path: string, permissions: any): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "createDirectory", "fun(self: sf.Sftp, path: string): sf.Sftp.Result");
    type_sf__Sftp.set_function("createDirectory",
        sol::overload(
            [](sf::Sftp& self, std::string path, std::filesystem::perms permissions, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.createDirectory(std::filesystem::path(path), permissions, timeout);
            },
            [](sf::Sftp& self, std::string path, std::filesystem::perms permissions) -> sf::Sftp::Result {
                return self.createDirectory(std::filesystem::path(path), permissions);
            },
            [](sf::Sftp& self, std::string path) -> sf::Sftp::Result {
                return self.createDirectory(std::filesystem::path(path));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Remove an existing directory\n\nUse this function with caution, the directory will\nbe removed permanently!\n\n\\param path    Path of the directory to remove\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of removing the directory\n\n\\see `createDirectory`, `rename`");
    LUASF_STUB_FUNCTION("sf.Sftp", "deleteDirectory", "fun(self: sf.Sftp, path: string, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "deleteDirectory", "fun(self: sf.Sftp, path: string): sf.Sftp.Result");
    type_sf__Sftp.set_function("deleteDirectory",
        sol::overload(
            [](sf::Sftp& self, std::string path, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.deleteDirectory(std::filesystem::path(path), timeout);
            },
            [](sf::Sftp& self, std::string path) -> sf::Sftp::Result {
                return self.deleteDirectory(std::filesystem::path(path));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Rename an existing file or directory\n\nIn POSIX renaming and moving and synonymous. If you want\nto move a file or directory from one place to another\nyou rename it from an old to a new path.\n\nIf a file exists at the specified new path, depending\non whether `ovewrite` is set to true, the rename operation\nwill overwrite it or not. If a directory is being moved,\nthe new path must either not point to non-existant\ndirectory or a directory that is empty. Non-empty\ndirectories cannot be overwritten by this operation.\n\n\\param oldPath   Old path to the file or directory\n\\param newPath   New path to the file or directory\n\\param overwrite Set to `true` to allow overwriting a file that exists at `newPath`\n\\param timeout   Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of the operation");
    LUASF_STUB_FUNCTION("sf.Sftp", "rename", "fun(self: sf.Sftp, oldPath: string, newPath: string, overwrite: boolean, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "rename", "fun(self: sf.Sftp, oldPath: string, newPath: string, overwrite: boolean): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "rename", "fun(self: sf.Sftp, oldPath: string, newPath: string): sf.Sftp.Result");
    type_sf__Sftp.set_function("rename",
        sol::overload(
            [](sf::Sftp& self, std::string oldPath, std::string newPath, bool overwrite, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.rename(std::filesystem::path(oldPath), std::filesystem::path(newPath), overwrite, timeout);
            },
            [](sf::Sftp& self, std::string oldPath, std::string newPath, bool overwrite) -> sf::Sftp::Result {
                return self.rename(std::filesystem::path(oldPath), std::filesystem::path(newPath), overwrite);
            },
            [](sf::Sftp& self, std::string oldPath, std::string newPath) -> sf::Sftp::Result {
                return self.rename(std::filesystem::path(oldPath), std::filesystem::path(newPath));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Remove an existing file\n\nUse this function with caution, the file will be\nremoved permanently!\n\n\\param path    Path to the file to remove\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of removing the file\n\n\\see `rename`");
    LUASF_STUB_FUNCTION("sf.Sftp", "deleteFile", "fun(self: sf.Sftp, path: string, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "deleteFile", "fun(self: sf.Sftp, path: string): sf.Sftp.Result");
    type_sf__Sftp.set_function("deleteFile",
        sol::overload(
            [](sf::Sftp& self, std::string path, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.deleteFile(std::filesystem::path(path), timeout);
            },
            [](sf::Sftp& self, std::string path) -> sf::Sftp::Result {
                return self.deleteFile(std::filesystem::path(path));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Download a file from the server\n\nThis function retrieves the data in the file at the\nremote path.\n\nThe file data is transferred in sequential blocks. For\nevery block of data transferred, the provided callback\nis called. The callback is passed a pointer to a data\nblock and the size of the data contained in the current\nblock. This size can change over time so it is important\nto always check the size value to know how much data is\nactually available. The callback should return `true` to\nindicate to the `download` function that it should\ncontinue to transfer data. If the data transfer should\nbe aborted earlier, `false` can be returned from the\ncallback.\n\nThe function returns once all the data in the remote file\nhas been transferred or an error occurs or the function\ntimes out.\n\nIf reading from the remote file should not start at the\nbeginning of the file, you can specify an offset in\nbytes at which reading should start.\n\n\\param remotePath Path of the remote file whose data to download\n\\param callback   Callback to be called for every available data block\n\\param offset     Byte offset into the remote file at which reading should start\n\\param timeout    Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of downloading the file\n\n\\see `upload`");
    LUASF_STUB_FUNCTION("sf.Sftp", "download", "fun(self: sf.Sftp, remotePath: string, callback: fun(data: string, size: integer): boolean, offset: integer, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "download", "fun(self: sf.Sftp, remotePath: string, callback: fun(data: string, size: integer): boolean, offset: integer): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "download", "fun(self: sf.Sftp, remotePath: string, callback: fun(data: string, size: integer): boolean): sf.Sftp.Result");
    type_sf__Sftp.set_function("download",
        sol::overload(
            [](sf::Sftp& self, std::string remotePath, sol::object callback, lua_sf::LuaIntegral<std::uint64_t> offset, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.download(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(const void*, std::size_t)>, lua_sf::callback::SftpDownloadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::download.callback", false}), offset.value(), timeout);
            },
            [](sf::Sftp& self, std::string remotePath, sol::object callback, lua_sf::LuaIntegral<std::uint64_t> offset) -> sf::Sftp::Result {
                return self.download(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(const void*, std::size_t)>, lua_sf::callback::SftpDownloadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::download.callback", false}), offset.value());
            },
            [](sf::Sftp& self, std::string remotePath, sol::object callback) -> sf::Sftp::Result {
                return self.download(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(const void*, std::size_t)>, lua_sf::callback::SftpDownloadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::download.callback", false}));
            }
        )
    );
    LUASF_STUB_DOC("\\brief Upload a file to the server\n\nThis function writes data into a file at the remote path.\n\nThe file data is transferred in sequential blocks. Every\ntime the function wants to send a new block of data the\nprovided callback is called. The callback is passed a\npointer to a data block and a reference to the size of\nthe data block. Data to be sent should be copied into\nthe data block using e.g. `std::memcpy` and the size value\nset to the actual number of bytes copied into the data\nblock. The size of the block can change over time so it\nis important to check the size value that is passed to\nthe callback to know how many bytes can actually be\ncopied into the data block. The callback should return\n`true` to indicate to the `upload` function that it\nshould continue to transfer data. Once the data transfer\nshould be stopped e.g. because there is no more data left\nto send, `false` can be returned from the callback.\n\nThe function returns once all the data has been sent or\nan error occurs or the function times out.\n\nIf a file does not exist at the remote path yet, it will\nbe created with the provided permissions.\n\nIf a file already exists at the remote path, setting\n`truncate` to `true` will truncate the existing file\ni.e. delete all pre-existing data before starting to\nwrite the new data into the file.\n\nSetting `append` to `true` will append to a file if\nit already exists.\n\nIf writing to the remote file should not start at the\nbeginning of the file, you can specify an offset in\nbytes at which writing should start.\n\nThe default permissions value is equivalent to `rw-r--r--`\nor 0644 in octal notation.\n\n\\param remotePath  Path of the remote file in which to upload the data\n\\param callback    Callback to be called for every available data block\n\\param permissions Permissions of the remote file if it has to be created\n\\param truncate    Set to `true` to truncate the remote file if it already exists\n\\param append      Set to `true` to append to the remote file if it already exists\n\\param offset      Byte offset into the remote file at which writing should start\n\\param timeout     Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of uploading the file\n\n\\see `download`");
    LUASF_STUB_FUNCTION("sf.Sftp", "upload", "fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions: any, truncate: boolean, append: boolean, offset: integer, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "upload", "fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions: any, truncate: boolean, append: boolean, offset: integer): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "upload", "fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions: any, truncate: boolean, append: boolean): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "upload", "fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions: any, truncate: boolean): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "upload", "fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions: any): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "upload", "fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil): sf.Sftp.Result");
    type_sf__Sftp.set_function("upload",
        sol::overload(
            [](sf::Sftp& self, std::string remotePath, sol::object callback, std::filesystem::perms permissions, bool truncate, bool append, lua_sf::LuaIntegral<std::uint64_t> offset, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
                return self.upload(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(void*, std::size_t&)>, lua_sf::callback::SftpUploadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::upload.callback", false}), permissions, truncate, append, offset.value(), timeout);
            },
            [](sf::Sftp& self, std::string remotePath, sol::object callback, std::filesystem::perms permissions, bool truncate, bool append, lua_sf::LuaIntegral<std::uint64_t> offset) -> sf::Sftp::Result {
                return self.upload(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(void*, std::size_t&)>, lua_sf::callback::SftpUploadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::upload.callback", false}), permissions, truncate, append, offset.value());
            },
            [](sf::Sftp& self, std::string remotePath, sol::object callback, std::filesystem::perms permissions, bool truncate, bool append) -> sf::Sftp::Result {
                return self.upload(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(void*, std::size_t&)>, lua_sf::callback::SftpUploadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::upload.callback", false}), permissions, truncate, append);
            },
            [](sf::Sftp& self, std::string remotePath, sol::object callback, std::filesystem::perms permissions, bool truncate) -> sf::Sftp::Result {
                return self.upload(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(void*, std::size_t&)>, lua_sf::callback::SftpUploadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::upload.callback", false}), permissions, truncate);
            },
            [](sf::Sftp& self, std::string remotePath, sol::object callback, std::filesystem::perms permissions) -> sf::Sftp::Result {
                return self.upload(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(void*, std::size_t&)>, lua_sf::callback::SftpUploadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::upload.callback", false}), permissions);
            },
            [](sf::Sftp& self, std::string remotePath, sol::object callback) -> sf::Sftp::Result {
                return self.upload(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(void*, std::size_t&)>, lua_sf::callback::SftpUploadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::upload.callback", false}));
            }
        )
    );
    auto type_sf__Sftp__Result = table_sf__Sftp.new_usertype<sf::Sftp::Result>("Result", sol::no_constructor);
    sol::table table_sf__Sftp__Result = table_sf__Sftp["Result"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sftp::Result>(lua);
    LUASF_STUB_DOC("\\brief SFTP result");
    LUASF_STUB_CLASS("sf.Sftp.Result");
    LUASF_STUB_DOC("\\brief Constructor\n\nThis constructor is used by the SFTP client to build\nthe result.\n\n\\param value   Result value\n\\param message Result message");
    LUASF_STUB_FUNCTION("sf.Sftp.Result", "new", "fun(value: sf.Sftp.Result.Value, message: string): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp.Result", "new", "fun(value: sf.Sftp.Result.Value): sf.Sftp.Result");
    type_sf__Sftp__Result.set_function("new", sol::factories(
        [](sf::Sftp::Result::Value value, std::string message) {
            return lua_sf::makeLuaSharedObject<sf::Sftp::Result>(value, message);
        },
        [](sf::Sftp::Result::Value value) {
            return lua_sf::makeLuaSharedObject<sf::Sftp::Result>(value);
        }
    ));
    LUASF_STUB_DOC("\\brief Check if the result is a success\n\nThis function is defined for convenience, it is\nequivalent to testing if the result value is `Value::Success`.\n\n\\return `true` if the result is `Value::Success`, `false` if it is not `Value::Success`");
    LUASF_STUB_FUNCTION("sf.Sftp.Result", "isOk", "fun(self: sf.Sftp.Result): boolean");
    type_sf__Sftp__Result.set_function("isOk",
        [](sf::Sftp::Result& self) -> bool {
            return self.isOk();
        }
    );
    LUASF_STUB_DOC("\\brief Get the result value\n\n\\return The result value");
    LUASF_STUB_FUNCTION("sf.Sftp.Result", "getValue", "fun(self: sf.Sftp.Result): sf.Sftp.Result.Value");
    type_sf__Sftp__Result.set_function("getValue",
        [](sf::Sftp::Result& self) -> sf::Sftp::Result::Value {
            return self.getValue();
        }
    );
    LUASF_STUB_DOC("\\brief Get the result message\n\n\\return The result message");
    LUASF_STUB_FUNCTION("sf.Sftp.Result", "getMessage", "fun(self: sf.Sftp.Result): string");
    type_sf__Sftp__Result.set_function("getMessage",
        sol::policies(
            [](sf::Sftp::Result& self) -> std::string {
                return std::string(self.getMessage());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Result values");
    LUASF_STUB_CLASS("sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Operation completed successfully");
    LUASF_STUB_FIELD("Success", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("The TCP socket has been disconnected");
    LUASF_STUB_FIELD("Disconnected", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Operation timed out");
    LUASF_STUB_FIELD("Timeout", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Connection refused");
    LUASF_STUB_FIELD("Refused", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Generic error");
    LUASF_STUB_FIELD("Error", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Error during banner receive");
    LUASF_STUB_FIELD("BannerReceive", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Error during banner send");
    LUASF_STUB_FIELD("BannerSend", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Invalid message authentication code");
    LUASF_STUB_FIELD("InvalidMac", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Allocation failure");
    LUASF_STUB_FIELD("AllocationFailure", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Error sending on socket");
    LUASF_STUB_FIELD("SocketSend", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Key exchange failed");
    LUASF_STUB_FIELD("KeyExchangeFailure", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Host key initialization failed");
    LUASF_STUB_FIELD("HostKeyInitialization", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Host key signing failed");
    LUASF_STUB_FIELD("HostKeySign", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Decryption failed");
    LUASF_STUB_FIELD("DecryptError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("SSH protocol error");
    LUASF_STUB_FIELD("ProtocolError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Password expired");
    LUASF_STUB_FIELD("PasswordExpired", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("File error");
    LUASF_STUB_FIELD("FileError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("No method found");
    LUASF_STUB_FIELD("MethodNone", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Authentication failed");
    LUASF_STUB_FIELD("AuthenticationFailed", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Public key unverified");
    LUASF_STUB_FIELD("PublicKeyUnverified", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Channel out of order");
    LUASF_STUB_FIELD("ChannelOutOfOrder", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Channel failure");
    LUASF_STUB_FIELD("ChannelFailure", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Channel request denied");
    LUASF_STUB_FIELD("ChannelRequestDenied", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Channel unknown");
    LUASF_STUB_FIELD("ChannelUnknown", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Channel window exceeded");
    LUASF_STUB_FIELD("ChannelWindowExceeded", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Channel packet exceeded");
    LUASF_STUB_FIELD("ChannelPacketExceeded", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Channel closed");
    LUASF_STUB_FIELD("ChannelClosed", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Channel EOF sent");
    LUASF_STUB_FIELD("ChannelEofSent", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("SCP protocol error");
    LUASF_STUB_FIELD("ScpProtocol", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Zlib error");
    LUASF_STUB_FIELD("ZlibError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Request denied");
    LUASF_STUB_FIELD("RequestDenied", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Method not supported");
    LUASF_STUB_FIELD("MethodNotSupported", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Invalid data");
    LUASF_STUB_FIELD("InvalidData", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Public key protocol error");
    LUASF_STUB_FIELD("PublicKeyProtocol", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Buffer too small");
    LUASF_STUB_FIELD("BufferTooSmall", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Bad usage");
    LUASF_STUB_FIELD("BadUse", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Compression error");
    LUASF_STUB_FIELD("CompressError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Out of boundary");
    LUASF_STUB_FIELD("OutOfBoundary", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Agent protocol error");
    LUASF_STUB_FIELD("AgentProtocol", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Socket receive error");
    LUASF_STUB_FIELD("SocketRecv", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Encryption failed");
    LUASF_STUB_FIELD("EncryptError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Bad socket");
    LUASF_STUB_FIELD("BadSocket", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Known hosts error");
    LUASF_STUB_FIELD("KnownHosts", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Channel window full");
    LUASF_STUB_FIELD("ChannelWindowFull", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Key file authentication failed");
    LUASF_STUB_FIELD("KeyFileAuthenticationFailed", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("End of file");
    LUASF_STUB_FIELD("EndOfFile", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("No such file");
    LUASF_STUB_FIELD("NoSuchFile", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Permission denied");
    LUASF_STUB_FIELD("PermissionDenied", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Failure");
    LUASF_STUB_FIELD("Failure", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Bad message");
    LUASF_STUB_FIELD("BadMessage", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("No connection");
    LUASF_STUB_FIELD("NoConnection", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Connection lost");
    LUASF_STUB_FIELD("ConnectionLost", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Operation unsupported");
    LUASF_STUB_FIELD("OperationUnsupported", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Invalid handle");
    LUASF_STUB_FIELD("InvalidHandle", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("No such path");
    LUASF_STUB_FIELD("NoSuchPath", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("File already exists");
    LUASF_STUB_FIELD("FileAlreadyExists", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Write protect");
    LUASF_STUB_FIELD("WriteProtect", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("No media");
    LUASF_STUB_FIELD("NoMedia", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("No space on filesystem");
    LUASF_STUB_FIELD("NoSpaceOnFileSystem", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Quota exceeded");
    LUASF_STUB_FIELD("QuotaExceeded", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Unknown principal");
    LUASF_STUB_FIELD("UnknownPrincipal", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Lock conflict");
    LUASF_STUB_FIELD("LockConflict", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Directory not empty");
    LUASF_STUB_FIELD("DirectoryNotEmpty", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Not a directory");
    LUASF_STUB_FIELD("NotADirectory", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Invalid filename");
    LUASF_STUB_FIELD("InvalidFilename", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Link loop");
    LUASF_STUB_FIELD("LinkLoop", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC("Generic SFTP error");
    LUASF_STUB_FIELD("SftpError", "sf.Sftp.Result.Value");
    table_sf__Sftp__Result.new_enum("Value",
        "Success", sf::Sftp::Result::Value::Success,
        "Disconnected", sf::Sftp::Result::Value::Disconnected,
        "Timeout", sf::Sftp::Result::Value::Timeout,
        "Refused", sf::Sftp::Result::Value::Refused,
        "Error", sf::Sftp::Result::Value::Error,
        "BannerReceive", sf::Sftp::Result::Value::BannerReceive,
        "BannerSend", sf::Sftp::Result::Value::BannerSend,
        "InvalidMac", sf::Sftp::Result::Value::InvalidMac,
        "AllocationFailure", sf::Sftp::Result::Value::AllocationFailure,
        "SocketSend", sf::Sftp::Result::Value::SocketSend,
        "KeyExchangeFailure", sf::Sftp::Result::Value::KeyExchangeFailure,
        "HostKeyInitialization", sf::Sftp::Result::Value::HostKeyInitialization,
        "HostKeySign", sf::Sftp::Result::Value::HostKeySign,
        "DecryptError", sf::Sftp::Result::Value::DecryptError,
        "ProtocolError", sf::Sftp::Result::Value::ProtocolError,
        "PasswordExpired", sf::Sftp::Result::Value::PasswordExpired,
        "FileError", sf::Sftp::Result::Value::FileError,
        "MethodNone", sf::Sftp::Result::Value::MethodNone,
        "AuthenticationFailed", sf::Sftp::Result::Value::AuthenticationFailed,
        "PublicKeyUnverified", sf::Sftp::Result::Value::PublicKeyUnverified,
        "ChannelOutOfOrder", sf::Sftp::Result::Value::ChannelOutOfOrder,
        "ChannelFailure", sf::Sftp::Result::Value::ChannelFailure,
        "ChannelRequestDenied", sf::Sftp::Result::Value::ChannelRequestDenied,
        "ChannelUnknown", sf::Sftp::Result::Value::ChannelUnknown,
        "ChannelWindowExceeded", sf::Sftp::Result::Value::ChannelWindowExceeded,
        "ChannelPacketExceeded", sf::Sftp::Result::Value::ChannelPacketExceeded,
        "ChannelClosed", sf::Sftp::Result::Value::ChannelClosed,
        "ChannelEofSent", sf::Sftp::Result::Value::ChannelEofSent,
        "ScpProtocol", sf::Sftp::Result::Value::ScpProtocol,
        "ZlibError", sf::Sftp::Result::Value::ZlibError,
        "RequestDenied", sf::Sftp::Result::Value::RequestDenied,
        "MethodNotSupported", sf::Sftp::Result::Value::MethodNotSupported,
        "InvalidData", sf::Sftp::Result::Value::InvalidData,
        "PublicKeyProtocol", sf::Sftp::Result::Value::PublicKeyProtocol,
        "BufferTooSmall", sf::Sftp::Result::Value::BufferTooSmall,
        "BadUse", sf::Sftp::Result::Value::BadUse,
        "CompressError", sf::Sftp::Result::Value::CompressError,
        "OutOfBoundary", sf::Sftp::Result::Value::OutOfBoundary,
        "AgentProtocol", sf::Sftp::Result::Value::AgentProtocol,
        "SocketRecv", sf::Sftp::Result::Value::SocketRecv,
        "EncryptError", sf::Sftp::Result::Value::EncryptError,
        "BadSocket", sf::Sftp::Result::Value::BadSocket,
        "KnownHosts", sf::Sftp::Result::Value::KnownHosts,
        "ChannelWindowFull", sf::Sftp::Result::Value::ChannelWindowFull,
        "KeyFileAuthenticationFailed", sf::Sftp::Result::Value::KeyFileAuthenticationFailed,
        "EndOfFile", sf::Sftp::Result::Value::EndOfFile,
        "NoSuchFile", sf::Sftp::Result::Value::NoSuchFile,
        "PermissionDenied", sf::Sftp::Result::Value::PermissionDenied,
        "Failure", sf::Sftp::Result::Value::Failure,
        "BadMessage", sf::Sftp::Result::Value::BadMessage,
        "NoConnection", sf::Sftp::Result::Value::NoConnection,
        "ConnectionLost", sf::Sftp::Result::Value::ConnectionLost,
        "OperationUnsupported", sf::Sftp::Result::Value::OperationUnsupported,
        "InvalidHandle", sf::Sftp::Result::Value::InvalidHandle,
        "NoSuchPath", sf::Sftp::Result::Value::NoSuchPath,
        "FileAlreadyExists", sf::Sftp::Result::Value::FileAlreadyExists,
        "WriteProtect", sf::Sftp::Result::Value::WriteProtect,
        "NoMedia", sf::Sftp::Result::Value::NoMedia,
        "NoSpaceOnFileSystem", sf::Sftp::Result::Value::NoSpaceOnFileSystem,
        "QuotaExceeded", sf::Sftp::Result::Value::QuotaExceeded,
        "UnknownPrincipal", sf::Sftp::Result::Value::UnknownPrincipal,
        "LockConflict", sf::Sftp::Result::Value::LockConflict,
        "DirectoryNotEmpty", sf::Sftp::Result::Value::DirectoryNotEmpty,
        "NotADirectory", sf::Sftp::Result::Value::NotADirectory,
        "InvalidFilename", sf::Sftp::Result::Value::InvalidFilename,
        "LinkLoop", sf::Sftp::Result::Value::LinkLoop,
        "SftpError", sf::Sftp::Result::Value::SftpError
    );
    auto type_sf__Sftp__PathResult = table_sf__Sftp.new_usertype<sf::Sftp::PathResult>("PathResult",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Sftp::Result>()
    );
    sol::table table_sf__Sftp__PathResult = table_sf__Sftp["PathResult"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sftp::PathResult>(lua);
    sol::table native_bases_sf__Sftp__PathResult = lua.create_table();
    native_bases_sf__Sftp__PathResult.add(lua["sf"]["Sftp"]["Result"].get<sol::table>());
    table_sf__Sftp__PathResult.raw_set("__nativeBases", native_bases_sf__Sftp__PathResult);
    LUASF_STUB_DOC("\\brief Result of an operation returning a path");
    LUASF_STUB_CLASS("sf.Sftp.PathResult", "sf.Sftp.Result");
    LUASF_STUB_DOC("\\brief Constructor\n\n\\param result Result\n\\param path   Path");
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "new", "fun(result: sf.Sftp.Result, path: string): sf.Sftp.PathResult");
    type_sf__Sftp__PathResult.set_function("new", sol::factories(
        [](const sf::Sftp::Result& result, std::string path) {
            return lua_sf::makeLuaSharedObject<sf::Sftp::PathResult>(result, std::filesystem::path(path));
        }
    ));
    LUASF_STUB_DOC("\\brief Check if the result is a success\n\nThis function is defined for convenience, it is\nequivalent to testing if the result value is `Value::Success`.\n\n\\return `true` if the result is `Value::Success`, `false` if it is not `Value::Success`");
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "isOk", "fun(self: sf.Sftp.PathResult): boolean");
    type_sf__Sftp__PathResult.set_function("isOk",
        [](sf::Sftp::PathResult& self) -> bool {
            return static_cast<sf::Sftp::Result&>(self).isOk();
        }
    );
    LUASF_STUB_DOC("\\brief Get the result value\n\n\\return The result value");
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "getValue", "fun(self: sf.Sftp.PathResult): sf.Sftp.Result.Value");
    type_sf__Sftp__PathResult.set_function("getValue",
        [](sf::Sftp::PathResult& self) -> sf::Sftp::Result::Value {
            return static_cast<sf::Sftp::Result&>(self).getValue();
        }
    );
    LUASF_STUB_DOC("\\brief Get the result message\n\n\\return The result message");
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "getMessage", "fun(self: sf.Sftp.PathResult): string");
    type_sf__Sftp__PathResult.set_function("getMessage",
        sol::policies(
            [](sf::Sftp::PathResult& self) -> std::string {
                return std::string(static_cast<sf::Sftp::Result&>(self).getMessage());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the path\n\n\\return The path");
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "getPath", "fun(self: sf.Sftp.PathResult): string");
    type_sf__Sftp__PathResult.set_function("getPath",
        sol::policies(
            [](sf::Sftp::PathResult& self) -> std::string {
                return (self.getPath()).string();
            },
            sol::self_dependency{}
        )
    );
    auto type_sf__Sftp__Attributes = table_sf__Sftp.new_usertype<sf::Sftp::Attributes>("Attributes", sol::no_constructor);
    sol::table table_sf__Sftp__Attributes = table_sf__Sftp["Attributes"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sftp::Attributes>(lua);
    LUASF_STUB_DOC("\\brief File or directory attributes");
    LUASF_STUB_CLASS("sf.Sftp.Attributes");
    LUASF_STUB_DOC("Path to the entry");
    LUASF_STUB_FIELD("path", "string");
    LUASF_STUB_DOC("Type of the entry");
    LUASF_STUB_FIELD("type", "any|nil");
    LUASF_STUB_DOC("Size of the entry");
    LUASF_STUB_FIELD("size", "integer|nil");
    LUASF_STUB_DOC("Permissions");
    LUASF_STUB_FIELD("permissions", "any|nil");
    LUASF_STUB_DOC("Owner user ID");
    LUASF_STUB_FIELD("userId", "integer|nil");
    LUASF_STUB_DOC("Group ID");
    LUASF_STUB_FIELD("groupId", "integer|nil");
    LUASF_STUB_DOC("Last access time");
    LUASF_STUB_FIELD("accessTime", "any|nil");
    LUASF_STUB_DOC("Last modification time");
    LUASF_STUB_FIELD("modificationTime", "any|nil");
    LUASF_STUB_FUNCTION("sf.Sftp.Attributes", "new", "fun(): sf.Sftp.Attributes");
    type_sf__Sftp__Attributes.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Sftp::Attributes>();
        }
    ));
    type_sf__Sftp__Attributes.set("path", sol::property(
        [](sf::Sftp::Attributes& self) -> std::string {
            return (self.path).string();
        },
        [](sf::Sftp::Attributes& self, std::string value) {
            self.path = std::filesystem::path(value);
        }
    ));
    type_sf__Sftp__Attributes.set("type", sol::property(
        [lua](sf::Sftp::Attributes& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.type);
        },
        [](sf::Sftp::Attributes& self, sol::object value) {
            auto value_optional = lua_sf::optional_from_object<std::filesystem::file_type>(value);
            self.type = value_optional;
        }
    ));
    type_sf__Sftp__Attributes.set("size", sol::property(
        [lua](sf::Sftp::Attributes& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.size);
        },
        [](sf::Sftp::Attributes& self, sol::object value) {
            auto value_optional = lua_sf::optional_from_object<std::uint64_t>(value);
            self.size = value_optional;
        }
    ));
    type_sf__Sftp__Attributes.set("permissions", sol::property(
        [lua](sf::Sftp::Attributes& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.permissions);
        },
        [](sf::Sftp::Attributes& self, sol::object value) {
            auto value_optional = lua_sf::optional_from_object<std::filesystem::perms>(value);
            self.permissions = value_optional;
        }
    ));
    type_sf__Sftp__Attributes.set("userId", sol::property(
        [lua](sf::Sftp::Attributes& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.userId);
        },
        [](sf::Sftp::Attributes& self, sol::object value) {
            auto value_optional = lua_sf::optional_from_object<std::uint64_t>(value);
            self.userId = value_optional;
        }
    ));
    type_sf__Sftp__Attributes.set("groupId", sol::property(
        [lua](sf::Sftp::Attributes& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.groupId);
        },
        [](sf::Sftp::Attributes& self, sol::object value) {
            auto value_optional = lua_sf::optional_from_object<std::uint64_t>(value);
            self.groupId = value_optional;
        }
    ));
    type_sf__Sftp__Attributes.set("accessTime", sol::property(
        [lua](sf::Sftp::Attributes& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.accessTime);
        },
        [](sf::Sftp::Attributes& self, sol::object value) {
            auto value_optional = lua_sf::optional_from_object<std::filesystem::file_time_type>(value);
            self.accessTime = value_optional;
        }
    ));
    type_sf__Sftp__Attributes.set("modificationTime", sol::property(
        [lua](sf::Sftp::Attributes& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.modificationTime);
        },
        [](sf::Sftp::Attributes& self, sol::object value) {
            auto value_optional = lua_sf::optional_from_object<std::filesystem::file_time_type>(value);
            self.modificationTime = value_optional;
        }
    ));
    auto type_sf__Sftp__AttributesResult = table_sf__Sftp.new_usertype<sf::Sftp::AttributesResult>("AttributesResult",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Sftp::Result>()
    );
    sol::table table_sf__Sftp__AttributesResult = table_sf__Sftp["AttributesResult"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sftp::AttributesResult>(lua);
    sol::table native_bases_sf__Sftp__AttributesResult = lua.create_table();
    native_bases_sf__Sftp__AttributesResult.add(lua["sf"]["Sftp"]["Result"].get<sol::table>());
    table_sf__Sftp__AttributesResult.raw_set("__nativeBases", native_bases_sf__Sftp__AttributesResult);
    LUASF_STUB_DOC("\\brief Result of an operation returning attributes");
    LUASF_STUB_CLASS("sf.Sftp.AttributesResult", "sf.Sftp.Result");
    LUASF_STUB_DOC("\\brief Constructor\n\n\\param result     Result\n\\param attributes Attributes");
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "new", "fun(result: sf.Sftp.Result, attributes: sf.Sftp.Attributes): sf.Sftp.AttributesResult");
    type_sf__Sftp__AttributesResult.set_function("new", sol::factories(
        [](const sf::Sftp::Result& result, sf::Sftp::Attributes attributes) {
            return lua_sf::makeLuaSharedObject<sf::Sftp::AttributesResult>(result, attributes);
        }
    ));
    LUASF_STUB_DOC("\\brief Check if the result is a success\n\nThis function is defined for convenience, it is\nequivalent to testing if the result value is `Value::Success`.\n\n\\return `true` if the result is `Value::Success`, `false` if it is not `Value::Success`");
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "isOk", "fun(self: sf.Sftp.AttributesResult): boolean");
    type_sf__Sftp__AttributesResult.set_function("isOk",
        [](sf::Sftp::AttributesResult& self) -> bool {
            return static_cast<sf::Sftp::Result&>(self).isOk();
        }
    );
    LUASF_STUB_DOC("\\brief Get the result value\n\n\\return The result value");
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "getValue", "fun(self: sf.Sftp.AttributesResult): sf.Sftp.Result.Value");
    type_sf__Sftp__AttributesResult.set_function("getValue",
        [](sf::Sftp::AttributesResult& self) -> sf::Sftp::Result::Value {
            return static_cast<sf::Sftp::Result&>(self).getValue();
        }
    );
    LUASF_STUB_DOC("\\brief Get the result message\n\n\\return The result message");
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "getMessage", "fun(self: sf.Sftp.AttributesResult): string");
    type_sf__Sftp__AttributesResult.set_function("getMessage",
        sol::policies(
            [](sf::Sftp::AttributesResult& self) -> std::string {
                return std::string(static_cast<sf::Sftp::Result&>(self).getMessage());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the attributes\n\n\\return The attributes");
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "getAttributes", "fun(self: sf.Sftp.AttributesResult): sf.Sftp.Attributes");
    type_sf__Sftp__AttributesResult.set_function("getAttributes",
        sol::policies(
            [](sf::Sftp::AttributesResult& self) {
                return std::cref(self.getAttributes());
            },
            sol::self_dependency{}
        )
    );
    auto type_sf__Sftp__ListingResult = table_sf__Sftp.new_usertype<sf::Sftp::ListingResult>("ListingResult",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Sftp::Result>()
    );
    sol::table table_sf__Sftp__ListingResult = table_sf__Sftp["ListingResult"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sftp::ListingResult>(lua);
    sol::table native_bases_sf__Sftp__ListingResult = lua.create_table();
    native_bases_sf__Sftp__ListingResult.add(lua["sf"]["Sftp"]["Result"].get<sol::table>());
    table_sf__Sftp__ListingResult.raw_set("__nativeBases", native_bases_sf__Sftp__ListingResult);
    LUASF_STUB_DOC("\\brief Result of an operation returning a directory listing");
    LUASF_STUB_CLASS("sf.Sftp.ListingResult", "sf.Sftp.Result");
    LUASF_STUB_DOC("\\brief Constructor\n\n\\param result  Result\n\\param listing Directory listing");
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "new", "fun(result: sf.Sftp.Result, listing: sf.Sftp.Attributes[]): sf.Sftp.ListingResult");
    type_sf__Sftp__ListingResult.set_function("new", sol::factories(
        [](const sf::Sftp::Result& result, sol::table listing) {
            auto listing_vector = lua_sf::array_from_object<sf::Sftp::Attributes>(listing);
            return lua_sf::makeLuaSharedObject<sf::Sftp::ListingResult>(result, listing_vector);
        }
    ));
    LUASF_STUB_DOC("\\brief Check if the result is a success\n\nThis function is defined for convenience, it is\nequivalent to testing if the result value is `Value::Success`.\n\n\\return `true` if the result is `Value::Success`, `false` if it is not `Value::Success`");
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "isOk", "fun(self: sf.Sftp.ListingResult): boolean");
    type_sf__Sftp__ListingResult.set_function("isOk",
        [](sf::Sftp::ListingResult& self) -> bool {
            return static_cast<sf::Sftp::Result&>(self).isOk();
        }
    );
    LUASF_STUB_DOC("\\brief Get the result value\n\n\\return The result value");
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "getValue", "fun(self: sf.Sftp.ListingResult): sf.Sftp.Result.Value");
    type_sf__Sftp__ListingResult.set_function("getValue",
        [](sf::Sftp::ListingResult& self) -> sf::Sftp::Result::Value {
            return static_cast<sf::Sftp::Result&>(self).getValue();
        }
    );
    LUASF_STUB_DOC("\\brief Get the result message\n\n\\return The result message");
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "getMessage", "fun(self: sf.Sftp.ListingResult): string");
    type_sf__Sftp__ListingResult.set_function("getMessage",
        sol::policies(
            [](sf::Sftp::ListingResult& self) -> std::string {
                return std::string(static_cast<sf::Sftp::Result&>(self).getMessage());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the directory listing\n\n\\return The directory listing");
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "getListing", "fun(self: sf.Sftp.ListingResult): sf.Sftp.Attributes[]");
    type_sf__Sftp__ListingResult.set_function("getListing",
        sol::policies(
            [lua](sf::Sftp::ListingResult& self) -> sol::object {
                return lua_sf::vector_to_object(lua, self.getListing());
            },
            sol::self_dependency{}
        )
    );
    auto type_sf__Sftp__SessionInfo = table_sf__Sftp.new_usertype<sf::Sftp::SessionInfo>("SessionInfo", sol::no_constructor);
    sol::table table_sf__Sftp__SessionInfo = table_sf__Sftp["SessionInfo"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sftp::SessionInfo>(lua);
    LUASF_STUB_DOC("\\brief Structure containing information about an active SFTP session");
    LUASF_STUB_CLASS("sf.Sftp.SessionInfo");
    LUASF_STUB_DOC("Host key");
    LUASF_STUB_FIELD("hostKey", "sf.Sftp.SessionInfo.HostKey");
    LUASF_STUB_DOC("Key exchange algorithm used in the session (RFC 4253)");
    LUASF_STUB_FIELD("keyExchangeAlgorithm", "string");
    LUASF_STUB_DOC("Host key algorithm used in the session (RFC 4253)");
    LUASF_STUB_FIELD("hostKeyAlgorithm", "string");
    LUASF_STUB_DOC("Client to server encryption algorithm used in the session (RFC 4253)");
    LUASF_STUB_FIELD("clientToServerEncryptionAlgorithm", "string");
    LUASF_STUB_DOC("Server to client encryption algorithm used in the session (RFC 4253)");
    LUASF_STUB_FIELD("serverToClientEncryptionAlgorithm", "string");
    LUASF_STUB_DOC("Client to server message authentication code algorithm used in the session (RFC 4253)");
    LUASF_STUB_FIELD("clientToServerMacAlgorithm", "string");
    LUASF_STUB_DOC("Server to client message authentication code algorithm used in the session (RFC 4253)");
    LUASF_STUB_FIELD("serverToClientMacAlgorithm", "string");
    LUASF_STUB_DOC("Client to server compression algorithm used in the session (RFC 4253)");
    LUASF_STUB_FIELD("clientToServerCompressionAlgorithm", "string");
    LUASF_STUB_DOC("Server to client compression algorithm used in the session (RFC 4253)");
    LUASF_STUB_FIELD("serverToClientCompressionAlgorithm", "string");
    LUASF_STUB_FUNCTION("sf.Sftp.SessionInfo", "new", "fun(): sf.Sftp.SessionInfo");
    type_sf__Sftp__SessionInfo.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Sftp::SessionInfo>();
        }
    ));
    type_sf__Sftp__SessionInfo["hostKey"] = sol::policies(&sf::Sftp::SessionInfo::hostKey, sol::self_dependency{});
    type_sf__Sftp__SessionInfo.set("keyExchangeAlgorithm", sol::property(
        [](sf::Sftp::SessionInfo& self) {
            return std::string(self.keyExchangeAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.keyExchangeAlgorithm = value;
        }
    ));
    type_sf__Sftp__SessionInfo.set("hostKeyAlgorithm", sol::property(
        [](sf::Sftp::SessionInfo& self) {
            return std::string(self.hostKeyAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.hostKeyAlgorithm = value;
        }
    ));
    type_sf__Sftp__SessionInfo.set("clientToServerEncryptionAlgorithm", sol::property(
        [](sf::Sftp::SessionInfo& self) {
            return std::string(self.clientToServerEncryptionAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.clientToServerEncryptionAlgorithm = value;
        }
    ));
    type_sf__Sftp__SessionInfo.set("serverToClientEncryptionAlgorithm", sol::property(
        [](sf::Sftp::SessionInfo& self) {
            return std::string(self.serverToClientEncryptionAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.serverToClientEncryptionAlgorithm = value;
        }
    ));
    type_sf__Sftp__SessionInfo.set("clientToServerMacAlgorithm", sol::property(
        [](sf::Sftp::SessionInfo& self) {
            return std::string(self.clientToServerMacAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.clientToServerMacAlgorithm = value;
        }
    ));
    type_sf__Sftp__SessionInfo.set("serverToClientMacAlgorithm", sol::property(
        [](sf::Sftp::SessionInfo& self) {
            return std::string(self.serverToClientMacAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.serverToClientMacAlgorithm = value;
        }
    ));
    type_sf__Sftp__SessionInfo.set("clientToServerCompressionAlgorithm", sol::property(
        [](sf::Sftp::SessionInfo& self) {
            return std::string(self.clientToServerCompressionAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.clientToServerCompressionAlgorithm = value;
        }
    ));
    type_sf__Sftp__SessionInfo.set("serverToClientCompressionAlgorithm", sol::property(
        [](sf::Sftp::SessionInfo& self) {
            return std::string(self.serverToClientCompressionAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.serverToClientCompressionAlgorithm = value;
        }
    ));
    auto type_sf__Sftp__SessionInfo__HostKey = table_sf__Sftp__SessionInfo.new_usertype<sf::Sftp::SessionInfo::HostKey>("HostKey", sol::no_constructor);
    sol::table table_sf__Sftp__SessionInfo__HostKey = table_sf__Sftp__SessionInfo["HostKey"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sftp::SessionInfo::HostKey>(lua);
    LUASF_STUB_DOC("\\brief Host key used to identify a host");
    LUASF_STUB_CLASS("sf.Sftp.SessionInfo.HostKey");
    LUASF_STUB_DOC("Host key type");
    LUASF_STUB_FIELD("type", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC("Host key data");
    LUASF_STUB_FIELD("data", "any[]");
    LUASF_STUB_DOC("Host key SHA1 hash");
    LUASF_STUB_FIELD("sha1", "any");
    LUASF_STUB_DOC("Host key SHA256 hash");
    LUASF_STUB_FIELD("sha256", "any");
    LUASF_STUB_FUNCTION("sf.Sftp.SessionInfo.HostKey", "new", "fun(): sf.Sftp.SessionInfo.HostKey");
    type_sf__Sftp__SessionInfo__HostKey.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Sftp::SessionInfo::HostKey>();
        }
    ));
    type_sf__Sftp__SessionInfo__HostKey["type"] = sol::policies(&sf::Sftp::SessionInfo::HostKey::type, sol::self_dependency{});
    type_sf__Sftp__SessionInfo__HostKey.set("data", sol::property(
        [lua](sf::Sftp::SessionInfo::HostKey& self) {
            return lua_sf::vector_to_object(lua, self.data);
        },
        [](sf::Sftp::SessionInfo::HostKey& self, sol::table value) {
            auto value_vector = lua_sf::array_from_object<std::byte>(value);
            self.data = value_vector;
        }
    ));
    type_sf__Sftp__SessionInfo__HostKey["sha1"] = sol::policies(&sf::Sftp::SessionInfo::HostKey::sha1, sol::self_dependency{});
    type_sf__Sftp__SessionInfo__HostKey["sha256"] = sol::policies(&sf::Sftp::SessionInfo::HostKey::sha256, sol::self_dependency{});
    LUASF_STUB_CLASS("sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC("Unknown key type");
    LUASF_STUB_FIELD("Unknown", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC("RSA");
    LUASF_STUB_FIELD("Rsa", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC("DSA");
    LUASF_STUB_FIELD("Dsa", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC("NIST P-256 ECDSA");
    LUASF_STUB_FIELD("Ecdsa256", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC("NIST P-384 ECDSA");
    LUASF_STUB_FIELD("Ecdsa384", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC("NIST P-521 ECDSA");
    LUASF_STUB_FIELD("Ecdsa521", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC("ED25519");
    LUASF_STUB_FIELD("Ed25519", "sf.Sftp.SessionInfo.HostKey.Type");
    table_sf__Sftp__SessionInfo__HostKey.new_enum("Type",
        "Unknown", sf::Sftp::SessionInfo::HostKey::Type::Unknown,
        "Rsa", sf::Sftp::SessionInfo::HostKey::Type::Rsa,
        "Dsa", sf::Sftp::SessionInfo::HostKey::Type::Dsa,
        "Ecdsa256", sf::Sftp::SessionInfo::HostKey::Type::Ecdsa256,
        "Ecdsa384", sf::Sftp::SessionInfo::HostKey::Type::Ecdsa384,
        "Ecdsa521", sf::Sftp::SessionInfo::HostKey::Type::Ecdsa521,
        "Ed25519", sf::Sftp::SessionInfo::HostKey::Type::Ed25519
    );
}
