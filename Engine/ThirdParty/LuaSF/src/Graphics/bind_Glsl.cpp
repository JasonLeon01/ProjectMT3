#include "Graphics/bind_Glsl.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Glsl(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    // Skipped namespace priv by generator policy.
    LUASF_STUB_VALUE("sf", "Vec2", "sf.Vector2f");
    sf["Vec2"] = lua["sf"]["Vector2f"].get<sol::table>();
    LUASF_STUB_VALUE("sf", "Ivec2", "sf.Vector2i");
    sf["Ivec2"] = lua["sf"]["Vector2i"].get<sol::table>();
    auto type_sf__Vector2_bool_ = sf.new_usertype<sf::Vector2<bool>>("Vector2b", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Vector2_bool_);
    sol::table table_sf__Vector2_bool_ = sf["Vector2b"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Class template for manipulating\n2-dimensional vectors");
    LUASF_STUB_CLASS("sf.Vector2b");
    LUASF_STUB_DOC("X coordinate of the vector");
    LUASF_STUB_FIELD("x", "boolean");
    LUASF_STUB_DOC("Y coordinate of the vector");
    LUASF_STUB_FIELD("y", "boolean");
    LUASF_STUB_DOC("\\brief Construct the vector from cartesian coordinates\n\n\\param x X coordinate\n\\param y Y coordinate");
    LUASF_STUB_FUNCTION("sf.Vector2b", "new", "fun(x: boolean, y: boolean): sf.Vector2b");
    LUASF_STUB_OVERLOAD("sf.Vector2b", "new", "fun(): sf.Vector2b");
    type_sf__Vector2_bool_.set_function("new", sol::factories(
        [](bool x, bool y) {
            return sf::Vector2<bool>{x, y};
        },
        []() {
            return sf::Vector2<bool>{};
        }
    ));
    type_sf__Vector2_bool_["x"] = sol::policies(&sf::Vector2<bool>::x, sol::self_dependency{});
    type_sf__Vector2_bool_["y"] = sol::policies(&sf::Vector2<bool>::y, sol::self_dependency{});
    LUASF_STUB_FUNCTION("sf.Vector2b", "unpack", "fun(self: sf.Vector2b): boolean, boolean");
    type_sf__Vector2_bool_.set_function("unpack", [](const sf::Vector2<bool>& self) {
        return std::make_tuple(self.x, self.y);
    });
    type_sf__Vector2_bool_[sol::meta_function::to_string] = [name = std::string("Vector2b")](const sf::Vector2<bool>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ")";
        return stream.str();
    };
    type_sf__Vector2_bool_[sol::meta_function::equal_to] = [](const sf::Vector2<bool>& left, const sf::Vector2<bool>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.Vector2b", "eq(sf.Vector2b): boolean");
    LUASF_STUB_VALUE("sf", "Bvec2", "sf.Vector2b");
    sf["Bvec2"] = lua["sf"]["Vector2b"].get<sol::table>();
    LUASF_STUB_VALUE("sf", "Vec3", "sf.Vector3f");
    sf["Vec3"] = lua["sf"]["Vector3f"].get<sol::table>();
    LUASF_STUB_VALUE("sf", "Ivec3", "sf.Vector3i");
    sf["Ivec3"] = lua["sf"]["Vector3i"].get<sol::table>();
    auto type_sf__Vector3_bool_ = sf.new_usertype<sf::Vector3<bool>>("Vector3b", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Vector3_bool_);
    sol::table table_sf__Vector3_bool_ = sf["Vector3b"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Utility template class for manipulating\n3-dimensional vectors");
    LUASF_STUB_CLASS("sf.Vector3b");
    LUASF_STUB_DOC("X coordinate of the vector");
    LUASF_STUB_FIELD("x", "boolean");
    LUASF_STUB_DOC("Y coordinate of the vector");
    LUASF_STUB_FIELD("y", "boolean");
    LUASF_STUB_DOC("Z coordinate of the vector");
    LUASF_STUB_FIELD("z", "boolean");
    LUASF_STUB_DOC("\\brief Construct the vector from its coordinates\n\n\\param x X coordinate\n\\param y Y coordinate\n\\param z Z coordinate");
    LUASF_STUB_FUNCTION("sf.Vector3b", "new", "fun(x: boolean, y: boolean, z: boolean): sf.Vector3b");
    LUASF_STUB_OVERLOAD("sf.Vector3b", "new", "fun(): sf.Vector3b");
    type_sf__Vector3_bool_.set_function("new", sol::factories(
        [](bool x, bool y, bool z) {
            return sf::Vector3<bool>{x, y, z};
        },
        []() {
            return sf::Vector3<bool>{};
        }
    ));
    type_sf__Vector3_bool_["x"] = sol::policies(&sf::Vector3<bool>::x, sol::self_dependency{});
    type_sf__Vector3_bool_["y"] = sol::policies(&sf::Vector3<bool>::y, sol::self_dependency{});
    type_sf__Vector3_bool_["z"] = sol::policies(&sf::Vector3<bool>::z, sol::self_dependency{});
    LUASF_STUB_FUNCTION("sf.Vector3b", "unpack", "fun(self: sf.Vector3b): boolean, boolean, boolean");
    type_sf__Vector3_bool_.set_function("unpack", [](const sf::Vector3<bool>& self) {
        return std::make_tuple(self.x, self.y, self.z);
    });
    type_sf__Vector3_bool_[sol::meta_function::to_string] = [name = std::string("Vector3b")](const sf::Vector3<bool>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ")";
        return stream.str();
    };
    type_sf__Vector3_bool_[sol::meta_function::equal_to] = [](const sf::Vector3<bool>& left, const sf::Vector3<bool>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.Vector3b", "eq(sf.Vector3b): boolean");
    LUASF_STUB_VALUE("sf", "Bvec3", "sf.Vector3b");
    sf["Bvec3"] = lua["sf"]["Vector3b"].get<sol::table>();
    auto type_sf__priv__Vector4_float_ = sf.new_usertype<sf::priv::Vector4<float>>("Vector4f", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__priv__Vector4_float_);
    sol::table table_sf__priv__Vector4_float_ = sf["Vector4f"].get<sol::table>();
    LUASF_STUB_DOC("\\brief 4D vector type, used to set uniforms in GLSL");
    LUASF_STUB_CLASS("sf.Vector4f");
    LUASF_STUB_DOC("1st component (X) of the 4D vector");
    LUASF_STUB_FIELD("x", "number");
    LUASF_STUB_DOC("2nd component (Y) of the 4D vector");
    LUASF_STUB_FIELD("y", "number");
    LUASF_STUB_DOC("3rd component (Z) of the 4D vector");
    LUASF_STUB_FIELD("z", "number");
    LUASF_STUB_DOC("4th component (W) of the 4D vector");
    LUASF_STUB_FIELD("w", "number");
    LUASF_STUB_DOC("\\brief Construct vector implicitly from color\n\nVector is normalized to [0, 1] for floats, and left as-is\nfor ints. Not defined for other template arguments.\n\n\\param color Color instance");
    LUASF_STUB_FUNCTION("sf.Vector4f", "new", "fun(x: number, y: number, z: number, w: number): sf.Vector4f");
    LUASF_STUB_OVERLOAD("sf.Vector4f", "new", "fun(color: sf.Color): sf.Vector4f");
    LUASF_STUB_OVERLOAD("sf.Vector4f", "new", "fun(): sf.Vector4f");
    type_sf__priv__Vector4_float_.set_function("new", sol::factories(
        [](float x, float y, float z, float w) {
            return sf::priv::Vector4<float>{x, y, z, w};
        },
        [](sf::Color color) {
            return sf::priv::Vector4<float>{color};
        },
        []() {
            return sf::priv::Vector4<float>{};
        }
    ));
    type_sf__priv__Vector4_float_["x"] = sol::policies(&sf::priv::Vector4<float>::x, sol::self_dependency{});
    type_sf__priv__Vector4_float_["y"] = sol::policies(&sf::priv::Vector4<float>::y, sol::self_dependency{});
    type_sf__priv__Vector4_float_["z"] = sol::policies(&sf::priv::Vector4<float>::z, sol::self_dependency{});
    type_sf__priv__Vector4_float_["w"] = sol::policies(&sf::priv::Vector4<float>::w, sol::self_dependency{});
    LUASF_STUB_FUNCTION("sf.Vector4f", "unpack", "fun(self: sf.Vector4f): number, number, number, number");
    type_sf__priv__Vector4_float_.set_function("unpack", [](const sf::priv::Vector4<float>& self) {
        return std::make_tuple(self.x, self.y, self.z, self.w);
    });
    type_sf__priv__Vector4_float_[sol::meta_function::to_string] = [name = std::string("Vector4f")](const sf::priv::Vector4<float>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ", " << self.w << ")";
        return stream.str();
    };
    LUASF_STUB_VALUE("sf", "Vec4", "sf.Vector4f");
    sf["Vec4"] = lua["sf"]["Vector4f"].get<sol::table>();
    auto type_sf__priv__Vector4_int_ = sf.new_usertype<sf::priv::Vector4<int>>("Vector4i", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__priv__Vector4_int_);
    sol::table table_sf__priv__Vector4_int_ = sf["Vector4i"].get<sol::table>();
    LUASF_STUB_DOC("\\brief 4D vector type, used to set uniforms in GLSL");
    LUASF_STUB_CLASS("sf.Vector4i");
    LUASF_STUB_DOC("1st component (X) of the 4D vector");
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC("2nd component (Y) of the 4D vector");
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC("3rd component (Z) of the 4D vector");
    LUASF_STUB_FIELD("z", "integer");
    LUASF_STUB_DOC("4th component (W) of the 4D vector");
    LUASF_STUB_FIELD("w", "integer");
    LUASF_STUB_DOC("\\brief Construct vector implicitly from color\n\nVector is normalized to [0, 1] for floats, and left as-is\nfor ints. Not defined for other template arguments.\n\n\\param color Color instance");
    LUASF_STUB_FUNCTION("sf.Vector4i", "new", "fun(x: integer, y: integer, z: integer, w: integer): sf.Vector4i");
    LUASF_STUB_OVERLOAD("sf.Vector4i", "new", "fun(color: sf.Color): sf.Vector4i");
    LUASF_STUB_OVERLOAD("sf.Vector4i", "new", "fun(): sf.Vector4i");
    type_sf__priv__Vector4_int_.set_function("new", sol::factories(
        [](lua_sf::LuaIntegral<int> x, lua_sf::LuaIntegral<int> y, lua_sf::LuaIntegral<int> z, lua_sf::LuaIntegral<int> w) {
            return sf::priv::Vector4<int>{x.value(), y.value(), z.value(), w.value()};
        },
        [](sf::Color color) {
            return sf::priv::Vector4<int>{color};
        },
        []() {
            return sf::priv::Vector4<int>{};
        }
    ));
    type_sf__priv__Vector4_int_.set("x", sol::property(
        [](sf::priv::Vector4<int>& self) {
            return self.x;
        },
        [](sf::priv::Vector4<int>& self, lua_sf::LuaIntegral<int> value) {
            self.x = value.value();
        }
    ));
    type_sf__priv__Vector4_int_.set("y", sol::property(
        [](sf::priv::Vector4<int>& self) {
            return self.y;
        },
        [](sf::priv::Vector4<int>& self, lua_sf::LuaIntegral<int> value) {
            self.y = value.value();
        }
    ));
    type_sf__priv__Vector4_int_.set("z", sol::property(
        [](sf::priv::Vector4<int>& self) {
            return self.z;
        },
        [](sf::priv::Vector4<int>& self, lua_sf::LuaIntegral<int> value) {
            self.z = value.value();
        }
    ));
    type_sf__priv__Vector4_int_.set("w", sol::property(
        [](sf::priv::Vector4<int>& self) {
            return self.w;
        },
        [](sf::priv::Vector4<int>& self, lua_sf::LuaIntegral<int> value) {
            self.w = value.value();
        }
    ));
    LUASF_STUB_FUNCTION("sf.Vector4i", "unpack", "fun(self: sf.Vector4i): integer, integer, integer, integer");
    type_sf__priv__Vector4_int_.set_function("unpack", [](const sf::priv::Vector4<int>& self) {
        return std::make_tuple(self.x, self.y, self.z, self.w);
    });
    type_sf__priv__Vector4_int_[sol::meta_function::to_string] = [name = std::string("Vector4i")](const sf::priv::Vector4<int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ", " << self.w << ")";
        return stream.str();
    };
    LUASF_STUB_VALUE("sf", "Ivec4", "sf.Vector4i");
    sf["Ivec4"] = lua["sf"]["Vector4i"].get<sol::table>();
    auto type_sf__priv__Vector4_bool_ = sf.new_usertype<sf::priv::Vector4<bool>>("Vector4b", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__priv__Vector4_bool_);
    sol::table table_sf__priv__Vector4_bool_ = sf["Vector4b"].get<sol::table>();
    LUASF_STUB_DOC("\\brief 4D vector type, used to set uniforms in GLSL");
    LUASF_STUB_CLASS("sf.Vector4b");
    LUASF_STUB_DOC("1st component (X) of the 4D vector");
    LUASF_STUB_FIELD("x", "boolean");
    LUASF_STUB_DOC("2nd component (Y) of the 4D vector");
    LUASF_STUB_FIELD("y", "boolean");
    LUASF_STUB_DOC("3rd component (Z) of the 4D vector");
    LUASF_STUB_FIELD("z", "boolean");
    LUASF_STUB_DOC("4th component (W) of the 4D vector");
    LUASF_STUB_FIELD("w", "boolean");
    LUASF_STUB_DOC("\\brief Default constructor, creates a zero vector");
    LUASF_STUB_FUNCTION("sf.Vector4b", "new", "fun(x: boolean, y: boolean, z: boolean, w: boolean): sf.Vector4b");
    LUASF_STUB_OVERLOAD("sf.Vector4b", "new", "fun(): sf.Vector4b");
    type_sf__priv__Vector4_bool_.set_function("new", sol::factories(
        [](bool x, bool y, bool z, bool w) {
            return sf::priv::Vector4<bool>{x, y, z, w};
        },
        []() {
            return sf::priv::Vector4<bool>{};
        }
    ));
    type_sf__priv__Vector4_bool_["x"] = sol::policies(&sf::priv::Vector4<bool>::x, sol::self_dependency{});
    type_sf__priv__Vector4_bool_["y"] = sol::policies(&sf::priv::Vector4<bool>::y, sol::self_dependency{});
    type_sf__priv__Vector4_bool_["z"] = sol::policies(&sf::priv::Vector4<bool>::z, sol::self_dependency{});
    type_sf__priv__Vector4_bool_["w"] = sol::policies(&sf::priv::Vector4<bool>::w, sol::self_dependency{});
    LUASF_STUB_FUNCTION("sf.Vector4b", "unpack", "fun(self: sf.Vector4b): boolean, boolean, boolean, boolean");
    type_sf__priv__Vector4_bool_.set_function("unpack", [](const sf::priv::Vector4<bool>& self) {
        return std::make_tuple(self.x, self.y, self.z, self.w);
    });
    type_sf__priv__Vector4_bool_[sol::meta_function::to_string] = [name = std::string("Vector4b")](const sf::priv::Vector4<bool>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ", " << self.w << ")";
        return stream.str();
    };
    LUASF_STUB_VALUE("sf", "Bvec4", "sf.Vector4b");
    sf["Bvec4"] = lua["sf"]["Vector4b"].get<sol::table>();
    auto type_sf__priv__Matrix_3__3_ = sf.new_usertype<sf::priv::Matrix<3, 3>>("Mat3", sol::no_constructor);
    sol::table table_sf__priv__Matrix_3__3_ = sf["Mat3"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Matrix type, used to set uniforms in GLSL");
    LUASF_STUB_CLASS("sf.Mat3");
    LUASF_STUB_DOC("Array holding matrix data");
    LUASF_STUB_FIELD("array", "number[]");
    LUASF_STUB_DOC("\\brief Construct implicitly from SFML transform\n\nThis constructor is only supported for 3x3 and 4x4\nmatrices.\n\n\\param transform Object containing a transform.");
    LUASF_STUB_FUNCTION("sf.Mat3", "new", "fun(transform: sf.Transform): sf.Mat3");
    LUASF_STUB_OVERLOAD("sf.Mat3", "new", "fun(values: number[]): sf.Mat3");
    type_sf__priv__Matrix_3__3_.set_function("new", sol::factories(
        [](const sf::Transform& transform) {
            return sf::priv::Matrix<3, 3>{transform};
        },
        [](sol::table values) {
            auto buffer = lua_sf::array_from_object<float>(values);
            if (buffer.size() != 9)
                throw std::runtime_error("matrix constructor expects exactly 9 float values");
            return sf::priv::Matrix<3, 3>{buffer.data()};
        }
    ));
    type_sf__priv__Matrix_3__3_.set("array", sol::property(
        [](const sf::priv::Matrix<3, 3>& self) {
            return sol::as_table(std::vector<float>(self.array.begin(), self.array.end()));
        },
        [](sf::priv::Matrix<3, 3>& self, sol::object values) {
            auto buffer = lua_sf::array_from_object<float>(values);
            if (buffer.size() != self.array.size())
                throw std::runtime_error("matrix array assignment has the wrong number of float values");
            std::copy(buffer.begin(), buffer.end(), self.array.begin());
        }));
    LUASF_STUB_FUNCTION("sf.Mat3", "copyMatrix", "fun(source: sf.Transform, dest: sf.Mat3)");
    type_sf__priv__Matrix_3__3_.set_function("copyMatrix", [](const sf::Transform& source, sf::priv::Matrix<3, 3>& dest) {
        sf::priv::copyMatrix(source, dest);
    });
    type_sf__priv__Matrix_3__3_[sol::meta_function::to_string] = [name = std::string("Mat3")](const sf::priv::Matrix<3, 3>& self) {
        std::ostringstream stream;
        stream << name << "(";
        for (std::size_t index = 0; index < self.array.size(); ++index) {
            if (index != 0)
                stream << ", ";
            stream << self.array[index];
        }
        stream << ")";
        return stream.str();
    };
    auto type_sf__priv__Matrix_4__4_ = sf.new_usertype<sf::priv::Matrix<4, 4>>("Mat4", sol::no_constructor);
    sol::table table_sf__priv__Matrix_4__4_ = sf["Mat4"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Matrix type, used to set uniforms in GLSL");
    LUASF_STUB_CLASS("sf.Mat4");
    LUASF_STUB_DOC("Array holding matrix data");
    LUASF_STUB_FIELD("array", "number[]");
    LUASF_STUB_DOC("\\brief Construct implicitly from SFML transform\n\nThis constructor is only supported for 3x3 and 4x4\nmatrices.\n\n\\param transform Object containing a transform.");
    LUASF_STUB_FUNCTION("sf.Mat4", "new", "fun(transform: sf.Transform): sf.Mat4");
    LUASF_STUB_OVERLOAD("sf.Mat4", "new", "fun(values: number[]): sf.Mat4");
    type_sf__priv__Matrix_4__4_.set_function("new", sol::factories(
        [](const sf::Transform& transform) {
            return sf::priv::Matrix<4, 4>{transform};
        },
        [](sol::table values) {
            auto buffer = lua_sf::array_from_object<float>(values);
            if (buffer.size() != 16)
                throw std::runtime_error("matrix constructor expects exactly 16 float values");
            return sf::priv::Matrix<4, 4>{buffer.data()};
        }
    ));
    type_sf__priv__Matrix_4__4_.set("array", sol::property(
        [](const sf::priv::Matrix<4, 4>& self) {
            return sol::as_table(std::vector<float>(self.array.begin(), self.array.end()));
        },
        [](sf::priv::Matrix<4, 4>& self, sol::object values) {
            auto buffer = lua_sf::array_from_object<float>(values);
            if (buffer.size() != self.array.size())
                throw std::runtime_error("matrix array assignment has the wrong number of float values");
            std::copy(buffer.begin(), buffer.end(), self.array.begin());
        }));
    LUASF_STUB_FUNCTION("sf.Mat4", "copyMatrix", "fun(source: sf.Transform, dest: sf.Mat4)");
    type_sf__priv__Matrix_4__4_.set_function("copyMatrix", [](const sf::Transform& source, sf::priv::Matrix<4, 4>& dest) {
        sf::priv::copyMatrix(source, dest);
    });
    type_sf__priv__Matrix_4__4_[sol::meta_function::to_string] = [name = std::string("Mat4")](const sf::priv::Matrix<4, 4>& self) {
        std::ostringstream stream;
        stream << name << "(";
        for (std::size_t index = 0; index < self.array.size(); ++index) {
            if (index != 0)
                stream << ", ";
            stream << self.array[index];
        }
        stream << ")";
        return stream.str();
    };
    // Skipped namespace priv by generator policy.
}
