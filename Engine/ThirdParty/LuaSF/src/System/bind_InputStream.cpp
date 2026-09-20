#include "System/bind_InputStream.hpp"

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

namespace { constexpr std::array<std::string_view, 5> docs = {
    "\\brief Abstract class for custom file input streams",
    "\\brief Read data from the stream\n\nAfter reading, the stream's reading position must be\nadvanced by the amount of bytes read.\n\n\\param data Buffer where to copy the read data\n\\param size Desired number of bytes to read\n\n\\return The number of bytes actually read, or `std::nullopt` on error",
    "\\brief Change the current reading position\n\n\\param position The position to seek to, from the beginning\n\n\\return The position actually sought to, or `std::nullopt` on error",
    "\\brief Get the current reading position in the stream\n\n\\return The current position, or `std::nullopt` on error.",
    "\\brief Return the size of the stream\n\n\\return The total number of bytes available in the stream, or `std::nullopt` on error",
}; }

void bind_InputStream(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__InputStream = lua_glue::BindClass<sf::InputStream>(sf, "InputStream");
    lua_glue::Table table_sf__InputStream = sf["InputStream"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::InputStream>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.InputStream");
    // sf::InputStream is abstract; constructor binding is omitted.
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.InputStream", "read", "fun(self: sf.InputStream, size: integer): integer|nil, any");
    lua_glue::BindCallable(type_sf__InputStream, "read",
        [lua](sf::InputStream& self, std::size_t size) {
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
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.InputStream", "seek", "fun(self: sf.InputStream, position: integer): integer|nil");
    lua_glue::BindCallable(type_sf__InputStream, "seek",
        [lua](sf::InputStream& self, lua_sf::LuaIntegral<std::size_t> position) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.seek(position.value()));
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.InputStream", "tell", "fun(self: sf.InputStream): integer|nil");
    lua_glue::BindCallable(type_sf__InputStream, "tell",
        [lua](sf::InputStream& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.tell());
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.InputStream", "getSize", "fun(self: sf.InputStream): integer|nil");
    lua_glue::BindCallable(type_sf__InputStream, "getSize",
        [lua](sf::InputStream& self) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.getSize());
        },
        docs[4]
    );
}
