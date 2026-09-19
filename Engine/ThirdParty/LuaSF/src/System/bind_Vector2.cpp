#include "System/bind_Vector2.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Vector2(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Vector2_int_ = sf.new_usertype<sf::Vector2<int>>("Vector2i", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Vector2_int_);
    sol::table table_sf__Vector2_int_ = sf["Vector2i"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Class template for manipulating\n2-dimensional vectors");
    LUASF_STUB_CLASS("sf.Vector2i");
    LUASF_STUB_DOC("X coordinate of the vector");
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC("Y coordinate of the vector");
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC("\\brief Construct the vector from cartesian coordinates\n\n\\param x X coordinate\n\\param y Y coordinate");
    LUASF_STUB_FUNCTION("sf.Vector2i", "new", "fun(x: integer, y: integer): sf.Vector2i");
    LUASF_STUB_OVERLOAD("sf.Vector2i", "new", "fun(): sf.Vector2i");
    type_sf__Vector2_int_.set_function("new", sol::factories(
        [](lua_sf::LuaIntegral<int> x, lua_sf::LuaIntegral<int> y) {
            return sf::Vector2<int>{x.value(), y.value()};
        },
        []() {
            return sf::Vector2<int>{};
        }
    ));
    type_sf__Vector2_int_.set("x", sol::property(
        [](sf::Vector2<int>& self) {
            return self.x;
        },
        [](sf::Vector2<int>& self, lua_sf::LuaIntegral<int> value) {
            self.x = value.value();
        }
    ));
    type_sf__Vector2_int_.set("y", sol::property(
        [](sf::Vector2<int>& self) {
            return self.y;
        },
        [](sf::Vector2<int>& self, lua_sf::LuaIntegral<int> value) {
            self.y = value.value();
        }
    ));
    LUASF_STUB_DOC("\\brief Square of vector's length.\n\nSuitable for comparisons, more efficient than `length()`.");
    LUASF_STUB_FUNCTION("sf.Vector2i", "lengthSquared", "fun(self: sf.Vector2i): integer");
    type_sf__Vector2_int_.set_function("lengthSquared",
        [](sf::Vector2<int>& self) -> int {
            return self.lengthSquared();
        }
    );
    LUASF_STUB_DOC("\\brief Returns a perpendicular vector.\n\nReturns `*this` rotated by +90 degrees; (x,y) becomes (-y,x).\nFor example, the vector (1,0) is transformed to (0,1).\n\nIn SFML's default coordinate system with +X right and +Y down,\nthis amounts to a clockwise rotation.");
    LUASF_STUB_FUNCTION("sf.Vector2i", "perpendicular", "fun(self: sf.Vector2i): sf.Vector2i");
    type_sf__Vector2_int_.set_function("perpendicular",
        [](sf::Vector2<int>& self) -> sf::Vector2<int> {
            return self.perpendicular();
        }
    );
    LUASF_STUB_DOC("\\brief Dot product of two 2D vectors.");
    LUASF_STUB_FUNCTION("sf.Vector2i", "dot", "fun(self: sf.Vector2i, rhs: sf.Vector2i): integer");
    type_sf__Vector2_int_.set_function("dot",
        [](sf::Vector2<int>& self, sf::Vector2<int> rhs) -> int {
            return self.dot(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Z component of the cross product of two 2D vectors.\n\nTreats the operands as 3D vectors, computes their cross product\nand returns the result's Z component (X and Y components are always zero).");
    LUASF_STUB_FUNCTION("sf.Vector2i", "cross", "fun(self: sf.Vector2i, rhs: sf.Vector2i): integer");
    type_sf__Vector2_int_.set_function("cross",
        [](sf::Vector2<int>& self, sf::Vector2<int> rhs) -> int {
            return self.cross(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise multiplication of `*this` and `rhs`.\n\nComputes `(lhs.x*rhs.x, lhs.y*rhs.y)`.\n\nScaling is the most common use case for component-wise multiplication/division.\nThis operation is also known as the Hadamard or Schur product.");
    LUASF_STUB_FUNCTION("sf.Vector2i", "componentWiseMul", "fun(self: sf.Vector2i, rhs: sf.Vector2i): sf.Vector2i");
    type_sf__Vector2_int_.set_function("componentWiseMul",
        [](sf::Vector2<int>& self, sf::Vector2<int> rhs) -> sf::Vector2<int> {
            return self.componentWiseMul(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise division of `*this` and `rhs`.\n\nComputes `(lhs.x/rhs.x, lhs.y/rhs.y)`.\n\nScaling is the most common use case for component-wise multiplication/division.\n\n\\pre Neither component of `rhs` is zero.");
    LUASF_STUB_FUNCTION("sf.Vector2i", "componentWiseDiv", "fun(self: sf.Vector2i, rhs: sf.Vector2i): sf.Vector2i");
    type_sf__Vector2_int_.set_function("componentWiseDiv",
        [](sf::Vector2<int>& self, sf::Vector2<int> rhs) -> sf::Vector2<int> {
            return self.componentWiseDiv(rhs);
        }
    );
    LUASF_STUB_FUNCTION("sf.Vector2i", "unpack", "fun(self: sf.Vector2i): integer, integer");
    type_sf__Vector2_int_.set_function("unpack", [](const sf::Vector2<int>& self) {
        return std::make_tuple(self.x, self.y);
    });
    type_sf__Vector2_int_[sol::meta_function::to_string] = [name = std::string("Vector2i")](const sf::Vector2<int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ")";
        return stream.str();
    };
    type_sf__Vector2_int_[sol::meta_function::unary_minus] = [](const sf::Vector2<int>& value) { return -value; };
    LUASF_STUB_OPERATOR("sf.Vector2i", "unm: sf.Vector2i");
    type_sf__Vector2_int_[sol::meta_function::addition] = [](const sf::Vector2<int>& left, const sf::Vector2<int>& right) { return left + right; };
    LUASF_STUB_OPERATOR("sf.Vector2i", "add(sf.Vector2i): sf.Vector2i");
    type_sf__Vector2_int_[sol::meta_function::subtraction] = [](const sf::Vector2<int>& left, const sf::Vector2<int>& right) { return left - right; };
    LUASF_STUB_OPERATOR("sf.Vector2i", "sub(sf.Vector2i): sf.Vector2i");
    type_sf__Vector2_int_[sol::meta_function::multiplication] = sol::overload(
        [](sf::Vector2<int> value, lua_sf::LuaIntegral<int> scalar) { return value * scalar.value(); },
        [](lua_sf::LuaIntegral<int> scalar, sf::Vector2<int> value) { return scalar.value() * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector2i", "mul(integer): sf.Vector2i");
    type_sf__Vector2_int_[sol::meta_function::division] = [](sf::Vector2<int> value, lua_sf::LuaIntegral<int> scalar) { return value / scalar.value(); };
    LUASF_STUB_OPERATOR("sf.Vector2i", "div(integer): sf.Vector2i");
    type_sf__Vector2_int_[sol::meta_function::equal_to] = [](const sf::Vector2<int>& left, const sf::Vector2<int>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.Vector2i", "eq(sf.Vector2i): boolean");
    auto type_sf__Vector2_unsigned_int_ = sf.new_usertype<sf::Vector2<unsigned int>>("Vector2u", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Vector2_unsigned_int_);
    sol::table table_sf__Vector2_unsigned_int_ = sf["Vector2u"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Class template for manipulating\n2-dimensional vectors");
    LUASF_STUB_CLASS("sf.Vector2u");
    LUASF_STUB_DOC("X coordinate of the vector");
    LUASF_STUB_FIELD("x", "integer");
    LUASF_STUB_DOC("Y coordinate of the vector");
    LUASF_STUB_FIELD("y", "integer");
    LUASF_STUB_DOC("\\brief Construct the vector from cartesian coordinates\n\n\\param x X coordinate\n\\param y Y coordinate");
    LUASF_STUB_FUNCTION("sf.Vector2u", "new", "fun(x: integer, y: integer): sf.Vector2u");
    LUASF_STUB_OVERLOAD("sf.Vector2u", "new", "fun(): sf.Vector2u");
    type_sf__Vector2_unsigned_int_.set_function("new", sol::factories(
        [](lua_sf::LuaIntegral<unsigned int> x, lua_sf::LuaIntegral<unsigned int> y) {
            return sf::Vector2<unsigned int>{x.value(), y.value()};
        },
        []() {
            return sf::Vector2<unsigned int>{};
        }
    ));
    type_sf__Vector2_unsigned_int_.set("x", sol::property(
        [](sf::Vector2<unsigned int>& self) {
            return self.x;
        },
        [](sf::Vector2<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.x = value.value();
        }
    ));
    type_sf__Vector2_unsigned_int_.set("y", sol::property(
        [](sf::Vector2<unsigned int>& self) {
            return self.y;
        },
        [](sf::Vector2<unsigned int>& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.y = value.value();
        }
    ));
    LUASF_STUB_DOC("\\brief Square of vector's length.\n\nSuitable for comparisons, more efficient than `length()`.");
    LUASF_STUB_FUNCTION("sf.Vector2u", "lengthSquared", "fun(self: sf.Vector2u): integer");
    type_sf__Vector2_unsigned_int_.set_function("lengthSquared",
        [](sf::Vector2<unsigned int>& self) -> unsigned int {
            return self.lengthSquared();
        }
    );
    LUASF_STUB_DOC("\\brief Returns a perpendicular vector.\n\nReturns `*this` rotated by +90 degrees; (x,y) becomes (-y,x).\nFor example, the vector (1,0) is transformed to (0,1).\n\nIn SFML's default coordinate system with +X right and +Y down,\nthis amounts to a clockwise rotation.");
    LUASF_STUB_FUNCTION("sf.Vector2u", "perpendicular", "fun(self: sf.Vector2u): sf.Vector2u");
    type_sf__Vector2_unsigned_int_.set_function("perpendicular",
        [](sf::Vector2<unsigned int>& self) -> sf::Vector2<unsigned int> {
            return self.perpendicular();
        }
    );
    LUASF_STUB_DOC("\\brief Dot product of two 2D vectors.");
    LUASF_STUB_FUNCTION("sf.Vector2u", "dot", "fun(self: sf.Vector2u, rhs: sf.Vector2u): integer");
    type_sf__Vector2_unsigned_int_.set_function("dot",
        [](sf::Vector2<unsigned int>& self, sf::Vector2<unsigned int> rhs) -> unsigned int {
            return self.dot(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Z component of the cross product of two 2D vectors.\n\nTreats the operands as 3D vectors, computes their cross product\nand returns the result's Z component (X and Y components are always zero).");
    LUASF_STUB_FUNCTION("sf.Vector2u", "cross", "fun(self: sf.Vector2u, rhs: sf.Vector2u): integer");
    type_sf__Vector2_unsigned_int_.set_function("cross",
        [](sf::Vector2<unsigned int>& self, sf::Vector2<unsigned int> rhs) -> unsigned int {
            return self.cross(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise multiplication of `*this` and `rhs`.\n\nComputes `(lhs.x*rhs.x, lhs.y*rhs.y)`.\n\nScaling is the most common use case for component-wise multiplication/division.\nThis operation is also known as the Hadamard or Schur product.");
    LUASF_STUB_FUNCTION("sf.Vector2u", "componentWiseMul", "fun(self: sf.Vector2u, rhs: sf.Vector2u): sf.Vector2u");
    type_sf__Vector2_unsigned_int_.set_function("componentWiseMul",
        [](sf::Vector2<unsigned int>& self, sf::Vector2<unsigned int> rhs) -> sf::Vector2<unsigned int> {
            return self.componentWiseMul(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise division of `*this` and `rhs`.\n\nComputes `(lhs.x/rhs.x, lhs.y/rhs.y)`.\n\nScaling is the most common use case for component-wise multiplication/division.\n\n\\pre Neither component of `rhs` is zero.");
    LUASF_STUB_FUNCTION("sf.Vector2u", "componentWiseDiv", "fun(self: sf.Vector2u, rhs: sf.Vector2u): sf.Vector2u");
    type_sf__Vector2_unsigned_int_.set_function("componentWiseDiv",
        [](sf::Vector2<unsigned int>& self, sf::Vector2<unsigned int> rhs) -> sf::Vector2<unsigned int> {
            return self.componentWiseDiv(rhs);
        }
    );
    LUASF_STUB_FUNCTION("sf.Vector2u", "unpack", "fun(self: sf.Vector2u): integer, integer");
    type_sf__Vector2_unsigned_int_.set_function("unpack", [](const sf::Vector2<unsigned int>& self) {
        return std::make_tuple(self.x, self.y);
    });
    type_sf__Vector2_unsigned_int_[sol::meta_function::to_string] = [name = std::string("Vector2u")](const sf::Vector2<unsigned int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ")";
        return stream.str();
    };
    type_sf__Vector2_unsigned_int_[sol::meta_function::unary_minus] = [](const sf::Vector2<unsigned int>& value) { return -value; };
    LUASF_STUB_OPERATOR("sf.Vector2u", "unm: sf.Vector2u");
    type_sf__Vector2_unsigned_int_[sol::meta_function::addition] = [](const sf::Vector2<unsigned int>& left, const sf::Vector2<unsigned int>& right) { return left + right; };
    LUASF_STUB_OPERATOR("sf.Vector2u", "add(sf.Vector2u): sf.Vector2u");
    type_sf__Vector2_unsigned_int_[sol::meta_function::subtraction] = [](const sf::Vector2<unsigned int>& left, const sf::Vector2<unsigned int>& right) { return left - right; };
    LUASF_STUB_OPERATOR("sf.Vector2u", "sub(sf.Vector2u): sf.Vector2u");
    type_sf__Vector2_unsigned_int_[sol::meta_function::multiplication] = sol::overload(
        [](sf::Vector2<unsigned int> value, lua_sf::LuaIntegral<unsigned int> scalar) { return value * scalar.value(); },
        [](lua_sf::LuaIntegral<unsigned int> scalar, sf::Vector2<unsigned int> value) { return scalar.value() * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector2u", "mul(integer): sf.Vector2u");
    type_sf__Vector2_unsigned_int_[sol::meta_function::division] = [](sf::Vector2<unsigned int> value, lua_sf::LuaIntegral<unsigned int> scalar) { return value / scalar.value(); };
    LUASF_STUB_OPERATOR("sf.Vector2u", "div(integer): sf.Vector2u");
    type_sf__Vector2_unsigned_int_[sol::meta_function::equal_to] = [](const sf::Vector2<unsigned int>& left, const sf::Vector2<unsigned int>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.Vector2u", "eq(sf.Vector2u): boolean");
    auto type_sf__Vector2_float_ = sf.new_usertype<sf::Vector2<float>>("Vector2f", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Vector2_float_);
    sol::table table_sf__Vector2_float_ = sf["Vector2f"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Class template for manipulating\n2-dimensional vectors");
    LUASF_STUB_CLASS("sf.Vector2f");
    LUASF_STUB_DOC("X coordinate of the vector");
    LUASF_STUB_FIELD("x", "number");
    LUASF_STUB_DOC("Y coordinate of the vector");
    LUASF_STUB_FIELD("y", "number");
    LUASF_STUB_DOC("\\brief Construct the vector from cartesian coordinates\n\n\\param x X coordinate\n\\param y Y coordinate");
    LUASF_STUB_FUNCTION("sf.Vector2f", "new", "fun(x: number, y: number): sf.Vector2f");
    LUASF_STUB_OVERLOAD("sf.Vector2f", "new", "fun(r: number, phi: sf.Angle): sf.Vector2f");
    LUASF_STUB_OVERLOAD("sf.Vector2f", "new", "fun(): sf.Vector2f");
    type_sf__Vector2_float_.set_function("new", sol::factories(
        [](float x, float y) {
            return sf::Vector2<float>{x, y};
        },
        [](float r, sf::Angle phi) {
            return sf::Vector2<float>{r, phi};
        },
        []() {
            return sf::Vector2<float>{};
        }
    ));
    type_sf__Vector2_float_["x"] = sol::policies(&sf::Vector2<float>::x, sol::self_dependency{});
    type_sf__Vector2_float_["y"] = sol::policies(&sf::Vector2<float>::y, sol::self_dependency{});
    LUASF_STUB_DOC("\\brief Length of the vector <i><b>(floating-point)</b></i>.\n\nIf you are not interested in the actual length, but only in comparisons, consider using `lengthSquared()`.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "length", "fun(self: sf.Vector2f): number");
    type_sf__Vector2_float_.set_function("length",
        [](sf::Vector2<float>& self) -> float {
            return self.length();
        }
    );
    LUASF_STUB_DOC("\\brief Square of vector's length.\n\nSuitable for comparisons, more efficient than `length()`.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "lengthSquared", "fun(self: sf.Vector2f): number");
    type_sf__Vector2_float_.set_function("lengthSquared",
        [](sf::Vector2<float>& self) -> float {
            return self.lengthSquared();
        }
    );
    LUASF_STUB_DOC("\\brief Vector with same direction but length 1 <i><b>(floating-point)</b></i>.\n\n\\pre `*this` is no zero vector.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "normalized", "fun(self: sf.Vector2f): sf.Vector2f");
    type_sf__Vector2_float_.set_function("normalized",
        [](sf::Vector2<float>& self) -> sf::Vector2<float> {
            return self.normalized();
        }
    );
    LUASF_STUB_DOC("\\brief Signed angle from `*this` to `rhs` <i><b>(floating-point)</b></i>.\n\n\\return The smallest angle which rotates `*this` in positive\nor negative direction, until it has the same direction as `rhs`.\nThe result has a sign and lies in the range [-180, 180) degrees.\n\\pre Neither `*this` nor `rhs` is a zero vector.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "angleTo", "fun(self: sf.Vector2f, rhs: sf.Vector2f): sf.Angle");
    type_sf__Vector2_float_.set_function("angleTo",
        [](sf::Vector2<float>& self, sf::Vector2<float> rhs) -> sf::Angle {
            return self.angleTo(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Signed angle from +X or (1,0) vector <i><b>(floating-point)</b></i>.\n\nFor example, the vector (1,0) corresponds to 0 degrees, (0,1) corresponds to 90 degrees.\n\n\\return Angle in the range [-180, 180) degrees.\n\\pre This vector is no zero vector.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "angle", "fun(self: sf.Vector2f): sf.Angle");
    type_sf__Vector2_float_.set_function("angle",
        [](sf::Vector2<float>& self) -> sf::Angle {
            return self.angle();
        }
    );
    LUASF_STUB_DOC("\\brief Rotate by angle \\c phi <i><b>(floating-point)</b></i>.\n\nReturns a vector with same length but different direction.\n\nIn SFML's default coordinate system with +X right and +Y down,\nthis amounts to a clockwise rotation by `phi`.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "rotatedBy", "fun(self: sf.Vector2f, phi: sf.Angle): sf.Vector2f");
    type_sf__Vector2_float_.set_function("rotatedBy",
        [](sf::Vector2<float>& self, sf::Angle phi) -> sf::Vector2<float> {
            return self.rotatedBy(phi);
        }
    );
    LUASF_STUB_DOC("\\brief Projection of this vector onto `axis` <i><b>(floating-point)</b></i>.\n\n\\param axis Vector being projected onto. Need not be normalized.\n\\pre `axis` must not have length zero.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "projectedOnto", "fun(self: sf.Vector2f, axis: sf.Vector2f): sf.Vector2f");
    type_sf__Vector2_float_.set_function("projectedOnto",
        [](sf::Vector2<float>& self, sf::Vector2<float> axis) -> sf::Vector2<float> {
            return self.projectedOnto(axis);
        }
    );
    LUASF_STUB_DOC("\\brief Returns a perpendicular vector.\n\nReturns `*this` rotated by +90 degrees; (x,y) becomes (-y,x).\nFor example, the vector (1,0) is transformed to (0,1).\n\nIn SFML's default coordinate system with +X right and +Y down,\nthis amounts to a clockwise rotation.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "perpendicular", "fun(self: sf.Vector2f): sf.Vector2f");
    type_sf__Vector2_float_.set_function("perpendicular",
        [](sf::Vector2<float>& self) -> sf::Vector2<float> {
            return self.perpendicular();
        }
    );
    LUASF_STUB_DOC("\\brief Dot product of two 2D vectors.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "dot", "fun(self: sf.Vector2f, rhs: sf.Vector2f): number");
    type_sf__Vector2_float_.set_function("dot",
        [](sf::Vector2<float>& self, sf::Vector2<float> rhs) -> float {
            return self.dot(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Z component of the cross product of two 2D vectors.\n\nTreats the operands as 3D vectors, computes their cross product\nand returns the result's Z component (X and Y components are always zero).");
    LUASF_STUB_FUNCTION("sf.Vector2f", "cross", "fun(self: sf.Vector2f, rhs: sf.Vector2f): number");
    type_sf__Vector2_float_.set_function("cross",
        [](sf::Vector2<float>& self, sf::Vector2<float> rhs) -> float {
            return self.cross(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise multiplication of `*this` and `rhs`.\n\nComputes `(lhs.x*rhs.x, lhs.y*rhs.y)`.\n\nScaling is the most common use case for component-wise multiplication/division.\nThis operation is also known as the Hadamard or Schur product.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "componentWiseMul", "fun(self: sf.Vector2f, rhs: sf.Vector2f): sf.Vector2f");
    type_sf__Vector2_float_.set_function("componentWiseMul",
        [](sf::Vector2<float>& self, sf::Vector2<float> rhs) -> sf::Vector2<float> {
            return self.componentWiseMul(rhs);
        }
    );
    LUASF_STUB_DOC("\\brief Component-wise division of `*this` and `rhs`.\n\nComputes `(lhs.x/rhs.x, lhs.y/rhs.y)`.\n\nScaling is the most common use case for component-wise multiplication/division.\n\n\\pre Neither component of `rhs` is zero.");
    LUASF_STUB_FUNCTION("sf.Vector2f", "componentWiseDiv", "fun(self: sf.Vector2f, rhs: sf.Vector2f): sf.Vector2f");
    type_sf__Vector2_float_.set_function("componentWiseDiv",
        [](sf::Vector2<float>& self, sf::Vector2<float> rhs) -> sf::Vector2<float> {
            return self.componentWiseDiv(rhs);
        }
    );
    LUASF_STUB_FUNCTION("sf.Vector2f", "unpack", "fun(self: sf.Vector2f): number, number");
    type_sf__Vector2_float_.set_function("unpack", [](const sf::Vector2<float>& self) {
        return std::make_tuple(self.x, self.y);
    });
    type_sf__Vector2_float_[sol::meta_function::to_string] = [name = std::string("Vector2f")](const sf::Vector2<float>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.x << ", " << self.y << ")";
        return stream.str();
    };
    type_sf__Vector2_float_[sol::meta_function::unary_minus] = [](const sf::Vector2<float>& value) { return -value; };
    LUASF_STUB_OPERATOR("sf.Vector2f", "unm: sf.Vector2f");
    type_sf__Vector2_float_[sol::meta_function::addition] = [](const sf::Vector2<float>& left, const sf::Vector2<float>& right) { return left + right; };
    LUASF_STUB_OPERATOR("sf.Vector2f", "add(sf.Vector2f): sf.Vector2f");
    type_sf__Vector2_float_[sol::meta_function::subtraction] = [](const sf::Vector2<float>& left, const sf::Vector2<float>& right) { return left - right; };
    LUASF_STUB_OPERATOR("sf.Vector2f", "sub(sf.Vector2f): sf.Vector2f");
    type_sf__Vector2_float_[sol::meta_function::multiplication] = sol::overload(
        [](sf::Vector2<float> value, float scalar) { return value * scalar; },
        [](float scalar, sf::Vector2<float> value) { return scalar * value; }
    );
    LUASF_STUB_OPERATOR("sf.Vector2f", "mul(number): sf.Vector2f");
    type_sf__Vector2_float_[sol::meta_function::division] = [](sf::Vector2<float> value, float scalar) { return value / scalar; };
    LUASF_STUB_OPERATOR("sf.Vector2f", "div(number): sf.Vector2f");
    type_sf__Vector2_float_[sol::meta_function::equal_to] = [](const sf::Vector2<float>& left, const sf::Vector2<float>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.Vector2f", "eq(sf.Vector2f): boolean");
}
