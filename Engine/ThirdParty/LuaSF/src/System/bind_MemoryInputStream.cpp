#include "System/bind_MemoryInputStream.hpp"

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

namespace { constexpr std::array<std::string_view, 6> docs = {
    "\\brief Implementation of input stream based on a memory chunk",
    "\\brief Construct the stream from its data\n\n\\param data        Pointer to the data in memory\n\\param sizeInBytes Size of the data, in bytes",
    "\\brief Read data from the stream\n\nAfter reading, the stream's reading position must be\nadvanced by the amount of bytes read.\n\n\\param data Buffer where to copy the read data\n\\param size Desired number of bytes to read\n\n\\return The number of bytes actually read, or `std::nullopt` on error",
    "\\brief Change the current reading position\n\n\\param position The position to seek to, from the beginning\n\n\\return The position actually sought to, or `std::nullopt` on error",
    "\\brief Get the current reading position in the stream\n\n\\return The current position, or `std::nullopt` on error.",
    "\\brief Return the size of the stream\n\n\\return The total number of bytes available in the stream, or `std::nullopt` on error",
}; }

void bind_MemoryInputStream(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__MemoryInputStream = lua_glue::BindClass<sf::MemoryInputStream>(sf, "MemoryInputStream");
    lua_glue::BindBase<sf::MemoryInputStream, sf::InputStream>(type_sf__MemoryInputStream);
    lua_glue::Table table_sf__MemoryInputStream = sf["MemoryInputStream"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::MemoryInputStream>(lua);
    lua_glue::Table native_bases_sf__MemoryInputStream = lua.create_table();
    native_bases_sf__MemoryInputStream.add(lua["sf"]["InputStream"].get<lua_glue::Table>());
    table_sf__MemoryInputStream.raw_set("__nativeBases", native_bases_sf__MemoryInputStream);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.MemoryInputStream", "sf.InputStream");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "new", "fun(data: any): sf.MemoryInputStream");
    lua_glue::BindCallable(type_sf__MemoryInputStream, "new",
        [](lua_glue::Object data) {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            auto object = lua_sf::makeLongLivedMemoryObject<sf::MemoryInputStream>(
                data_buffer->data(),
                static_cast<std::size_t>(data_buffer->size()));
            lua_sf::rememberLongLivedMemory(*object, std::move(data_buffer));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "read", "fun(self: sf.MemoryInputStream, size: integer): integer|nil, any");
    lua_glue::BindCallable(type_sf__MemoryInputStream, "read",
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
            return std::make_tuple(lua_sf::optional_to_object(lua, result), lua_glue::AsTable(data_buffer));
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "seek", "fun(self: sf.MemoryInputStream, position: integer): integer|nil");
    lua_glue::BindCallable(type_sf__MemoryInputStream, "seek",
        [lua](sf::MemoryInputStream& self, lua_sf::LuaIntegral<std::size_t> position) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.seek(position.value()));
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "tell", "fun(self: sf.MemoryInputStream): integer|nil");
    lua_glue::BindCallable(type_sf__MemoryInputStream, "tell",
        [lua](sf::MemoryInputStream& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.tell());
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.MemoryInputStream", "getSize", "fun(self: sf.MemoryInputStream): integer|nil");
    lua_glue::BindCallable(type_sf__MemoryInputStream, "getSize",
        [lua](sf::MemoryInputStream& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.getSize());
        },
        docs[5]
    );
}
