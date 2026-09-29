#include "System/bind_Vector2.hpp"

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

namespace { constexpr std::array<std::string_view, 18> docs = {
    "\\brief Class template for manipulating\n2-dimensional vectors",
    "X coordinate of the vector",
    "Y coordinate of the vector",
    "\\brief Default constructor\n\nCreates a `Vector2(0, 0)`.",
    "\\brief Construct the vector from cartesian coordinates\n\n\\param x X coordinate\n\\param y Y coordinate",
    "\\brief Square of vector's length.\n\nSuitable for comparisons, more efficient than `length()`.",
    "\\brief Returns a perpendicular vector.\n\nReturns `*this` rotated by +90 degrees; (x,y) becomes (-y,x).\nFor example, the vector (1,0) is transformed to (0,1).\n\nIn SFML's default coordinate system with +X right and +Y down,\nthis amounts to a clockwise rotation.",
    "\\brief Dot product of two 2D vectors.",
    "\\brief Z component of the cross product of two 2D vectors.\n\nTreats the operands as 3D vectors, computes their cross product\nand returns the result's Z component (X and Y components are always zero).",
    "\\brief Component-wise multiplication of `*this` and `rhs`.\n\nComputes `(lhs.x*rhs.x, lhs.y*rhs.y)`.\n\nScaling is the most common use case for component-wise multiplication/division.\nThis operation is also known as the Hadamard or Schur product.",
    "\\brief Component-wise division of `*this` and `rhs`.\n\nComputes `(lhs.x/rhs.x, lhs.y/rhs.y)`.\n\nScaling is the most common use case for component-wise multiplication/division.\n\n\\pre Neither component of `rhs` is zero.",
    "\\brief Construct the vector from polar coordinates <i><b>(floating-point)</b></i>\n\n\\param r   Length of vector (can be negative)\n\\param phi Angle from X axis\n\nNote that this constructor is lossy: calling `length()` and `angle()`\nmay return values different to those provided in this constructor.\n\nIn particular, these transforms can be applied:\n* `Vector2(r, phi) == Vector2(-r, phi + 180_deg)`\n* `Vector2(r, phi) == Vector2(r, phi + n * 360_deg)`",
    "\\brief Length of the vector <i><b>(floating-point)</b></i>.\n\nIf you are not interested in the actual length, but only in comparisons, consider using `lengthSquared()`.",
    "\\brief Vector with same direction but length 1 <i><b>(floating-point)</b></i>.\n\n\\pre `*this` is no zero vector.",
    "\\brief Signed angle from `*this` to `rhs` <i><b>(floating-point)</b></i>.\n\n\\return The smallest angle which rotates `*this` in positive\nor negative direction, until it has the same direction as `rhs`.\nThe result has a sign and lies in the range [-180, 180) degrees.\n\\pre Neither `*this` nor `rhs` is a zero vector.",
    "\\brief Signed angle from +X or (1,0) vector <i><b>(floating-point)</b></i>.\n\nFor example, the vector (1,0) corresponds to 0 degrees, (0,1) corresponds to 90 degrees.\n\n\\return Angle in the range [-180, 180) degrees.\n\\pre This vector is no zero vector.",
    "\\brief Rotate by angle \\c phi <i><b>(floating-point)</b></i>.\n\nReturns a vector with same length but different direction.\n\nIn SFML's default coordinate system with +X right and +Y down,\nthis amounts to a clockwise rotation by `phi`.",
    "\\brief Projection of this vector onto `axis` <i><b>(floating-point)</b></i>.\n\n\\param axis Vector being projected onto. Need not be normalized.\n\\pre `axis` must not have length zero.",
}; }

