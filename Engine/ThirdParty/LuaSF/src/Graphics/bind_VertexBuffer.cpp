#include "Graphics/bind_VertexBuffer.hpp"

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

namespace { constexpr std::array<std::string_view, 23> docs = {
    "\\brief Vertex buffer storage for one or more 2D primitives",
    "\\brief Default constructor\n\nCreates an empty vertex buffer.",
    "\\brief Construct a `VertexBuffer` with a specific `PrimitiveType`\n\nCreates an empty vertex buffer and sets its primitive type to \\p type.\n\n\\param type Type of primitive",
    "\\brief Construct a `VertexBuffer` with a specific usage specifier\n\nCreates an empty vertex buffer and sets its usage to \\p usage.\n\n\\param usage Usage specifier",
    "\\brief Construct a `VertexBuffer` with a specific `PrimitiveType` and usage specifier\n\nCreates an empty vertex buffer and sets its primitive type\nto \\p type and usage to \\p usage.\n\n\\param type  Type of primitive\n\\param usage Usage specifier",
    "\\brief Create the vertex buffer\n\nCreates the vertex buffer and allocates enough graphics\nmemory to hold `vertexCount` vertices. Any previously\nallocated memory is freed in the process.\n\nIn order to deallocate previously allocated memory pass 0\nas `vertexCount`. Don't forget to recreate with a non-zero\nvalue when graphics memory should be allocated again.\n\n\\param vertexCount Number of vertices worth of memory to allocate\n\n\\return `true` if creation was successful",
    "\\brief Return the vertex count\n\n\\return Number of vertices in the vertex buffer",
    "\\brief Update the whole buffer from an array of vertices\n\nThe vertex array is assumed to have the same size as\nthe created buffer.\n\nNo additional check is performed on the size of the vertex\narray. Passing invalid arguments will lead to undefined\nbehavior.\n\nThis function does nothing if `vertices` is null or if the\nbuffer was not previously created.\n\n\\param vertices Array of vertices to copy to the buffer\n\n\\return `true` if the update was successful",
    "\\brief Update a part of the buffer from an array of vertices\n\n`offset` is specified as the number of vertices to skip\nfrom the beginning of the buffer.\n\nIf `offset` is 0 and `vertexCount` is equal to the size of\nthe currently created buffer, its whole contents are replaced.\n\nIf `offset` is 0 and `vertexCount` is greater than the\nsize of the currently created buffer, a new buffer is created\ncontaining the vertex data.\n\nIf `offset` is 0 and `vertexCount` is less than the size of\nthe currently created buffer, only the corresponding region\nis updated.\n\nIf `offset` is not 0 and `offset` + `vertexCount` is greater\nthan the size of the currently created buffer, the update fails.\n\nNo additional check is performed on the size of the vertex\narray. Passing invalid arguments will lead to undefined\nbehavior.\n\n\\param vertices    Array of vertices to copy to the buffer\n\\param vertexCount Number of vertices to copy\n\\param offset      Offset in the buffer to copy to\n\n\\return `true` if the update was successful",
    "\\brief Copy the contents of another buffer into this buffer\n\n\\param vertexBuffer Vertex buffer whose contents to copy into this vertex buffer\n\n\\return `true` if the copy was successful",
    "\\brief Swap the contents of this vertex buffer with those of another\n\n\\param right Instance to swap with",
    "\\brief Get the underlying OpenGL handle of the vertex buffer.\n\nYou shouldn't need to use this function, unless you have\nvery specific stuff to implement that SFML doesn't support,\nor implement a temporary workaround until a bug is fixed.\n\n\\return OpenGL handle of the vertex buffer or 0 if not yet created",
    "\\brief Set the type of primitives to draw\n\nThis function defines how the vertices must be interpreted\nwhen it's time to draw them.\n\nThe default primitive type is `sf::PrimitiveType::Points`.\n\n\\param type Type of primitive",
    "\\brief Get the type of primitives drawn by the vertex buffer\n\n\\return Primitive type",
    "\\brief Set the usage specifier of this vertex buffer\n\nThis function provides a hint about how this vertex buffer is\ngoing to be used in terms of data update frequency.\n\nAfter changing the usage specifier, the vertex buffer has\nto be updated with new data for the usage specifier to\ntake effect.\n\nThe default usage type is `sf::VertexBuffer::Usage::Stream`.\n\n\\param usage Usage specifier",
    "\\brief Get the usage specifier of this vertex buffer\n\n\\return Usage specifier",
    "\\brief Bind a vertex buffer for rendering\n\nThis function is not part of the graphics API, it mustn't be\nused when drawing SFML entities. It must be used only if you\nmix `sf::VertexBuffer` with OpenGL code.\n\n\\code\nsf::VertexBuffer vb1, vb2;\n...\nsf::VertexBuffer::bind(&vb1);\n// draw OpenGL stuff that use vb1...\nsf::VertexBuffer::bind(&vb2);\n// draw OpenGL stuff that use vb2...\nsf::VertexBuffer::bind(nullptr);\n// draw OpenGL stuff that use no vertex buffer...\n\\endcode\n\n\\param vertexBuffer Pointer to the vertex buffer to bind, can be null to use no vertex buffer",
    "\\brief Tell whether or not the system supports vertex buffers\n\nThis function should always be called before using\nthe vertex buffer features. If it returns `false`, then\nany attempt to use `sf::VertexBuffer` will fail.\n\n\\return `true` if vertex buffers are supported, `false` otherwise",
    "\\brief Usage specifiers\n\nIf data is going to be updated once or more every frame,\nset the usage to Stream. If data is going to be set once\nand used for a long time without being modified, set the\nusage to Static. For everything else Dynamic should be a\ngood compromise.",
    "Constantly changing data",
    "Occasionally changing data",
    "Rarely changing data",
    "\\brief Swap the contents of one vertex buffer with those of another\n\n\\param left First instance to swap\n\\param right Second instance to swap",
}; }

