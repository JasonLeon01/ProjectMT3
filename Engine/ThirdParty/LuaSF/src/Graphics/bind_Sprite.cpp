#include "Graphics/bind_Sprite.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Sprite(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Sprite = sf.new_usertype<sf::Sprite>("Sprite",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::Drawable, sf::Transformable>()
    );
    sol::table table_sf__Sprite = sf["Sprite"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sprite>(lua);
    sol::table native_bases_sf__Sprite = lua.create_table();
    native_bases_sf__Sprite.add(lua["sf"]["Drawable"].get<sol::table>());
    native_bases_sf__Sprite.add(lua["sf"]["Transformable"].get<sol::table>());
    table_sf__Sprite.raw_set("__nativeBases", native_bases_sf__Sprite);
    LUASF_STUB_DOC("\\brief Drawable representation of a texture, with its\nown transformations, color, etc.");
    LUASF_STUB_CLASS("sf.Sprite", "sf.Drawable, sf.Transformable");
    LUASF_STUB_DOC("\\brief Construct the sprite from a sub-rectangle of a source texture\n\n\\param texture   Source texture\n\\param rectangle Sub-rectangle of the texture to assign to the sprite\n\n\\see `setTexture`, `setTextureRect`");
    LUASF_STUB_FUNCTION("sf.Sprite", "new", "fun(texture: sf.Texture, rectangle: sf.IntRect): sf.Sprite");
    LUASF_STUB_OVERLOAD("sf.Sprite", "new", "fun(texture: sf.Texture): sf.Sprite");
    type_sf__Sprite.set_function("new", sol::factories(
        [](const sf::Texture& texture, const sf::IntRect& rectangle) {
            return lua_sf::makeLuaSharedObject<sf::Sprite>(texture, rectangle);
        },
        [](const sf::Texture& texture) {
            return lua_sf::makeLuaSharedObject<sf::Sprite>(texture);
        }
    ));
    LUASF_STUB_DOC("\\brief set the position of the object\n\nThis function completely overwrites the previous position.\nSee the move function to apply an offset based on the previous position instead.\nThe default position of a transformable object is (0, 0).\n\nNote that `sf::Text` may appear offset when positioned.\nThis is because its local bounds are influenced by font metrics (e.g. tallest characters)\nto consistently align with the text's baseline. As such the `getGlobalBounds()`\nposition may not match the position you set.\n\nTo account for this offset, the local bounds need to be considered.\nEither by including it in the position calculation:\n\\code\ntext.setPosition(position - text.getLocalBounds().position);\n\\endcode\nOr by adjusting the text's origin:\n\\code\ntext.setOrigin(text.getLocalBounds().position);\ntext.setPosition(position);\n\\endcode\n\n\\param position New position\n\n\\see `move`, `getPosition`");
    LUASF_STUB_FUNCTION("sf.Sprite", "setPosition", "fun(self: sf.Sprite, position: sf.Vector2f)");
    type_sf__Sprite.set_function("setPosition",
        [](sf::Sprite& self, sf::Vector2f position) {
            static_cast<sf::Transformable&>(self).setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief set the orientation of the object\n\nThis function completely overwrites the previous rotation.\nSee the rotate function to add an angle based on the previous rotation instead.\nThe default rotation of a transformable object is 0.\n\n\\param angle New rotation\n\n\\see `rotate`, `getRotation`");
    LUASF_STUB_FUNCTION("sf.Sprite", "setRotation", "fun(self: sf.Sprite, angle: sf.Angle)");
    type_sf__Sprite.set_function("setRotation",
        [](sf::Sprite& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).setRotation(angle);
        }
    );
    LUASF_STUB_DOC("\\brief set the scale factors of the object\n\nThis function completely overwrites the previous scale.\nSee the scale function to add a factor based on the previous scale instead.\nThe default scale of a transformable object is (1, 1).\n\n\\param factors New scale factors\n\n\\see `scale`, `getScale`");
    LUASF_STUB_FUNCTION("sf.Sprite", "setScale", "fun(self: sf.Sprite, factors: sf.Vector2f)");
    type_sf__Sprite.set_function("setScale",
        [](sf::Sprite& self, sf::Vector2f factors) {
            static_cast<sf::Transformable&>(self).setScale(factors);
        }
    );
    LUASF_STUB_DOC("\\brief set the local origin of the object\n\nThe origin of an object defines the center point for\nall transformations (position, scale, rotation).\nThe coordinates of this point must be relative to the\ntop-left corner of the object, and ignore all\ntransformations (position, scale, rotation).\nThe default origin of a transformable object is (0, 0).\n\n\\param origin New origin\n\n\\see `getOrigin`");
    LUASF_STUB_FUNCTION("sf.Sprite", "setOrigin", "fun(self: sf.Sprite, origin: sf.Vector2f)");
    type_sf__Sprite.set_function("setOrigin",
        [](sf::Sprite& self, sf::Vector2f origin) {
            static_cast<sf::Transformable&>(self).setOrigin(origin);
        }
    );
    LUASF_STUB_DOC("\\brief get the position of the object\n\n\\return Current position\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.Sprite", "getPosition", "fun(self: sf.Sprite): sf.Vector2f");
    type_sf__Sprite.set_function("getPosition",
        [](sf::Sprite& self) -> sf::Vector2f {
            return static_cast<sf::Transformable&>(self).getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief get the orientation of the object\n\nThe rotation is always in the range [0, 360].\n\n\\return Current rotation\n\n\\see `setRotation`");
    LUASF_STUB_FUNCTION("sf.Sprite", "getRotation", "fun(self: sf.Sprite): sf.Angle");
    type_sf__Sprite.set_function("getRotation",
        [](sf::Sprite& self) -> sf::Angle {
            return static_cast<sf::Transformable&>(self).getRotation();
        }
    );
    LUASF_STUB_DOC("\\brief get the current scale of the object\n\n\\return Current scale factors\n\n\\see `setScale`");
    LUASF_STUB_FUNCTION("sf.Sprite", "getScale", "fun(self: sf.Sprite): sf.Vector2f");
    type_sf__Sprite.set_function("getScale",
        [](sf::Sprite& self) -> sf::Vector2f {
            return static_cast<sf::Transformable&>(self).getScale();
        }
    );
    LUASF_STUB_DOC("\\brief get the local origin of the object\n\n\\return Current origin\n\n\\see `setOrigin`");
    LUASF_STUB_FUNCTION("sf.Sprite", "getOrigin", "fun(self: sf.Sprite): sf.Vector2f");
    type_sf__Sprite.set_function("getOrigin",
        [](sf::Sprite& self) -> sf::Vector2f {
            return static_cast<sf::Transformable&>(self).getOrigin();
        }
    );
    LUASF_STUB_DOC("\\brief Move the object by a given offset\n\nThis function adds to the current position of the object,\nunlike `setPosition` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setPosition(object.getPosition() + offset);\n\\endcode\n\n\\param offset Offset\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.Sprite", "move", "fun(self: sf.Sprite, offset: sf.Vector2f)");
    type_sf__Sprite.set_function("move",
        [](sf::Sprite& self, sf::Vector2f offset) {
            static_cast<sf::Transformable&>(self).move(offset);
        }
    );
    LUASF_STUB_DOC("\\brief Rotate the object\n\nThis function adds to the current rotation of the object,\nunlike `setRotation` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setRotation(object.getRotation() + angle);\n\\endcode\n\n\\param angle Angle of rotation");
    LUASF_STUB_FUNCTION("sf.Sprite", "rotate", "fun(self: sf.Sprite, angle: sf.Angle)");
    type_sf__Sprite.set_function("rotate",
        [](sf::Sprite& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).rotate(angle);
        }
    );
    LUASF_STUB_DOC("\\brief Scale the object\n\nThis function multiplies the current scale of the object,\nunlike `setScale` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nsf::Vector2f scale = object.getScale();\nobject.setScale(scale.x * factor.x, scale.y * factor.y);\n\\endcode\n\n\\param factor Scale factors\n\n\\see `setScale`");
    LUASF_STUB_FUNCTION("sf.Sprite", "scale", "fun(self: sf.Sprite, factor: sf.Vector2f)");
    type_sf__Sprite.set_function("scale",
        [](sf::Sprite& self, sf::Vector2f factor) {
            static_cast<sf::Transformable&>(self).scale(factor);
        }
    );
    LUASF_STUB_DOC("\\brief get the combined transform of the object\n\n\\return Transform combining the position/rotation/scale/origin of the object\n\n\\see `getInverseTransform`");
    LUASF_STUB_FUNCTION("sf.Sprite", "getTransform", "fun(self: sf.Sprite): sf.Transform");
    type_sf__Sprite.set_function("getTransform",
        sol::policies(
            [](sf::Sprite& self) {
                return std::cref(static_cast<sf::Transformable&>(self).getTransform());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief get the inverse of the combined transform of the object\n\n\\return Inverse of the combined transformations applied to the object\n\n\\see `getTransform`");
    LUASF_STUB_FUNCTION("sf.Sprite", "getInverseTransform", "fun(self: sf.Sprite): sf.Transform");
    type_sf__Sprite.set_function("getInverseTransform",
        sol::policies(
            [](sf::Sprite& self) {
                return std::cref(static_cast<sf::Transformable&>(self).getInverseTransform());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Change the source texture of the sprite\n\nThe `texture` argument refers to a texture that must\nexist as long as the sprite uses it. Indeed, the sprite\ndoesn't store its own copy of the texture, but rather keeps\na pointer to the one that you passed to this function.\nIf the source texture is destroyed and the sprite tries to\nuse it, the behavior is undefined.\nIf `resetRect` is `true`, the `TextureRect` property of\nthe sprite is automatically adjusted to the size of the new\ntexture. If it is `false`, the texture rect is left unchanged.\n\n\\param texture   New texture\n\\param resetRect Should the texture rect be reset to the size of the new texture?\n\n\\see `getTexture`, `setTextureRect`");
    LUASF_STUB_FUNCTION("sf.Sprite", "setTexture", "fun(self: sf.Sprite, texture: sf.Texture, resetRect: boolean)");
    LUASF_STUB_OVERLOAD("sf.Sprite", "setTexture", "fun(self: sf.Sprite, texture: sf.Texture)");
    type_sf__Sprite.set_function("setTexture",
        sol::overload(
            [](sf::Sprite& self, const sf::Texture& texture, bool resetRect) {
                self.setTexture(texture, resetRect);
            },
            [](sf::Sprite& self, const sf::Texture& texture) {
                self.setTexture(texture);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Set the sub-rectangle of the texture that the sprite will display\n\nThe texture rect is useful when you don't want to display\nthe whole texture, but rather a part of it.\nBy default, the texture rect covers the entire texture.\n\n\\param rectangle Rectangle defining the region of the texture to display\n\n\\see `getTextureRect`, `setTexture`");
    LUASF_STUB_FUNCTION("sf.Sprite", "setTextureRect", "fun(self: sf.Sprite, rectangle: sf.IntRect)");
    type_sf__Sprite.set_function("setTextureRect",
        [](sf::Sprite& self, const sf::IntRect& rectangle) {
            self.setTextureRect(rectangle);
        }
    );
    LUASF_STUB_DOC("\\brief Set the global color of the sprite\n\nThis color is modulated (multiplied) with the sprite's\ntexture. It can be used to colorize the sprite, or change\nits global opacity.\nBy default, the sprite's color is opaque white.\n\n\\param color New color of the sprite\n\n\\see `getColor`");
    LUASF_STUB_FUNCTION("sf.Sprite", "setColor", "fun(self: sf.Sprite, color: sf.Color)");
    type_sf__Sprite.set_function("setColor",
        [](sf::Sprite& self, sf::Color color) {
            self.setColor(color);
        }
    );
    LUASF_STUB_DOC("\\brief Get the source texture of the sprite\n\nThe returned reference is const, which means that you can't\nmodify the texture when you retrieve it with this function.\n\n\\return Reference to the sprite's texture\n\n\\see `setTexture`");
    LUASF_STUB_FUNCTION("sf.Sprite", "getTexture", "fun(self: sf.Sprite): sf.Texture");
    type_sf__Sprite.set_function("getTexture",
        sol::policies(
            [](sf::Sprite& self) {
                return std::cref(self.getTexture());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the sub-rectangle of the texture displayed by the sprite\n\n\\return Texture rectangle of the sprite\n\n\\see `setTextureRect`");
    LUASF_STUB_FUNCTION("sf.Sprite", "getTextureRect", "fun(self: sf.Sprite): sf.IntRect");
    type_sf__Sprite.set_function("getTextureRect",
        sol::policies(
            [](sf::Sprite& self) {
                return std::cref(self.getTextureRect());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the global color of the sprite\n\n\\return Global color of the sprite\n\n\\see `setColor`");
    LUASF_STUB_FUNCTION("sf.Sprite", "getColor", "fun(self: sf.Sprite): sf.Color");
    type_sf__Sprite.set_function("getColor",
        [](sf::Sprite& self) -> sf::Color {
            return self.getColor();
        }
    );
    LUASF_STUB_DOC("\\brief Get the local bounding rectangle of the entity\n\nThe returned rectangle is in local coordinates, which means\nthat it ignores the transformations (translation, rotation,\nscale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\nentity in the entity's coordinate system.\n\n\\return Local bounding rectangle of the entity");
    LUASF_STUB_FUNCTION("sf.Sprite", "getLocalBounds", "fun(self: sf.Sprite): sf.FloatRect");
    type_sf__Sprite.set_function("getLocalBounds",
        [](sf::Sprite& self) -> sf::FloatRect {
            return self.getLocalBounds();
        }
    );
    LUASF_STUB_DOC("\\brief Get the global bounding rectangle of the entity\n\nThe returned rectangle is in global coordinates, which means\nthat it takes into account the transformations (translation,\nrotation, scale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\nsprite in the global 2D world's coordinate system.\n\n\\return Global bounding rectangle of the entity");
    LUASF_STUB_FUNCTION("sf.Sprite", "getGlobalBounds", "fun(self: sf.Sprite): sf.FloatRect");
    type_sf__Sprite.set_function("getGlobalBounds",
        [](sf::Sprite& self) -> sf::FloatRect {
            return self.getGlobalBounds();
        }
    );
}
