#include "Graphics/bind_VertexArray.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_VertexArray(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__VertexArray = sf.new_usertype<sf::VertexArray>("VertexArray",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Drawable>()
    );
    sol::table table_sf__VertexArray = sf["VertexArray"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::VertexArray>(lua);
    sol::table native_bases_sf__VertexArray = lua.create_table();
    native_bases_sf__VertexArray.add(lua["sf"]["Drawable"].get<sol::table>());
    table_sf__VertexArray.raw_set("__nativeBases", native_bases_sf__VertexArray);
    LUASF_STUB_DOC("\\brief Set of one or more 2D primitives");
    LUASF_STUB_CLASS("sf.VertexArray", "sf.Drawable");
    LUASF_STUB_DOC("\\brief Construct the vertex array with a type and an initial number of vertices\n\n\\param type        Type of primitives\n\\param vertexCount Initial number of vertices in the array");
    LUASF_STUB_FUNCTION("sf.VertexArray", "new", "fun(type: sf.PrimitiveType, vertexCount: integer): sf.VertexArray");
    LUASF_STUB_OVERLOAD("sf.VertexArray", "new", "fun(type: sf.PrimitiveType): sf.VertexArray");
    LUASF_STUB_OVERLOAD("sf.VertexArray", "new", "fun(): sf.VertexArray");
    type_sf__VertexArray.set_function("new", sol::factories(
        [](sf::PrimitiveType type, lua_sf::LuaIntegral<std::size_t> vertexCount) {
            return lua_sf::makeLuaSharedObject<sf::VertexArray>(type, vertexCount.value());
        },
        [](sf::PrimitiveType type) {
            return lua_sf::makeLuaSharedObject<sf::VertexArray>(type);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::VertexArray>();
        }
    ));
    LUASF_STUB_DOC("\\brief Return the vertex count\n\n\\return Number of vertices in the array");
    LUASF_STUB_FUNCTION("sf.VertexArray", "getVertexCount", "fun(self: sf.VertexArray): integer");
    type_sf__VertexArray.set_function("getVertexCount",
        [](sf::VertexArray& self) -> std::size_t {
            return self.getVertexCount();
        }
    );
    LUASF_STUB_DOC("\\brief Clear the vertex array\n\nThis function removes all the vertices from the array.\nIt doesn't deallocate the corresponding memory, so that\nadding new vertices after clearing doesn't involve\nreallocating all the memory.");
    LUASF_STUB_FUNCTION("sf.VertexArray", "clear", "fun(self: sf.VertexArray)");
    type_sf__VertexArray.set_function("clear",
        [](sf::VertexArray& self) {
            self.clear();
        }
    );
    LUASF_STUB_DOC("\\brief Resize the vertex array\n\nIf `vertexCount` is greater than the current size, the previous\nvertices are kept and new (default-constructed) vertices are\nadded.\nIf `vertexCount` is less than the current size, existing vertices\nare removed from the array.\n\n\\param vertexCount New size of the array (number of vertices)");
    LUASF_STUB_FUNCTION("sf.VertexArray", "resize", "fun(self: sf.VertexArray, vertexCount: integer)");
    type_sf__VertexArray.set_function("resize",
        [](sf::VertexArray& self, lua_sf::LuaIntegral<std::size_t> vertexCount) {
            self.resize(vertexCount.value());
        }
    );
    LUASF_STUB_DOC("\\brief Add a vertex to the array\n\n\\param vertex Vertex to add");
    LUASF_STUB_FUNCTION("sf.VertexArray", "append", "fun(self: sf.VertexArray, vertex: sf.Vertex)");
    type_sf__VertexArray.set_function("append",
        [](sf::VertexArray& self, const sf::Vertex& vertex) {
            self.append(vertex);
        }
    );
    LUASF_STUB_DOC("\\brief Set the type of primitives to draw\n\nThis function defines how the vertices must be interpreted\nwhen it's time to draw them:\n\\li As points\n\\li As lines\n\\li As triangles\nThe default primitive type is `sf::PrimitiveType::Points`.\n\n\\param type Type of primitive");
    LUASF_STUB_FUNCTION("sf.VertexArray", "setPrimitiveType", "fun(self: sf.VertexArray, type: sf.PrimitiveType)");
    type_sf__VertexArray.set_function("setPrimitiveType",
        [](sf::VertexArray& self, sf::PrimitiveType type) {
            self.setPrimitiveType(type);
        }
    );
    LUASF_STUB_DOC("\\brief Get the type of primitives drawn by the vertex array\n\n\\return Primitive type");
    LUASF_STUB_FUNCTION("sf.VertexArray", "getPrimitiveType", "fun(self: sf.VertexArray): sf.PrimitiveType");
    type_sf__VertexArray.set_function("getPrimitiveType",
        [](sf::VertexArray& self) -> sf::PrimitiveType {
            return self.getPrimitiveType();
        }
    );
    LUASF_STUB_DOC("\\brief Compute the bounding rectangle of the vertex array\n\nThis function returns the minimal axis-aligned rectangle\nthat contains all the vertices of the array.\n\n\\return Bounding rectangle of the vertex array");
    LUASF_STUB_FUNCTION("sf.VertexArray", "getBounds", "fun(self: sf.VertexArray): sf.FloatRect");
    type_sf__VertexArray.set_function("getBounds",
        [](sf::VertexArray& self) -> sf::FloatRect {
            return self.getBounds();
        }
    );
    auto index_operator_key_sf__VertexArray = [](sol::object key) -> std::size_t {
        if (key.get_type() != sol::type::number || !key.is<lua_Integer>())
            throw std::invalid_argument("sf.VertexArray index must be an integer");
        const lua_Integer index = key.as<lua_Integer>();
        if (index < 0)
            throw std::out_of_range("sf.VertexArray index must be non-negative");
        return static_cast<std::size_t>(index);
    };
    type_sf__VertexArray[sol::meta_function::index] =
        sol::policies(
            [table_sf__VertexArray, index_operator_key_sf__VertexArray](sol::this_state state, sf::VertexArray& self, sol::object key) -> sol::object {
            if (key.get_type() != sol::type::number)
                return table_sf__VertexArray.get<sol::object>(key);
            const std::size_t index = index_operator_key_sf__VertexArray(key);
            return sol::make_object(state, std::ref(self.operator[](index)));
            },
            sol::self_dependency{}
        )
    ;
    LUASF_STUB_OPERATOR("sf.VertexArray", "get(integer): sf.Vertex");
    LUASF_STUB_INDEX_FIELD("sf.VertexArray", "integer", "sf.Vertex");
    type_sf__VertexArray[sol::meta_function::new_index] =
        [index_operator_key_sf__VertexArray](sf::VertexArray& self, sol::object key, const sf::Vertex& value) {
            const std::size_t index = index_operator_key_sf__VertexArray(key);
            self.operator[](index) = value;
        }
    ;
    LUASF_STUB_OPERATOR("sf.VertexArray", "set(integer, sf.Vertex)");
    // Skipped sf::VertexArray::begin(): unsupported return type std::vector<sf::Vertex>::iterator.
    // Skipped sf::VertexArray::begin(): unsupported return type std::vector<sf::Vertex>::const_iterator.
    // Skipped sf::VertexArray::end(): unsupported return type std::vector<sf::Vertex>::iterator.
    // Skipped sf::VertexArray::end(): unsupported return type std::vector<sf::Vertex>::const_iterator.
}
