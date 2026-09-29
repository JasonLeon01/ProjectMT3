#include "Graphics/bind_Glsl.hpp"

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

namespace { constexpr std::array<std::string_view, 20> docs = {
    "\\brief Class template for manipulating\n2-dimensional vectors",
    "X coordinate of the vector",
    "Y coordinate of the vector",
    "\\brief Default constructor\n\nCreates a `Vector2(0, 0)`.",
    "\\brief Construct the vector from cartesian coordinates\n\n\\param x X coordinate\n\\param y Y coordinate",
    "\\brief Utility template class for manipulating\n3-dimensional vectors",
    "Z coordinate of the vector",
    "\\brief Default constructor\n\nCreates a `Vector3(0, 0, 0)`.",
    "\\brief Construct the vector from its coordinates\n\n\\param x X coordinate\n\\param y Y coordinate\n\\param z Z coordinate",
    "\\brief 4D vector type, used to set uniforms in GLSL",
    "1st component (X) of the 4D vector",
    "2nd component (Y) of the 4D vector",
    "3rd component (Z) of the 4D vector",
    "4th component (W) of the 4D vector",
    "\\brief Default constructor, creates a zero vector",
    "",
    "\\brief Construct vector implicitly from color\n\nVector is normalized to [0, 1] for floats, and left as-is\nfor ints. Not defined for other template arguments.\n\n\\param color Color instance",
    "\\brief Matrix type, used to set uniforms in GLSL",
    "Array holding matrix data",
    "\\brief Construct implicitly from SFML transform\n\nThis constructor is only supported for 3x3 and 4x4\nmatrices.\n\n\\param transform Object containing a transform.",
}; }

