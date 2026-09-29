#include "Graphics/bind_Transform.hpp"

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

namespace { constexpr std::array<std::string_view, 14> docs = {
    "\\brief 3x3 transform matrix",
    "\\brief Default constructor\n\nCreates an identity transform (a transform that does nothing).",
    "\\brief Construct a transform from a 3x3 matrix\n\n\\param a00 Element (0, 0) of the matrix\n\\param a01 Element (0, 1) of the matrix\n\\param a02 Element (0, 2) of the matrix\n\\param a10 Element (1, 0) of the matrix\n\\param a11 Element (1, 1) of the matrix\n\\param a12 Element (1, 2) of the matrix\n\\param a20 Element (2, 0) of the matrix\n\\param a21 Element (2, 1) of the matrix\n\\param a22 Element (2, 2) of the matrix",
    "\\brief Return the transform as a 4x4 matrix\n\nThis function returns a pointer to an array of 16 floats\ncontaining the transform elements as a 4x4 matrix, which\nis directly compatible with OpenGL functions.\n\n\\code\nsf::Transform transform = ...;\nglUniformMatrix4fv(location, 1, GL_FALSE, transform.getMatrix());\n\\endcode\n\n\\return Pointer to a 4x4 matrix",
    "\\brief Return the inverse of the transform\n\nIf the inverse cannot be computed, an identity transform\nis returned.\n\n\\return A new transform which is the inverse of self",
    "\\brief Transform a 2D point\n\nThese two statements are equivalent:\n\\code\nsf::Vector2f transformedPoint = matrix.transformPoint(point);\nsf::Vector2f transformedPoint = matrix * point;\n\\endcode\n\n\\param point Point to transform\n\n\\return Transformed point",
    "\\brief Transform a rectangle\n\nSince SFML doesn't provide support for oriented rectangles,\nthe result of this function is always an axis-aligned\nrectangle. Which means that if the transform contains a\nrotation, the bounding rectangle of the transformed rectangle\nis returned.\n\n\\param rectangle Rectangle to transform\n\n\\return Transformed rectangle",
    "\\brief Combine the current transform with another one\n\nThe result is a transform that is equivalent to applying\n`transform` followed by `*this`. Mathematically, it is\nequivalent to a matrix multiplication `(*this) * transform`.\n\nThese two statements are equivalent:\n\\code\nleft.combine(right);\nleft *= right;\n\\endcode\n\n\\param transform Transform to combine with this transform\n\n\\return Reference to `*this`",
    "\\brief Combine the current transform with a translation\n\nThis function returns a reference to `*this`, so that calls\ncan be chained.\n\\code\nsf::Transform transform;\ntransform.translate(sf::Vector2f(100, 200)).rotate(sf::degrees(45));\n\\endcode\n\n\\param offset Translation offset to apply\n\n\\return Reference to `*this`\n\n\\see `rotate`, `scale`",
    "\\brief Combine the current transform with a rotation\n\nThis function returns a reference to `*this`, so that calls\ncan be chained.\n\\code\nsf::Transform transform;\ntransform.rotate(sf::degrees(90)).translate(50, 20);\n\\endcode\n\n\\param angle Rotation angle\n\n\\return Reference to `*this`\n\n\\see `translate`, `scale`",
    "\\brief Combine the current transform with a rotation\n\nThe center of rotation is provided for convenience as a second\nargument, so that you can build rotations around arbitrary points\nmore easily (and efficiently) than the usual\n`translate(-center).rotate(angle).translate(center)`.\n\nThis function returns a reference to `*this`, so that calls\ncan be chained.\n\\code\nsf::Transform transform;\ntransform.rotate(sf::degrees(90), sf::Vector2f(8, 3)).translate(sf::Vector2f(50, 20));\n\\endcode\n\n\\param angle Rotation angle\n\\param center Center of rotation\n\n\\return Reference to `*this`\n\n\\see `translate`, `scale`",
    "\\brief Combine the current transform with a scaling\n\nThis function returns a reference to `*this`, so that calls\ncan be chained.\n\\code\nsf::Transform transform;\ntransform.scale(sf::Vector2f(2, 1)).rotate(sf::degrees(45));\n\\endcode\n\n\\param factors Scaling factors\n\n\\return Reference to `*this`\n\n\\see `translate`, `rotate`",
    "\\brief Combine the current transform with a scaling\n\nThe center of scaling is provided for convenience as a second\nargument, so that you can build scaling around arbitrary points\nmore easily (and efficiently) than the usual\n`translate(-center).scale(factors).translate(center)`.\n\nThis function returns a reference to `*this`, so that calls\ncan be chained.\n\\code\nsf::Transform transform;\ntransform.scale(sf::Vector2f(2, 1), sf::Vector2f(8, 3)).rotate(45);\n\\endcode\n\n\\param factors Scaling factors\n\\param center Center of scaling\n\n\\return Reference to `*this`\n\n\\see `translate`, `rotate`",
    "The identity transform (does nothing)",
}; }

