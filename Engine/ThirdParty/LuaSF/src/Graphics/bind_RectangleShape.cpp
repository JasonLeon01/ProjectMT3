#include "Graphics/bind_RectangleShape.hpp"

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

namespace { constexpr std::array<std::string_view, 37> docs = {
    "\\brief Specialized shape representing a rectangle",
    "\\brief Default constructor\n\n\\param size Size of the rectangle",
    "\\brief set the position of the object\n\nThis function completely overwrites the previous position.\nSee the move function to apply an offset based on the previous position instead.\nThe default position of a transformable object is (0, 0).\n\nNote that `sf::Text` may appear offset when positioned.\nThis is because its local bounds are influenced by font metrics (e.g. tallest characters)\nto consistently align with the text's baseline. As such the `getGlobalBounds()`\nposition may not match the position you set.\n\nTo account for this offset, the local bounds need to be considered.\nEither by including it in the position calculation:\n\\code\ntext.setPosition(position - text.getLocalBounds().position);\n\\endcode\nOr by adjusting the text's origin:\n\\code\ntext.setOrigin(text.getLocalBounds().position);\ntext.setPosition(position);\n\\endcode\n\n\\param position New position\n\n\\see `move`, `getPosition`",
    "\\brief set the orientation of the object\n\nThis function completely overwrites the previous rotation.\nSee the rotate function to add an angle based on the previous rotation instead.\nThe default rotation of a transformable object is 0.\n\n\\param angle New rotation\n\n\\see `rotate`, `getRotation`",
    "\\brief set the scale factors of the object\n\nThis function completely overwrites the previous scale.\nSee the scale function to add a factor based on the previous scale instead.\nThe default scale of a transformable object is (1, 1).\n\n\\param factors New scale factors\n\n\\see `scale`, `getScale`",
    "\\brief set the local origin of the object\n\nThe origin of an object defines the center point for\nall transformations (position, scale, rotation).\nThe coordinates of this point must be relative to the\ntop-left corner of the object, and ignore all\ntransformations (position, scale, rotation).\nThe default origin of a transformable object is (0, 0).\n\n\\param origin New origin\n\n\\see `getOrigin`",
    "\\brief get the position of the object\n\n\\return Current position\n\n\\see `setPosition`",
    "\\brief get the orientation of the object\n\nThe rotation is always in the range [0, 360].\n\n\\return Current rotation\n\n\\see `setRotation`",
    "\\brief get the current scale of the object\n\n\\return Current scale factors\n\n\\see `setScale`",
    "\\brief get the local origin of the object\n\n\\return Current origin\n\n\\see `setOrigin`",
    "\\brief Move the object by a given offset\n\nThis function adds to the current position of the object,\nunlike `setPosition` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setPosition(object.getPosition() + offset);\n\\endcode\n\n\\param offset Offset\n\n\\see `setPosition`",
    "\\brief Rotate the object\n\nThis function adds to the current rotation of the object,\nunlike `setRotation` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setRotation(object.getRotation() + angle);\n\\endcode\n\n\\param angle Angle of rotation",
    "\\brief Scale the object\n\nThis function multiplies the current scale of the object,\nunlike `setScale` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nsf::Vector2f scale = object.getScale();\nobject.setScale(scale.x * factor.x, scale.y * factor.y);\n\\endcode\n\n\\param factor Scale factors\n\n\\see `setScale`",
    "\\brief get the combined transform of the object\n\n\\return Transform combining the position/rotation/scale/origin of the object\n\n\\see `getInverseTransform`",
    "\\brief get the inverse of the combined transform of the object\n\n\\return Inverse of the combined transformations applied to the object\n\n\\see `getTransform`",
    "\\brief Change the source texture of the shape\n\nThe `texture` argument refers to a texture that must\nexist as long as the shape uses it. Indeed, the shape\ndoesn't store its own copy of the texture, but rather keeps\na pointer to the one that you passed to this function.\nIf the source texture is destroyed and the shape tries to\nuse it, the behavior is undefined.\n`texture` can be a null pointer to disable texturing.\nIf `resetRect` is `true`, the `TextureRect` property of\nthe shape is automatically adjusted to the size of the new\ntexture. If it is `false`, the texture rect is left unchanged.\n\n\\param texture   New texture\n\\param resetRect Should the texture rect be reset to the size of the new texture?\n\n\\see `getTexture`, `setTextureRect`",
    "\\brief Set the sub-rectangle of the texture that the shape will display\n\nThe texture rect is useful when you don't want to display\nthe whole texture, but rather a part of it.\nBy default, the texture rect covers the entire texture.\n\n\\param rect Rectangle defining the region of the texture to display\n\n\\see `getTextureRect`, `setTexture`",
    "\\brief Set the fill color of the shape\n\nThis color is modulated (multiplied) with the shape's\ntexture if any. It can be used to colorize the shape,\nor change its global opacity.\nYou can use `sf::Color::Transparent` to make the inside of\nthe shape transparent, and have the outline alone.\nBy default, the shape's fill color is opaque white.\n\n\\param color New color of the shape\n\n\\see `getFillColor`, `setOutlineColor`",
    "\\brief Set the outline color of the shape\n\nBy default, the shape's outline color is opaque white.\n\n\\param color New outline color of the shape\n\n\\see `getOutlineColor`, `setFillColor`",
    "\\brief Set the thickness of the shape's outline\n\nNote that negative values are allowed (so that the outline\nexpands towards the center of the shape), and using zero\ndisables the outline.\nBy default, the outline thickness is 0.\n\n\\param thickness New outline thickness\n\n\\see `getOutlineThickness`",
    "\\brief Set the limit on the ratio between miter length and outline thickness\n\nOutline segments around each shape corner are joined either\nwith a miter or a bevel join.\n- A miter join is formed by extending outline segments until\nthey intersect. The distance between the point of\nintersection and the shape's corner is the miter length.\n- A bevel join is formed by connecting outline segments with\na straight line perpendicular to the corner's bissector.\n\nThe miter limit is used to determine whether ouline segments\naround a corner are joined with a bevel or a miter.\nWhen the ratio between the miter length and outline thickness\nexceeds the miter limit, a bevel is used instead of a miter.\n\nThe miter limit is linked to the maximum inner angle of a\ncorner below which a bevel is used by the following formula:\n\nmiterLimit = 1 / sin(angle / 2)\n\nThe miter limit must be greater than or equal to 1.\nBy default, the miter limit is 10.\n\n\\param miterLimit New miter limit\n\n\\see getMiterLimit",
    "\\brief Get the source texture of the shape\n\nIf the shape has no source texture, a `nullptr` is returned.\nThe returned pointer is const, which means that you can't\nmodify the texture when you retrieve it with this function.\n\n\\return Pointer to the shape's texture\n\n\\see `setTexture`",
    "\\brief Get the sub-rectangle of the texture displayed by the shape\n\n\\return Texture rectangle of the shape\n\n\\see `setTextureRect`",
    "\\brief Get the fill color of the shape\n\n\\return Fill color of the shape\n\n\\see `setFillColor`",
    "\\brief Get the outline color of the shape\n\n\\return Outline color of the shape\n\n\\see `setOutlineColor`",
    "\\brief Get the outline thickness of the shape\n\n\\return Outline thickness of the shape\n\n\\see `setOutlineThickness`",
    "\\brief Get the limit on the ratio between miter length and outline thickness\n\n\\return Limit on the ratio between miter length and outline thickness\n\n\\see setMiterLimit",
    "\\brief Get the total number of points of the shape\n\n\\return Number of points of the shape\n\n\\see `getPoint`",
    "\\brief Get a point of the shape\n\nThe returned point is in local coordinates, that is,\nthe shape's transforms (position, rotation, scale) are\nnot taken into account.\nThe result is undefined if `index` is out of the valid range.\n\n\\param index Index of the point to get, in range [0 .. getPointCount() - 1]\n\n\\return `index`-th point of the shape\n\n\\see `getPointCount`",
    "\\brief Get the geometric center of the shape\n\nThe returned point is in local coordinates, that is,\nthe shape's transforms (position, rotation, scale) are\nnot taken into account.\n\n\\return The geometric center of the shape",
    "\\brief Get the local bounding rectangle of the entity\n\nThe returned rectangle is in local coordinates, which means\nthat it ignores the transformations (translation, rotation,\nscale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\nentity in the entity's coordinate system.\n\n\\return Local bounding rectangle of the entity",
    "\\brief Get the global (non-minimal) bounding rectangle of the entity\n\nThe returned rectangle is in global coordinates, which means\nthat it takes into account the transformations (translation,\nrotation, scale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\nshape in the global 2D world's coordinate system.\n\nThis function does not necessarily return the _minimal_\nbounding rectangle. It merely ensures that the returned\nrectangle covers all the vertices (but possibly more).\nThis allows for a fast approximation of the bounds as a\nfirst check; you may want to use more precise checks\non top of that.\n\n\\return Global bounding rectangle of the entity",
    "\\brief Set the size of the rectangle\n\n\\param size New size of the rectangle\n\n\\see `getSize`",
    "\\brief Get the size of the rectangle\n\n\\return Size of the rectangle\n\n\\see `setSize`",
    "\\brief Get the number of points defining the shape\n\n\\return Number of points of the shape. For rectangle\nshapes, this number is always 4.",
    "\\brief Get a point of the rectangle\n\nThe returned point is in local coordinates, that is,\nthe shape's transforms (position, rotation, scale) are\nnot taken into account.\nThe result is undefined if `index` is out of the valid range.\n\n\\param index Index of the point to get, in range [0 .. 3]\n\n\\return `index`-th point of the shape",
    "\\brief Get the geometric center of the rectangle\n\nThe returned point is in local coordinates, that is,\nthe shape's transforms (position, rotation, scale) are\nnot taken into account.\n\n\\return The geometric center of the shape",
}; }

