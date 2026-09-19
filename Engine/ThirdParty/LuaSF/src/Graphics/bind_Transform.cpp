#include "Graphics/bind_Transform.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Transform(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Transform = sf.new_usertype<sf::Transform>("Transform", sol::no_constructor);
    sol::table table_sf__Transform = sf["Transform"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Transform>(lua);
    LUASF_STUB_DOC("\\brief 3x3 transform matrix");
    LUASF_STUB_CLASS("sf.Transform");
    LUASF_STUB_DOC("\\brief Construct a transform from a 3x3 matrix\n\n\\param a00 Element (0, 0) of the matrix\n\\param a01 Element (0, 1) of the matrix\n\\param a02 Element (0, 2) of the matrix\n\\param a10 Element (1, 0) of the matrix\n\\param a11 Element (1, 1) of the matrix\n\\param a12 Element (1, 2) of the matrix\n\\param a20 Element (2, 0) of the matrix\n\\param a21 Element (2, 1) of the matrix\n\\param a22 Element (2, 2) of the matrix");
    LUASF_STUB_FUNCTION("sf.Transform", "new", "fun(a00: number, a01: number, a02: number, a10: number, a11: number, a12: number, a20: number, a21: number, a22: number): sf.Transform");
    LUASF_STUB_OVERLOAD("sf.Transform", "new", "fun(): sf.Transform");
    type_sf__Transform.set_function("new", sol::factories(
        [](float a00, float a01, float a02, float a10, float a11, float a12, float a20, float a21, float a22) {
            return lua_sf::makeLuaSharedObject<sf::Transform>(a00, a01, a02, a10, a11, a12, a20, a21, a22);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::Transform>();
        }
    ));
    LUASF_STUB_DOC("\\brief Return the transform as a 4x4 matrix\n\nThis function returns a pointer to an array of 16 floats\ncontaining the transform elements as a 4x4 matrix, which\nis directly compatible with OpenGL functions.\n\n\\code\nsf::Transform transform = ...;\nglUniformMatrix4fv(location, 1, GL_FALSE, transform.getMatrix());\n\\endcode\n\n\\return Pointer to a 4x4 matrix");
    LUASF_STUB_FUNCTION("sf.Transform", "getMatrix", "fun(self: sf.Transform): number[]");
    type_sf__Transform.set_function("getMatrix",
        [](sf::Transform& self) {
            const auto* result = self.getMatrix();
            if (!result)
                return sol::as_table(std::vector<float>{});
            std::vector<float> result_values(result, result + static_cast<std::size_t>(16));
            return sol::as_table(std::move(result_values));
        }
    );
    LUASF_STUB_DOC("\\brief Return the inverse of the transform\n\nIf the inverse cannot be computed, an identity transform\nis returned.\n\n\\return A new transform which is the inverse of self");
    LUASF_STUB_FUNCTION("sf.Transform", "getInverse", "fun(self: sf.Transform): sf.Transform");
    type_sf__Transform.set_function("getInverse",
        [](sf::Transform& self) -> sf::Transform {
            return self.getInverse();
        }
    );
    LUASF_STUB_DOC("\\brief Transform a 2D point\n\nThese two statements are equivalent:\n\\code\nsf::Vector2f transformedPoint = matrix.transformPoint(point);\nsf::Vector2f transformedPoint = matrix * point;\n\\endcode\n\n\\param point Point to transform\n\n\\return Transformed point");
    LUASF_STUB_FUNCTION("sf.Transform", "transformPoint", "fun(self: sf.Transform, point: sf.Vector2f): sf.Vector2f");
    type_sf__Transform.set_function("transformPoint",
        [](sf::Transform& self, sf::Vector2f point) -> sf::Vector2f {
            return self.transformPoint(point);
        }
    );
    LUASF_STUB_DOC("\\brief Transform a rectangle\n\nSince SFML doesn't provide support for oriented rectangles,\nthe result of this function is always an axis-aligned\nrectangle. Which means that if the transform contains a\nrotation, the bounding rectangle of the transformed rectangle\nis returned.\n\n\\param rectangle Rectangle to transform\n\n\\return Transformed rectangle");
    LUASF_STUB_FUNCTION("sf.Transform", "transformRect", "fun(self: sf.Transform, rectangle: sf.FloatRect): sf.FloatRect");
    type_sf__Transform.set_function("transformRect",
        [](sf::Transform& self, const sf::FloatRect& rectangle) -> sf::FloatRect {
            return self.transformRect(rectangle);
        }
    );
    LUASF_STUB_DOC("\\brief Combine the current transform with another one\n\nThe result is a transform that is equivalent to applying\n`transform` followed by `*this`. Mathematically, it is\nequivalent to a matrix multiplication `(*this) * transform`.\n\nThese two statements are equivalent:\n\\code\nleft.combine(right);\nleft *= right;\n\\endcode\n\n\\param transform Transform to combine with this transform\n\n\\return Reference to `*this`");
    LUASF_STUB_FUNCTION("sf.Transform", "combine", "fun(self: sf.Transform, transform: sf.Transform): sf.Transform");
    type_sf__Transform.set_function("combine",
        sol::policies(
            [](sf::Transform& self, const sf::Transform& transform) {
                return std::ref(self.combine(transform));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Combine the current transform with a translation\n\nThis function returns a reference to `*this`, so that calls\ncan be chained.\n\\code\nsf::Transform transform;\ntransform.translate(sf::Vector2f(100, 200)).rotate(sf::degrees(45));\n\\endcode\n\n\\param offset Translation offset to apply\n\n\\return Reference to `*this`\n\n\\see `rotate`, `scale`");
    LUASF_STUB_FUNCTION("sf.Transform", "translate", "fun(self: sf.Transform, offset: sf.Vector2f): sf.Transform");
    type_sf__Transform.set_function("translate",
        sol::policies(
            [](sf::Transform& self, sf::Vector2f offset) {
                return std::ref(self.translate(offset));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Combine the current transform with a rotation\n\nThe center of rotation is provided for convenience as a second\nargument, so that you can build rotations around arbitrary points\nmore easily (and efficiently) than the usual\n`translate(-center).rotate(angle).translate(center)`.\n\nThis function returns a reference to `*this`, so that calls\ncan be chained.\n\\code\nsf::Transform transform;\ntransform.rotate(sf::degrees(90), sf::Vector2f(8, 3)).translate(sf::Vector2f(50, 20));\n\\endcode\n\n\\param angle Rotation angle\n\\param center Center of rotation\n\n\\return Reference to `*this`\n\n\\see `translate`, `scale`");
    LUASF_STUB_FUNCTION("sf.Transform", "rotate", "fun(self: sf.Transform, angle: sf.Angle, center: sf.Vector2f): sf.Transform");
    LUASF_STUB_OVERLOAD("sf.Transform", "rotate", "fun(self: sf.Transform, angle: sf.Angle): sf.Transform");
    type_sf__Transform.set_function("rotate",
        sol::policies(
            sol::overload(
                [](sf::Transform& self, sf::Angle angle, sf::Vector2f center) {
                    return std::ref(self.rotate(angle, center));
                },
                [](sf::Transform& self, sf::Angle angle) {
                    return std::ref(self.rotate(angle));
                }
            ),
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Combine the current transform with a scaling\n\nThe center of scaling is provided for convenience as a second\nargument, so that you can build scaling around arbitrary points\nmore easily (and efficiently) than the usual\n`translate(-center).scale(factors).translate(center)`.\n\nThis function returns a reference to `*this`, so that calls\ncan be chained.\n\\code\nsf::Transform transform;\ntransform.scale(sf::Vector2f(2, 1), sf::Vector2f(8, 3)).rotate(45);\n\\endcode\n\n\\param factors Scaling factors\n\\param center Center of scaling\n\n\\return Reference to `*this`\n\n\\see `translate`, `rotate`");
    LUASF_STUB_FUNCTION("sf.Transform", "scale", "fun(self: sf.Transform, factors: sf.Vector2f, center: sf.Vector2f): sf.Transform");
    LUASF_STUB_OVERLOAD("sf.Transform", "scale", "fun(self: sf.Transform, factors: sf.Vector2f): sf.Transform");
    type_sf__Transform.set_function("scale",
        sol::policies(
            sol::overload(
                [](sf::Transform& self, sf::Vector2f factors, sf::Vector2f center) {
                    return std::ref(self.scale(factors, center));
                },
                [](sf::Transform& self, sf::Vector2f factors) {
                    return std::ref(self.scale(factors));
                }
            ),
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("The identity transform (does nothing)");
    LUASF_STUB_VALUE("sf.Transform", "Identity", "sf.Transform");
    table_sf__Transform["Identity"] = sf::Transform::Identity;
}
