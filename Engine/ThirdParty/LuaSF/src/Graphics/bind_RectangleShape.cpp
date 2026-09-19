#include "Graphics/bind_RectangleShape.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_RectangleShape(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__RectangleShape = sf.new_usertype<sf::RectangleShape>("RectangleShape",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Shape, sf::Drawable, sf::Transformable>()
    );
    sol::table table_sf__RectangleShape = sf["RectangleShape"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::RectangleShape>(lua);
    sol::table native_bases_sf__RectangleShape = lua.create_table();
    native_bases_sf__RectangleShape.add(lua["sf"]["Shape"].get<sol::table>());
    table_sf__RectangleShape.raw_set("__nativeBases", native_bases_sf__RectangleShape);
    LUASF_STUB_DOC("\\brief Specialized shape representing a rectangle");
    LUASF_STUB_CLASS("sf.RectangleShape", "sf.Shape, sf.Drawable, sf.Transformable");
    LUASF_STUB_DOC("\\brief Default constructor\n\n\\param size Size of the rectangle");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "new", "fun(size: sf.Vector2f): sf.RectangleShape");
    LUASF_STUB_OVERLOAD("sf.RectangleShape", "new", "fun(): sf.RectangleShape");
    type_sf__RectangleShape.set_function("new", sol::factories(
        [](sf::Vector2f size) {
            return lua_sf::makeLuaSharedObject<sf::RectangleShape>(size);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::RectangleShape>();
        }
    ));
    LUASF_STUB_DOC("\\brief set the position of the object\n\nThis function completely overwrites the previous position.\nSee the move function to apply an offset based on the previous position instead.\nThe default position of a transformable object is (0, 0).\n\nNote that `sf::Text` may appear offset when positioned.\nThis is because its local bounds are influenced by font metrics (e.g. tallest characters)\nto consistently align with the text's baseline. As such the `getGlobalBounds()`\nposition may not match the position you set.\n\nTo account for this offset, the local bounds need to be considered.\nEither by including it in the position calculation:\n\\code\ntext.setPosition(position - text.getLocalBounds().position);\n\\endcode\nOr by adjusting the text's origin:\n\\code\ntext.setOrigin(text.getLocalBounds().position);\ntext.setPosition(position);\n\\endcode\n\n\\param position New position\n\n\\see `move`, `getPosition`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setPosition", "fun(self: sf.RectangleShape, position: sf.Vector2f)");
    type_sf__RectangleShape.set_function("setPosition",
        [](sf::RectangleShape& self, sf::Vector2f position) {
            static_cast<sf::Transformable&>(self).setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief set the orientation of the object\n\nThis function completely overwrites the previous rotation.\nSee the rotate function to add an angle based on the previous rotation instead.\nThe default rotation of a transformable object is 0.\n\n\\param angle New rotation\n\n\\see `rotate`, `getRotation`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setRotation", "fun(self: sf.RectangleShape, angle: sf.Angle)");
    type_sf__RectangleShape.set_function("setRotation",
        [](sf::RectangleShape& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).setRotation(angle);
        }
    );
    LUASF_STUB_DOC("\\brief set the scale factors of the object\n\nThis function completely overwrites the previous scale.\nSee the scale function to add a factor based on the previous scale instead.\nThe default scale of a transformable object is (1, 1).\n\n\\param factors New scale factors\n\n\\see `scale`, `getScale`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setScale", "fun(self: sf.RectangleShape, factors: sf.Vector2f)");
    type_sf__RectangleShape.set_function("setScale",
        [](sf::RectangleShape& self, sf::Vector2f factors) {
            static_cast<sf::Transformable&>(self).setScale(factors);
        }
    );
    LUASF_STUB_DOC("\\brief set the local origin of the object\n\nThe origin of an object defines the center point for\nall transformations (position, scale, rotation).\nThe coordinates of this point must be relative to the\ntop-left corner of the object, and ignore all\ntransformations (position, scale, rotation).\nThe default origin of a transformable object is (0, 0).\n\n\\param origin New origin\n\n\\see `getOrigin`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setOrigin", "fun(self: sf.RectangleShape, origin: sf.Vector2f)");
    type_sf__RectangleShape.set_function("setOrigin",
        [](sf::RectangleShape& self, sf::Vector2f origin) {
            static_cast<sf::Transformable&>(self).setOrigin(origin);
        }
    );
    LUASF_STUB_DOC("\\brief get the position of the object\n\n\\return Current position\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getPosition", "fun(self: sf.RectangleShape): sf.Vector2f");
    type_sf__RectangleShape.set_function("getPosition",
        [](sf::RectangleShape& self) -> sf::Vector2f {
            return static_cast<sf::Transformable&>(self).getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief get the orientation of the object\n\nThe rotation is always in the range [0, 360].\n\n\\return Current rotation\n\n\\see `setRotation`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getRotation", "fun(self: sf.RectangleShape): sf.Angle");
    type_sf__RectangleShape.set_function("getRotation",
        [](sf::RectangleShape& self) -> sf::Angle {
            return static_cast<sf::Transformable&>(self).getRotation();
        }
    );
    LUASF_STUB_DOC("\\brief get the current scale of the object\n\n\\return Current scale factors\n\n\\see `setScale`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getScale", "fun(self: sf.RectangleShape): sf.Vector2f");
    type_sf__RectangleShape.set_function("getScale",
        [](sf::RectangleShape& self) -> sf::Vector2f {
            return static_cast<sf::Transformable&>(self).getScale();
        }
    );
    LUASF_STUB_DOC("\\brief get the local origin of the object\n\n\\return Current origin\n\n\\see `setOrigin`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getOrigin", "fun(self: sf.RectangleShape): sf.Vector2f");
    type_sf__RectangleShape.set_function("getOrigin",
        [](sf::RectangleShape& self) -> sf::Vector2f {
            return static_cast<sf::Transformable&>(self).getOrigin();
        }
    );
    LUASF_STUB_DOC("\\brief Move the object by a given offset\n\nThis function adds to the current position of the object,\nunlike `setPosition` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setPosition(object.getPosition() + offset);\n\\endcode\n\n\\param offset Offset\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "move", "fun(self: sf.RectangleShape, offset: sf.Vector2f)");
    type_sf__RectangleShape.set_function("move",
        [](sf::RectangleShape& self, sf::Vector2f offset) {
            static_cast<sf::Transformable&>(self).move(offset);
        }
    );
    LUASF_STUB_DOC("\\brief Rotate the object\n\nThis function adds to the current rotation of the object,\nunlike `setRotation` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setRotation(object.getRotation() + angle);\n\\endcode\n\n\\param angle Angle of rotation");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "rotate", "fun(self: sf.RectangleShape, angle: sf.Angle)");
    type_sf__RectangleShape.set_function("rotate",
        [](sf::RectangleShape& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).rotate(angle);
        }
    );
    LUASF_STUB_DOC("\\brief Scale the object\n\nThis function multiplies the current scale of the object,\nunlike `setScale` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nsf::Vector2f scale = object.getScale();\nobject.setScale(scale.x * factor.x, scale.y * factor.y);\n\\endcode\n\n\\param factor Scale factors\n\n\\see `setScale`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "scale", "fun(self: sf.RectangleShape, factor: sf.Vector2f)");
    type_sf__RectangleShape.set_function("scale",
        [](sf::RectangleShape& self, sf::Vector2f factor) {
            static_cast<sf::Transformable&>(self).scale(factor);
        }
    );
    LUASF_STUB_DOC("\\brief get the combined transform of the object\n\n\\return Transform combining the position/rotation/scale/origin of the object\n\n\\see `getInverseTransform`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getTransform", "fun(self: sf.RectangleShape): sf.Transform");
    type_sf__RectangleShape.set_function("getTransform",
        sol::policies(
            [](sf::RectangleShape& self) {
                return std::cref(static_cast<sf::Transformable&>(self).getTransform());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief get the inverse of the combined transform of the object\n\n\\return Inverse of the combined transformations applied to the object\n\n\\see `getTransform`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getInverseTransform", "fun(self: sf.RectangleShape): sf.Transform");
    type_sf__RectangleShape.set_function("getInverseTransform",
        sol::policies(
            [](sf::RectangleShape& self) {
                return std::cref(static_cast<sf::Transformable&>(self).getInverseTransform());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Change the source texture of the shape\n\nThe `texture` argument refers to a texture that must\nexist as long as the shape uses it. Indeed, the shape\ndoesn't store its own copy of the texture, but rather keeps\na pointer to the one that you passed to this function.\nIf the source texture is destroyed and the shape tries to\nuse it, the behavior is undefined.\n`texture` can be a null pointer to disable texturing.\nIf `resetRect` is `true`, the `TextureRect` property of\nthe shape is automatically adjusted to the size of the new\ntexture. If it is `false`, the texture rect is left unchanged.\n\n\\param texture   New texture\n\\param resetRect Should the texture rect be reset to the size of the new texture?\n\n\\see `getTexture`, `setTextureRect`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setTexture", "fun(self: sf.RectangleShape, texture: sf.Texture, resetRect: boolean)");
    LUASF_STUB_OVERLOAD("sf.RectangleShape", "setTexture", "fun(self: sf.RectangleShape, texture: sf.Texture)");
    type_sf__RectangleShape.set_function("setTexture",
        sol::overload(
            [](sf::RectangleShape& self, const sf::Texture* texture, bool resetRect) {
                static_cast<sf::Shape&>(self).setTexture(texture, resetRect);
            },
            [](sf::RectangleShape& self, const sf::Texture* texture) {
                static_cast<sf::Shape&>(self).setTexture(texture);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Set the sub-rectangle of the texture that the shape will display\n\nThe texture rect is useful when you don't want to display\nthe whole texture, but rather a part of it.\nBy default, the texture rect covers the entire texture.\n\n\\param rect Rectangle defining the region of the texture to display\n\n\\see `getTextureRect`, `setTexture`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setTextureRect", "fun(self: sf.RectangleShape, rect: sf.IntRect)");
    type_sf__RectangleShape.set_function("setTextureRect",
        [](sf::RectangleShape& self, const sf::IntRect& rect) {
            static_cast<sf::Shape&>(self).setTextureRect(rect);
        }
    );
    LUASF_STUB_DOC("\\brief Set the fill color of the shape\n\nThis color is modulated (multiplied) with the shape's\ntexture if any. It can be used to colorize the shape,\nor change its global opacity.\nYou can use `sf::Color::Transparent` to make the inside of\nthe shape transparent, and have the outline alone.\nBy default, the shape's fill color is opaque white.\n\n\\param color New color of the shape\n\n\\see `getFillColor`, `setOutlineColor`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setFillColor", "fun(self: sf.RectangleShape, color: sf.Color)");
    type_sf__RectangleShape.set_function("setFillColor",
        [](sf::RectangleShape& self, sf::Color color) {
            static_cast<sf::Shape&>(self).setFillColor(color);
        }
    );
    LUASF_STUB_DOC("\\brief Set the outline color of the shape\n\nBy default, the shape's outline color is opaque white.\n\n\\param color New outline color of the shape\n\n\\see `getOutlineColor`, `setFillColor`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setOutlineColor", "fun(self: sf.RectangleShape, color: sf.Color)");
    type_sf__RectangleShape.set_function("setOutlineColor",
        [](sf::RectangleShape& self, sf::Color color) {
            static_cast<sf::Shape&>(self).setOutlineColor(color);
        }
    );
    LUASF_STUB_DOC("\\brief Set the thickness of the shape's outline\n\nNote that negative values are allowed (so that the outline\nexpands towards the center of the shape), and using zero\ndisables the outline.\nBy default, the outline thickness is 0.\n\n\\param thickness New outline thickness\n\n\\see `getOutlineThickness`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setOutlineThickness", "fun(self: sf.RectangleShape, thickness: number)");
    type_sf__RectangleShape.set_function("setOutlineThickness",
        [](sf::RectangleShape& self, float thickness) {
            static_cast<sf::Shape&>(self).setOutlineThickness(thickness);
        }
    );
    LUASF_STUB_DOC("\\brief Set the limit on the ratio between miter length and outline thickness\n\nOutline segments around each shape corner are joined either\nwith a miter or a bevel join.\n- A miter join is formed by extending outline segments until\nthey intersect. The distance between the point of\nintersection and the shape's corner is the miter length.\n- A bevel join is formed by connecting outline segments with\na straight line perpendicular to the corner's bissector.\n\nThe miter limit is used to determine whether ouline segments\naround a corner are joined with a bevel or a miter.\nWhen the ratio between the miter length and outline thickness\nexceeds the miter limit, a bevel is used instead of a miter.\n\nThe miter limit is linked to the maximum inner angle of a\ncorner below which a bevel is used by the following formula:\n\nmiterLimit = 1 / sin(angle / 2)\n\nThe miter limit must be greater than or equal to 1.\nBy default, the miter limit is 10.\n\n\\param miterLimit New miter limit\n\n\\see getMiterLimit");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setMiterLimit", "fun(self: sf.RectangleShape, miterLimit: number)");
    type_sf__RectangleShape.set_function("setMiterLimit",
        [](sf::RectangleShape& self, float miterLimit) {
            static_cast<sf::Shape&>(self).setMiterLimit(miterLimit);
        }
    );
    LUASF_STUB_DOC("\\brief Get the source texture of the shape\n\nIf the shape has no source texture, a `nullptr` is returned.\nThe returned pointer is const, which means that you can't\nmodify the texture when you retrieve it with this function.\n\n\\return Pointer to the shape's texture\n\n\\see `setTexture`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getTexture", "fun(self: sf.RectangleShape): sf.Texture");
    type_sf__RectangleShape.set_function("getTexture",
        [](sf::RectangleShape& self) -> const sf::Texture* {
            return static_cast<sf::Shape&>(self).getTexture();
        }
    );
    LUASF_STUB_DOC("\\brief Get the sub-rectangle of the texture displayed by the shape\n\n\\return Texture rectangle of the shape\n\n\\see `setTextureRect`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getTextureRect", "fun(self: sf.RectangleShape): sf.IntRect");
    type_sf__RectangleShape.set_function("getTextureRect",
        sol::policies(
            [](sf::RectangleShape& self) {
                return std::cref(static_cast<sf::Shape&>(self).getTextureRect());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the fill color of the shape\n\n\\return Fill color of the shape\n\n\\see `setFillColor`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getFillColor", "fun(self: sf.RectangleShape): sf.Color");
    type_sf__RectangleShape.set_function("getFillColor",
        [](sf::RectangleShape& self) -> sf::Color {
            return static_cast<sf::Shape&>(self).getFillColor();
        }
    );
    LUASF_STUB_DOC("\\brief Get the outline color of the shape\n\n\\return Outline color of the shape\n\n\\see `setOutlineColor`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getOutlineColor", "fun(self: sf.RectangleShape): sf.Color");
    type_sf__RectangleShape.set_function("getOutlineColor",
        [](sf::RectangleShape& self) -> sf::Color {
            return static_cast<sf::Shape&>(self).getOutlineColor();
        }
    );
    LUASF_STUB_DOC("\\brief Get the outline thickness of the shape\n\n\\return Outline thickness of the shape\n\n\\see `setOutlineThickness`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getOutlineThickness", "fun(self: sf.RectangleShape): number");
    type_sf__RectangleShape.set_function("getOutlineThickness",
        [](sf::RectangleShape& self) -> float {
            return static_cast<sf::Shape&>(self).getOutlineThickness();
        }
    );
    LUASF_STUB_DOC("\\brief Get the limit on the ratio between miter length and outline thickness\n\n\\return Limit on the ratio between miter length and outline thickness\n\n\\see setMiterLimit");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getMiterLimit", "fun(self: sf.RectangleShape): number");
    type_sf__RectangleShape.set_function("getMiterLimit",
        [](sf::RectangleShape& self) -> float {
            return static_cast<sf::Shape&>(self).getMiterLimit();
        }
    );
    LUASF_STUB_DOC("\\brief Get the number of points defining the shape\n\n\\return Number of points of the shape. For rectangle\nshapes, this number is always 4.");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getPointCount", "fun(self: sf.RectangleShape): integer");
    type_sf__RectangleShape.set_function("getPointCount",
        [](sf::RectangleShape& self) -> std::size_t {
            return self.getPointCount();
        }
    );
    LUASF_STUB_DOC("\\brief Get a point of the rectangle\n\nThe returned point is in local coordinates, that is,\nthe shape's transforms (position, rotation, scale) are\nnot taken into account.\nThe result is undefined if `index` is out of the valid range.\n\n\\param index Index of the point to get, in range [0 .. 3]\n\n\\return `index`-th point of the shape");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getPoint", "fun(self: sf.RectangleShape, index: integer): sf.Vector2f");
    type_sf__RectangleShape.set_function("getPoint",
        [](sf::RectangleShape& self, lua_sf::LuaIntegral<std::size_t> index) -> sf::Vector2f {
            return self.getPoint(index.value());
        }
    );
    LUASF_STUB_DOC("\\brief Get the geometric center of the rectangle\n\nThe returned point is in local coordinates, that is,\nthe shape's transforms (position, rotation, scale) are\nnot taken into account.\n\n\\return The geometric center of the shape");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getGeometricCenter", "fun(self: sf.RectangleShape): sf.Vector2f");
    type_sf__RectangleShape.set_function("getGeometricCenter",
        [](sf::RectangleShape& self) -> sf::Vector2f {
            return self.getGeometricCenter();
        }
    );
    LUASF_STUB_DOC("\\brief Get the local bounding rectangle of the entity\n\nThe returned rectangle is in local coordinates, which means\nthat it ignores the transformations (translation, rotation,\nscale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\nentity in the entity's coordinate system.\n\n\\return Local bounding rectangle of the entity");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getLocalBounds", "fun(self: sf.RectangleShape): sf.FloatRect");
    type_sf__RectangleShape.set_function("getLocalBounds",
        [](sf::RectangleShape& self) -> sf::FloatRect {
            return static_cast<sf::Shape&>(self).getLocalBounds();
        }
    );
    LUASF_STUB_DOC("\\brief Get the global (non-minimal) bounding rectangle of the entity\n\nThe returned rectangle is in global coordinates, which means\nthat it takes into account the transformations (translation,\nrotation, scale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\nshape in the global 2D world's coordinate system.\n\nThis function does not necessarily return the _minimal_\nbounding rectangle. It merely ensures that the returned\nrectangle covers all the vertices (but possibly more).\nThis allows for a fast approximation of the bounds as a\nfirst check; you may want to use more precise checks\non top of that.\n\n\\return Global bounding rectangle of the entity");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getGlobalBounds", "fun(self: sf.RectangleShape): sf.FloatRect");
    type_sf__RectangleShape.set_function("getGlobalBounds",
        [](sf::RectangleShape& self) -> sf::FloatRect {
            return static_cast<sf::Shape&>(self).getGlobalBounds();
        }
    );
    LUASF_STUB_DOC("\\brief Set the size of the rectangle\n\n\\param size New size of the rectangle\n\n\\see `getSize`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "setSize", "fun(self: sf.RectangleShape, size: sf.Vector2f)");
    type_sf__RectangleShape.set_function("setSize",
        [](sf::RectangleShape& self, sf::Vector2f size) {
            self.setSize(size);
        }
    );
    LUASF_STUB_DOC("\\brief Get the size of the rectangle\n\n\\return Size of the rectangle\n\n\\see `setSize`");
    LUASF_STUB_FUNCTION("sf.RectangleShape", "getSize", "fun(self: sf.RectangleShape): sf.Vector2f");
    type_sf__RectangleShape.set_function("getSize",
        [](sf::RectangleShape& self) -> sf::Vector2f {
            return self.getSize();
        }
    );
}
