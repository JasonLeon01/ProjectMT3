#include "System/bind_Vector3.hpp"

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

namespace { constexpr std::array<std::string_view, 13> docs = {
    "\\brief Utility template class for manipulating\n3-dimensional vectors",
    "X coordinate of the vector",
    "Y coordinate of the vector",
    "Z coordinate of the vector",
    "\\brief Default constructor\n\nCreates a `Vector3(0, 0, 0)`.",
    "\\brief Construct the vector from its coordinates\n\n\\param x X coordinate\n\\param y Y coordinate\n\\param z Z coordinate",
    "\\brief Square of vector's length.\n\nSuitable for comparisons, more efficient than `length()`.",
    "\\brief Dot product of two 3D vectors.",
    "\\brief Cross product of two 3D vectors.",
    "\\brief Component-wise multiplication of `*this` and `rhs`.\n\nComputes `(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z)`.\n\nScaling is the most common use case for component-wise multiplication/division.\nThis operation is also known as the Hadamard or Schur product.",
    "\\brief Component-wise division of `*this` and `rhs`.\n\nComputes `(lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z)`.\n\nScaling is the most common use case for component-wise multiplication/division.\n\n\\pre Neither component of `rhs` is zero.",
    "\\brief Length of the vector <i><b>(floating-point)</b></i>.\n\nIf you are not interested in the actual length, but only in comparisons, consider using `lengthSquared()`.",
    "\\brief Vector with same direction but length 1 <i><b>(floating-point)</b></i>.\n\n\\pre `*this` is no zero vector.",
}; }

