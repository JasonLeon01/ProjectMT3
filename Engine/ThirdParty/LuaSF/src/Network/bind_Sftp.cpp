#include "Network/bind_Sftp.hpp"

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

namespace { constexpr std::array<std::string_view, 131> docs = {
    "\\brief An SSH File Transfer Protocol (SFTP) client",
    "\\brief Default constructor",
    "\\brief Connect to the specified SFTP server\n\nThe port has a default value of 22, which is the standard\nport used by the SFTP protocol.\nThis function tries to connect to the server so it may take\na while to complete, especially if the server is not\nreachable. To avoid blocking your application for too long,\nyou can use a timeout. The default value, `Time::Zero`, means that the\nsystem timeout will be used (which is usually pretty long).\n\n\\param server  Name or address of the SFTP server to connect to\n\\param port    Port used for the connection\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of the connection attempt\n\n\\see `disconnect`",
    "\\brief Disconnect the connection with the server\n\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of disconnecting the connection with the server\n\n\\see `connect`",
    "\\brief Get SSH session information\n\nAfter connecting to the server and before actually\nlogging in the SSH session information of the underlying\nconnection will be available.\n\nThe session information contains among other things the\npublic key identifying the remote host and the connection\nparameters such as encryption and compression used. The\nidentifiers used follow the RFC 4253 specification.\n\nIf the session information is not available `std::nullopt`\nwill be returned.\n\nBecause SSH was developed as a parallel standard to\nSSL/TLS and automatic host certificate verification wasn't\nwidespread at the time, relying on the user to check the\nauthenticity of the host key was the typical method used\nto verify that they were connecting to the legitimate host,\nassuming the private key of the remote host was not\ncompromised.\n\nIf connection security is a high priority, examining\nthe parameters and aborting the connection if any weak\nalgorithms are used is also possible.\n\n\\return SSH session information or `std::nullopt` if it is not available",
    "\\brief Log in using a username and a password\n\nLogging in is mandatory after connecting to the server.\nUsers that are not logged in cannot perform any operation.\n\n\\param name     User name\n\\param password Password\n\\param timeout  Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of attempting to log in to the server",
    "\\brief Log in using a public/private key pair\n\nLogging in is mandatory after connecting to the server.\nUsers that are not logged in cannot perform any operation.\n\nThis overload allows logging into the SFTP server using\npublic key authentication.\n\nThe public and private key data should be provided in PEM\nformat. PEM encoded data can be easily recognized by their\n`-----BEGIN ............-----` header and\n`-----END ............-----` footer.\n\nEven though it is technically possible to derive the\npublic key from the private key, due to backend\nlimitations, providing a pre-generated public key as well\nis necessary for this function to be able to succeed.\n\nIf the private key is protected by a passphrase the\npassphrase can be provided as a NULL terminated string.\nIf the private key is not protected by a passphrase the\npassphrase should be set to the empty string.\n\n\\param name                 User name\n\\param publicKeyData        Public key data\n\\param publicKeyLength      Public key data length\n\\param privateKeyData       Private key data\n\\param privateKeyLength     Private key data length\n\\param privateKeyPassphrase Private key passphrase, NULL terminated\n\\param timeout              Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of attempting to log in to the server",
    "\\brief Log in using a public/private key pair\n\nLogging in is mandatory after connecting to the server.\nUsers that are not logged in cannot perform any operation.\n\nThis overload allows logging into the SFTP server using\npublic key authentication.\n\nThe public and private key data should be provided in PEM\nformat. PEM encoded data can be easily recognized by their\n`-----BEGIN ............-----` header and\n`-----END ............-----` footer.\n\nEven though it is technically possible to derive the\npublic key from the private key, due to backend\nlimitations, providing a pre-generated public key as well\nis necessary for this function to be able to succeed.\n\nIf the private key is protected by a passphrase the\npassphrase can be provided as a NULL terminated string.\nIf the private key is not protected by a passphrase the\npassphrase should be set to the empty string.\n\n\\param name                 User name\n\\param publicKeyData        Public key data\n\\param privateKeyData       Private key data\n\\param privateKeyPassphrase Private key passphrase\n\\param timeout              Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of attempting to log in to the server",
    "\\brief Resolve a remote path into an absolute remote path\n\nPaths can contain links and other reserved path identifiers\nsuch as . and .. referring to the current directory and\nparent directory respectively.\n\nWhen determining the absolute path, which does not contain\nlinks or . or .. is necessary, this function can be used.\n\nResolving \".\" will return the absolute path to the current\nworking directory of the user after logging in to the SFTP\nserver.\n\n\\param path    Path to convert into an absolute path\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of converting the path into an absolute path\n\n\\see `getWorkingDirectory`",
    "\\brief Get the current working directory on the server\n\nThis is an alias for calling `resolvePath(\".\")`.\n\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of getting the current working directory\n\n\\see `resolvePath`",
    "\\brief Get the attributes of a remote file or directory\n\nDepending on whether `path` refers to a file or directory,\nthe attributes can contain e.g. the type of file, the file\nowner, group, file size, modification and access times.\n\nIf links are not to be followed, `followLinks` can be set\nto `false`. In this case the attributes of the link itself\nwill be returned.\n\n\\param path        Path to the remote file or directory whose attributes to get\n\\param followLinks `true` to follow links, `false` to return attributes of the link itself\n\\param timeout     Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of getting the attributes\n\n\\see `getDirectoryListing`",
    "\\brief Get the contents of the given directory\n\nThis function retrieves the sub-directories and files\ncontained in the given directory. It is not recursive.\n\n\\param path    Path of the directory whose contents to list\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of getting the contents of the given directory\n\n\\see `getAttributes`",
    "\\brief Create a new directory\n\nThe new directory is created as a child of the current\nworking directory.\n\nThe default permissions value is equivalent to `rwxr-xr-x`\nor 0755 in octal notation.\n\n\\param path        Path of the directory to create\n\\param permissions Permissions of the directory to create\n\\param timeout     Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of creating the directory\n\n\\see `deleteDirectory`, `rename`",
    "\\brief Remove an existing directory\n\nUse this function with caution, the directory will\nbe removed permanently!\n\n\\param path    Path of the directory to remove\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of removing the directory\n\n\\see `createDirectory`, `rename`",
    "\\brief Rename an existing file or directory\n\nIn POSIX renaming and moving and synonymous. If you want\nto move a file or directory from one place to another\nyou rename it from an old to a new path.\n\nIf a file exists at the specified new path, depending\non whether `ovewrite` is set to true, the rename operation\nwill overwrite it or not. If a directory is being moved,\nthe new path must either not point to non-existant\ndirectory or a directory that is empty. Non-empty\ndirectories cannot be overwritten by this operation.\n\n\\param oldPath   Old path to the file or directory\n\\param newPath   New path to the file or directory\n\\param overwrite Set to `true` to allow overwriting a file that exists at `newPath`\n\\param timeout   Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of the operation",
    "\\brief Remove an existing file\n\nUse this function with caution, the file will be\nremoved permanently!\n\n\\param path    Path to the file to remove\n\\param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of removing the file\n\n\\see `rename`",
    "\\brief Download a file from the server\n\nThis function retrieves the data in the file at the\nremote path.\n\nThe file data is transferred in sequential blocks. For\nevery block of data transferred, the provided callback\nis called. The callback is passed a pointer to a data\nblock and the size of the data contained in the current\nblock. This size can change over time so it is important\nto always check the size value to know how much data is\nactually available. The callback should return `true` to\nindicate to the `download` function that it should\ncontinue to transfer data. If the data transfer should\nbe aborted earlier, `false` can be returned from the\ncallback.\n\nThe function returns once all the data in the remote file\nhas been transferred or an error occurs or the function\ntimes out.\n\nIf reading from the remote file should not start at the\nbeginning of the file, you can specify an offset in\nbytes at which reading should start.\n\n\\param remotePath Path of the remote file whose data to download\n\\param callback   Callback to be called for every available data block\n\\param offset     Byte offset into the remote file at which reading should start\n\\param timeout    Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of downloading the file\n\n\\see `upload`",
    "\\brief Upload a file to the server\n\nThis function writes data into a file at the remote path.\n\nThe file data is transferred in sequential blocks. Every\ntime the function wants to send a new block of data the\nprovided callback is called. The callback is passed a\npointer to a data block and a reference to the size of\nthe data block. Data to be sent should be copied into\nthe data block using e.g. `std::memcpy` and the size value\nset to the actual number of bytes copied into the data\nblock. The size of the block can change over time so it\nis important to check the size value that is passed to\nthe callback to know how many bytes can actually be\ncopied into the data block. The callback should return\n`true` to indicate to the `upload` function that it\nshould continue to transfer data. Once the data transfer\nshould be stopped e.g. because there is no more data left\nto send, `false` can be returned from the callback.\n\nThe function returns once all the data has been sent or\nan error occurs or the function times out.\n\nIf a file does not exist at the remote path yet, it will\nbe created with the provided permissions.\n\nIf a file already exists at the remote path, setting\n`truncate` to `true` will truncate the existing file\ni.e. delete all pre-existing data before starting to\nwrite the new data into the file.\n\nSetting `append` to `true` will append to a file if\nit already exists.\n\nIf writing to the remote file should not start at the\nbeginning of the file, you can specify an offset in\nbytes at which writing should start.\n\nThe default permissions value is equivalent to `rw-r--r--`\nor 0644 in octal notation.\n\n\\param remotePath  Path of the remote file in which to upload the data\n\\param callback    Callback to be called for every available data block\n\\param permissions Permissions of the remote file if it has to be created\n\\param truncate    Set to `true` to truncate the remote file if it already exists\n\\param append      Set to `true` to append to the remote file if it already exists\n\\param offset      Byte offset into the remote file at which writing should start\n\\param timeout     Maximum time to wait, optionally a predicate can be provided for more fine-grained control\n\n\\return Result of uploading the file\n\n\\see `download`",
    "\\brief SFTP result",
    "\\brief Constructor\n\nThis constructor is used by the SFTP client to build\nthe result.\n\n\\param value   Result value\n\\param message Result message",
    "\\brief Check if the result is a success\n\nThis function is defined for convenience, it is\nequivalent to testing if the result value is `Value::Success`.\n\n\\return `true` if the result is `Value::Success`, `false` if it is not `Value::Success`",
    "\\brief Get the result value\n\n\\return The result value",
    "\\brief Get the result message\n\n\\return The result message",
    "\\brief Result values",
    "Operation completed successfully",
    "The TCP socket has been disconnected",
    "Operation timed out",
    "Connection refused",
    "Generic error",
    "Error during banner receive",
    "Error during banner send",
    "Invalid message authentication code",
    "Allocation failure",
    "Error sending on socket",
    "Key exchange failed",
    "Host key initialization failed",
    "Host key signing failed",
    "Decryption failed",
    "SSH protocol error",
    "Password expired",
    "File error",
    "No method found",
    "Authentication failed",
    "Public key unverified",
    "Channel out of order",
    "Channel failure",
    "Channel request denied",
    "Channel unknown",
    "Channel window exceeded",
    "Channel packet exceeded",
    "Channel closed",
    "Channel EOF sent",
    "SCP protocol error",
    "Zlib error",
    "Request denied",
    "Method not supported",
    "Invalid data",
    "Public key protocol error",
    "Buffer too small",
    "Bad usage",
    "Compression error",
    "Out of boundary",
    "Agent protocol error",
    "Socket receive error",
    "Encryption failed",
    "Bad socket",
    "Known hosts error",
    "Channel window full",
    "Key file authentication failed",
    "End of file",
    "No such file",
    "Permission denied",
    "Failure",
    "Bad message",
    "No connection",
    "Connection lost",
    "Operation unsupported",
    "Invalid handle",
    "No such path",
    "File already exists",
    "Write protect",
    "No media",
    "No space on filesystem",
    "Quota exceeded",
    "Unknown principal",
    "Lock conflict",
    "Directory not empty",
    "Not a directory",
    "Invalid filename",
    "Link loop",
    "Generic SFTP error",
    "\\brief Result of an operation returning a path",
    "\\brief Constructor\n\n\\param result Result\n\\param path   Path",
    "\\brief Get the path\n\n\\return The path",
    "\\brief File or directory attributes",
    "Path to the entry",
    "Type of the entry",
    "Size of the entry",
    "Permissions",
    "Owner user ID",
    "Group ID",
    "Last access time",
    "Last modification time",
    "\\brief Result of an operation returning attributes",
    "\\brief Constructor\n\n\\param result     Result\n\\param attributes Attributes",
    "\\brief Get the attributes\n\n\\return The attributes",
    "\\brief Result of an operation returning a directory listing",
    "\\brief Constructor\n\n\\param result  Result\n\\param listing Directory listing",
    "\\brief Get the directory listing\n\n\\return The directory listing",
    "\\brief Structure containing information about an active SFTP session",
    "Host key",
    "Key exchange algorithm used in the session (RFC 4253)",
    "Host key algorithm used in the session (RFC 4253)",
    "Client to server encryption algorithm used in the session (RFC 4253)",
    "Server to client encryption algorithm used in the session (RFC 4253)",
    "Client to server message authentication code algorithm used in the session (RFC 4253)",
    "Server to client message authentication code algorithm used in the session (RFC 4253)",
    "Client to server compression algorithm used in the session (RFC 4253)",
    "Server to client compression algorithm used in the session (RFC 4253)",
    "\\brief Host key used to identify a host",
    "Host key type",
    "Host key data",
    "Host key SHA1 hash",
    "Host key SHA256 hash",
    "Unknown key type",
    "RSA",
    "DSA",
    "NIST P-256 ECDSA",
    "NIST P-384 ECDSA",
    "NIST P-521 ECDSA",
    "ED25519",
}; }

