#include "Graphics/bind_VertexBuffer.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_VertexBuffer(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__VertexBuffer = sf.new_usertype<sf::VertexBuffer>("VertexBuffer",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Drawable>()
    );
    sol::table table_sf__VertexBuffer = sf["VertexBuffer"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::VertexBuffer>(lua);
    sol::table native_bases_sf__VertexBuffer = lua.create_table();
    native_bases_sf__VertexBuffer.add(lua["sf"]["Drawable"].get<sol::table>());
    table_sf__VertexBuffer.raw_set("__nativeBases", native_bases_sf__VertexBuffer);
    LUASF_STUB_DOC("\\brief Vertex buffer storage for one or more 2D primitives");
    LUASF_STUB_CLASS("sf.VertexBuffer", "sf.Drawable");
    LUASF_STUB_DOC("\\brief Construct a `VertexBuffer` with a specific `PrimitiveType` and usage specifier\n\nCreates an empty vertex buffer and sets its primitive type\nto \\p type and usage to \\p usage.\n\n\\param type  Type of primitive\n\\param usage Usage specifier");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "new", "fun(type: sf.PrimitiveType, usage: sf.VertexBuffer.Usage): sf.VertexBuffer");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "new", "fun(type: sf.PrimitiveType): sf.VertexBuffer");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "new", "fun(usage: sf.VertexBuffer.Usage): sf.VertexBuffer");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "new", "fun(): sf.VertexBuffer");
    type_sf__VertexBuffer.set_function("new", sol::factories(
        [](sf::PrimitiveType type, sf::VertexBuffer::Usage usage) {
            return lua_sf::makeLuaSharedObject<sf::VertexBuffer>(type, usage);
        },
        [](sf::PrimitiveType type) {
            return lua_sf::makeLuaSharedObject<sf::VertexBuffer>(type);
        },
        [](sf::VertexBuffer::Usage usage) {
            return lua_sf::makeLuaSharedObject<sf::VertexBuffer>(usage);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::VertexBuffer>();
        }
    ));
    LUASF_STUB_DOC("\\brief Create the vertex buffer\n\nCreates the vertex buffer and allocates enough graphics\nmemory to hold `vertexCount` vertices. Any previously\nallocated memory is freed in the process.\n\nIn order to deallocate previously allocated memory pass 0\nas `vertexCount`. Don't forget to recreate with a non-zero\nvalue when graphics memory should be allocated again.\n\n\\param vertexCount Number of vertices worth of memory to allocate\n\n\\return `true` if creation was successful");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "create", "fun(self: sf.VertexBuffer, vertexCount: integer): boolean");
    type_sf__VertexBuffer.set_function("create",
        [](sf::VertexBuffer& self, lua_sf::LuaIntegral<std::size_t> vertexCount) -> bool {
            return self.create(vertexCount.value());
        }
    );
    LUASF_STUB_DOC("\\brief Return the vertex count\n\n\\return Number of vertices in the vertex buffer");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "getVertexCount", "fun(self: sf.VertexBuffer): integer");
    type_sf__VertexBuffer.set_function("getVertexCount",
        [](sf::VertexBuffer& self) -> std::size_t {
            return self.getVertexCount();
        }
    );
    LUASF_STUB_DOC("\\brief Copy the contents of another buffer into this buffer\n\n\\param vertexBuffer Vertex buffer whose contents to copy into this vertex buffer\n\n\\return `true` if the copy was successful");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "update", "fun(self: sf.VertexBuffer, vertexBuffer: sf.VertexBuffer): boolean");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "update", "fun(self: sf.VertexBuffer, vertices: any, offset: integer): boolean");
    LUASF_STUB_OVERLOAD("sf.VertexBuffer", "update", "fun(self: sf.VertexBuffer, vertices: any): boolean");
    type_sf__VertexBuffer.set_function("update",
        sol::overload(
            [](sf::VertexBuffer& self, const sf::VertexBuffer& vertexBuffer) -> bool {
                return self.update(vertexBuffer);
            },
            [](sf::VertexBuffer& self, sol::object vertices, lua_sf::LuaIntegral<unsigned int> offset) -> bool {
                auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
                return self.update(vertices_buffer.data(), static_cast<std::size_t>(vertices_buffer.size()), offset.value());
            },
            [](sf::VertexBuffer& self, sol::object vertices) -> bool {
                auto vertices_buffer = lua_sf::array_from_object<sf::Vertex>(vertices);
                return self.update(vertices_buffer.data());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Swap the contents of this vertex buffer with those of another\n\n\\param right Instance to swap with");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "swap", "fun(self: sf.VertexBuffer, right: sf.VertexBuffer)");
    type_sf__VertexBuffer.set_function("swap",
        [](sf::VertexBuffer& self, sf::VertexBuffer& right) {
            self.swap(right);
        }
    );
    LUASF_STUB_DOC("\\brief Get the underlying OpenGL handle of the vertex buffer.\n\nYou shouldn't need to use this function, unless you have\nvery specific stuff to implement that SFML doesn't support,\nor implement a temporary workaround until a bug is fixed.\n\n\\return OpenGL handle of the vertex buffer or 0 if not yet created");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "getNativeHandle", "fun(self: sf.VertexBuffer): integer");
    type_sf__VertexBuffer.set_function("getNativeHandle",
        [](sf::VertexBuffer& self) -> unsigned int {
            return self.getNativeHandle();
        }
    );
    LUASF_STUB_DOC("\\brief Set the type of primitives to draw\n\nThis function defines how the vertices must be interpreted\nwhen it's time to draw them.\n\nThe default primitive type is `sf::PrimitiveType::Points`.\n\n\\param type Type of primitive");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "setPrimitiveType", "fun(self: sf.VertexBuffer, type: sf.PrimitiveType)");
    type_sf__VertexBuffer.set_function("setPrimitiveType",
        [](sf::VertexBuffer& self, sf::PrimitiveType type) {
            self.setPrimitiveType(type);
        }
    );
    LUASF_STUB_DOC("\\brief Get the type of primitives drawn by the vertex buffer\n\n\\return Primitive type");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "getPrimitiveType", "fun(self: sf.VertexBuffer): sf.PrimitiveType");
    type_sf__VertexBuffer.set_function("getPrimitiveType",
        [](sf::VertexBuffer& self) -> sf::PrimitiveType {
            return self.getPrimitiveType();
        }
    );
    LUASF_STUB_DOC("\\brief Set the usage specifier of this vertex buffer\n\nThis function provides a hint about how this vertex buffer is\ngoing to be used in terms of data update frequency.\n\nAfter changing the usage specifier, the vertex buffer has\nto be updated with new data for the usage specifier to\ntake effect.\n\nThe default usage type is `sf::VertexBuffer::Usage::Stream`.\n\n\\param usage Usage specifier");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "setUsage", "fun(self: sf.VertexBuffer, usage: sf.VertexBuffer.Usage)");
    type_sf__VertexBuffer.set_function("setUsage",
        [](sf::VertexBuffer& self, sf::VertexBuffer::Usage usage) {
            self.setUsage(usage);
        }
    );
    LUASF_STUB_DOC("\\brief Get the usage specifier of this vertex buffer\n\n\\return Usage specifier");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "getUsage", "fun(self: sf.VertexBuffer): sf.VertexBuffer.Usage");
    type_sf__VertexBuffer.set_function("getUsage",
        [](sf::VertexBuffer& self) -> sf::VertexBuffer::Usage {
            return self.getUsage();
        }
    );
    LUASF_STUB_DOC("\\brief Bind a vertex buffer for rendering\n\nThis function is not part of the graphics API, it mustn't be\nused when drawing SFML entities. It must be used only if you\nmix `sf::VertexBuffer` with OpenGL code.\n\n\\code\nsf::VertexBuffer vb1, vb2;\n...\nsf::VertexBuffer::bind(&vb1);\n// draw OpenGL stuff that use vb1...\nsf::VertexBuffer::bind(&vb2);\n// draw OpenGL stuff that use vb2...\nsf::VertexBuffer::bind(nullptr);\n// draw OpenGL stuff that use no vertex buffer...\n\\endcode\n\n\\param vertexBuffer Pointer to the vertex buffer to bind, can be null to use no vertex buffer");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "bind", "fun(vertexBuffer: sf.VertexBuffer)");
    type_sf__VertexBuffer.set_function("bind",
        [](const sf::VertexBuffer* vertexBuffer) {
            sf::VertexBuffer::bind(vertexBuffer);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether or not the system supports vertex buffers\n\nThis function should always be called before using\nthe vertex buffer features. If it returns `false`, then\nany attempt to use `sf::VertexBuffer` will fail.\n\n\\return `true` if vertex buffers are supported, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.VertexBuffer", "isAvailable", "fun(): boolean");
    type_sf__VertexBuffer.set_function("isAvailable",
        []() -> bool {
            return sf::VertexBuffer::isAvailable();
        }
    );
    LUASF_STUB_DOC("\\brief Usage specifiers\n\nIf data is going to be updated once or more every frame,\nset the usage to Stream. If data is going to be set once\nand used for a long time without being modified, set the\nusage to Static. For everything else Dynamic should be a\ngood compromise.");
    LUASF_STUB_CLASS("sf.VertexBuffer.Usage");
    LUASF_STUB_DOC("Constantly changing data");
    LUASF_STUB_FIELD("Stream", "sf.VertexBuffer.Usage");
    LUASF_STUB_DOC("Occasionally changing data");
    LUASF_STUB_FIELD("Dynamic", "sf.VertexBuffer.Usage");
    LUASF_STUB_DOC("Rarely changing data");
    LUASF_STUB_FIELD("Static", "sf.VertexBuffer.Usage");
    table_sf__VertexBuffer.new_enum("Usage",
        "Stream", sf::VertexBuffer::Usage::Stream,
        "Dynamic", sf::VertexBuffer::Usage::Dynamic,
        "Static", sf::VertexBuffer::Usage::Static
    );
    LUASF_STUB_DOC("\\brief Swap the contents of one vertex buffer with those of another\n\n\\param left First instance to swap\n\\param right Second instance to swap");
    LUASF_STUB_FUNCTION("sf", "swap", "fun(left: sf.VertexBuffer, right: sf.VertexBuffer)");
    sf.set_function("swap",
        [](sf::VertexBuffer& left, sf::VertexBuffer& right) {
            sf::swap(left, right);
        }
    );
}