void bind_Vector2(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Vector2_int_ = lua_glue::BindStruct<sf::Vector2<int>>(sf, "Vector2i");
    lua_glue::Table table_sf__Vector2_int_ = sf["Vector2i"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Vector2i");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Vector2i", "new", "fun(x: integer, y: integer): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.Vector2i", "new", "fun(): sf.Vector2i");
    lua_glue::BindCallable(type_sf__Vector2_int_, "new",
        [](lua_sf::LuaIntegral<int> x, lua_sf::LuaIntegral<int> y) {
            return sf::Vector2<int>{x.value(), y.value()};
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Vector2_int_, "new",
        []() {
            return sf::Vector2<int>{};
        },
        docs[3]
    );
    lua_glue::BindProperty(type_sf__Vector2_int_, "x",
        [](const sf::Vector2<int>& self) {
            return self.x;
        },
        [](sf::Vector2<int>& self, lua_sf::LuaIntegral<int> value) {
            self.x = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Vector2_int_, "y",
        [](const sf::Vector2<int>& self) {
            return self.y;
        },
        [](sf::Vector2<int>& self, lua_sf::LuaIntegral<int> value) {
            self.y = value.value();
        }
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Vector2i", "lengthSquared", "fun(self: sf.Vector2i): integer");
    lua_glue::BindCallable(type_sf__Vector2_int_, "lengthSquared",
        [](const sf::Vector2<int>& self) -> int {
            return self.lengthSquared();
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Vector2i", "perpendicular", "fun(self: sf.Vector2i): sf.Vector2i");
    lua_glue::BindCallable(type_sf__Vector2_int_, "perpendicular",
        [](const sf::Vector2<int>& self) -> sf::Vector2<int> {
            return self.perpendicular();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Vector2i", "dot", "fun(self: sf.Vector2i, rhs: sf.Vector2i): integer");
    lua_glue::BindCallable(type_sf__Vector2_int_, "dot",
        [](const sf::Vector2<int>& self, sf::Vector2<int> rhs) -> int {
            return self.dot(rhs);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Vector2i", "cross", "fun(self: sf.Vector2i, rhs: sf.Vector2i): integer");
    lua_glue::BindCallable(type_sf__Vector2_int_, "cross",
        [](const sf::Vector2<int>& self, sf::Vector2<int> rhs) -> int {
            return self.cross(rhs);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Vector2i", "componentWiseMul", "fun(self: sf.Vector2i, rhs: sf.Vector2i): sf.Vector2i");
    lua_glue::BindCallable(type_sf__Vector2_int_, "componentWiseMul",
        [](const sf::Vector2<int>& self, sf::Vector2<int> rhs) -> sf::Vector2<int> {
            return self.componentWiseMul(rhs);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Vector2i", "componentWiseDiv", "fun(self: sf.Vector2i, rhs: sf.Vector2i): sf.Vector2i");
    lua_glue::BindCallable(type_sf__Vector2_int_, "componentWiseDiv",
        [](const sf::Vector2<int>& self, sf::Vector2<int> rhs) -> sf::Vector2<int> {
            return self.componentWiseDiv(rhs);
        },
        docs[10]
    );
    LUASF_STUB_FUNCTION("sf.Vector2i", "unpack", "fun(self: sf.Vector2i): integer, integer");
    lua_glue::BindCallable(type_sf__Vector2_int_, "unpack", [](const sf::Vector2<int>& self) {
        return std::make_tuple(self.x, self.y);
    });
    lua_glue::BindMetamethod(type_sf__Vector2_int_, "__tostring", [name = std::string("Vector2i")](const sf::Vector2<int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Vector2_int_, "__unm", [](const sf::Vector2<int>& value) { return -value; });
    LUASF_STUB_OPERATOR("sf.Vector2i", "unm: sf.Vector2i");
    lua_glue::BindMetamethod(type_sf__Vector2_int_, "__add", [](const sf::Vector2<int>& left, const sf::Vector2<int>& right) { return left + right; });
    LUASF_STUB_OPERATOR("sf.Vector2i", "add(sf.Vector2i): sf.Vector2i");
    lua_glue::BindMetamethod(type_sf__Vector2_int_, "__sub", [](const sf::Vector2<int>& left, const sf::Vector2<int>& right) { return left - right; });
    LUASF_STUB_OPERATOR("sf.Vector2i", "sub(sf.Vector2i): sf.Vector2i");
    lua_glue::BindMetamethod(type_sf__Vector2_int_, "__mul",
        [](sf::Vector2<int> value, lua_sf::LuaIntegral<int> scalar) { return value * scalar.value(); }
    );
    lua_glue::BindMetamethod(type_sf__Vector2_int_, "__mul",
        [](lua_sf::LuaIntegral<int> scalar, sf::Vector2<int> value) { return scalar.value() * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector2i", "mul(integer): sf.Vector2i");
    lua_glue::BindMetamethod(type_sf__Vector2_int_, "__div", [](sf::Vector2<int> value, lua_sf::LuaIntegral<int> scalar) { return value / scalar.value(); });
    LUASF_STUB_OPERATOR("sf.Vector2i", "div(integer): sf.Vector2i");
    lua_glue::BindMetamethod(type_sf__Vector2_int_, "__eq", [](const sf::Vector2<int>& left, const sf::Vector2<int>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.Vector2i", "eq(sf.Vector2i): boolean");
    LUASF_STUB_FUNCTION("sf.Vector2i", "copy", "fun(self: sf.Vector2i): sf.Vector2i");
    LUASF_STUB_FUNCTION("sf.Vector2i", "deepcopy", "fun(self: sf.Vector2i): sf.Vector2i");
    auto type_sf__Vector2_unsigned_int_ = lua_glue::BindStruct<sf::Vector2<unsigned int>>(sf, "Vector2u");
    lua_glue::Table table_sf__Vector2_unsigned_int_ = sf["Vector2u"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Vector2u");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Vector2u", "new", "fun(x: integer, y: integer): sf.Vector2u");
    LUASF_STUB_OVERLOAD("sf.Vector2u", "new", "fun(): sf.Vector2u");
    lua_glue::BindCallable(type_sf__Vector2_unsigned_int_, "new",
        [](lua_sf::LuaIntegral<unsigned int> x, lua_sf::LuaIntegral<unsigned int> y) {
            return sf::Vector2<unsigned int>{x.value(), y.value()};
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Vector2_unsigned_int_, "new",
        []() {
            return sf::Vector2<unsigned int>{};
        },
        docs[3]
    );
    lua_glue::BindProperty(type_sf__Vector2_unsigned_int_, "x",
        [](const sf::Vector2<unsigned int>& self) {
            return self.x;
        },
        [](sf::Vector2<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.x = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Vector2_unsigned_int_, "y",
        [](const sf::Vector2<unsigned int>& self) {
            return self.y;
        },
        [](sf::Vector2<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.y = value.value();
        }
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Vector2u", "lengthSquared", "fun(self: sf.Vector2u): integer");
    lua_glue::BindCallable(type_sf__Vector2_unsigned_int_, "lengthSquared",
        [](const sf::Vector2<unsigned int>& self) -> unsigned int {
            return self.lengthSquared();
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Vector2u", "perpendicular", "fun(self: sf.Vector2u): sf.Vector2u");
    lua_glue::BindCallable(type_sf__Vector2_unsigned_int_, "perpendicular",
        [](const sf::Vector2<unsigned int>& self) -> sf::Vector2<unsigned int> {
            return self.perpendicular();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Vector2u", "dot", "fun(self: sf.Vector2u, rhs: sf.Vector2u): integer");
    lua_glue::BindCallable(type_sf__Vector2_unsigned_int_, "dot",
        [](const sf::Vector2<unsigned int>& self, sf::Vector2<unsigned int> rhs) -> unsigned int {
            return self.dot(rhs);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Vector2u", "cross", "fun(self: sf.Vector2u, rhs: sf.Vector2u): integer");
    lua_glue::BindCallable(type_sf__Vector2_unsigned_int_, "cross",
        [](const sf::Vector2<unsigned int>& self, sf::Vector2<unsigned int> rhs) -> unsigned int {
            return self.cross(rhs);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Vector2u", "componentWiseMul", "fun(self: sf.Vector2u, rhs: sf.Vector2u): sf.Vector2u");
    lua_glue::BindCallable(type_sf__Vector2_unsigned_int_, "componentWiseMul",
        [](const sf::Vector2<unsigned int>& self, sf::Vector2<unsigned int> rhs) -> sf::Vector2<unsigned int> {
            return self.componentWiseMul(rhs);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Vector2u", "componentWiseDiv", "fun(self: sf.Vector2u, rhs: sf.Vector2u): sf.Vector2u");
    lua_glue::BindCallable(type_sf__Vector2_unsigned_int_, "componentWiseDiv",
        [](const sf::Vector2<unsigned int>& self, sf::Vector2<unsigned int> rhs) -> sf::Vector2<unsigned int> {
            return self.componentWiseDiv(rhs);
        },
        docs[10]
    );
    LUASF_STUB_FUNCTION("sf.Vector2u", "unpack", "fun(self: sf.Vector2u): integer, integer");
    lua_glue::BindCallable(type_sf__Vector2_unsigned_int_, "unpack", [](const sf::Vector2<unsigned int>& self) {
        return std::make_tuple(self.x, self.y);
    });
    lua_glue::BindMetamethod(type_sf__Vector2_unsigned_int_, "__tostring", [name = std::string("Vector2u")](const sf::Vector2<unsigned int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Vector2_unsigned_int_, "__unm", [](const sf::Vector2<unsigned int>& value) { return -value; });
    LUASF_STUB_OPERATOR("sf.Vector2u", "unm: sf.Vector2u");
    lua_glue::BindMetamethod(type_sf__Vector2_unsigned_int_, "__add", [](const sf::Vector2<unsigned int>& left, const sf::Vector2<unsigned int>& right) { return left + right; });
    LUASF_STUB_OPERATOR("sf.Vector2u", "add(sf.Vector2u): sf.Vector2u");
    lua_glue::BindMetamethod(type_sf__Vector2_unsigned_int_, "__sub", [](const sf::Vector2<unsigned int>& left, const sf::Vector2<unsigned int>& right) { return left - right; });
    LUASF_STUB_OPERATOR("sf.Vector2u", "sub(sf.Vector2u): sf.Vector2u");
    lua_glue::BindMetamethod(type_sf__Vector2_unsigned_int_, "__mul",
        [](sf::Vector2<unsigned int> value, lua_sf::LuaIntegral<unsigned int> scalar) { return value * scalar.value(); }
    );
    lua_glue::BindMetamethod(type_sf__Vector2_unsigned_int_, "__mul",
        [](lua_sf::LuaIntegral<unsigned int> scalar, sf::Vector2<unsigned int> value) { return scalar.value() * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector2u", "mul(integer): sf.Vector2u");
    lua_glue::BindMetamethod(type_sf__Vector2_unsigned_int_, "__div", [](sf::Vector2<unsigned int> value, lua_sf::LuaIntegral<unsigned int> scalar) { return value / scalar.value(); });
    LUASF_STUB_OPERATOR("sf.Vector2u", "div(integer): sf.Vector2u");
    lua_glue::BindMetamethod(type_sf__Vector2_unsigned_int_, "__eq", [](const sf::Vector2<unsigned int>& left, const sf::Vector2<unsigned int>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.Vector2u", "eq(sf.Vector2u): boolean");
    LUASF_STUB_FUNCTION("sf.Vector2u", "copy", "fun(self: sf.Vector2u): sf.Vector2u");
    LUASF_STUB_FUNCTION("sf.Vector2u", "deepcopy", "fun(self: sf.Vector2u): sf.Vector2u");
    auto type_sf__Vector2_float_ = lua_glue::BindStruct<sf::Vector2<float>>(sf, "Vector2f");
    lua_glue::Table table_sf__Vector2_float_ = sf["Vector2f"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Vector2f");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("x", "number");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("y", "number");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "new", "fun(x: number, y: number): sf.Vector2f");
    LUASF_STUB_OVERLOAD("sf.Vector2f", "new", "fun(r: number, phi: sf.Angle): sf.Vector2f");
    LUASF_STUB_OVERLOAD("sf.Vector2f", "new", "fun(): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Vector2_float_, "new",
        [](float x, float y) {
            return sf::Vector2<float>{x, y};
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Vector2_float_, "new",
        [](float r, sf::Angle phi) {
            return sf::Vector2<float>{r, phi};
        },
        docs[11]
    );
    lua_glue::BindCallable(type_sf__Vector2_float_, "new",
        []() {
            return sf::Vector2<float>{};
        },
        docs[3]
    );
    lua_glue::BindAttr<float>(type_sf__Vector2_float_, "x", &sf::Vector2<float>::x);
    lua_glue::BindAttr<float>(type_sf__Vector2_float_, "y", &sf::Vector2<float>::y);
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "length", "fun(self: sf.Vector2f): number");
    lua_glue::BindCallable(type_sf__Vector2_float_, "length",
        [](const sf::Vector2<float>& self) -> float {
            return self.length();
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "lengthSquared", "fun(self: sf.Vector2f): number");
    lua_glue::BindCallable(type_sf__Vector2_float_, "lengthSquared",
        [](const sf::Vector2<float>& self) -> float {
            return self.lengthSquared();
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "normalized", "fun(self: sf.Vector2f): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Vector2_float_, "normalized",
        [](const sf::Vector2<float>& self) -> sf::Vector2<float> {
            return self.normalized();
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "angleTo", "fun(self: sf.Vector2f, rhs: sf.Vector2f): sf.Angle");
    lua_glue::BindCallable(type_sf__Vector2_float_, "angleTo",
        [](const sf::Vector2<float>& self, sf::Vector2<float> rhs) -> sf::Angle {
            return self.angleTo(rhs);
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "angle", "fun(self: sf.Vector2f): sf.Angle");
    lua_glue::BindCallable(type_sf__Vector2_float_, "angle",
        [](const sf::Vector2<float>& self) -> sf::Angle {
            return self.angle();
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "rotatedBy", "fun(self: sf.Vector2f, phi: sf.Angle): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Vector2_float_, "rotatedBy",
        [](const sf::Vector2<float>& self, sf::Angle phi) -> sf::Vector2<float> {
            return self.rotatedBy(phi);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "projectedOnto", "fun(self: sf.Vector2f, axis: sf.Vector2f): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Vector2_float_, "projectedOnto",
        [](const sf::Vector2<float>& self, sf::Vector2<float> axis) -> sf::Vector2<float> {
            return self.projectedOnto(axis);
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "perpendicular", "fun(self: sf.Vector2f): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Vector2_float_, "perpendicular",
        [](const sf::Vector2<float>& self) -> sf::Vector2<float> {
            return self.perpendicular();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "dot", "fun(self: sf.Vector2f, rhs: sf.Vector2f): number");
    lua_glue::BindCallable(type_sf__Vector2_float_, "dot",
        [](const sf::Vector2<float>& self, sf::Vector2<float> rhs) -> float {
            return self.dot(rhs);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "cross", "fun(self: sf.Vector2f, rhs: sf.Vector2f): number");
    lua_glue::BindCallable(type_sf__Vector2_float_, "cross",
        [](const sf::Vector2<float>& self, sf::Vector2<float> rhs) -> float {
            return self.cross(rhs);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "componentWiseMul", "fun(self: sf.Vector2f, rhs: sf.Vector2f): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Vector2_float_, "componentWiseMul",
        [](const sf::Vector2<float>& self, sf::Vector2<float> rhs) -> sf::Vector2<float> {
            return self.componentWiseMul(rhs);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Vector2f", "componentWiseDiv", "fun(self: sf.Vector2f, rhs: sf.Vector2f): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Vector2_float_, "componentWiseDiv",
        [](const sf::Vector2<float>& self, sf::Vector2<float> rhs) -> sf::Vector2<float> {
            return self.componentWiseDiv(rhs);
        },
        docs[10]
    );
    LUASF_STUB_FUNCTION("sf.Vector2f", "unpack", "fun(self: sf.Vector2f): number, number");
    lua_glue::BindCallable(type_sf__Vector2_float_, "unpack", [](const sf::Vector2<float>& self) {
        return std::make_tuple(self.x, self.y);
    });
    lua_glue::BindMetamethod(type_sf__Vector2_float_, "__tostring", [name = std::string("Vector2f")](const sf::Vector2<float>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Vector2_float_, "__unm", [](const sf::Vector2<float>& value) { return -value; });
    LUASF_STUB_OPERATOR("sf.Vector2f", "unm: sf.Vector2f");
    lua_glue::BindMetamethod(type_sf__Vector2_float_, "__add", [](const sf::Vector2<float>& left, const sf::Vector2<float>& right) { return left + right; });
    LUASF_STUB_OPERATOR("sf.Vector2f", "add(sf.Vector2f): sf.Vector2f");
    lua_glue::BindMetamethod(type_sf__Vector2_float_, "__sub", [](const sf::Vector2<float>& left, const sf::Vector2<float>& right) { return left - right; });
    LUASF_STUB_OPERATOR("sf.Vector2f", "sub(sf.Vector2f): sf.Vector2f");
    lua_glue::BindMetamethod(type_sf__Vector2_float_, "__mul",
        [](sf::Vector2<float> value, float scalar) { return value * scalar; }
    );
    lua_glue::BindMetamethod(type_sf__Vector2_float_, "__mul",
        [](float scalar, sf::Vector2<float> value) { return scalar * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector2f", "mul(number): sf.Vector2f");
    lua_glue::BindMetamethod(type_sf__Vector2_float_, "__div", [](sf::Vector2<float> value, float scalar) { return value / scalar; });
    LUASF_STUB_OPERATOR("sf.Vector2f", "div(number): sf.Vector2f");
    lua_glue::BindMetamethod(type_sf__Vector2_float_, "__eq", [](const sf::Vector2<float>& left, const sf::Vector2<float>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.Vector2f", "eq(sf.Vector2f): boolean");
    LUASF_STUB_FUNCTION("sf.Vector2f", "copy", "fun(self: sf.Vector2f): sf.Vector2f");
    LUASF_STUB_FUNCTION("sf.Vector2f", "deepcopy", "fun(self: sf.Vector2f): sf.Vector2f");
}
