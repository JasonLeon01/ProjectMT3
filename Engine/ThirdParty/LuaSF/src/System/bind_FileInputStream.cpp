#include "System/bind_FileInputStream.hpp"

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

namespace { constexpr std::array<std::string_view, 8> docs = {
    "\\brief Implementation of input stream based on a file",
    "\\brief Default constructor\n\nConstruct a file input stream that is not associated\nwith a file to read.",
    "\\brief Construct the stream from a file path\n\n\\param filename Name of the file to open\n\n\\throws sf::Exception on error",
    "\\brief Read data from the stream\n\nAfter reading, the stream's reading position must be\nadvanced by the amount of bytes read.\n\n\\param data Buffer where to copy the read data\n\\param size Desired number of bytes to read\n\n\\return The number of bytes actually read, or `std::nullopt` on error",
    "\\brief Change the current reading position\n\n\\param position The position to seek to, from the beginning\n\n\\return The position actually sought to, or `std::nullopt` on error",
    "\\brief Get the current reading position in the stream\n\n\\return The current position, or `std::nullopt` on error.",
    "\\brief Return the size of the stream\n\n\\return The total number of bytes available in the stream, or `std::nullopt` on error",
    "\\brief Open the stream from a file path\n\nOn OpenHarmony/HarmonyOS, a path beginning with `rawfile:/`\nexplicitly addresses the HAP rawfile directory. A relative path is\nfirst opened from the application filesystem and then, if that fails,\nfrom rawfile. `SFML::Main` initializes the native resource manager\nbefore application code starts.\nOn Android, paths are first opened from the application filesystem.\nRelative paths fall back to the packaged asset directory when no\nfilesystem file exists.\n\n\\param filename Name of the file to open\n\n\\return `true` on success, `false` on error",
}; }

void bind_FileInputStream(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__FileInputStream = lua_glue::BindClass<sf::FileInputStream>(sf, "FileInputStream");
    lua_glue::BindBase<sf::FileInputStream, sf::InputStream>(type_sf__FileInputStream);
    lua_glue::Table table_sf__FileInputStream = sf["FileInputStream"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::FileInputStream>(lua);
    lua_glue::Table native_bases_sf__FileInputStream = lua.create_table();
    native_bases_sf__FileInputStream.add(lua["sf"]["InputStream"].get<lua_glue::Table>());
    table_sf__FileInputStream.raw_set("__nativeBases", native_bases_sf__FileInputStream);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.FileInputStream", "sf.InputStream");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.FileInputStream", "new", "fun(filename: string): sf.FileInputStream");
    LUASF_STUB_OVERLOAD("sf.FileInputStream", "new", "fun(): sf.FileInputStream");
    lua_glue::BindCallable(type_sf__FileInputStream, "new",
        [](std::string filename) {
            return lua_sf::makeLuaSharedObject<sf::FileInputStream>(std::filesystem::path(filename));
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__FileInputStream, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::FileInputStream>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.FileInputStream", "read", "fun(self: sf.FileInputStream, size: integer): integer|nil, any");
    lua_glue::BindCallable(type_sf__FileInputStream, "read",
        [lua](sf::FileInputStream& self, std::size_t size) {
            std::vector<std::byte> data_buffer(size);
            auto result = self.read(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
            if (result)
            {
                const auto data_buffer_written = static_cast<std::size_t>(*result);
                if (data_buffer_written < data_buffer.size())
                    data_buffer.resize(data_buffer_written);
            }
            else
            {
                data_buffer.clear();
            }
            return std::make_tuple(lua_sf::optional_to_object(lua, result), lua_glue::AsTable(data_buffer));
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.FileInputStream", "seek", "fun(self: sf.FileInputStream, position: integer): integer|nil");
    lua_glue::BindCallable(type_sf__FileInputStream, "seek",
        [lua](sf::FileInputStream& self, lua_sf::LuaIntegral<std::size_t> position) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.seek(position.value()));
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.FileInputStream", "tell", "fun(self: sf.FileInputStream): integer|nil");
    lua_glue::BindCallable(type_sf__FileInputStream, "tell",
        [lua](sf::FileInputStream& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.tell());
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.FileInputStream", "getSize", "fun(self: sf.FileInputStream): integer|nil");
    lua_glue::BindCallable(type_sf__FileInputStream, "getSize",
        [lua](sf::FileInputStream& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.getSize());
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.FileInputStream", "open", "fun(self: sf.FileInputStream, filename: string): boolean");
    lua_glue::BindCallable(type_sf__FileInputStream, "open",
        [](sf::FileInputStream& self, std::string filename) -> bool {
            return self.open(std::filesystem::path(filename));
        },
        docs[7]
    );
}