void bind_Glsl(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    LUASF_STUB_VALUE("sf", "Vec2", "sf.Vector2f");
    sf["Vec2"] = lua["sf"]["Vector2f"].get<lua_glue::Table>();
    LUASF_STUB_VALUE("sf", "Ivec2", "sf.Vector2i");
    sf["Ivec2"] = lua["sf"]["Vector2i"].get<lua_glue::Table>();
    auto type_sf__Vector2_bool_ = lua_glue::BindStruct<sf::Vector2<bool>>(sf, "Vector2b");
    lua_glue::Table table_sf__Vector2_bool_ = sf["Vector2b"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Vector2b");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("x", "boolean");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("y", "boolean");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Vector2b", "new", "fun(x: boolean, y: boolean): sf.Vector2b");
    LUASF_STUB_OVERLOAD("sf.Vector2b", "new", "fun(): sf.Vector2b");
    lua_glue::BindCallable(type_sf__Vector2_bool_, "new",
        [](bool x, bool y) {
            return sf::Vector2<bool>{x, y};
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Vector2_bool_, "new",
        []() {
            return sf::Vector2<bool>{};
        },
        docs[3]
    );
    lua_glue::BindAttr<bool>(type_sf__Vector2_bool_, "x", &sf::Vector2<bool>::x);
    lua_glue::BindAttr<bool>(type_sf__Vector2_bool_, "y", &sf::Vector2<bool>::y);
    LUASF_STUB_FUNCTION("sf.Vector2b", "unpack", "fun(self: sf.Vector2b): boolean, boolean");
    lua_glue::BindCallable(type_sf__Vector2_bool_, "unpack", [](const sf::Vector2<bool>& self) {
        return std::make_tuple(self.x, self.y);
    });
    lua_glue::BindMetamethod(type_sf__Vector2_bool_, "__tostring", [name = std::string("Vector2b")](const sf::Vector2<bool>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Vector2_bool_, "__eq", [](const sf::Vector2<bool>& left, const sf::Vector2<bool>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.Vector2b", "eq(sf.Vector2b): boolean");
    LUASF_STUB_FUNCTION("sf.Vector2b", "copy", "fun(self: sf.Vector2b): sf.Vector2b");
    LUASF_STUB_FUNCTION("sf.Vector2b", "deepcopy", "fun(self: sf.Vector2b): sf.Vector2b");
    LUASF_STUB_VALUE("sf", "Bvec2", "sf.Vector2b");
    sf["Bvec2"] = lua["sf"]["Vector2b"].get<lua_glue::Table>();
    LUASF_STUB_VALUE("sf", "Vec3", "sf.Vector3f");
    sf["Vec3"] = lua["sf"]["Vector3f"].get<lua_glue::Table>();
    LUASF_STUB_VALUE("sf", "Ivec3", "sf.Vector3i");
    sf["Ivec3"] = lua["sf"]["Vector3i"].get<lua_glue::Table>();
    auto type_sf__Vector3_bool_ = lua_glue::BindStruct<sf::Vector3<bool>>(sf, "Vector3b");
    lua_glue::Table table_sf__Vector3_bool_ = sf["Vector3b"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_CLASS("sf.Vector3b");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("x", "boolean");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("y", "boolean");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("z", "boolean");
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Vector3b", "new", "fun(x: boolean, y: boolean, z: boolean): sf.Vector3b");
    LUASF_STUB_OVERLOAD("sf.Vector3b", "new", "fun(): sf.Vector3b");
    lua_glue::BindCallable(type_sf__Vector3_bool_, "new",
        [](bool x, bool y, bool z) {
            return sf::Vector3<bool>{x, y, z};
        },
        docs[8]
    );
    lua_glue::BindCallable(type_sf__Vector3_bool_, "new",
        []() {
            return sf::Vector3<bool>{};
        },
        docs[7]
    );
    lua_glue::BindAttr<bool>(type_sf__Vector3_bool_, "x", &sf::Vector3<bool>::x);
    lua_glue::BindAttr<bool>(type_sf__Vector3_bool_, "y", &sf::Vector3<bool>::y);
    lua_glue::BindAttr<bool>(type_sf__Vector3_bool_, "z", &sf::Vector3<bool>::z);
    LUASF_STUB_FUNCTION("sf.Vector3b", "unpack", "fun(self: sf.Vector3b): boolean, boolean, boolean");
    lua_glue::BindCallable(type_sf__Vector3_bool_, "unpack", [](const sf::Vector3<bool>& self) {
        return std::make_tuple(self.x, self.y, self.z);
    });
    lua_glue::BindMetamethod(type_sf__Vector3_bool_, "__tostring", [name = std::string("Vector3b")](const sf::Vector3<bool>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Vector3_bool_, "__eq", [](const sf::Vector3<bool>& left, const sf::Vector3<bool>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.Vector3b", "eq(sf.Vector3b): boolean");
    LUASF_STUB_FUNCTION("sf.Vector3b", "copy", "fun(self: sf.Vector3b): sf.Vector3b");
    LUASF_STUB_FUNCTION("sf.Vector3b", "deepcopy", "fun(self: sf.Vector3b): sf.Vector3b");
    LUASF_STUB_VALUE("sf", "Bvec3", "sf.Vector3b");
    sf["Bvec3"] = lua["sf"]["Vector3b"].get<lua_glue::Table>();
    auto type_sf__priv__Vector4_float_ = lua_glue::BindStruct<sf::priv::Vector4<float>>(sf, "Vector4f");
    lua_glue::Table table_sf__priv__Vector4_float_ = sf["Vector4f"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_CLASS("sf.Vector4f");
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FIELD("x", "number");
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FIELD("y", "number");
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FIELD("z", "number");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FIELD("w", "number");
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Vector4f", "new", "fun(x: number, y: number, z: number, w: number): sf.Vector4f");
    LUASF_STUB_OVERLOAD("sf.Vector4f", "new", "fun(color: sf.Color): sf.Vector4f");
    LUASF_STUB_OVERLOAD("sf.Vector4f", "new", "fun(): sf.Vector4f");
    lua_glue::BindCallable(type_sf__priv__Vector4_float_, "new",
        [](float x, float y, float z, float w) {
            return sf::priv::Vector4<float>{x, y, z, w};
        },
        docs[15]
    );
    lua_glue::BindCallable(type_sf__priv__Vector4_float_, "new",
        [](sf::Color color) {
            return sf::priv::Vector4<float>{color};
        },
        docs[16]
    );
    lua_glue::BindCallable(type_sf__priv__Vector4_float_, "new",
        []() {
            return sf::priv::Vector4<float>{};
        },
        docs[14]
    );
    lua_glue::BindAttr<float>(type_sf__priv__Vector4_float_, "x", &sf::priv::Vector4<float>::x);
    lua_glue::BindAttr<float>(type_sf__priv__Vector4_float_, "y", &sf::priv::Vector4<float>::y);
    lua_glue::BindAttr<float>(type_sf__priv__Vector4_float_, "z", &sf::priv::Vector4<float>::z);
    lua_glue::BindAttr<float>(type_sf__priv__Vector4_float_, "w", &sf::priv::Vector4<float>::w);
    LUASF_STUB_FUNCTION("sf.Vector4f", "unpack", "fun(self: sf.Vector4f): number, number, number, number");
    lua_glue::BindCallable(type_sf__priv__Vector4_float_, "unpack", [](const sf::priv::Vector4<float>& self) {
        return std::make_tuple(self.x, self.y, self.z, self.w);
    });
    lua_glue::BindMetamethod(type_sf__priv__Vector4_float_, "__tostring", [name = std::string("Vector4f")](const sf::priv::Vector4<float>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ", " << self.w << ")";
        return stream.str();
    });
    LUASF_STUB_FUNCTION("sf.Vector4f", "copy", "fun(self: sf.Vector4f): sf.Vector4f");
    LUASF_STUB_FUNCTION("sf.Vector4f", "deepcopy", "fun(self: sf.Vector4f): sf.Vector4f");
    LUASF_STUB_VALUE("sf", "Vec4", "sf.Vector4f");
    sf["Vec4"] = lua["sf"]["Vector4f"].get<lua_glue::Table>();
    auto type_sf__priv__Vector4_int_ = lua_glue::BindStruct<sf::priv::Vector4<int>>(sf, "Vector4i");
    lua_glue::Table table_sf__priv__Vector4_int_ = sf["Vector4i"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_CLASS("sf.Vector4i");
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FIELD("z", "integer");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FIELD("w", "integer");
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Vector4i", "new", "fun(x: integer, y: integer, z: integer, w: integer): sf.Vector4i");
    LUASF_STUB_OVERLOAD("sf.Vector4i", "new", "fun(color: sf.Color): sf.Vector4i");
    LUASF_STUB_OVERLOAD("sf.Vector4i", "new", "fun(): sf.Vector4i");
    lua_glue::BindCallable(type_sf__priv__Vector4_int_, "new",
        [](lua_sf::LuaIntegral<int> x, lua_sf::LuaIntegral<int> y, lua_sf::LuaIntegral<int> z, lua_sf::LuaIntegral<int> w) {
            return sf::priv::Vector4<int>{x.value(), y.value(), z.value(), w.value()};
        },
        docs[15]
    );
    lua_glue::BindCallable(type_sf__priv__Vector4_int_, "new",
        [](sf::Color color) {
            return sf::priv::Vector4<int>{color};
        },
        docs[16]
    );
    lua_glue::BindCallable(type_sf__priv__Vector4_int_, "new",
        []() {
            return sf::priv::Vector4<int>{};
        },
        docs[14]
    );
    lua_glue::BindProperty(type_sf__priv__Vector4_int_, "x",
        [](const sf::priv::Vector4<int>& self) {
            return self.x;
        },
        [](sf::priv::Vector4<int>& self, lua_sf::LuaIntegral<int> value) {
            self.x = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__priv__Vector4_int_, "y",
        [](const sf::priv::Vector4<int>& self) {
            return self.y;
        },
        [](sf::priv::Vector4<int>& self, lua_sf::LuaIntegral<int> value) {
            self.y = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__priv__Vector4_int_, "z",
        [](const sf::priv::Vector4<int>& self) {
            return self.z;
        },
        [](sf::priv::Vector4<int>& self, lua_sf::LuaIntegral<int> value) {
            self.z = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__priv__Vector4_int_, "w",
        [](const sf::priv::Vector4<int>& self) {
            return self.w;
        },
        [](sf::priv::Vector4<int>& self, lua_sf::LuaIntegral<int> value) {
            self.w = value.value();
        }
    );
    LUASF_STUB_FUNCTION("sf.Vector4i", "unpack", "fun(self: sf.Vector4i): integer, integer, integer, integer");
    lua_glue::BindCallable(type_sf__priv__Vector4_int_, "unpack", [](const sf::priv::Vector4<int>& self) {
        return std::make_tuple(self.x, self.y, self.z, self.w);
    });
    lua_glue::BindMetamethod(type_sf__priv__Vector4_int_, "__tostring", [name = std::string("Vector4i")](const sf::priv::Vector4<int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ", " << self.w << ")";
        return stream.str();
    });
    LUASF_STUB_FUNCTION("sf.Vector4i", "copy", "fun(self: sf.Vector4i): sf.Vector4i");
    LUASF_STUB_FUNCTION("sf.Vector4i", "deepcopy", "fun(self: sf.Vector4i): sf.Vector4i");
    LUASF_STUB_VALUE("sf", "Ivec4", "sf.Vector4i");
    sf["Ivec4"] = lua["sf"]["Vector4i"].get<lua_glue::Table>();
    auto type_sf__priv__Vector4_bool_ = lua_glue::BindStruct<sf::priv::Vector4<bool>>(sf, "Vector4b");
    lua_glue::Table table_sf__priv__Vector4_bool_ = sf["Vector4b"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_CLASS("sf.Vector4b");
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FIELD("x", "boolean");
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FIELD("y", "boolean");
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FIELD("z", "boolean");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FIELD("w", "boolean");
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Vector4b", "new", "fun(x: boolean, y: boolean, z: boolean, w: boolean): sf.Vector4b");
    LUASF_STUB_OVERLOAD("sf.Vector4b", "new", "fun(): sf.Vector4b");
    lua_glue::BindCallable(type_sf__priv__Vector4_bool_, "new",
        [](bool x, bool y, bool z, bool w) {
            return sf::priv::Vector4<bool>{x, y, z, w};
        },
        docs[15]
    );
    lua_glue::BindCallable(type_sf__priv__Vector4_bool_, "new",
        []() {
            return sf::priv::Vector4<bool>{};
        },
        docs[14]
    );
    lua_glue::BindAttr<bool>(type_sf__priv__Vector4_bool_, "x", &sf::priv::Vector4<bool>::x);
    lua_glue::BindAttr<bool>(type_sf__priv__Vector4_bool_, "y", &sf::priv::Vector4<bool>::y);
    lua_glue::BindAttr<bool>(type_sf__priv__Vector4_bool_, "z", &sf::priv::Vector4<bool>::z);
    lua_glue::BindAttr<bool>(type_sf__priv__Vector4_bool_, "w", &sf::priv::Vector4<bool>::w);
    LUASF_STUB_FUNCTION("sf.Vector4b", "unpack", "fun(self: sf.Vector4b): boolean, boolean, boolean, boolean");
    lua_glue::BindCallable(type_sf__priv__Vector4_bool_, "unpack", [](const sf::priv::Vector4<bool>& self) {
        return std::make_tuple(self.x, self.y, self.z, self.w);
    });
    lua_glue::BindMetamethod(type_sf__priv__Vector4_bool_, "__tostring", [name = std::string("Vector4b")](const sf::priv::Vector4<bool>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ", " << self.w << ")";
        return stream.str();
    });
    LUASF_STUB_FUNCTION("sf.Vector4b", "copy", "fun(self: sf.Vector4b): sf.Vector4b");
    LUASF_STUB_FUNCTION("sf.Vector4b", "deepcopy", "fun(self: sf.Vector4b): sf.Vector4b");
    LUASF_STUB_VALUE("sf", "Bvec4", "sf.Vector4b");
    sf["Bvec4"] = lua["sf"]["Vector4b"].get<lua_glue::Table>();
    auto type_sf__priv__Matrix_3__3_ = lua_glue::BindStruct<sf::priv::Matrix<3, 3>>(sf, "Mat3");
    lua_glue::Table table_sf__priv__Matrix_3__3_ = sf["Mat3"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_CLASS("sf.Mat3");
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FIELD("array", "number[]");
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Mat3", "new", "fun(transform: sf.Transform): sf.Mat3");
    LUASF_STUB_OVERLOAD("sf.Mat3", "new", "fun(values: number[]): sf.Mat3");
    lua_glue::BindCallable(type_sf__priv__Matrix_3__3_, "new",
        [](const sf::Transform& transform) {
            return sf::priv::Matrix<3, 3>{transform};
        },
        docs[19]
    );
    lua_glue::BindCallable(type_sf__priv__Matrix_3__3_, "new",
        [](lua_glue::Table values) {
            auto buffer = lua_sf::array_from_object<float>(values);
            if (buffer.size() != 9)
                throw std::runtime_error("matrix constructor expects exactly 9 float values");
            return sf::priv::Matrix<3, 3>{buffer.data()};
        }
    );
    lua_glue::BindProperty(type_sf__priv__Matrix_3__3_, "array",
        [](const sf::priv::Matrix<3, 3>& self) {
            return lua_glue::AsTable(std::vector<float>(self.array.begin(), self.array.end()));
        },
        [](sf::priv::Matrix<3, 3>& self, lua_glue::Object values) {
            auto buffer = lua_sf::array_from_object<float>(values);
            if (buffer.size() != self.array.size())
                throw std::runtime_error("matrix array assignment has the wrong number of float values");
            std::copy(buffer.begin(), buffer.end(), self.array.begin());
        });
    LUASF_STUB_FUNCTION("sf.Mat3", "copyMatrix", "fun(source: sf.Transform, dest: sf.Mat3)");
    lua_glue::BindCallable(type_sf__priv__Matrix_3__3_, "copyMatrix", [](const sf::Transform& source, sf::priv::Matrix<3, 3>& dest) {
        sf::priv::copyMatrix(source, dest);
    });
    lua_glue::BindMetamethod(type_sf__priv__Matrix_3__3_, "__tostring", [name = std::string("Mat3")](const sf::priv::Matrix<3, 3>& self) {
        std::ostringstream stream;
        stream << name << "(";
        for (std::size_t index = 0; index < self.array.size(); ++index) {
            if (index != 0)
                stream << ", ";
            stream << self.array[index];
        }
        stream << ")";
        return stream.str();
    });
    LUASF_STUB_FUNCTION("sf.Mat3", "copy", "fun(self: sf.Mat3): sf.Mat3");
    LUASF_STUB_FUNCTION("sf.Mat3", "deepcopy", "fun(self: sf.Mat3): sf.Mat3");
    auto type_sf__priv__Matrix_4__4_ = lua_glue::BindStruct<sf::priv::Matrix<4, 4>>(sf, "Mat4");
    lua_glue::Table table_sf__priv__Matrix_4__4_ = sf["Mat4"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_CLASS("sf.Mat4");
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FIELD("array", "number[]");
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Mat4", "new", "fun(transform: sf.Transform): sf.Mat4");
    LUASF_STUB_OVERLOAD("sf.Mat4", "new", "fun(values: number[]): sf.Mat4");
    lua_glue::BindCallable(type_sf__priv__Matrix_4__4_, "new",
        [](const sf::Transform& transform) {
            return sf::priv::Matrix<4, 4>{transform};
        },
        docs[19]
    );
    lua_glue::BindCallable(type_sf__priv__Matrix_4__4_, "new",
        [](lua_glue::Table values) {
            auto buffer = lua_sf::array_from_object<float>(values);
            if (buffer.size() != 16)
                throw std::runtime_error("matrix constructor expects exactly 16 float values");
            return sf::priv::Matrix<4, 4>{buffer.data()};
        }
    );
    lua_glue::BindProperty(type_sf__priv__Matrix_4__4_, "array",
        [](const sf::priv::Matrix<4, 4>& self) {
            return lua_glue::AsTable(std::vector<float>(self.array.begin(), self.array.end()));
        },
        [](sf::priv::Matrix<4, 4>& self, lua_glue::Object values) {
            auto buffer = lua_sf::array_from_object<float>(values);
            if (buffer.size() != self.array.size())
                throw std::runtime_error("matrix array assignment has the wrong number of float values");
            std::copy(buffer.begin(), buffer.end(), self.array.begin());
        });
    LUASF_STUB_FUNCTION("sf.Mat4", "copyMatrix", "fun(source: sf.Transform, dest: sf.Mat4)");
    lua_glue::BindCallable(type_sf__priv__Matrix_4__4_, "copyMatrix", [](const sf::Transform& source, sf::priv::Matrix<4, 4>& dest) {
        sf::priv::copyMatrix(source, dest);
    });
    lua_glue::BindMetamethod(type_sf__priv__Matrix_4__4_, "__tostring", [name = std::string("Mat4")](const sf::priv::Matrix<4, 4>& self) {
        std::ostringstream stream;
        stream << name << "(";
        for (std::size_t index = 0; index < self.array.size(); ++index) {
            if (index != 0)
                stream << ", ";
            stream << self.array[index];
        }
        stream << ")";
        return stream.str();
    });
    LUASF_STUB_FUNCTION("sf.Mat4", "copy", "fun(self: sf.Mat4): sf.Mat4");
    LUASF_STUB_FUNCTION("sf.Mat4", "deepcopy", "fun(self: sf.Mat4): sf.Mat4");
    // Skipped namespace priv by generator policy.
}