void bind_RectangleShape(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__RectangleShape = lua_glue::BindClass<sf::RectangleShape>(sf, "RectangleShape");
    lua_glue::BindBase<sf::RectangleShape, sf::Shape>(type_sf__RectangleShape);
    lua_glue::BindBase<sf::RectangleShape, sf::Drawable>(type_sf__RectangleShape);
    lua_glue::BindBase<sf::RectangleShape, sf::Transformable>(type_sf__RectangleShape);
    lua_glue::Table table_sf__RectangleShape = sf["RectangleShape"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::RectangleShape>(lua);
    lua_glue::Table native_bases_sf__RectangleShape = lua.create_table();
    native_bases_sf__RectangleShape.add(lua["sf"]["Shape"].get<lua_glue::Table>());
    table_sf__RectangleShape.raw_set("__nativeBases", native_bases_sf__RectangleShape);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.RectangleShape", "sf.Shape, sf.Drawable, sf.Transformable");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "new", "fun(size?: sf.Vector2f): sf.RectangleShape");
    lua_glue::BindCallable(type_sf__RectangleShape, "new",
        [](sf::Vector2f size) {
            return lua_sf::makeLuaSharedObject<sf::RectangleShape>(size);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return sf::Vector2<float>{ };
        }}},
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setPosition", "fun(self: sf.RectangleShape, position: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setPosition",
        [](sf::RectangleShape& self, sf::Vector2f position) {
            static_cast<sf::Transformable&>(self).setPosition(position);
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setRotation", "fun(self: sf.RectangleShape, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setRotation",
        [](sf::RectangleShape& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).setRotation(angle);
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setScale", "fun(self: sf.RectangleShape, factors: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setScale",
        [](sf::RectangleShape& self, sf::Vector2f factors) {
            static_cast<sf::Transformable&>(self).setScale(factors);
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setOrigin", "fun(self: sf.RectangleShape, origin: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setOrigin",
        [](sf::RectangleShape& self, sf::Vector2f origin) {
            static_cast<sf::Transformable&>(self).setOrigin(origin);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getPosition", "fun(self: sf.RectangleShape): sf.Vector2f");
    lua_glue::BindCallable(type_sf__RectangleShape, "getPosition",
        [](const sf::RectangleShape& self) -> sf::Vector2f {
            return static_cast<const sf::Transformable&>(self).getPosition();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getRotation", "fun(self: sf.RectangleShape): sf.Angle");
    lua_glue::BindCallable(type_sf__RectangleShape, "getRotation",
        [](const sf::RectangleShape& self) -> sf::Angle {
            return static_cast<const sf::Transformable&>(self).getRotation();
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getScale", "fun(self: sf.RectangleShape): sf.Vector2f");
    lua_glue::BindCallable(type_sf__RectangleShape, "getScale",
        [](const sf::RectangleShape& self) -> sf::Vector2f {
            return static_cast<const sf::Transformable&>(self).getScale();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getOrigin", "fun(self: sf.RectangleShape): sf.Vector2f");
    lua_glue::BindCallable(type_sf__RectangleShape, "getOrigin",
        [](const sf::RectangleShape& self) -> sf::Vector2f {
            return static_cast<const sf::Transformable&>(self).getOrigin();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "move", "fun(self: sf.RectangleShape, offset: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__RectangleShape, "move",
        [](sf::RectangleShape& self, sf::Vector2f offset) {
            static_cast<sf::Transformable&>(self).move(offset);
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "rotate", "fun(self: sf.RectangleShape, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__RectangleShape, "rotate",
        [](sf::RectangleShape& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).rotate(angle);
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "scale", "fun(self: sf.RectangleShape, factor: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__RectangleShape, "scale",
        [](sf::RectangleShape& self, sf::Vector2f factor) {
            static_cast<sf::Transformable&>(self).scale(factor);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getTransform", "fun(self: sf.RectangleShape): sf.Transform");
    lua_glue::BindCallable(type_sf__RectangleShape, "getTransform",
        [](const sf::RectangleShape& self) {
            return std::cref(static_cast<const sf::Transformable&>(self).getTransform());
        },
        docs[13],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getInverseTransform", "fun(self: sf.RectangleShape): sf.Transform");
    lua_glue::BindCallable(type_sf__RectangleShape, "getInverseTransform",
        [](const sf::RectangleShape& self) {
            return std::cref(static_cast<const sf::Transformable&>(self).getInverseTransform());
        },
        docs[14],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setTexture", "fun(self: sf.RectangleShape, texture: sf.Texture, resetRect?: boolean)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setTexture",
        [](sf::RectangleShape& self, const sf::Texture* texture, bool resetRect) {
            static_cast<sf::Shape&>(self).setTexture(texture, resetRect);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setTextureRect", "fun(self: sf.RectangleShape, rect: sf.IntRect)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setTextureRect",
        [](sf::RectangleShape& self, const sf::IntRect& rect) {
            static_cast<sf::Shape&>(self).setTextureRect(rect);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setFillColor", "fun(self: sf.RectangleShape, color: sf.Color)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setFillColor",
        [](sf::RectangleShape& self, sf::Color color) {
            static_cast<sf::Shape&>(self).setFillColor(color);
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setOutlineColor", "fun(self: sf.RectangleShape, color: sf.Color)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setOutlineColor",
        [](sf::RectangleShape& self, sf::Color color) {
            static_cast<sf::Shape&>(self).setOutlineColor(color);
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setOutlineThickness", "fun(self: sf.RectangleShape, thickness: number)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setOutlineThickness",
        [](sf::RectangleShape& self, float thickness) {
            static_cast<sf::Shape&>(self).setOutlineThickness(thickness);
        },
        docs[19]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setMiterLimit", "fun(self: sf.RectangleShape, miterLimit: number)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setMiterLimit",
        [](sf::RectangleShape& self, float miterLimit) {
            static_cast<sf::Shape&>(self).setMiterLimit(miterLimit);
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getTexture", "fun(self: sf.RectangleShape): sf.Texture");
    lua_glue::BindCallable(type_sf__RectangleShape, "getTexture",
        [](const sf::RectangleShape& self) -> const sf::Texture* {
            return static_cast<const sf::Shape&>(self).getTexture();
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getTextureRect", "fun(self: sf.RectangleShape): sf.IntRect");
    lua_glue::BindCallable(type_sf__RectangleShape, "getTextureRect",
        [](const sf::RectangleShape& self) {
            return std::cref(static_cast<const sf::Shape&>(self).getTextureRect());
        },
        docs[22],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getFillColor", "fun(self: sf.RectangleShape): sf.Color");
    lua_glue::BindCallable(type_sf__RectangleShape, "getFillColor",
        [](const sf::RectangleShape& self) -> sf::Color {
            return static_cast<const sf::Shape&>(self).getFillColor();
        },
        docs[23]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getOutlineColor", "fun(self: sf.RectangleShape): sf.Color");
    lua_glue::BindCallable(type_sf__RectangleShape, "getOutlineColor",
        [](const sf::RectangleShape& self) -> sf::Color {
            return static_cast<const sf::Shape&>(self).getOutlineColor();
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getOutlineThickness", "fun(self: sf.RectangleShape): number");
    lua_glue::BindCallable(type_sf__RectangleShape, "getOutlineThickness",
        [](const sf::RectangleShape& self) -> float {
            return static_cast<const sf::Shape&>(self).getOutlineThickness();
        },
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getMiterLimit", "fun(self: sf.RectangleShape): number");
    lua_glue::BindCallable(type_sf__RectangleShape, "getMiterLimit",
        [](const sf::RectangleShape& self) -> float {
            return static_cast<const sf::Shape&>(self).getMiterLimit();
        },
        docs[26]
    );
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getPointCount", "fun(self: sf.RectangleShape): integer");
    lua_glue::BindCallable(type_sf__RectangleShape, "getPointCount",
        [](const sf::RectangleShape& self) -> std::size_t {
            return self.getPointCount();
        },
        docs[34]
    );
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getPoint", "fun(self: sf.RectangleShape, index: integer): sf.Vector2f");
    lua_glue::BindCallable(type_sf__RectangleShape, "getPoint",
        [](const sf::RectangleShape& self, lua_sf::LuaIntegral<std::size_t> index) -> sf::Vector2f {
            return self.getPoint(index.value());
        },
        docs[35]
    );
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getGeometricCenter", "fun(self: sf.RectangleShape): sf.Vector2f");
    lua_glue::BindCallable(type_sf__RectangleShape, "getGeometricCenter",
        [](const sf::RectangleShape& self) -> sf::Vector2f {
            return self.getGeometricCenter();
        },
        docs[36]
    );
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getLocalBounds", "fun(self: sf.RectangleShape): sf.FloatRect");
    lua_glue::BindCallable(type_sf__RectangleShape, "getLocalBounds",
        [](const sf::RectangleShape& self) -> sf::FloatRect {
            return static_cast<const sf::Shape&>(self).getLocalBounds();
        },
        docs[30]
    );
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getGlobalBounds", "fun(self: sf.RectangleShape): sf.FloatRect");
    lua_glue::BindCallable(type_sf__RectangleShape, "getGlobalBounds",
        [](const sf::RectangleShape& self) -> sf::FloatRect {
            return static_cast<const sf::Shape&>(self).getGlobalBounds();
        },
        docs[31]
    );
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setSize", "fun(self: sf.RectangleShape, size: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__RectangleShape, "setSize",
        [](sf::RectangleShape& self, sf::Vector2f size) {
            self.setSize(size);
        },
        docs[32]
    );
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getSize", "fun(self: sf.RectangleShape): sf.Vector2f");
    lua_glue::BindCallable(type_sf__RectangleShape, "getSize",
        [](const sf::RectangleShape& self) -> sf::Vector2f {
            return self.getSize();
        },
        docs[33]
    );
}