void bind_VertexBuffer(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__VertexBuffer = lua_glue::BindClass<sf::VertexBuffer>(sf, "VertexBuffer");
    lua_glue::BindBase<sf::VertexBuffer, sf::Drawable>(type_sf__VertexBuffer);
    lua_glue::Table table_sf__VertexBuffer = sf["VertexBuffer"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::VertexBuffer>(lua);
    lua_glue::Table native_bases_sf__VertexBuffer = lua.create_table();
    native_bases_sf__VertexBuffer.add(lua["sf"]["Drawable"].get<lua_glue::Table>());
    table_sf__VertexBuffer.raw_set("__nativeBases", native_bases_sf__VertexBuffer);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.VertexBuffer", "sf.Drawable");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "new", "fun(type: sf.PrimitiveType, usage: sf.VertexBuffer.Usage): sf.VertexBuffer");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "new", "fun(type: sf.PrimitiveType): sf.VertexBuffer");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "new", "fun(usage: sf.VertexBuffer.Usage): sf.VertexBuffer");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "new", "fun(): sf.VertexBuffer");
    lua_glue::BindCallable(type_sf__VertexBuffer, "new",
        [](sf::PrimitiveType type, sf::VertexBuffer::Usage usage) {
            return lua_sf::makeLuaSharedObject<sf::VertexBuffer>(type, usage);
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__VertexBuffer, "new",
        [](sf::PrimitiveType type) {
            return lua_sf::makeLuaSharedObject<sf::VertexBuffer>(type);
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__VertexBuffer, "new",
        [](sf::VertexBuffer::Usage usage) {
            return lua_sf::makeLuaSharedObject<sf::VertexBuffer>(usage);
        },
        docs[3]
    );
    lua_glue::BindCallable(type_sf__VertexBuffer, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::VertexBuffer>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "create", "fun(self: sf.VertexBuffer, vertexCount: integer): boolean");
    lua_glue::BindCallable(type_sf__VertexBuffer, "create",
        [](sf::VertexBuffer& self, lua_sf::LuaIntegral<std::size_t> vertexCount) -> bool {
            return self.create(vertexCount.value());
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "getVertexCount", "fun(self: sf.VertexBuffer): integer");
    lua_glue::BindCallable(type_sf__VertexBuffer, "getVertexCount",
        [](const sf::VertexBuffer& self) -> std::size_t {
            return self.getVertexCount();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "update", "fun(self: sf.VertexBuffer, vertexBuffer: sf.VertexBuffer): boolean");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "update", "fun(self: sf.VertexBuffer, vertices: any, offset: integer): boolean");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "update", "fun(self: sf.VertexBuffer, vertices: any): boolean");
    lua_glue::BindCallable(type_sf__VertexBuffer, "update",
        [](sf::VertexBuffer& self, const sf::VertexBuffer& vertexBuffer) -> bool {
            return self.update(vertexBuffer);
        },
        docs[9]
    );
    lua_glue::BindCallable(type_sf__VertexBuffer, "update",
        [](sf::VertexBuffer& self, lua_glue::Object vertices, lua_sf::LuaIntegral<unsigned int> offset) -> bool {
            auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
            return self.update(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), offset.value());
        },
        docs[8]
    );
    lua_glue::BindCallable(type_sf__VertexBuffer, "update",
        [](sf::VertexBuffer& self, lua_glue::Object vertices) -> bool {
            auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
            return self.update(vertices_buffer.data());
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "swap", "fun(self: sf.VertexBuffer, right: sf.VertexBuffer)");
    lua_glue::BindCallable(type_sf__VertexBuffer, "swap",
        [](sf::VertexBuffer& self, sf::VertexBuffer& right) {
            self.swap(right);
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "getNativeHandle", "fun(self: sf.VertexBuffer): integer");
    lua_glue::BindCallable(type_sf__VertexBuffer, "getNativeHandle",
        [](const sf::VertexBuffer& self) -> unsigned int {
            return self.getNativeHandle();
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "setPrimitiveType", "fun(self: sf.VertexBuffer, type: sf.PrimitiveType)");
    lua_glue::BindCallable(type_sf__VertexBuffer, "setPrimitiveType",
        [](sf::VertexBuffer& self, sf::PrimitiveType type) {
            self.setPrimitiveType(type);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "getPrimitiveType", "fun(self: sf.VertexBuffer): sf.PrimitiveType");
    lua_glue::BindCallable(type_sf__VertexBuffer, "getPrimitiveType",
        [](const sf::VertexBuffer& self) -> sf::PrimitiveType {
            return self.getPrimitiveType();
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "setUsage", "fun(self: sf.VertexBuffer, usage: sf.VertexBuffer.Usage)");
    lua_glue::BindCallable(type_sf__VertexBuffer, "setUsage",
        [](sf::VertexBuffer& self, sf::VertexBuffer::Usage usage) {
            self.setUsage(usage);
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "getUsage", "fun(self: sf.VertexBuffer): sf.VertexBuffer.Usage");
    lua_glue::BindCallable(type_sf__VertexBuffer, "getUsage",
        [](const sf::VertexBuffer& self) -> sf::VertexBuffer::Usage {
            return self.getUsage();
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "bind", "fun(vertexBuffer: sf.VertexBuffer)");
    lua_glue::BindCallable(type_sf__VertexBuffer, "bind",
        [](const sf::VertexBuffer* vertexBuffer) {
            sf::VertexBuffer::bind(vertexBuffer);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "isAvailable", "fun(): boolean");
    lua_glue::BindCallable(type_sf__VertexBuffer, "isAvailable",
        []() -> bool {
            return sf::VertexBuffer::isAvailable();
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_CLASS("sf.VertexBuffer.Usage");
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FIELD("Stream", "sf.VertexBuffer.Usage");
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FIELD("Dynamic", "sf.VertexBuffer.Usage");
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FIELD("Static", "sf.VertexBuffer.Usage");
    lua_glue::BindEnum<sf::VertexBuffer::Usage>(table_sf__VertexBuffer, "Usage", {
        {"Stream", sf::VertexBuffer::Usage::Stream},
        {"Dynamic", sf::VertexBuffer::Usage::Dynamic},
        {"Static", sf::VertexBuffer::Usage::Static}
    });
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf", "swap", "fun(left: sf.VertexBuffer, right: sf.VertexBuffer)");
    lua_glue::BindCallable(sf, "swap",
        [](sf::VertexBuffer& left, sf::VertexBuffer& right) {
            sf::swap(left, right);
        },
        docs[22]
    );
}
