#include "System/bind_Vector3.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Vector3(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Vector3_int_ = sf.new_usertype<sf::Vector3<int>>("Vector3i", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Vector3_int_);
    sol::table table_sf__Vector3_int_ = sf["Vector3i"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Utility template class for manipulating\n3-dimensional vectors");
    LUASF_STUB_CLASS("sf.Vector3i");
    LUASF_STUB_DOC("X coordinate of the vector");
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC("Y coordinate of the vector");
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC("Z coordinate of the vector");
    LUASF_STUB_FIELD("z", "integer");
    LUASF_STUB_DOC("\\brief Construct the vector from its coordinates\n\n\\param x X coordinate\n\\param y Y coordinate\n\\param z Z coordinate");
    LUASF_STUB_FUNCTION("sf.Vector3i", "new", "fun(x: integer, y: integer, z: integer): sf.Vector3i");
    LUASF_STUB_OVERLOAD("sf.Vector3i", "new", "fun(): sf.Vector3i");
    type_sf__Vector3_int_.set_function("new", sol::factories(
        [](lua_sf::LuaIntegral<int> x, lua_sf::LuaIntegral<int> y, lua_sf::LuaIntegral<int> z) {
            return sf::Vector3<int>{x.value(), y.value(), z.value()};
        },
        []() {
            return sf::Vector3<int>{};
        }
    ));
    type_sf__Vector3_int_.set("x", sol::property(
        [](sf::Vector3<int>& self) {
            return self.x;
        },
        [](sf::Vector3<int>& self, lua_sf::LuaIntegral<int> value) {
            self.x = value.value();
        }
    ));
    type_sf__Vector3_int_.set("y", sol::property(
        [](sf::Vector3<int>& self) {
            return self.y;
        },
        [](sf::Vector3<int>& self, lua_sf::LuaIntegral<int> value) {
            self.y = value.value();
        }
    ));
    type_sf__Vector3_int_.set("z", sol::property(
        [](sf::Vector3<int>& self) {
            return self.z;
        },
        [](sf::Vector3<int>& self, lua_sf::LuaIntegral<int> value) {
            self.z = value.value();
        }
    ));
    LUASF_STUB_DOC("\\brief Square of vector's length.\n\nSuitable for comparisons, more efficient than `length()`.");
    LUASF_STUB_FUNCTION("sf.Vector3i", "lengthSquared", "fun(self: sf.Vector3i): integer");
    type_sf__Vector3_int_.set_function("lengthSquared",
        [](sf::Vector3<int>& self) -> int {
            return self.lengthSquared();
        }
    );
    LUASF_STUB_DOC("\\brief Dot product of two 3D vectors.");
    LUASF_STUB_FUNCTION("sf.Vector3i", "dot", "fun(self: sf.Vector3i, rhs: sf.Vector3i): integer");
    type_sf__Vector3_int_.set_function("dot",
        [](sf::Vector3<int>& self, const sf::Vector3<int>& rhs) -> int {
            return self.dot(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Cross product of two 3D vectors.");
    LUASF_STUB_FUNCTION("sf.Vector3i", "cross", "fun(self: sf.Vector3i, rhs: sf.Vector3i): sf.Vector3i");
    type_sf__Vector3_int_.set_function("cross",
        [](sf::Vector3<int>& self, const sf::Vector3<int>& rhs) -> sf::Vector3<int> {
            return self.cross(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise multiplication of `*this` and `rhs`.\n\nComputes `(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z)`.\n\nScaling is the most common use case for component-wise multiplication/division.\nThis operation is also known as the Hadamard or Schur product.");
    LUASF_STUB_FUNCTION("sf.Vector3i", "componentWiseMul", "fun(self: sf.Vector3i, rhs: sf.Vector3i): sf.Vector3i");
    type_sf__Vector3_int_.set_function("componentWiseMul",
        [](sf::Vector3<int>& self, const sf::Vector3<int>& rhs) -> sf::Vector3<int> {
            return self.componentWiseMul(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise division of `*this` and `rhs`.\n\nComputes `(lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z)`.\n\nScaling is the most common use case for component-wise multiplication/division.\n\n\\pre Neither component of `rhs` is zero.");
    LUASF_STUB_FUNCTION("sf.Vector3i", "componentWiseDiv", "fun(self: sf.Vector3i, rhs: sf.Vector3i): sf.Vector3i");
    type_sf__Vector3_int_.set_function("componentWiseDiv",
        [](sf::Vector3<int>& self, const sf::Vector3<int>& rhs) -> sf::Vector3<int> {
            return self.componentWiseDiv(rhs);
        }
    );
    LUASF_STUB_FUNCTION("sf.Vector3i", "unpack", "fun(self: sf.Vector3i): integer, integer, integer");
    type_sf__Vector3_int_.set_function("unpack", [](const sf::Vector3<int>& self) {
        return std::make_tuple(self.x, self.y, self.z);
    });
    type_sf__Vector3_int_[sol::meta_function::to_string] = [name = std::string("Vector3i")](const sf::Vector3<int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ")";
        return stream.str();
    };
    type_sf__Vector3_int_[sol::meta_function::unary_minus] = [](const sf::Vector3<int>& value) { return -value; };
    LUASF_STUB_OPERATOR("sf.Vector3i", "unm: sf.Vector3i");
    type_sf__Vector3_int_[sol::meta_function::addition] = [](const sf::Vector3<int>& left, const sf::Vector3<int>& right) { return left + right; };
    LUASF_STUB_OPERATOR("sf.Vector3i", "add(sf.Vector3i): sf.Vector3i");
    type_sf__Vector3_int_[sol::meta_function::subtraction] = [](const sf::Vector3<int>& left, const sf::Vector3<int>& right) { return left - right; };
    LUASF_STUB_OPERATOR("sf.Vector3i", "sub(sf.Vector3i): sf.Vector3i");
    type_sf__Vector3_int_[sol::meta_function::multiplication] = sol::overload(
        [](sf::Vector3<int> value, lua_sf::LuaIntegral<int> scalar) { return value * scalar.value(); },
        [](lua_sf::LuaIntegral<int> scalar, sf::Vector3<int> value) { return scalar.value() * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector3i", "mul(integer): sf.Vector3i");
    type_sf__Vector3_int_[sol::meta_function::division] = [](sf::Vector3<int> value, lua_sf::LuaIntegral<int> scalar) { return value / scalar.value(); };
    LUASF_STUB_OPERATOR("sf.Vector3i", "div(integer): sf.Vector3i");
    type_sf__Vector3_int_[sol::meta_function::equal_to] = [](const sf::Vector3<int>& left, const sf::Vector3<int>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.Vector3i", "eq(sf.Vector3i): boolean");
    auto type_sf__Vector3_unsigned_int_ = sf.new_usertype<sf::Vector3<unsigned int>>("Vector3u", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Vector3_unsigned_int_);
    sol::table table_sf__Vector3_unsigned_int_ = sf["Vector3u"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Utility template class for manipulating\n3-dimensional vectors");
    LUASF_STUB_CLASS("sf.Vector3u");
    LUASF_STUB_DOC("X coordinate of the vector");
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC("Y coordinate of the vector");
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC("Z coordinate of the vector");
    LUASF_STUB_FIELD("z", "integer");
    LUASF_STUB_DOC("\\brief Construct the vector from its coordinates\n\n\\param x X coordinate\n\\param y Y coordinate\n\\param z Z coordinate");
    LUASF_STUB_FUNCTION("sf.Vector3u", "new", "fun(x: integer, y: integer, z: integer): sf.Vector3u");
    LUASF_STUB_OVERLOAD("sf.Vector3u", "new", "fun(): sf.Vector3u");
    type_sf__Vector3_unsigned_int_.set_function("new", sol::factories(
        [](lua_sf::LuaIntegral<unsigned int> x, lua_sf::LuaIntegral<unsigned int> y, lua_sf::LuaIntegral<unsigned int> z) {
            return sf::Vector3<unsigned int>{x.value(), y.value(), z.value()};
        },
        []() {
            return sf::Vector3<unsigned int>{};
        }
    ));
    type_sf__Vector3_unsigned_int_.set("x", sol::property(
        [](sf::Vector3<unsigned int>& self) {
            return self.x;
        },
        [](sf::Vector3<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.x = value.value();
        }
    ));
    type_sf__Vector3_unsigned_int_.set("y", sol::property(
        [](sf::Vector3<unsigned int>& self) {
            return self.y;
        },
        [](sf::Vector3<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.y = value.value();
        }
    ));
    type_sf__Vector3_unsigned_int_.set("z", sol::property(
        [](sf::Vector3<unsigned int>& self) {
            return self.z;
        },
        [](sf::Vector3<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.z = value.value();
        }
    ));
    LUASF_STUB_DOC("\\brief Square of vector's length.\n\nSuitable for comparisons, more efficient than `length()`.");
    LUASF_STUB_FUNCTION("sf.Vector3u", "lengthSquared", "fun(self: sf.Vector3u): integer");
    type_sf__Vector3_unsigned_int_.set_function("lengthSquared",
        [](sf::Vector3<unsigned int>& self) -> unsigned int {
            return self.lengthSquared();
        }
    );
    LUASF_STUB_DOC("\\brief Dot product of two 3D vectors.");
    LUASF_STUB_FUNCTION("sf.Vector3u", "dot", "fun(self: sf.Vector3u, rhs: sf.Vector3u): integer");
    type_sf__Vector3_unsigned_int_.set_function("dot",
        [](sf::Vector3<unsigned int>& self, const sf::Vector3<unsigned int>& rhs) -> unsigned int {
            return self.dot(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Cross product of two 3D vectors.");
    LUASF_STUB_FUNCTION("sf.Vector3u", "cross", "fun(self: sf.Vector3u, rhs: sf.Vector3u): sf.Vector3u");
    type_sf__Vector3_unsigned_int_.set_function("cross",
        [](sf::Vector3<unsigned int>& self, const sf::Vector3<unsigned int>& rhs) -> sf::Vector3<unsigned int> {
            return self.cross(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise multiplication of `*this` and `rhs`.\n\nComputes `(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z)`.\n\nScaling is the most common use case for component-wise multiplication/division.\nThis operation is also known as the Hadamard or Schur product.");
    LUASF_STUB_FUNCTION("sf.Vector3u", "componentWiseMul", "fun(self: sf.Vector3u, rhs: sf.Vector3u): sf.Vector3u");
    type_sf__Vector3_unsigned_int_.set_function("componentWiseMul",
        [](sf::Vector3<unsigned int>& self, const sf::Vector3<unsigned int>& rhs) -> sf::Vector3<unsigned int> {
            return self.componentWiseMul(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise division of `*this` and `rhs`.\n\nComputes `(lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z)`.\n\nScaling is the most common use case for component-wise multiplication/division.\n\n\\pre Neither component of `rhs` is zero.");
    LUASF_STUB_FUNCTION("sf.Vector3u", "componentWiseDiv", "fun(self: sf.Vector3u, rhs: sf.Vector3u): sf.Vector3u");
    type_sf__Vector3_unsigned_int_.set_function("componentWiseDiv",
        [](sf::Vector3<unsigned int>& self, const sf::Vector3<unsigned int>& rhs) -> sf::Vector3<unsigned int> {
            return self.componentWiseDiv(rhs);
        }
    );
    LUASF_STUB_FUNCTION("sf.Vector3u", "unpack", "fun(self: sf.Vector3u): integer, integer, integer");
    type_sf__Vector3_unsigned_int_.set_function("unpack", [](const sf::Vector3<unsigned int>& self) {
        return std::make_tuple(self.x, self.y, self.z);
    });
    type_sf__Vector3_unsigned_int_[sol::meta_function::to_string] = [name = std::string("Vector3u")](const sf::Vector3<unsigned int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ")";
        return stream.str();
    };
    type_sf__Vector3_unsigned_int_[sol::meta_function::unary_minus] = [](const sf::Vector3<unsigned int>& value) { return -value; };
    LUASF_STUB_OPERATOR("sf.Vector3u", "unm: sf.Vector3u");
    type_sf__Vector3_unsigned_int_[sol::meta_function::addition] = [](const sf::Vector3<unsigned int>& left, const sf::Vector3<unsigned int>& right) { return left + right; };
    LUASF_STUB_OPERATOR("sf.Vector3u", "add(sf.Vector3u): sf.Vector3u");
    type_sf__Vector3_unsigned_int_[sol::meta_function::subtraction] = [](const sf::Vector3<unsigned int>& left, const sf::Vector3<unsigned int>& right) { return left - right; };
    LUASF_STUB_OPERATOR("sf.Vector3u", "sub(sf.Vector3u): sf.Vector3u");
    type_sf__Vector3_unsigned_int_[sol::meta_function::multiplication] = sol::overload(
        [](sf::Vector3<unsigned int> value, lua_sf::LuaIntegral<unsigned int> scalar) { return value * scalar.value(); },
        [](lua_sf::LuaIntegral<unsigned int> scalar, sf::Vector3<unsigned int> value) { return scalar.value() * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector3u", "mul(integer): sf.Vector3u");
    type_sf__Vector3_unsigned_int_[sol::meta_function::division] = [](sf::Vector3<unsigned int> value, lua_sf::LuaIntegral<unsigned int> scalar) { return value / scalar.value(); };
    LUASF_STUB_OPERATOR("sf.Vector3u", "div(integer): sf.Vector3u");
    type_sf__Vector3_unsigned_int_[sol::meta_function::equal_to] = [](const sf::Vector3<unsigned int>& left, const sf::Vector3<unsigned int>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.Vector3u", "eq(sf.Vector3u): boolean");
    auto type_sf__Vector3_float_ = sf.new_usertype<sf::Vector3<float>>("Vector3f", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Vector3_float_);
    sol::table table_sf__Vector3_float_ = sf["Vector3f"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Utility template class for manipulating\n3-dimensional vectors");
    LUASF_STUB_CLASS("sf.Vector3f");
    LUASF_STUB_DOC("X coordinate of the vector");
    LUASF_STUB_FIELD("x", "number");
    LUASF_STUB_DOC("Y coordinate of the vector");
    LUASF_STUB_FIELD("y", "number");
    LUASF_STUB_DOC("Z coordinate of the vector");
    LUASF_STUB_FIELD("z", "number");
    LUASF_STUB_DOC("\\brief Construct the vector from its coordinates\n\n\\param x X coordinate\n\\param y Y coordinate\n\\param z Z coordinate");
    LUASF_STUB_FUNCTION("sf.Vector3f", "new", "fun(x: number, y: number, z: number): sf.Vector3f");
    LUASF_STUB_OVERLOAD("sf.Vector3f", "new", "fun(): sf.Vector3f");
    type_sf__Vector3_float_.set_function("new", sol::factories(
        [](float x, float y, float z) {
            return sf::Vector3<float>{x, y, z};
        },
        []() {
            return sf::Vector3<float>{};
        }
    ));
    type_sf__Vector3_float_["x"] = sol::policies(&sf::Vector3<float>::x, sol::self_dependency{});
    type_sf__Vector3_float_["y"] = sol::policies(&sf::Vector3<float>::y, sol::self_dependency{});
    type_sf__Vector3_float_["z"] = sol::policies(&sf::Vector3<float>::z, sol::self_dependency{});
    LUASF_STUB_DOC("\\brief Length of the vector <i><b>(floating-point)</b></i>.\n\nIf you are not interested in the actual length, but only in comparisons, consider using `lengthSquared()`.");
    LUASF_STUB_FUNCTION("sf.Vector3f", "length", "fun(self: sf.Vector3f): number");
    type_sf__Vector3_float_.set_function("length",
        [](sf::Vector3<float>& self) -> float {
            return self.length();
        }
    );
    LUASF_STUB_DOC("\\brief Square of vector's length.\n\nSuitable for comparisons, more efficient than `length()`.");
    LUASF_STUB_FUNCTION("sf.Vector3f", "lengthSquared", "fun(self: sf.Vector3f): number");
    type_sf__Vector3_float_.set_function("lengthSquared",
        [](sf::Vector3<float>& self) -> float {
            return self.lengthSquared();
        }
    );
    LUASF_STUB_DOC("\\brief Vector with same direction but length 1 <i><b>(floating-point)</b></i>.\n\n\\pre `*this` is no zero vector.");
    LUASF_STUB_FUNCTION("sf.Vector3f", "normalized", "fun(self: sf.Vector3f): sf.Vector3f");
    type_sf__Vector3_float_.set_function("normalized",
        [](sf::Vector3<float>& self) -> sf::Vector3<float> {
            return self.normalized();
        }
    );
    LUASF_STUB_DOC("\\brief Dot product of two 3D vectors.");
    LUASF_STUB_FUNCTION("sf.Vector3f", "dot", "fun(self: sf.Vector3f, rhs: sf.Vector3f): number");
    type_sf__Vector3_float_.set_function("dot",
        [](sf::Vector3<float>& self, const sf::Vector3<float>& rhs) -> float {
            return self.dot(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Cross product of two 3D vectors.");
    LUASF_STUB_FUNCTION("sf.Vector3f", "cross", "fun(self: sf.Vector3f, rhs: sf.Vector3f): sf.Vector3f");
    type_sf__Vector3_float_.set_function("cross",
        [](sf::Vector3<float>& self, const sf::Vector3<float>& rhs) -> sf::Vector3<float> {
            return self.cross(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise multiplication of `*this` and `rhs`.\n\nComputes `(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z)`.\n\nScaling is the most common use case for component-wise multiplication/division.\nThis operation is also known as the Hadamard or Schur product.");
    LUASF_STUB_FUNCTION("sf.Vector3f", "componentWiseMul", "fun(self: sf.Vector3f, rhs: sf.Vector3f): sf.Vector3f");
    type_sf__Vector3_float_.set_function("componentWiseMul",
        [](sf::Vector3<float>& self, const sf::Vector3<float>& rhs) -> sf::Vector3<float> {
            return self.componentWiseMul(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise division of `*this` and `rhs`.\n\nComputes `(lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z)`.\n\nScaling is the most common use case for component-wise multiplication/division.\n\n\\pre Neither component of `rhs` is zero.");
    LUASF_STUB_FUNCTION("sf.Vector3f", "componentWiseDiv", "fun(self: sf.Vector3f, rhs: sf.Vector3f): sf.Vector3f");
    type_sf__Vector3_float_.set_function("componentWiseDiv",
        [](sf::Vector3<float>& self, const sf::Vector3<float>& rhs) -> sf::Vector3<float> {
            return self.componentWiseDiv(rhs);
        }
    );
    LUASF_STUB_FUNCTION("sf.Vector3f", "unpack", "fun(self: sf.Vector3f): number, number, number");
    type_sf__Vector3_float_.set_function("unpack", [](const sf::Vector3<float>& self) {
        return std::make_tuple(self.x, self.y, self.z);
    });
    type_sf__Vector3_float_[sol::meta_function::to_string] = [name = std::string("Vector3f")](const sf::Vector3<float>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ")";
        return stream.str();
    };
    type_sf__Vector3_float_[sol::meta_function::unary_minus] = [](const sf::Vector3<float>& value) { return -value; };
    LUASF_STUB_OPERATOR("sf.Vector3f", "unm: sf.Vector3f");
    type_sf__Vector3_float_[sol::meta_function::addition] = [](const sf::Vector3<float>& left, const sf::Vector3<float>& right) { return left + right; };
    LUASF_STUB_OPERATOR("sf.Vector3f", "add(sf.Vector3f): sf.Vector3f");
    type_sf__Vector3_float_[sol::meta_function::subtraction] = [](const sf::Vector3<float>& left, const sf::Vector3<float>& right) { return left - right; };
    LUASF_STUB_OPERATOR("sf.Vector3f", "sub(sf.Vector3f): sf.Vector3f");
    type_sf__Vector3_float_[sol::meta_function::multiplication] = sol::overload(
        [](sf::Vector3<float> value, float scalar) { return value * scalar; },
        [](float scalar, sf::Vector3<float> value) { return scalar * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector3f", "mul(number): sf.Vector3f");
    type_sf__Vector3_float_[sol::meta_function::division] = [](sf::Vector3<float> value, float scalar) { return value / scalar; };
    LUASF_STUB_OPERATOR("sf.Vector3f", "div(number): sf.Vector3f");
    type_sf__Vector3_float_[sol::meta_function::equal_to] = [](const sf::Vector3<float>& left, const sf::Vector3<float>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.Vector3f", "eq(sf.Vector3f): boolean");
}
