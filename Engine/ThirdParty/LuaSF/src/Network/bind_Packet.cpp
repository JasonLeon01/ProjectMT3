#include "Network/bind_Packet.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Packet(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Packet = sf.new_usertype<sf::Packet>("Packet", sol::no_constructor);
    sol::table table_sf__Packet = sf["Packet"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Packet>(lua);
    LUASF_STUB_DOC("\\brief Utility class to build blocks of data to transfer\nover the network");
    LUASF_STUB_CLASS("sf.Packet");
    LUASF_STUB_DOC("\\brief Default constructor\n\nCreates an empty packet.");
    LUASF_STUB_FUNCTION("sf.Packet", "new", "fun(): sf.Packet");
    type_sf__Packet.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Packet>();
        }
    ));
    LUASF_STUB_DOC("\\brief Append data to the end of the packet\n\n\\param data        Pointer to the sequence of bytes to append\n\\param sizeInBytes Number of bytes to append\n\n\\see `clear`\n\\see `getReadPosition`");
    LUASF_STUB_FUNCTION("sf.Packet", "append", "fun(self: sf.Packet, data: any)");
    type_sf__Packet.set_function("append",
        [](sf::Packet& self, sol::object data) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            self.append(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
        }
    );
    LUASF_STUB_DOC("\\brief Get the current reading position in the packet\n\nThe next read operation will read data from this position\n\n\\return The byte offset of the current read position\n\n\\see `append`");
    LUASF_STUB_FUNCTION("sf.Packet", "getReadPosition", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("getReadPosition",
        [](sf::Packet& self) -> std::size_t {
            return self.getReadPosition();
        }
    );
    LUASF_STUB_DOC("\\brief Clear the packet\n\nAfter calling Clear, the packet is empty.\n\n\\see `append`");
    LUASF_STUB_FUNCTION("sf.Packet", "clear", "fun(self: sf.Packet)");
    type_sf__Packet.set_function("clear",
        [](sf::Packet& self) {
            self.clear();
        }
    );
    LUASF_STUB_DOC("\\brief Get a pointer to the data contained in the packet\n\nWarning: the returned pointer may become invalid after\nyou append data to the packet, therefore it should never\nbe stored.\nThe return pointer is a `nullptr` if the packet is empty.\n\n\\return Pointer to the data\n\n\\see `getDataSize`");
    LUASF_STUB_FUNCTION("sf.Packet", "getData", "fun(self: sf.Packet): nil");
    type_sf__Packet.set_function("getData",
        [](sf::Packet& self) -> const void* {
            return self.getData();
        }
    );
    LUASF_STUB_DOC("\\brief Get the size of the data contained in the packet\n\nThis function returns the number of bytes pointed to by\nwhat `getData` returns.\n\n\\return Data size, in bytes\n\n\\see `getData`");
    LUASF_STUB_FUNCTION("sf.Packet", "getDataSize", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("getDataSize",
        [](sf::Packet& self) -> std::size_t {
            return self.getDataSize();
        }
    );
    LUASF_STUB_DOC("\\brief Tell if the reading position has reached the\nend of the packet\n\nThis function is useful to know if there is some data\nleft to be read, without actually reading it.\n\n\\return `true` if all data was read, `false` otherwise\n\n\\see `operator` bool");
    LUASF_STUB_FUNCTION("sf.Packet", "endOfPacket", "fun(self: sf.Packet): boolean");
    type_sf__Packet.set_function("endOfPacket",
        [](sf::Packet& self) -> bool {
            return self.endOfPacket();
        }
    );
    type_sf__Packet[sol::meta_function::bitwise_left_shift] =
        sol::policies(
            sol::overload(
                [](sf::Packet& self, bool data) {
                    return std::ref(self.operator<<(data));
                },
                [](sf::Packet& self, lua_sf::LuaIntegral<std::int8_t> data) {
                    return std::ref(self.operator<<(data.value()));
                },
                [](sf::Packet& self, lua_sf::LuaIntegral<std::uint8_t> data) {
                    return std::ref(self.operator<<(data.value()));
                },
                [](sf::Packet& self, lua_sf::LuaIntegral<std::int16_t> data) {
                    return std::ref(self.operator<<(data.value()));
                },
                [](sf::Packet& self, lua_sf::LuaIntegral<std::uint16_t> data) {
                    return std::ref(self.operator<<(data.value()));
                },
                [](sf::Packet& self, lua_sf::LuaIntegral<std::int32_t> data) {
                    return std::ref(self.operator<<(data.value()));
                },
                [](sf::Packet& self, lua_sf::LuaIntegral<std::uint32_t> data) {
                    return std::ref(self.operator<<(data.value()));
                },
                [](sf::Packet& self, lua_sf::LuaIntegral<std::int64_t> data) {
                    return std::ref(self.operator<<(data.value()));
                },
                [](sf::Packet& self, lua_sf::LuaIntegral<std::uint64_t> data) {
                    return std::ref(self.operator<<(data.value()));
                },
                [](sf::Packet& self, float data) {
                    return std::ref(self.operator<<(data));
                },
                [](sf::Packet& self, double data) {
                    return std::ref(self.operator<<(data));
                },
                [](sf::Packet& self, std::string data) {
                    return std::ref(self.operator<<(data.c_str()));
                }
            ),
            sol::self_dependency{}
        )
    ;
    LUASF_STUB_OPERATOR("sf.Packet", "shl(any): sf.Packet");
    LUASF_STUB_FUNCTION("sf.Packet", "writeBool", "fun(self: sf.Packet, data: boolean): sf.Packet");
    type_sf__Packet.set_function("writeBool",
        sol::policies(
            [](sf::Packet& self, bool data) {
                return std::ref(self.operator<<(data));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeInt8", "fun(self: sf.Packet, data: integer): sf.Packet");
    type_sf__Packet.set_function("writeInt8",
        sol::policies(
            [](sf::Packet& self, lua_sf::LuaIntegral<std::int8_t> data) {
                return std::ref(self.operator<<(data.value()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeUInt8", "fun(self: sf.Packet, data: integer): sf.Packet");
    type_sf__Packet.set_function("writeUInt8",
        sol::policies(
            [](sf::Packet& self, lua_sf::LuaIntegral<std::uint8_t> data) {
                return std::ref(self.operator<<(data.value()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeInt16", "fun(self: sf.Packet, data: integer): sf.Packet");
    type_sf__Packet.set_function("writeInt16",
        sol::policies(
            [](sf::Packet& self, lua_sf::LuaIntegral<std::int16_t> data) {
                return std::ref(self.operator<<(data.value()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeUInt16", "fun(self: sf.Packet, data: integer): sf.Packet");
    type_sf__Packet.set_function("writeUInt16",
        sol::policies(
            [](sf::Packet& self, lua_sf::LuaIntegral<std::uint16_t> data) {
                return std::ref(self.operator<<(data.value()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeInt32", "fun(self: sf.Packet, data: integer): sf.Packet");
    type_sf__Packet.set_function("writeInt32",
        sol::policies(
            [](sf::Packet& self, lua_sf::LuaIntegral<std::int32_t> data) {
                return std::ref(self.operator<<(data.value()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeUInt32", "fun(self: sf.Packet, data: integer): sf.Packet");
    type_sf__Packet.set_function("writeUInt32",
        sol::policies(
            [](sf::Packet& self, lua_sf::LuaIntegral<std::uint32_t> data) {
                return std::ref(self.operator<<(data.value()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeInt64", "fun(self: sf.Packet, data: integer): sf.Packet");
    type_sf__Packet.set_function("writeInt64",
        sol::policies(
            [](sf::Packet& self, lua_sf::LuaIntegral<std::int64_t> data) {
                return std::ref(self.operator<<(data.value()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeUInt64", "fun(self: sf.Packet, data: integer): sf.Packet");
    type_sf__Packet.set_function("writeUInt64",
        sol::policies(
            [](sf::Packet& self, lua_sf::LuaIntegral<std::uint64_t> data) {
                return std::ref(self.operator<<(data.value()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeFloat", "fun(self: sf.Packet, data: number): sf.Packet");
    type_sf__Packet.set_function("writeFloat",
        sol::policies(
            [](sf::Packet& self, float data) {
                return std::ref(self.operator<<(data));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeDouble", "fun(self: sf.Packet, data: number): sf.Packet");
    type_sf__Packet.set_function("writeDouble",
        sol::policies(
            [](sf::Packet& self, double data) {
                return std::ref(self.operator<<(data));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeString", "fun(self: sf.Packet, data: string): sf.Packet");
    type_sf__Packet.set_function("writeString",
        sol::policies(
            [](sf::Packet& self, std::string data) {
                return std::ref(self.operator<<(data.c_str()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeWideString", "fun(self: sf.Packet, data: string): sf.Packet");
    type_sf__Packet.set_function("writeWideString",
        sol::policies(
            [](sf::Packet& self, std::string data) {
                const std::wstring data_wide = lua_sf::to_sf_string(data).toWideString();
                return std::ref(self.operator<<(data_wide.c_str()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_FUNCTION("sf.Packet", "writeSfString", "fun(self: sf.Packet, data: string): sf.Packet");
    type_sf__Packet.set_function("writeSfString",
        sol::policies(
            [](sf::Packet& self, std::string data) {
                return std::ref(self.operator<<(lua_sf::to_sf_string(data)));
            },
            sol::self_dependency{}
        )
    );
    auto read_operator_sf__Packet = [](sol::this_state state, sf::Packet& self, std::string type) -> sol::object {
        if (type == "bool" || type == "boolean")
        {
            bool value{};
            self.operator>>(value);
            return sol::make_object(state, value);
        }
        if (type == "int8")
        {
            std::int8_t value{};
            self.operator>>(value);
            return sol::make_object(state, static_cast<int>(value));
        }
        if (type == "uint8")
        {
            std::uint8_t value{};
            self.operator>>(value);
            return sol::make_object(state, static_cast<unsigned int>(value));
        }
        if (type == "int16")
        {
            std::int16_t value{};
            self.operator>>(value);
            return sol::make_object(state, static_cast<int>(value));
        }
        if (type == "uint16")
        {
            std::uint16_t value{};
            self.operator>>(value);
            return sol::make_object(state, static_cast<unsigned int>(value));
        }
        if (type == "int32" || type == "int" || type == "integer")
        {
            std::int32_t value{};
            self.operator>>(value);
            return sol::make_object(state, static_cast<int>(value));
        }
        if (type == "uint32" || type == "uint")
        {
            std::uint32_t value{};
            self.operator>>(value);
            return sol::make_object(state, static_cast<unsigned int>(value));
        }
        if (type == "int64")
        {
            std::int64_t value{};
            self.operator>>(value);
            return sol::make_object(state, static_cast<std::int64_t>(value));
        }
        if (type == "uint64")
        {
            std::uint64_t value{};
            self.operator>>(value);
            return sol::make_object(state, static_cast<std::uint64_t>(value));
        }
        if (type == "float")
        {
            float value{};
            self.operator>>(value);
            return sol::make_object(state, value);
        }
        if (type == "double" || type == "number")
        {
            double value{};
            self.operator>>(value);
            return sol::make_object(state, value);
        }
        if (type == "string" || type == "std::string")
        {
            std::string value{};
            self.operator>>(value);
            return sol::make_object(state, value);
        }
        if (type == "wstring" || type == "wideString")
        {
            std::wstring value{};
            self.operator>>(value);
            return sol::make_object(state, lua_sf::to_utf8_string(sf::String(value)));
        }
        if (type == "sfString" || type == "sf::String")
        {
            sf::String value{};
            self.operator>>(value);
            return sol::make_object(state, lua_sf::to_utf8_string(value));
        }
        throw std::runtime_error("sf.Packet: operator>> unknown read type " + type);
    };
    type_sf__Packet[sol::meta_function::bitwise_right_shift] =
        [read_operator_sf__Packet](sol::this_state state, sf::Packet& self, std::string type) -> sol::object {
            return read_operator_sf__Packet(state, self, std::move(type));
        }
    ;
    LUASF_STUB_OPERATOR("sf.Packet", "shr(string): any");
    LUASF_STUB_FUNCTION("sf.Packet", "readBool", "fun(self: sf.Packet): boolean");
    type_sf__Packet.set_function("readBool",
        [](sf::Packet& self) {
            bool value{};
            self.operator>>(value);
            return value;
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readInt8", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("readInt8",
        [](sf::Packet& self) {
            std::int8_t value{};
            self.operator>>(value);
            return static_cast<int>(value);
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readUInt8", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("readUInt8",
        [](sf::Packet& self) {
            std::uint8_t value{};
            self.operator>>(value);
            return static_cast<unsigned int>(value);
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readInt16", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("readInt16",
        [](sf::Packet& self) {
            std::int16_t value{};
            self.operator>>(value);
            return static_cast<int>(value);
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readUInt16", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("readUInt16",
        [](sf::Packet& self) {
            std::uint16_t value{};
            self.operator>>(value);
            return static_cast<unsigned int>(value);
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readInt32", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("readInt32",
        [](sf::Packet& self) {
            std::int32_t value{};
            self.operator>>(value);
            return static_cast<int>(value);
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readUInt32", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("readUInt32",
        [](sf::Packet& self) {
            std::uint32_t value{};
            self.operator>>(value);
            return static_cast<unsigned int>(value);
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readInt64", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("readInt64",
        [](sf::Packet& self) {
            std::int64_t value{};
            self.operator>>(value);
            return static_cast<std::int64_t>(value);
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readUInt64", "fun(self: sf.Packet): integer");
    type_sf__Packet.set_function("readUInt64",
        [](sf::Packet& self) {
            std::uint64_t value{};
            self.operator>>(value);
            return static_cast<std::uint64_t>(value);
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readFloat", "fun(self: sf.Packet): number");
    type_sf__Packet.set_function("readFloat",
        [](sf::Packet& self) {
            float value{};
            self.operator>>(value);
            return value;
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readDouble", "fun(self: sf.Packet): number");
    type_sf__Packet.set_function("readDouble",
        [](sf::Packet& self) {
            double value{};
            self.operator>>(value);
            return value;
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readString", "fun(self: sf.Packet): string");
    type_sf__Packet.set_function("readString",
        [](sf::Packet& self) {
            std::string value{};
            self.operator>>(value);
            return value;
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readWideString", "fun(self: sf.Packet): string");
    type_sf__Packet.set_function("readWideString",
        [](sf::Packet& self) {
            std::wstring value{};
            self.operator>>(value);
            return lua_sf::to_utf8_string(sf::String(value));
        }
    );
    LUASF_STUB_FUNCTION("sf.Packet", "readSfString", "fun(self: sf.Packet): string");
    type_sf__Packet.set_function("readSfString",
        [](sf::Packet& self) {
            sf::String value{};
            self.operator>>(value);
            return lua_sf::to_utf8_string(value);
        }
    );
    // Skipped operator sf::Packet::operator>>(char *): output pointer operators require caller-owned buffers.
    // Skipped operator sf::Packet::operator>>(wchar_t *): output pointer operators require caller-owned buffers.
}