void bind_Transform(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Transform = lua_glue::BindStruct<sf::Transform>(sf, "Transform");
    lua_glue::Table table_sf__Transform = sf["Transform"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Transform");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Transform", "new", "fun(a00: number, a01: number, a02: number, a10: number, a11: number, a12: number, a20: number, a21: number, a22: number): sf.Transform");
    LUASF_STUB_OVERLOAD("sf.Transform", "new", "fun(): sf.Transform");
    lua_glue::BindCallable(type_sf__Transform, "new",
        [](float a00, float a01, float a02, float a10, float a11, float a12, float a20, float a21, float a22) {
            return sf::Transform{a00, a01, a02, a10, a11, a12, a20, a21, a22};
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__Transform, "new",
        []() {
            return sf::Transform{};
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Transform", "getMatrix", "fun(self: sf.Transform): number[]");
    lua_glue::BindCallable(type_sf__Transform, "getMatrix",
        [](const sf::Transform& self) {
            const auto* result = self.getMatrix();
            if (!result)
                return lua_glue::AsTable(std::vector<float>{});
            std::vector<float> result_values(result, result + static_cast<std::size_t>(16));
            return lua_glue::AsTable(std::move(result_values));
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Transform", "getInverse", "fun(self: sf.Transform): sf.Transform");
    lua_glue::BindCallable(type_sf__Transform, "getInverse",
        [](const sf::Transform& self) -> sf::Transform {
            return self.getInverse();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Transform", "transformPoint", "fun(self: sf.Transform, point: sf.Vector2f): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Transform, "transformPoint",
        [](const sf::Transform& self, sf::Vector2f point) -> sf::Vector2f {
            return self.transformPoint(point);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Transform", "transformRect", "fun(self: sf.Transform, rectangle: sf.FloatRect): sf.FloatRect");
    lua_glue::BindCallable(type_sf__Transform, "transformRect",
        [](const sf::Transform& self, const sf::FloatRect& rectangle) -> sf::FloatRect {
            return self.transformRect(rectangle);
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Transform", "combine", "fun(self: sf.Transform, transform: sf.Transform): sf.Transform");
    lua_glue::BindCallable(type_sf__Transform, "combine",
        [](sf::Transform& self, const sf::Transform& transform) {
            return std::ref(self.combine(transform));
        },
        docs[7],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Transform", "translate", "fun(self: sf.Transform, offset: sf.Vector2f): sf.Transform");
    lua_glue::BindCallable(type_sf__Transform, "translate",
        [](sf::Transform& self, sf::Vector2f offset) {
            return std::ref(self.translate(offset));
        },
        docs[8],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Transform", "rotate", "fun(self: sf.Transform, angle: sf.Angle, center: sf.Vector2f): sf.Transform");
    LUASF_STUB_OVERLOAD("sf.Transform", "rotate", "fun(self: sf.Transform, angle: sf.Angle): sf.Transform");
    lua_glue::BindCallable(type_sf__Transform, "rotate",
        [](sf::Transform& self, sf::Angle angle, sf::Vector2f center) {
            return std::ref(self.rotate(angle, center));
        },
        docs[10],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    lua_glue::BindCallable(type_sf__Transform, "rotate",
        [](sf::Transform& self, sf::Angle angle) {
            return std::ref(self.rotate(angle));
        },
        docs[9],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Transform", "scale", "fun(self: sf.Transform, factors: sf.Vector2f, center: sf.Vector2f): sf.Transform");
    LUASF_STUB_OVERLOAD("sf.Transform", "scale", "fun(self: sf.Transform, factors: sf.Vector2f): sf.Transform");
    lua_glue::BindCallable(type_sf__Transform, "scale",
        [](sf::Transform& self, sf::Vector2f factors, sf::Vector2f center) {
            return std::ref(self.scale(factors, center));
        },
        docs[12],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    lua_glue::BindCallable(type_sf__Transform, "scale",
        [](sf::Transform& self, sf::Vector2f factors) {
            return std::ref(self.scale(factors));
        },
        docs[11],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_VALUE("sf.Transform", "Identity", "sf.Transform");
    lua_glue::BindStaticAttr<const sf::Transform>(table_sf__Transform, "Identity", &sf::Transform::Identity);
    LUASF_STUB_FUNCTION("sf.Transform", "copy", "fun(self: sf.Transform): sf.Transform");
    LUASF_STUB_FUNCTION("sf.Transform", "deepcopy", "fun(self: sf.Transform): sf.Transform");
}
