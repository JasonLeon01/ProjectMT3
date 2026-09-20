#include "System/bind_FileInputStream.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_FileInputStream(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__FileInputStream = sf.new_usertype<sf::FileInputStream>("FileInputStream",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::InputStream>()
    );
    sol::table table_sf__FileInputStream = sf["FileInputStream"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::FileInputStream>(lua);
    sol::table native_bases_sf__FileInputStream = lua.create_table();
    native_bases_sf__FileInputStream.add(lua["sf"]["InputStream"].get<sol::table>());
    table_sf__FileInputStream.raw_set("__nativeBases", native_bases_sf__FileInputStream);
    LUASF_STUB_DOC("\\brief Implementation of input stream based on a file");
    LUASF_STUB_CLASS("sf.FileInputStream", "sf.InputStream");
    LUASF_STUB_DOC("\\brief Construct the stream from a file path\n\n\\param filename Name of the file to open\n\n\\throws sf::Exception on error");
    LUASF_STUB_FUNCTION("sf.FileInputStream", "new", "fun(filename: string): sf.FileInputStream");
    LUASF_STUB_OVERLOAD("sf.FileInputStream", "new", "fun(): sf.FileInputStream");
    type_sf__FileInputStream.set_function("new", sol::factories(
        [](std::string filename) {
            return lua_sf::makeLuaSharedObject<sf::FileInputStream>(std::filesystem::path(filename));
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::FileInputStream>();
        }
    ));
    LUASF_STUB_DOC("\\brief Read data from the stream\n\nAfter reading, the stream's reading position must be\nadvanced by the amount of bytes read.\n\n\\param data Buffer where to copy the read data\n\\param size Desired number of bytes to read\n\n\\return The number of bytes actually read, or `std::nullopt` on error");
    LUASF_STUB_FUNCTION("sf.FileInputStream", "read", "fun(self: sf.FileInputStream, size: integer): integer|nil, any");
    type_sf__FileInputStream.set_function("read",
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
            return std::make_tuple(lua_sf::optional_to_object(lua, result), sol::as_table(data_buffer));
        }
    );
    LUASF_STUB_DOC("\\brief Change the current reading position\n\n\\param position The position to seek to, from the beginning\n\n\\return The position actually sought to, or `std::nullopt` on error");
    LUASF_STUB_FUNCTION("sf.FileInputStream", "seek", "fun(self: sf.FileInputStream, position: integer): integer|nil");
    type_sf__FileInputStream.set_function("seek",
        [lua](sf::FileInputStream& self, lua_sf::LuaIntegral<std::size_t> position) -> sol::object {
            return lua_sf::optional_to_object(lua, self.seek(position.value()));
        }
    );
    LUASF_STUB_DOC("\\brief Get the current reading position in the stream\n\n\\return The current position, or `std::nullopt` on error.");
    LUASF_STUB_FUNCTION("sf.FileInputStream", "tell", "fun(self: sf.FileInputStream): integer|nil");
    type_sf__FileInputStream.set_function("tell",
        [lua](sf::FileInputStream& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.tell());
        }
    );
    LUASF_STUB_DOC("\\brief Return the size of the stream\n\n\\return The total number of bytes available in the stream, or `std::nullopt` on error");
    LUASF_STUB_FUNCTION("sf.FileInputStream", "getSize", "fun(self: sf.FileInputStream): integer|nil");
    type_sf__FileInputStream.set_function("getSize",
        [lua](sf::FileInputStream& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.getSize());
        }
    );
    LUASF_STUB_DOC("\\brief Open the stream from a file path\n\nOn Android, paths are first opened from the application filesystem.\nRelative paths fall back to the packaged asset directory when no\nfilesystem file exists.\n\n\\param filename Name of the file to open\n\n\\return `true` on success, `false` on error");
    LUASF_STUB_FUNCTION("sf.FileInputStream", "open", "fun(self: sf.FileInputStream, filename: string): boolean");
    type_sf__FileInputStream.set_function("open",
        [](sf::FileInputStream& self, std::string filename) -> bool {
            return self.open(std::filesystem::path(filename));
        }
    );
}
