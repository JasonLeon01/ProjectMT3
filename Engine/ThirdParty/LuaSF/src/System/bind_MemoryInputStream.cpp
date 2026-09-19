#include "System/bind_MemoryInputStream.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_MemoryInputStream(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__MemoryInputStream = sf.new_usertype<sf::MemoryInputStream>("MemoryInputStream",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::InputStream>()
    );
    sol::table table_sf__MemoryInputStream = sf["MemoryInputStream"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::MemoryInputStream>(lua);
    sol::table native_bases_sf__MemoryInputStream = lua.create_table();
    native_bases_sf__MemoryInputStream.add(lua["sf"]["InputStream"].get<sol::table>());
    table_sf__MemoryInputStream.raw_set("__nativeBases", native_bases_sf__MemoryInputStream);
    LUASF_STUB_DOC("\\brief Implementation of input stream based on a memory chunk");
    LUASF_STUB_CLASS("sf.MemoryInputStream", "sf.InputStream");
    LUASF_STUB_DOC("\\brief Construct the stream from its data\n\n\\param data        Pointer to the data in memory\n\\param sizeInBytes Size of the data, in bytes");
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "new", "fun(data: any): sf.MemoryInputStream");
    type_sf__MemoryInputStream.set_function("new", sol::factories(
        [](sol::object data) {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            auto object = lua_sf::makeLongLivedMemoryObject<sf::MemoryInputStream>(
                data_buffer->data(),
                static_cast<std::size_t>(data_buffer->size()));
            lua_sf::rememberLongLivedMemory(*object, std::move(data_buffer));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        }
    ));
    LUASF_STUB_DOC("\\brief Read data from the stream\n\nAfter reading, the stream's reading position must be\nadvanced by the amount of bytes read.\n\n\\param data Buffer where to copy the read data\n\\param size Desired number of bytes to read\n\n\\return The number of bytes actually read, or `std::nullopt` on error");
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "read", "fun(self: sf.MemoryInputStream, size: integer): integer|nil, any");
    type_sf__MemoryInputStream.set_function("read",
        [lua](sf::MemoryInputStream& self, std::size_t size) {
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
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "seek", "fun(self: sf.MemoryInputStream, position: integer): integer|nil");
    type_sf__MemoryInputStream.set_function("seek",
        [lua](sf::MemoryInputStream& self, lua_sf::LuaIntegral<std::size_t> position) -> sol::object {
            return lua_sf::optional_to_object(lua, self.seek(position.value()));
        }
    );
    LUASF_STUB_DOC("\\brief Get the current reading position in the stream\n\n\\return The current position, or `std::nullopt` on error.");
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "tell", "fun(self: sf.MemoryInputStream): integer|nil");
    type_sf__MemoryInputStream.set_function("tell",
        [lua](sf::MemoryInputStream& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.tell());
        }
    );
    LUASF_STUB_DOC("\\brief Return the size of the stream\n\n\\return The total number of bytes available in the stream, or `std::nullopt` on error");
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "getSize", "fun(self: sf.MemoryInputStream): integer|nil");
    type_sf__MemoryInputStream.set_function("getSize",
        [lua](sf::MemoryInputStream& self) -> sol::object {
            return lua_sf::optional_to_object(lua, self.getSize());
        }
    );
}