void bind_Sftp(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Sftp = lua_glue::BindClass<sf::Sftp>(sf, "Sftp");
    lua_glue::Table table_sf__Sftp = sf["Sftp"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Sftp>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Sftp");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Sftp", "new", "fun(): sf.Sftp");
    lua_glue::BindCallable(type_sf__Sftp, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Sftp>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Sftp", "connect", "fun(self: sf.Sftp, server: sf.IpAddress, port?: integer, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp, "connect",
        [](sf::Sftp& self, sf::IpAddress server, lua_sf::LuaIntegral<unsigned short> port, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.connect(server, port.value(), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned short>(22);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Sftp", "disconnect", "fun(self: sf.Sftp, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp, "disconnect",
        [](sf::Sftp& self, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.disconnect(timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Sftp", "getSessionInfo", "fun(self: sf.Sftp): sf.Sftp.SessionInfo|nil");
    lua_glue::BindCallable(type_sf__Sftp, "getSessionInfo",
        [lua](const sf::Sftp& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.getSessionInfo());
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, publicKeyData: string, publicKeyLength: integer, privateKeyData: string, privateKeyLength: integer, privateKeyPassphrase?: string, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, publicKeyData: string, privateKeyData: string, privateKeyPassphrase?: string, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    LUASF_STUB_OVERLOAD("sf.Sftp", "login", "fun(self: sf.Sftp, name: string, password: string, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp, "login",
        [](sf::Sftp& self, std::string name, std::string publicKeyData, lua_sf::LuaIntegral<std::size_t> publicKeyLength, std::string privateKeyData, lua_sf::LuaIntegral<std::size_t> privateKeyLength, std::string privateKeyPassphrase, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.login(name, publicKeyData.c_str(), publicKeyLength.value(), privateKeyData.c_str(), privateKeyLength.value(), privateKeyPassphrase.c_str(), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            const auto* result = static_cast<const char*>("");
            return result ? std::string(result) : std::string{};
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[6]
    );
    lua_glue::BindCallable(type_sf__Sftp, "login",
        [](sf::Sftp& self, std::string name, std::string publicKeyData, std::string privateKeyData, std::string privateKeyPassphrase, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.login(name, publicKeyData, privateKeyData, privateKeyPassphrase, timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::string(std::string_view{ });
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[7]
    );
    lua_glue::BindCallable(type_sf__Sftp, "login",
        [](sf::Sftp& self, std::string name, std::string password, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.login(name, password, timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[5]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Sftp", "resolvePath", "fun(self: sf.Sftp, path: string, timeout?: sf.TimeoutWithPredicate): sf.Sftp.PathResult");
    lua_glue::BindCallable(type_sf__Sftp, "resolvePath",
        [](sf::Sftp& self, std::string path, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::PathResult {
            return self.resolvePath(std::filesystem::path(path), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Sftp", "getWorkingDirectory", "fun(self: sf.Sftp, timeout?: sf.TimeoutWithPredicate): sf.Sftp.PathResult");
    lua_glue::BindCallable(type_sf__Sftp, "getWorkingDirectory",
        [](sf::Sftp& self, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::PathResult {
            return self.getWorkingDirectory(timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Sftp", "getAttributes", "fun(self: sf.Sftp, path: string, followLinks?: boolean, timeout?: sf.TimeoutWithPredicate): sf.Sftp.AttributesResult");
    lua_glue::BindCallable(type_sf__Sftp, "getAttributes",
        [](sf::Sftp& self, std::string path, bool followLinks, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::AttributesResult {
            return self.getAttributes(std::filesystem::path(path), followLinks, timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(true);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Sftp", "getDirectoryListing", "fun(self: sf.Sftp, path: string, timeout?: sf.TimeoutWithPredicate): sf.Sftp.ListingResult");
    lua_glue::BindCallable(type_sf__Sftp, "getDirectoryListing",
        [](sf::Sftp& self, std::string path, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::ListingResult {
            return self.getDirectoryListing(std::filesystem::path(path), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Sftp", "createDirectory", "fun(self: sf.Sftp, path: string, permissions?: any, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp, "createDirectory",
        [](sf::Sftp& self, std::string path, std::filesystem::perms permissions, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.createDirectory(std::filesystem::path(path), permissions, timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<std::filesystem::perms>((std::filesystem::perms::owner_all | std::filesystem::perms::group_read | std::filesystem::perms::group_exec | std::filesystem::perms::others_read | std::filesystem::perms::others_exec));
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Sftp", "deleteDirectory", "fun(self: sf.Sftp, path: string, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp, "deleteDirectory",
        [](sf::Sftp& self, std::string path, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.deleteDirectory(std::filesystem::path(path), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Sftp", "rename", "fun(self: sf.Sftp, oldPath: string, newPath: string, overwrite?: boolean, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp, "rename",
        [](sf::Sftp& self, std::string oldPath, std::string newPath, bool overwrite, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.rename(std::filesystem::path(oldPath), std::filesystem::path(newPath), overwrite, timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Sftp", "deleteFile", "fun(self: sf.Sftp, path: string, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp, "deleteFile",
        [](sf::Sftp& self, std::string path, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.deleteFile(std::filesystem::path(path), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Sftp", "download", "fun(self: sf.Sftp, remotePath: string, callback: fun(data: string, size: integer): boolean, offset?: integer, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp, "download",
        [](sf::Sftp& self, std::string remotePath, lua_glue::Object callback, lua_sf::LuaIntegral<std::uint64_t> offset, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.download(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(const void*, std::size_t)>, lua_sf::callback::SftpDownloadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::download.callback", false}), offset.value(), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned long long>(0);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Sftp", "upload", "fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions?: any, truncate?: boolean, append?: boolean, offset?: integer, timeout?: sf.TimeoutWithPredicate): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp, "upload",
        [](sf::Sftp& self, std::string remotePath, lua_glue::Object callback, std::filesystem::perms permissions, bool truncate, bool append, lua_sf::LuaIntegral<std::uint64_t> offset, const sf::TimeoutWithPredicate& timeout) -> sf::Sftp::Result {
            return self.upload(std::filesystem::path(remotePath), lua_sf::callback::from_object<std::function<bool(void*, std::size_t&)>, lua_sf::callback::SftpUploadBufferCodec>(callback, lua_sf::callback::CallbackOptions{"sf::Sftp::upload.callback", false}), permissions, truncate, append, offset.value(), timeout);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<std::filesystem::perms>((std::filesystem::perms::owner_read | std::filesystem::perms::owner_write | std::filesystem::perms::group_read | std::filesystem::perms::others_read));
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(true);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned long long>(0);
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::TimeoutWithPredicate>(Time::Zero);
        }}},
        docs[17]
    );
    auto type_sf__Sftp__Result = lua_glue::BindClass<sf::Sftp::Result>(table_sf__Sftp, "Result");
    lua_glue::Table table_sf__Sftp__Result = table_sf__Sftp["Result"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Sftp::Result>(lua);
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_CLASS("sf.Sftp.Result");
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Sftp.Result", "new", "fun(value: sf.Sftp.Result.Value, message?: string): sf.Sftp.Result");
    lua_glue::BindCallable(type_sf__Sftp__Result, "new",
        [](sf::Sftp::Result::Value value, std::string message) {
            return lua_sf::makeLuaSharedObject<sf::Sftp::Result>(value, message);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return std::string(static_cast<std::string>(""));
        }}},
        docs[19]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Sftp.Result", "isOk", "fun(self: sf.Sftp.Result): boolean");
    lua_glue::BindCallable(type_sf__Sftp__Result, "isOk",
        [](const sf::Sftp::Result& self) -> bool {
            return self.isOk();
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Sftp.Result", "getValue", "fun(self: sf.Sftp.Result): sf.Sftp.Result.Value");
    lua_glue::BindCallable(type_sf__Sftp__Result, "getValue",
        [](const sf::Sftp::Result& self) -> sf::Sftp::Result::Value {
            return self.getValue();
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Sftp.Result", "getMessage", "fun(self: sf.Sftp.Result): string");
    lua_glue::BindCallable(type_sf__Sftp__Result, "getMessage",
        [](const sf::Sftp::Result& self) -> std::string {
            return std::string(self.getMessage());
        },
        docs[22],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_CLASS("sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FIELD("Success", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FIELD("Disconnected", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FIELD("Timeout", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FIELD("Refused", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FIELD("Error", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_FIELD("BannerReceive", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FIELD("BannerSend", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FIELD("InvalidMac", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FIELD("AllocationFailure", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FIELD("SocketSend", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FIELD("KeyExchangeFailure", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FIELD("HostKeyInitialization", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FIELD("HostKeySign", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[37]);
    LUASF_STUB_FIELD("DecryptError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FIELD("ProtocolError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FIELD("PasswordExpired", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[40]);
    LUASF_STUB_FIELD("FileError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[41]);
    LUASF_STUB_FIELD("MethodNone", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[42]);
    LUASF_STUB_FIELD("AuthenticationFailed", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[43]);
    LUASF_STUB_FIELD("PublicKeyUnverified", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[44]);
    LUASF_STUB_FIELD("ChannelOutOfOrder", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[45]);
    LUASF_STUB_FIELD("ChannelFailure", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[46]);
    LUASF_STUB_FIELD("ChannelRequestDenied", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[47]);
    LUASF_STUB_FIELD("ChannelUnknown", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[48]);
    LUASF_STUB_FIELD("ChannelWindowExceeded", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[49]);
    LUASF_STUB_FIELD("ChannelPacketExceeded", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[50]);
    LUASF_STUB_FIELD("ChannelClosed", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[51]);
    LUASF_STUB_FIELD("ChannelEofSent", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[52]);
    LUASF_STUB_FIELD("ScpProtocol", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[53]);
    LUASF_STUB_FIELD("ZlibError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[54]);
    LUASF_STUB_FIELD("RequestDenied", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[55]);
    LUASF_STUB_FIELD("MethodNotSupported", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[56]);
    LUASF_STUB_FIELD("InvalidData", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[57]);
    LUASF_STUB_FIELD("PublicKeyProtocol", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[58]);
    LUASF_STUB_FIELD("BufferTooSmall", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[59]);
    LUASF_STUB_FIELD("BadUse", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[60]);
    LUASF_STUB_FIELD("CompressError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[61]);
    LUASF_STUB_FIELD("OutOfBoundary", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[62]);
    LUASF_STUB_FIELD("AgentProtocol", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[63]);
    LUASF_STUB_FIELD("SocketRecv", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[64]);
    LUASF_STUB_FIELD("EncryptError", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[65]);
    LUASF_STUB_FIELD("BadSocket", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[66]);
    LUASF_STUB_FIELD("KnownHosts", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[67]);
    LUASF_STUB_FIELD("ChannelWindowFull", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[68]);
    LUASF_STUB_FIELD("KeyFileAuthenticationFailed", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[69]);
    LUASF_STUB_FIELD("EndOfFile", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[70]);
    LUASF_STUB_FIELD("NoSuchFile", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[71]);
    LUASF_STUB_FIELD("PermissionDenied", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[72]);
    LUASF_STUB_FIELD("Failure", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[73]);
    LUASF_STUB_FIELD("BadMessage", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[74]);
    LUASF_STUB_FIELD("NoConnection", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[75]);
    LUASF_STUB_FIELD("ConnectionLost", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[76]);
    LUASF_STUB_FIELD("OperationUnsupported", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[77]);
    LUASF_STUB_FIELD("InvalidHandle", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[78]);
    LUASF_STUB_FIELD("NoSuchPath", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[79]);
    LUASF_STUB_FIELD("FileAlreadyExists", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[80]);
    LUASF_STUB_FIELD("WriteProtect", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[81]);
    LUASF_STUB_FIELD("NoMedia", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[82]);
    LUASF_STUB_FIELD("NoSpaceOnFileSystem", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[83]);
    LUASF_STUB_FIELD("QuotaExceeded", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[84]);
    LUASF_STUB_FIELD("UnknownPrincipal", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[85]);
    LUASF_STUB_FIELD("LockConflict", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[86]);
    LUASF_STUB_FIELD("DirectoryNotEmpty", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[87]);
    LUASF_STUB_FIELD("NotADirectory", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[88]);
    LUASF_STUB_FIELD("InvalidFilename", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[89]);
    LUASF_STUB_FIELD("LinkLoop", "sf.Sftp.Result.Value");
    LUASF_STUB_DOC(docs[90]);
    LUASF_STUB_FIELD("SftpError", "sf.Sftp.Result.Value");
    lua_glue::BindEnum<sf::Sftp::Result::Value>(table_sf__Sftp__Result, "Value", {
        {"Success", sf::Sftp::Result::Value::Success},
        {"Disconnected", sf::Sftp::Result::Value::Disconnected},
        {"Timeout", sf::Sftp::Result::Value::Timeout},
        {"Refused", sf::Sftp::Result::Value::Refused},
        {"Error", sf::Sftp::Result::Value::Error},
        {"BannerReceive", sf::Sftp::Result::Value::BannerReceive},
        {"BannerSend", sf::Sftp::Result::Value::BannerSend},
        {"InvalidMac", sf::Sftp::Result::Value::InvalidMac},
        {"AllocationFailure", sf::Sftp::Result::Value::AllocationFailure},
        {"SocketSend", sf::Sftp::Result::Value::SocketSend},
        {"KeyExchangeFailure", sf::Sftp::Result::Value::KeyExchangeFailure},
        {"HostKeyInitialization", sf::Sftp::Result::Value::HostKeyInitialization},
        {"HostKeySign", sf::Sftp::Result::Value::HostKeySign},
        {"DecryptError", sf::Sftp::Result::Value::DecryptError},
        {"ProtocolError", sf::Sftp::Result::Value::ProtocolError},
        {"PasswordExpired", sf::Sftp::Result::Value::PasswordExpired},
        {"FileError", sf::Sftp::Result::Value::FileError},
        {"MethodNone", sf::Sftp::Result::Value::MethodNone},
        {"AuthenticationFailed", sf::Sftp::Result::Value::AuthenticationFailed},
        {"PublicKeyUnverified", sf::Sftp::Result::Value::PublicKeyUnverified},
        {"ChannelOutOfOrder", sf::Sftp::Result::Value::ChannelOutOfOrder},
        {"ChannelFailure", sf::Sftp::Result::Value::ChannelFailure},
        {"ChannelRequestDenied", sf::Sftp::Result::Value::ChannelRequestDenied},
        {"ChannelUnknown", sf::Sftp::Result::Value::ChannelUnknown},
        {"ChannelWindowExceeded", sf::Sftp::Result::Value::ChannelWindowExceeded},
        {"ChannelPacketExceeded", sf::Sftp::Result::Value::ChannelPacketExceeded},
        {"ChannelClosed", sf::Sftp::Result::Value::ChannelClosed},
        {"ChannelEofSent", sf::Sftp::Result::Value::ChannelEofSent},
        {"ScpProtocol", sf::Sftp::Result::Value::ScpProtocol},
        {"ZlibError", sf::Sftp::Result::Value::ZlibError},
        {"RequestDenied", sf::Sftp::Result::Value::RequestDenied},
        {"MethodNotSupported", sf::Sftp::Result::Value::MethodNotSupported},
        {"InvalidData", sf::Sftp::Result::Value::InvalidData},
        {"PublicKeyProtocol", sf::Sftp::Result::Value::PublicKeyProtocol},
        {"BufferTooSmall", sf::Sftp::Result::Value::BufferTooSmall},
        {"BadUse", sf::Sftp::Result::Value::BadUse},
        {"CompressError", sf::Sftp::Result::Value::CompressError},
        {"OutOfBoundary", sf::Sftp::Result::Value::OutOfBoundary},
        {"AgentProtocol", sf::Sftp::Result::Value::AgentProtocol},
        {"SocketRecv", sf::Sftp::Result::Value::SocketRecv},
        {"EncryptError", sf::Sftp::Result::Value::EncryptError},
        {"BadSocket", sf::Sftp::Result::Value::BadSocket},
        {"KnownHosts", sf::Sftp::Result::Value::KnownHosts},
        {"ChannelWindowFull", sf::Sftp::Result::Value::ChannelWindowFull},
        {"KeyFileAuthenticationFailed", sf::Sftp::Result::Value::KeyFileAuthenticationFailed},
        {"EndOfFile", sf::Sftp::Result::Value::EndOfFile},
        {"NoSuchFile", sf::Sftp::Result::Value::NoSuchFile},
        {"PermissionDenied", sf::Sftp::Result::Value::PermissionDenied},
        {"Failure", sf::Sftp::Result::Value::Failure},
        {"BadMessage", sf::Sftp::Result::Value::BadMessage},
        {"NoConnection", sf::Sftp::Result::Value::NoConnection},
        {"ConnectionLost", sf::Sftp::Result::Value::ConnectionLost},
        {"OperationUnsupported", sf::Sftp::Result::Value::OperationUnsupported},
        {"InvalidHandle", sf::Sftp::Result::Value::InvalidHandle},
        {"NoSuchPath", sf::Sftp::Result::Value::NoSuchPath},
        {"FileAlreadyExists", sf::Sftp::Result::Value::FileAlreadyExists},
        {"WriteProtect", sf::Sftp::Result::Value::WriteProtect},
        {"NoMedia", sf::Sftp::Result::Value::NoMedia},
        {"NoSpaceOnFileSystem", sf::Sftp::Result::Value::NoSpaceOnFileSystem},
        {"QuotaExceeded", sf::Sftp::Result::Value::QuotaExceeded},
        {"UnknownPrincipal", sf::Sftp::Result::Value::UnknownPrincipal},
        {"LockConflict", sf::Sftp::Result::Value::LockConflict},
        {"DirectoryNotEmpty", sf::Sftp::Result::Value::DirectoryNotEmpty},
        {"NotADirectory", sf::Sftp::Result::Value::NotADirectory},
        {"InvalidFilename", sf::Sftp::Result::Value::InvalidFilename},
        {"LinkLoop", sf::Sftp::Result::Value::LinkLoop},
        {"SftpError", sf::Sftp::Result::Value::SftpError}
    });
    auto type_sf__Sftp__PathResult = lua_glue::BindClass<sf::Sftp::PathResult>(table_sf__Sftp, "PathResult");
    lua_glue::BindBase<sf::Sftp::PathResult, sf::Sftp::Result>(type_sf__Sftp__PathResult);
    lua_glue::Table table_sf__Sftp__PathResult = table_sf__Sftp["PathResult"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Sftp::PathResult>(lua);
    lua_glue::Table native_bases_sf__Sftp__PathResult = lua.create_table();
    native_bases_sf__Sftp__PathResult.add(lua["sf"]["Sftp"]["Result"].get<lua_glue::Table>());
    table_sf__Sftp__PathResult.raw_set("__nativeBases", native_bases_sf__Sftp__PathResult);
    LUASF_STUB_DOC(docs[91]);
    LUASF_STUB_CLASS("sf.Sftp.PathResult", "sf.Sftp.Result");
    LUASF_STUB_DOC(docs[92]);
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "new", "fun(result: sf.Sftp.Result, path: string): sf.Sftp.PathResult");
    lua_glue::BindCallable(type_sf__Sftp__PathResult, "new",
        [](const sf::Sftp::Result& result, std::string path) {
            return lua_sf::makeLuaSharedObject<sf::Sftp::PathResult>(result, std::filesystem::path(path));
        },
        docs[92]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "isOk", "fun(self: sf.Sftp.PathResult): boolean");
    lua_glue::BindCallable(type_sf__Sftp__PathResult, "isOk",
        [](const sf::Sftp::PathResult& self) -> bool {
            return static_cast<const sf::Sftp::Result&>(self).isOk();
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "getValue", "fun(self: sf.Sftp.PathResult): sf.Sftp.Result.Value");
    lua_glue::BindCallable(type_sf__Sftp__PathResult, "getValue",
        [](const sf::Sftp::PathResult& self) -> sf::Sftp::Result::Value {
            return static_cast<const sf::Sftp::Result&>(self).getValue();
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "getMessage", "fun(self: sf.Sftp.PathResult): string");
    lua_glue::BindCallable(type_sf__Sftp__PathResult, "getMessage",
        [](const sf::Sftp::PathResult& self) -> std::string {
            return std::string(static_cast<const sf::Sftp::Result&>(self).getMessage());
        },
        docs[22],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[93]);
    LUASF_STUB_FUNCTION("sf.Sftp.PathResult", "getPath", "fun(self: sf.Sftp.PathResult): string");
    lua_glue::BindCallable(type_sf__Sftp__PathResult, "getPath",
        [](const sf::Sftp::PathResult& self) -> std::string {
            return (self.getPath()).string();
        },
        docs[93],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    auto type_sf__Sftp__Attributes = lua_glue::BindClass<sf::Sftp::Attributes>(table_sf__Sftp, "Attributes");
    lua_glue::Table table_sf__Sftp__Attributes = table_sf__Sftp["Attributes"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Sftp::Attributes>(lua);
    LUASF_STUB_DOC(docs[94]);
    LUASF_STUB_CLASS("sf.Sftp.Attributes");
    LUASF_STUB_DOC(docs[95]);
    LUASF_STUB_FIELD("path", "string");
    LUASF_STUB_DOC(docs[96]);
    LUASF_STUB_FIELD("type", "any|nil");
    LUASF_STUB_DOC(docs[97]);
    LUASF_STUB_FIELD("size", "integer|nil");
    LUASF_STUB_DOC(docs[98]);
    LUASF_STUB_FIELD("permissions", "any|nil");
    LUASF_STUB_DOC(docs[99]);
    LUASF_STUB_FIELD("userId", "integer|nil");
    LUASF_STUB_DOC(docs[100]);
    LUASF_STUB_FIELD("groupId", "integer|nil");
    LUASF_STUB_DOC(docs[101]);
    LUASF_STUB_FIELD("accessTime", "any|nil");
    LUASF_STUB_DOC(docs[102]);
    LUASF_STUB_FIELD("modificationTime", "any|nil");
    LUASF_STUB_FUNCTION("sf.Sftp.Attributes", "new", "fun(): sf.Sftp.Attributes");
    lua_glue::BindCallable(type_sf__Sftp__Attributes, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Sftp::Attributes>();
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__Attributes, "path",
        [](const sf::Sftp::Attributes& self) -> std::string {
            return (self.path).string();
        },
        [](sf::Sftp::Attributes& self, std::string value) {
            self.path = std::filesystem::path(value);
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__Attributes, "type",
        [lua](const sf::Sftp::Attributes& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.type);
        },
        [](sf::Sftp::Attributes& self, lua_glue::Object value) {
            auto value_optional = lua_sf::optional_from_object<std::filesystem::file_type>(value);
            self.type = value_optional;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__Attributes, "size",
        [lua](const sf::Sftp::Attributes& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.size);
        },
        [](sf::Sftp::Attributes& self, lua_glue::Object value) {
            auto value_optional = lua_sf::optional_from_object<std::uint64_t>(value);
            self.size = value_optional;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__Attributes, "permissions",
        [lua](const sf::Sftp::Attributes& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.permissions);
        },
        [](sf::Sftp::Attributes& self, lua_glue::Object value) {
            auto value_optional = lua_sf::optional_from_object<std::filesystem::perms>(value);
            self.permissions = value_optional;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__Attributes, "userId",
        [lua](const sf::Sftp::Attributes& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.userId);
        },
        [](sf::Sftp::Attributes& self, lua_glue::Object value) {
            auto value_optional = lua_sf::optional_from_object<std::uint64_t>(value);
            self.userId = value_optional;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__Attributes, "groupId",
        [lua](const sf::Sftp::Attributes& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.groupId);
        },
        [](sf::Sftp::Attributes& self, lua_glue::Object value) {
            auto value_optional = lua_sf::optional_from_object<std::uint64_t>(value);
            self.groupId = value_optional;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__Attributes, "accessTime",
        [lua](const sf::Sftp::Attributes& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.accessTime);
        },
        [](sf::Sftp::Attributes& self, lua_glue::Object value) {
            auto value_optional = lua_sf::optional_from_object<std::filesystem::file_time_type>(value);
            self.accessTime = value_optional;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__Attributes, "modificationTime",
        [lua](const sf::Sftp::Attributes& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.modificationTime);
        },
        [](sf::Sftp::Attributes& self, lua_glue::Object value) {
            auto value_optional = lua_sf::optional_from_object<std::filesystem::file_time_type>(value);
            self.modificationTime = value_optional;
        }
    );
    auto type_sf__Sftp__AttributesResult = lua_glue::BindClass<sf::Sftp::AttributesResult>(table_sf__Sftp, "AttributesResult");
    lua_glue::BindBase<sf::Sftp::AttributesResult, sf::Sftp::Result>(type_sf__Sftp__AttributesResult);
    lua_glue::Table table_sf__Sftp__AttributesResult = table_sf__Sftp["AttributesResult"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Sftp::AttributesResult>(lua);
    lua_glue::Table native_bases_sf__Sftp__AttributesResult = lua.create_table();
    native_bases_sf__Sftp__AttributesResult.add(lua["sf"]["Sftp"]["Result"].get<lua_glue::Table>());
    table_sf__Sftp__AttributesResult.raw_set("__nativeBases", native_bases_sf__Sftp__AttributesResult);
    LUASF_STUB_DOC(docs[103]);
    LUASF_STUB_CLASS("sf.Sftp.AttributesResult", "sf.Sftp.Result");
    LUASF_STUB_DOC(docs[104]);
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "new", "fun(result: sf.Sftp.Result, attributes: sf.Sftp.Attributes): sf.Sftp.AttributesResult");
    lua_glue::BindCallable(type_sf__Sftp__AttributesResult, "new",
        [](const sf::Sftp::Result& result, sf::Sftp::Attributes attributes) {
            return lua_sf::makeLuaSharedObject<sf::Sftp::AttributesResult>(result, attributes);
        },
        docs[104]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "isOk", "fun(self: sf.Sftp.AttributesResult): boolean");
    lua_glue::BindCallable(type_sf__Sftp__AttributesResult, "isOk",
        [](const sf::Sftp::AttributesResult& self) -> bool {
            return static_cast<const sf::Sftp::Result&>(self).isOk();
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "getValue", "fun(self: sf.Sftp.AttributesResult): sf.Sftp.Result.Value");
    lua_glue::BindCallable(type_sf__Sftp__AttributesResult, "getValue",
        [](const sf::Sftp::AttributesResult& self) -> sf::Sftp::Result::Value {
            return static_cast<const sf::Sftp::Result&>(self).getValue();
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "getMessage", "fun(self: sf.Sftp.AttributesResult): string");
    lua_glue::BindCallable(type_sf__Sftp__AttributesResult, "getMessage",
        [](const sf::Sftp::AttributesResult& self) -> std::string {
            return std::string(static_cast<const sf::Sftp::Result&>(self).getMessage());
        },
        docs[22],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[105]);
    LUASF_STUB_FUNCTION("sf.Sftp.AttributesResult", "getAttributes", "fun(self: sf.Sftp.AttributesResult): sf.Sftp.Attributes");
    lua_glue::BindCallable(type_sf__Sftp__AttributesResult, "getAttributes",
        [](const sf::Sftp::AttributesResult& self) {
            return std::cref(self.getAttributes());
        },
        docs[105],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    auto type_sf__Sftp__ListingResult = lua_glue::BindClass<sf::Sftp::ListingResult>(table_sf__Sftp, "ListingResult");
    lua_glue::BindBase<sf::Sftp::ListingResult, sf::Sftp::Result>(type_sf__Sftp__ListingResult);
    lua_glue::Table table_sf__Sftp__ListingResult = table_sf__Sftp["ListingResult"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Sftp::ListingResult>(lua);
    lua_glue::Table native_bases_sf__Sftp__ListingResult = lua.create_table();
    native_bases_sf__Sftp__ListingResult.add(lua["sf"]["Sftp"]["Result"].get<lua_glue::Table>());
    table_sf__Sftp__ListingResult.raw_set("__nativeBases", native_bases_sf__Sftp__ListingResult);
    LUASF_STUB_DOC(docs[106]);
    LUASF_STUB_CLASS("sf.Sftp.ListingResult", "sf.Sftp.Result");
    LUASF_STUB_DOC(docs[107]);
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "new", "fun(result: sf.Sftp.Result, listing: sf.Sftp.Attributes[]): sf.Sftp.ListingResult");
    lua_glue::BindCallable(type_sf__Sftp__ListingResult, "new",
        [](const sf::Sftp::Result& result, lua_glue::Table listing) {
            auto listing_vector = lua_sf::array_from_object<sf::Sftp::Attributes>(listing);
            return lua_sf::makeLuaSharedObject<sf::Sftp::ListingResult>(result, listing_vector);
        },
        docs[107]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "isOk", "fun(self: sf.Sftp.ListingResult): boolean");
    lua_glue::BindCallable(type_sf__Sftp__ListingResult, "isOk",
        [](const sf::Sftp::ListingResult& self) -> bool {
            return static_cast<const sf::Sftp::Result&>(self).isOk();
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "getValue", "fun(self: sf.Sftp.ListingResult): sf.Sftp.Result.Value");
    lua_glue::BindCallable(type_sf__Sftp__ListingResult, "getValue",
        [](const sf::Sftp::ListingResult& self) -> sf::Sftp::Result::Value {
            return static_cast<const sf::Sftp::Result&>(self).getValue();
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "getMessage", "fun(self: sf.Sftp.ListingResult): string");
    lua_glue::BindCallable(type_sf__Sftp__ListingResult, "getMessage",
        [](const sf::Sftp::ListingResult& self) -> std::string {
            return std::string(static_cast<const sf::Sftp::Result&>(self).getMessage());
        },
        docs[22],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[108]);
    LUASF_STUB_FUNCTION("sf.Sftp.ListingResult", "getListing", "fun(self: sf.Sftp.ListingResult): sf.Sftp.Attributes[]");
    lua_glue::BindCallable(type_sf__Sftp__ListingResult, "getListing",
        [lua](const sf::Sftp::ListingResult& self) -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, self.getListing());
        },
        docs[108],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    auto type_sf__Sftp__SessionInfo = lua_glue::BindClass<sf::Sftp::SessionInfo>(table_sf__Sftp, "SessionInfo");
    lua_glue::Table table_sf__Sftp__SessionInfo = table_sf__Sftp["SessionInfo"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Sftp::SessionInfo>(lua);
    LUASF_STUB_DOC(docs[109]);
    LUASF_STUB_CLASS("sf.Sftp.SessionInfo");
    LUASF_STUB_DOC(docs[110]);
    LUASF_STUB_FIELD("hostKey", "sf.Sftp.SessionInfo.HostKey");
    LUASF_STUB_DOC(docs[111]);
    LUASF_STUB_FIELD("keyExchangeAlgorithm", "string");
    LUASF_STUB_DOC(docs[112]);
    LUASF_STUB_FIELD("hostKeyAlgorithm", "string");
    LUASF_STUB_DOC(docs[113]);
    LUASF_STUB_FIELD("clientToServerEncryptionAlgorithm", "string");
    LUASF_STUB_DOC(docs[114]);
    LUASF_STUB_FIELD("serverToClientEncryptionAlgorithm", "string");
    LUASF_STUB_DOC(docs[115]);
    LUASF_STUB_FIELD("clientToServerMacAlgorithm", "string");
    LUASF_STUB_DOC(docs[116]);
    LUASF_STUB_FIELD("serverToClientMacAlgorithm", "string");
    LUASF_STUB_DOC(docs[117]);
    LUASF_STUB_FIELD("clientToServerCompressionAlgorithm", "string");
    LUASF_STUB_DOC(docs[118]);
    LUASF_STUB_FIELD("serverToClientCompressionAlgorithm", "string");
    LUASF_STUB_FUNCTION("sf.Sftp.SessionInfo", "new", "fun(): sf.Sftp.SessionInfo");
    lua_glue::BindCallable(type_sf__Sftp__SessionInfo, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Sftp::SessionInfo>();
        }
    );
    lua_glue::BindAttr<sf::Sftp::SessionInfo::HostKey>(type_sf__Sftp__SessionInfo, "hostKey", &sf::Sftp::SessionInfo::hostKey);
    lua_glue::BindProperty(type_sf__Sftp__SessionInfo, "keyExchangeAlgorithm",
        [](const sf::Sftp::SessionInfo& self) {
            return std::string(self.keyExchangeAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.keyExchangeAlgorithm = value;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__SessionInfo, "hostKeyAlgorithm",
        [](const sf::Sftp::SessionInfo& self) {
            return std::string(self.hostKeyAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.hostKeyAlgorithm = value;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__SessionInfo, "clientToServerEncryptionAlgorithm",
        [](const sf::Sftp::SessionInfo& self) {
            return std::string(self.clientToServerEncryptionAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.clientToServerEncryptionAlgorithm = value;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__SessionInfo, "serverToClientEncryptionAlgorithm",
        [](const sf::Sftp::SessionInfo& self) {
            return std::string(self.serverToClientEncryptionAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.serverToClientEncryptionAlgorithm = value;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__SessionInfo, "clientToServerMacAlgorithm",
        [](const sf::Sftp::SessionInfo& self) {
            return std::string(self.clientToServerMacAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.clientToServerMacAlgorithm = value;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__SessionInfo, "serverToClientMacAlgorithm",
        [](const sf::Sftp::SessionInfo& self) {
            return std::string(self.serverToClientMacAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.serverToClientMacAlgorithm = value;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__SessionInfo, "clientToServerCompressionAlgorithm",
        [](const sf::Sftp::SessionInfo& self) {
            return std::string(self.clientToServerCompressionAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.clientToServerCompressionAlgorithm = value;
        }
    );
    lua_glue::BindProperty(type_sf__Sftp__SessionInfo, "serverToClientCompressionAlgorithm",
        [](const sf::Sftp::SessionInfo& self) {
            return std::string(self.serverToClientCompressionAlgorithm);
        },
        [](sf::Sftp::SessionInfo& self, std::string value) {
            self.serverToClientCompressionAlgorithm = value;
        }
    );
    auto type_sf__Sftp__SessionInfo__HostKey = lua_glue::BindClass<sf::Sftp::SessionInfo::HostKey>(table_sf__Sftp__SessionInfo, "HostKey");
    lua_glue::Table table_sf__Sftp__SessionInfo__HostKey = table_sf__Sftp__SessionInfo["HostKey"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Sftp::SessionInfo::HostKey>(lua);
    LUASF_STUB_DOC(docs[119]);
    LUASF_STUB_CLASS("sf.Sftp.SessionInfo.HostKey");
    LUASF_STUB_DOC(docs[120]);
    LUASF_STUB_FIELD("type", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC(docs[121]);
    LUASF_STUB_FIELD("data", "any[]");
    LUASF_STUB_DOC(docs[122]);
    LUASF_STUB_FIELD("sha1", "any");
    LUASF_STUB_DOC(docs[123]);
    LUASF_STUB_FIELD("sha256", "any");
    LUASF_STUB_FUNCTION("sf.Sftp.SessionInfo.HostKey", "new", "fun(): sf.Sftp.SessionInfo.HostKey");
    lua_glue::BindCallable(type_sf__Sftp__SessionInfo__HostKey, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Sftp::SessionInfo::HostKey>();
        }
    );
    lua_glue::BindAttr<sf::Sftp::SessionInfo::HostKey::Type>(type_sf__Sftp__SessionInfo__HostKey, "type", &sf::Sftp::SessionInfo::HostKey::type);
    lua_glue::BindProperty(type_sf__Sftp__SessionInfo__HostKey, "data",
        [lua](const sf::Sftp::SessionInfo::HostKey& self) {
            return lua_sf::vector_to_object(lua, self.data);
        },
        [](sf::Sftp::SessionInfo::HostKey& self, lua_glue::Table value) {
            auto value_vector = lua_sf::array_from_object<std::byte>(value);
            self.data = value_vector;
        }
    );
    lua_glue::BindAttr<std::array<std::byte, 20>>(type_sf__Sftp__SessionInfo__HostKey, "sha1", &sf::Sftp::SessionInfo::HostKey::sha1);
    lua_glue::BindAttr<std::array<std::byte, 32>>(type_sf__Sftp__SessionInfo__HostKey, "sha256", &sf::Sftp::SessionInfo::HostKey::sha256);
    LUASF_STUB_CLASS("sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC(docs[124]);
    LUASF_STUB_FIELD("Unknown", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC(docs[125]);
    LUASF_STUB_FIELD("Rsa", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC(docs[126]);
    LUASF_STUB_FIELD("Dsa", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC(docs[127]);
    LUASF_STUB_FIELD("Ecdsa256", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC(docs[128]);
    LUASF_STUB_FIELD("Ecdsa384", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC(docs[129]);
    LUASF_STUB_FIELD("Ecdsa521", "sf.Sftp.SessionInfo.HostKey.Type");
    LUASF_STUB_DOC(docs[130]);
    LUASF_STUB_FIELD("Ed25519", "sf.Sftp.SessionInfo.HostKey.Type");
    lua_glue::BindEnum<sf::Sftp::SessionInfo::HostKey::Type>(table_sf__Sftp__SessionInfo__HostKey, "Type", {
        {"Unknown", sf::Sftp::SessionInfo::HostKey::Type::Unknown},
        {"Rsa", sf::Sftp::SessionInfo::HostKey::Type::Rsa},
        {"Dsa", sf::Sftp::SessionInfo::HostKey::Type::Dsa},
        {"Ecdsa256", sf::Sftp::SessionInfo::HostKey::Type::Ecdsa256},
        {"Ecdsa384", sf::Sftp::SessionInfo::HostKey::Type::Ecdsa384},
        {"Ecdsa521", sf::Sftp::SessionInfo::HostKey::Type::Ecdsa521},
        {"Ed25519", sf::Sftp::SessionInfo::HostKey::Type::Ed25519}
    });
}
