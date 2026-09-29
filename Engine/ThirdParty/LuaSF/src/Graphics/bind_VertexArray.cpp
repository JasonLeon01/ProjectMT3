#include "Graphics/bind_VertexArray.hpp"

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

namespace { constexpr std::array<std::string_view, 10> docs = {
    "\\brief Set of one or more 2D primitives",
    "\\brief Default constructor\n\nCreates an empty vertex array.",
    "\\brief Construct the vertex array with a type and an initial number of vertices\n\n\\param type        Type of primitives\n\\param vertexCount Initial number of vertices in the array",
    "\\brief Return the vertex count\n\n\\return Number of vertices in the array",
    "\\brief Clear the vertex array\n\nThis function removes all the vertices from the array.\nIt doesn't deallocate the corresponding memory, so that\nadding new vertices after clearing doesn't involve\nreallocating all the memory.",
    "\\brief Resize the vertex array\n\nIf `vertexCount` is greater than the current size, the previous\nvertices are kept and new (default-constructed) vertices are\nadded.\nIf `vertexCount` is less than the current size, existing vertices\nare removed from the array.\n\n\\param vertexCount New size of the array (number of vertices)",
    "\\brief Add a vertex to the array\n\n\\param vertex Vertex to add",
    "\\brief Set the type of primitives to draw\n\nThis function defines how the vertices must be interpreted\nwhen it's time to draw them:\n\\li As points\n\\li As lines\n\\li As triangles\nThe default primitive type is `sf::PrimitiveType::Points`.\n\n\\param type Type of primitive",
    "\\brief Get the type of primitives drawn by the vertex array\n\n\\return Primitive type",
    "\\brief Compute the bounding rectangle of the vertex array\n\nThis function returns the minimal axis-aligned rectangle\nthat contains all the vertices of the array.\n\n\\return Bounding rectangle of the vertex array",
}; }

void bind_VertexArray(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__VertexArray = lua_glue::BindClass<sf::VertexArray>(sf, "VertexArray");
    lua_glue::BindBase<sf::VertexArray, sf::Drawable>(type_sf__VertexArray);
    lua_glue::Table table_sf__VertexArray = sf["VertexArray"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::VertexArray>(lua);
    lua_glue::Table native_bases_sf__VertexArray = lua.create_table();
    native_bases_sf__VertexArray.add(lua["sf"]["Drawable"].get<lua_glue::Table>());
    table_sf__VertexArray.raw_set("__nativeBases", native_bases_sf__VertexArray);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.VertexArray", "sf.Drawable");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.VertexArray", "new", "fun(type: sf.PrimitiveType, vertexCount?: integer): sf.VertexArray");
    LUASF_STUB_OVERLOAD("sf.VertexArray", "new", "fun(): sf.VertexArray");
    lua_glue::BindCallable(type_sf__VertexArray, "new",
        [](sf::PrimitiveType type, lua_sf::LuaIntegral<std::size_t> vertexCount) {
            return lua_sf::makeLuaSharedObject<sf::VertexArray>(type, vertexCount.value());
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned long>(0);
        }}},
        docs[2]
    );
    lua_glue::BindCallable(type_sf__VertexArray, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::VertexArray>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.VertexArray", "getVertexCount", "fun(self: sf.VertexArray): integer");
    lua_glue::BindCallable(type_sf__VertexArray, "getVertexCount",
        [](const sf::VertexArray& self) -> std::size_t {
            return self.getVertexCount();
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.VertexArray", "clear", "fun(self: sf.VertexArray)");
    lua_glue::BindCallable(type_sf__VertexArray, "clear",
        [](sf::VertexArray& self) {
            self.clear();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.VertexArray", "resize", "fun(self: sf.VertexArray, vertexCount: integer)");
    lua_glue::BindCallable(type_sf__VertexArray, "resize",
        [](sf::VertexArray& self, lua_sf::LuaIntegral<std::size_t> vertexCount) {
            self.resize(vertexCount.value());
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.VertexArray", "append", "fun(self: sf.VertexArray, vertex: sf.Vertex)");
    lua_glue::BindCallable(type_sf__VertexArray, "append",
        [](sf::VertexArray& self, const sf::Vertex& vertex) {
            self.append(vertex);
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.VertexArray", "setPrimitiveType", "fun(self: sf.VertexArray, type: sf.PrimitiveType)");
    lua_glue::BindCallable(type_sf__VertexArray, "setPrimitiveType",
        [](sf::VertexArray& self, sf::PrimitiveType type) {
            self.setPrimitiveType(type);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.VertexArray", "getPrimitiveType", "fun(self: sf.VertexArray): sf.PrimitiveType");
    lua_glue::BindCallable(type_sf__VertexArray, "getPrimitiveType",
        [](const sf::VertexArray& self) -> sf::PrimitiveType {
            return self.getPrimitiveType();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.VertexArray", "getBounds", "fun(self: sf.VertexArray): sf.FloatRect");
    lua_glue::BindCallable(type_sf__VertexArray, "getBounds",
        [](const sf::VertexArray& self) -> sf::FloatRect {
            return self.getBounds();
        },
        docs[9]
    );
    auto index_operator_key_sf__VertexArray = [](lua_glue::Object key) -> std::size_t {
        if (key.get_type() != lua_glue::Type::Number || !key.is<lua_Integer>())
            throw std::invalid_argument("sf.VertexArray index must be an integer");
        const lua_Integer index = key.as<lua_Integer>();
        if (index < 0)
            throw std::out_of_range("sf.VertexArray index must be non-negative");
        return static_cast<std::size_t>(index);
    };
    lua_glue::BindMetamethod(type_sf__VertexArray, "__index",
        [table_sf__VertexArray, index_operator_key_sf__VertexArray](lua_glue::ThisState state, sf::VertexArray& self, lua_glue::Object key) -> lua_glue::Object {
            if (key.get_type() != lua_glue::Type::Number)
                return table_sf__VertexArray.get<lua_glue::Object>(key);
            const std::size_t index = index_operator_key_sf__VertexArray(key);
            return lua_glue::MakeObject(state.value, std::ref(self.operator[](index)));
            },
            lua_glue::ReturnPolicy::ReferenceInternal)
    ;
    LUASF_STUB_OPERATOR("sf.VertexArray", "get(integer): sf.Vertex");
    LUASF_STUB_INDEX_FIELD("sf.VertexArray", "integer", "sf.Vertex");
    lua_glue::BindMetamethod(type_sf__VertexArray, "__newindex",
        [index_operator_key_sf__VertexArray](sf::VertexArray& self, lua_glue::Object key, const sf::Vertex& value) {
            const std::size_t index = index_operator_key_sf__VertexArray(key);
            self.operator[](index) = value;
        })
    ;
    LUASF_STUB_OPERATOR("sf.VertexArray", "set(integer, sf.Vertex)");
    // Skipped sf::VertexArray::begin(): unsupported return type std::vector<sf::Vertex>::iterator.
    // Skipped sf::VertexArray::begin(): unsupported return type std::vector<sf::Vertex>::const_iterator.
    // Skipped sf::VertexArray::end(): unsupported return type std::vector<sf::Vertex>::iterator.
    // Skipped sf::VertexArray::end(): unsupported return type std::vector<sf::Vertex>::const_iterator.
}