void bind_Vector3(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Vector3_int_ = lua_glue::BindStruct<sf::Vector3<int>>(sf, "Vector3i");
    lua_glue::Table table_sf__Vector3_int_ = sf["Vector3i"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Vector3i");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("z", "integer");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Vector3i", "new", "fun(x: integer, y: integer, z: integer): sf.Vector3i");
    LUASF_STUB_OVERLOAD("sf.Vector3i", "new", "fun(): sf.Vector3i");
    lua_glue::BindCallable(type_sf__Vector3_int_, "new",
        [](lua_sf::LuaIntegral<int> x, lua_sf::LuaIntegral<int> y, lua_sf::LuaIntegral<int> z) {
            return sf::Vector3<int>{x.value(), y.value(), z.value()};
        },
        docs[5]
    );
    lua_glue::BindCallable(type_sf__Vector3_int_, "new",
        []() {
            return sf::Vector3<int>{};
        },
        docs[4]
    );
    lua_glue::BindProperty(type_sf__Vector3_int_, "x",
        [](const sf::Vector3<int>& self) {
            return self.x;
        },
        [](sf::Vector3<int>& self, lua_sf::LuaIntegral<int> value) {
            self.x = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Vector3_int_, "y",
        [](const sf::Vector3<int>& self) {
            return self.y;
        },
        [](sf::Vector3<int>& self, lua_sf::LuaIntegral<int> value) {
            self.y = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Vector3_int_, "z",
        [](const sf::Vector3<int>& self) {
            return self.z;
        },
        [](sf::Vector3<int>& self, lua_sf::LuaIntegral<int> value) {
            self.z = value.value();
        }
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Vector3i", "lengthSquared", "fun(self: sf.Vector3i): integer");
    lua_glue::BindCallable(type_sf__Vector3_int_, "lengthSquared",
        [](const sf::Vector3<int>& self) -> int {
            return self.lengthSquared();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Vector3i", "dot", "fun(self: sf.Vector3i, rhs: sf.Vector3i): integer");
    lua_glue::BindCallable(type_sf__Vector3_int_, "dot",
        [](const sf::Vector3<int>& self, const sf::Vector3<int>& rhs) -> int {
            return self.dot(rhs);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Vector3i", "cross", "fun(self: sf.Vector3i, rhs: sf.Vector3i): sf.Vector3i");
    lua_glue::BindCallable(type_sf__Vector3_int_, "cross",
        [](const sf::Vector3<int>& self, const sf::Vector3<int>& rhs) -> sf::Vector3<int> {
            return self.cross(rhs);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Vector3i", "componentWiseMul", "fun(self: sf.Vector3i, rhs: sf.Vector3i): sf.Vector3i");
    lua_glue::BindCallable(type_sf__Vector3_int_, "componentWiseMul",
        [](const sf::Vector3<int>& self, const sf::Vector3<int>& rhs) -> sf::Vector3<int> {
            return self.componentWiseMul(rhs);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Vector3i", "componentWiseDiv", "fun(self: sf.Vector3i, rhs: sf.Vector3i): sf.Vector3i");
    lua_glue::BindCallable(type_sf__Vector3_int_, "componentWiseDiv",
        [](const sf::Vector3<int>& self, const sf::Vector3<int>& rhs) -> sf::Vector3<int> {
            return self.componentWiseDiv(rhs);
        },
        docs[10]
    );
    LUASF_STUB_FUNCTION("sf.Vector3i", "unpack", "fun(self: sf.Vector3i): integer, integer, integer");
    lua_glue::BindCallable(type_sf__Vector3_int_, "unpack", [](const sf::Vector3<int>& self) {
        return std::make_tuple(self.x, self.y, self.z);
    });
    lua_glue::BindMetamethod(type_sf__Vector3_int_, "__tostring", [name = std::string("Vector3i")](const sf::Vector3<int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Vector3_int_, "__unm", [](const sf::Vector3<int>& value) { return -value; });
    LUASF_STUB_OPERATOR("sf.Vector3i", "unm: sf.Vector3i");
    lua_glue::BindMetamethod(type_sf__Vector3_int_, "__add", [](const sf::Vector3<int>& left, const sf::Vector3<int>& right) { return left + right; });
    LUASF_STUB_OPERATOR("sf.Vector3i", "add(sf.Vector3i): sf.Vector3i");
    lua_glue::BindMetamethod(type_sf__Vector3_int_, "__sub", [](const sf::Vector3<int>& left, const sf::Vector3<int>& right) { return left - right; });
    LUASF_STUB_OPERATOR("sf.Vector3i", "sub(sf.Vector3i): sf.Vector3i");
    lua_glue::BindMetamethod(type_sf__Vector3_int_, "__mul",
        [](sf::Vector3<int> value, lua_sf::LuaIntegral<int> scalar) { return value * scalar.value(); }
    );
    lua_glue::BindMetamethod(type_sf__Vector3_int_, "__mul",
        [](lua_sf::LuaIntegral<int> scalar, sf::Vector3<int> value) { return scalar.value() * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector3i", "mul(integer): sf.Vector3i");
    lua_glue::BindMetamethod(type_sf__Vector3_int_, "__div", [](sf::Vector3<int> value, lua_sf::LuaIntegral<int> scalar) { return value / scalar.value(); });
    LUASF_STUB_OPERATOR("sf.Vector3i", "div(integer): sf.Vector3i");
    lua_glue::BindMetamethod(type_sf__Vector3_int_, "__eq", [](const sf::Vector3<int>& left, const sf::Vector3<int>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.Vector3i", "eq(sf.Vector3i): boolean");
    LUASF_STUB_FUNCTION("sf.Vector3i", "copy", "fun(self: sf.Vector3i): sf.Vector3i");
    LUASF_STUB_FUNCTION("sf.Vector3i", "deepcopy", "fun(self: sf.Vector3i): sf.Vector3i");
    auto type_sf__Vector3_unsigned_int_ = lua_glue::BindStruct<sf::Vector3<unsigned int>>(sf, "Vector3u");
    lua_glue::Table table_sf__Vector3_unsigned_int_ = sf["Vector3u"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Vector3u");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("z", "integer");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Vector3u", "new", "fun(x: integer, y: integer, z: integer): sf.Vector3u");
    LUASF_STUB_OVERLOAD("sf.Vector3u", "new", "fun(): sf.Vector3u");
    lua_glue::BindCallable(type_sf__Vector3_unsigned_int_, "new",
        [](lua_sf::LuaIntegral<unsigned int> x, lua_sf::LuaIntegral<unsigned int> y, lua_sf::LuaIntegral<unsigned int> z) {
            return sf::Vector3<unsigned int>{x.value(), y.value(), z.value()};
        },
        docs[5]
    );
    lua_glue::BindCallable(type_sf__Vector3_unsigned_int_, "new",
        []() {
            return sf::Vector3<unsigned int>{};
        },
        docs[4]
    );
    lua_glue::BindProperty(type_sf__Vector3_unsigned_int_, "x",
        [](const sf::Vector3<unsigned int>& self) {
            return self.x;
        },
        [](sf::Vector3<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.x = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Vector3_unsigned_int_, "y",
        [](const sf::Vector3<unsigned int>& self) {
            return self.y;
        },
        [](sf::Vector3<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.y = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Vector3_unsigned_int_, "z",
        [](const sf::Vector3<unsigned int>& self) {
            return self.z;
        },
        [](sf::Vector3<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.z = value.value();
        }
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Vector3u", "lengthSquared", "fun(self: sf.Vector3u): integer");
    lua_glue::BindCallable(type_sf__Vector3_unsigned_int_, "lengthSquared",
        [](const sf::Vector3<unsigned int>& self) -> unsigned int {
            return self.lengthSquared();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Vector3u", "dot", "fun(self: sf.Vector3u, rhs: sf.Vector3u): integer");
    lua_glue::BindCallable(type_sf__Vector3_unsigned_int_, "dot",
        [](const sf::Vector3<unsigned int>& self, const sf::Vector3<unsigned int>& rhs) -> unsigned int {
            return self.dot(rhs);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Vector3u", "cross", "fun(self: sf.Vector3u, rhs: sf.Vector3u): sf.Vector3u");
    lua_glue::BindCallable(type_sf__Vector3_unsigned_int_, "cross",
        [](const sf::Vector3<unsigned int>& self, const sf::Vector3<unsigned int>& rhs) -> sf::Vector3<unsigned int> {
            return self.cross(rhs);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Vector3u", "componentWiseMul", "fun(self: sf.Vector3u, rhs: sf.Vector3u): sf.Vector3u");
    lua_glue::BindCallable(type_sf__Vector3_unsigned_int_, "componentWiseMul",
        [](const sf::Vector3<unsigned int>& self, const sf::Vector3<unsigned int>& rhs) -> sf::Vector3<unsigned int> {
            return self.componentWiseMul(rhs);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Vector3u", "componentWiseDiv", "fun(self: sf.Vector3u, rhs: sf.Vector3u): sf.Vector3u");
    lua_glue::BindCallable(type_sf__Vector3_unsigned_int_, "componentWiseDiv",
        [](const sf::Vector3<unsigned int>& self, const sf::Vector3<unsigned int>& rhs) -> sf::Vector3<unsigned int> {
            return self.componentWiseDiv(rhs);
        },
        docs[10]
    );
    LUASF_STUB_FUNCTION("sf.Vector3u", "unpack", "fun(self: sf.Vector3u): integer, integer, integer");
    lua_glue::BindCallable(type_sf__Vector3_unsigned_int_, "unpack", [](const sf::Vector3<unsigned int>& self) {
        return std::make_tuple(self.x, self.y, self.z);
    });
    lua_glue::BindMetamethod(type_sf__Vector3_unsigned_int_, "__tostring", [name = std::string("Vector3u")](const sf::Vector3<unsigned int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Vector3_unsigned_int_, "__unm", [](const sf::Vector3<unsigned int>& value) { return -value; });
    LUASF_STUB_OPERATOR("sf.Vector3u", "unm: sf.Vector3u");
    lua_glue::BindMetamethod(type_sf__Vector3_unsigned_int_, "__add", [](const sf::Vector3<unsigned int>& left, const sf::Vector3<unsigned int>& right) { return left + right; });
    LUASF_STUB_OPERATOR("sf.Vector3u", "add(sf.Vector3u): sf.Vector3u");
    lua_glue::BindMetamethod(type_sf__Vector3_unsigned_int_, "__sub", [](const sf::Vector3<unsigned int>& left, const sf::Vector3<unsigned int>& right) { return left - right; });
    LUASF_STUB_OPERATOR("sf.Vector3u", "sub(sf.Vector3u): sf.Vector3u");
    lua_glue::BindMetamethod(type_sf__Vector3_unsigned_int_, "__mul",
        [](sf::Vector3<unsigned int> value, lua_sf::LuaIntegral<unsigned int> scalar) { return value * scalar.value(); }
    );
    lua_glue::BindMetamethod(type_sf__Vector3_unsigned_int_, "__mul",
        [](lua_sf::LuaIntegral<unsigned int> scalar, sf::Vector3<unsigned int> value) { return scalar.value() * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector3u", "mul(integer): sf.Vector3u");
    lua_glue::BindMetamethod(type_sf__Vector3_unsigned_int_, "__div", [](sf::Vector3<unsigned int> value, lua_sf::LuaIntegral<unsigned int> scalar) { return value / scalar.value(); });
    LUASF_STUB_OPERATOR("sf.Vector3u", "div(integer): sf.Vector3u");
    lua_glue::BindMetamethod(type_sf__Vector3_unsigned_int_, "__eq", [](const sf::Vector3<unsigned int>& left, const sf::Vector3<unsigned int>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.Vector3u", "eq(sf.Vector3u): boolean");
    LUASF_STUB_FUNCTION("sf.Vector3u", "copy", "fun(self: sf.Vector3u): sf.Vector3u");
    LUASF_STUB_FUNCTION("sf.Vector3u", "deepcopy", "fun(self: sf.Vector3u): sf.Vector3u");
    auto type_sf__Vector3_float_ = lua_glue::BindStruct<sf::Vector3<float>>(sf, "Vector3f");
    lua_glue::Table table_sf__Vector3_float_ = sf["Vector3f"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Vector3f");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("x", "number");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("y", "number");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("z", "number");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Vector3f", "new", "fun(x: number, y: number, z: number): sf.Vector3f");
    LUASF_STUB_OVERLOAD("sf.Vector3f", "new", "fun(): sf.Vector3f");
    lua_glue::BindCallable(type_sf__Vector3_float_, "new",
        [](float x, float y, float z) {
            return sf::Vector3<float>{x, y, z};
        },
        docs[5]
    );
    lua_glue::BindCallable(type_sf__Vector3_float_, "new",
        []() {
            return sf::Vector3<float>{};
        },
        docs[4]
    );
    lua_glue::BindAttr<float>(type_sf__Vector3_float_, "x", &sf::Vector3<float>::x);
    lua_glue::BindAttr<float>(type_sf__Vector3_float_, "y", &sf::Vector3<float>::y);
    lua_glue::BindAttr<float>(type_sf__Vector3_float_, "z", &sf::Vector3<float>::z);
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Vector3f", "length", "fun(self: sf.Vector3f): number");
    lua_glue::BindCallable(type_sf__Vector3_float_, "length",
        [](const sf::Vector3<float>& self) -> float {
            return self.length();
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Vector3f", "lengthSquared", "fun(self: sf.Vector3f): number");
    lua_glue::BindCallable(type_sf__Vector3_float_, "lengthSquared",
        [](const sf::Vector3<float>& self) -> float {
            return self.lengthSquared();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Vector3f", "normalized", "fun(self: sf.Vector3f): sf.Vector3f");
    lua_glue::BindCallable(type_sf__Vector3_float_, "normalized",
        [](const sf::Vector3<float>& self) -> sf::Vector3<float> {
            return self.normalized();
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Vector3f", "dot", "fun(self: sf.Vector3f, rhs: sf.Vector3f): number");
    lua_glue::BindCallable(type_sf__Vector3_float_, "dot",
        [](const sf::Vector3<float>& self, const sf::Vector3<float>& rhs) -> float {
            return self.dot(rhs);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Vector3f", "cross", "fun(self: sf.Vector3f, rhs: sf.Vector3f): sf.Vector3f");
    lua_glue::BindCallable(type_sf__Vector3_float_, "cross",
        [](const sf::Vector3<float>& self, const sf::Vector3<float>& rhs) -> sf::Vector3<float> {
            return self.cross(rhs);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Vector3f", "componentWiseMul", "fun(self: sf.Vector3f, rhs: sf.Vector3f): sf.Vector3f");
    lua_glue::BindCallable(type_sf__Vector3_float_, "componentWiseMul",
        [](const sf::Vector3<float>& self, const sf::Vector3<float>& rhs) -> sf::Vector3<float> {
            return self.componentWiseMul(rhs);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Vector3f", "componentWiseDiv", "fun(self: sf.Vector3f, rhs: sf.Vector3f): sf.Vector3f");
    lua_glue::BindCallable(type_sf__Vector3_float_, "componentWiseDiv",
        [](const sf::Vector3<float>& self, const sf::Vector3<float>& rhs) -> sf::Vector3<float> {
            return self.componentWiseDiv(rhs);
        },
        docs[10]
    );
    LUASF_STUB_FUNCTION("sf.Vector3f", "unpack", "fun(self: sf.Vector3f): number, number, number");
    lua_glue::BindCallable(type_sf__Vector3_float_, "unpack", [](const sf::Vector3<float>& self) {
        return std::make_tuple(self.x, self.y, self.z);
    });
    lua_glue::BindMetamethod(type_sf__Vector3_float_, "__tostring", [name = std::string("Vector3f")](const sf::Vector3<float>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ", " << self.z << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Vector3_float_, "__unm", [](const sf::Vector3<float>& value) { return -value; });
    LUASF_STUB_OPERATOR("sf.Vector3f", "unm: sf.Vector3f");
    lua_glue::BindMetamethod(type_sf__Vector3_float_, "__add", [](const sf::Vector3<float>& left, const sf::Vector3<float>& right) { return left + right; });
    LUASF_STUB_OPERATOR("sf.Vector3f", "add(sf.Vector3f): sf.Vector3f");
    lua_glue::BindMetamethod(type_sf__Vector3_float_, "__sub", [](const sf::Vector3<float>& left, const sf::Vector3<float>& right) { return left - right; });
    LUASF_STUB_OPERATOR("sf.Vector3f", "sub(sf.Vector3f): sf.Vector3f");
    lua_glue::BindMetamethod(type_sf__Vector3_float_, "__mul",
        [](sf::Vector3<float> value, float scalar) { return value * scalar; }
    );
    lua_glue::BindMetamethod(type_sf__Vector3_float_, "__mul",
        [](float scalar, sf::Vector3<float> value) { return scalar * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector3f", "mul(number): sf.Vector3f");
    lua_glue::BindMetamethod(type_sf__Vector3_float_, "__div", [](sf::Vector3<float> value, float scalar) { return value / scalar; });
    LUASF_STUB_OPERATOR("sf.Vector3f", "div(number): sf.Vector3f");
    lua_glue::BindMetamethod(type_sf__Vector3_float_, "__eq", [](const sf::Vector3<float>& left, const sf::Vector3<float>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.Vector3f", "eq(sf.Vector3f): boolean");
    LUASF_STUB_FUNCTION("sf.Vector3f", "copy", "fun(self: sf.Vector3f): sf.Vector3f");
    LUASF_STUB_FUNCTION("sf.Vector3f", "deepcopy", "fun(self: sf.Vector3f): sf.Vector3f");
}
