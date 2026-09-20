#include "Graphics/bind_Sprite.hpp"

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

namespace { constexpr std::array<std::string_view, 24> docs = {
    "\\brief Drawable representation of a texture, with its\nown transformations, color, etc.",
    "\\brief Construct the sprite from a source texture\n\n\\param texture Source texture\n\n\\see `setTexture`",
    "\\brief Construct the sprite from a sub-rectangle of a source texture\n\n\\param texture   Source texture\n\\param rectangle Sub-rectangle of the texture to assign to the sprite\n\n\\see `setTexture`, `setTextureRect`",
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
    "\\brief Change the source texture of the sprite\n\nThe `texture` argument refers to a texture that must\nexist as long as the sprite uses it. Indeed, the sprite\ndoesn't store its own copy of the texture, but rather keeps\na pointer to the one that you passed to this function.\nIf the source texture is destroyed and the sprite tries to\nuse it, the behavior is undefined.\nIf `resetRect` is `true`, the `TextureRect` property of\nthe sprite is automatically adjusted to the size of the new\ntexture. If it is `false`, the texture rect is left unchanged.\n\n\\param texture   New texture\n\\param resetRect Should the texture rect be reset to the size of the new texture?\n\n\\see `getTexture`, `setTextureRect`",
    "\\brief Set the sub-rectangle of the texture that the sprite will display\n\nThe texture rect is useful when you don't want to display\nthe whole texture, but rather a part of it.\nBy default, the texture rect covers the entire texture.\n\n\\param rectangle Rectangle defining the region of the texture to display\n\n\\see `getTextureRect`, `setTexture`",
    "\\brief Set the global color of the sprite\n\nThis color is modulated (multiplied) with the sprite's\ntexture. It can be used to colorize the sprite, or change\nits global opacity.\nBy default, the sprite's color is opaque white.\n\n\\param color New color of the sprite\n\n\\see `getColor`",
    "\\brief Get the source texture of the sprite\n\nThe returned reference is const, which means that you can't\nmodify the texture when you retrieve it with this function.\n\n\\return Reference to the sprite's texture\n\n\\see `setTexture`",
    "\\brief Get the sub-rectangle of the texture displayed by the sprite\n\n\\return Texture rectangle of the sprite\n\n\\see `setTextureRect`",
    "\\brief Get the global color of the sprite\n\n\\return Global color of the sprite\n\n\\see `setColor`",
    "\\brief Get the local bounding rectangle of the entity\n\nThe returned rectangle is in local coordinates, which means\nthat it ignores the transformations (translation, rotation,\nscale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\nentity in the entity's coordinate system.\n\n\\return Local bounding rectangle of the entity",
    "\\brief Get the global bounding rectangle of the entity\n\nThe returned rectangle is in global coordinates, which means\nthat it takes into account the transformations (translation,\nrotation, scale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\nsprite in the global 2D world's coordinate system.\n\n\\return Global bounding rectangle of the entity",
}; }

void bind_Sprite(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Sprite = lua_glue::BindClass<sf::Sprite>(sf, "Sprite");
    lua_glue::BindBase<sf::Sprite, sf::Drawable>(type_sf__Sprite);
    lua_glue::BindBase<sf::Sprite, sf::Transformable>(type_sf__Sprite);
    lua_glue::Table table_sf__Sprite = sf["Sprite"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Sprite>(lua);
    lua_glue::Table native_bases_sf__Sprite = lua.create_table();
    native_bases_sf__Sprite.add(lua["sf"]["Drawable"].get<lua_glue::Table>());
    native_bases_sf__Sprite.add(lua["sf"]["Transformable"].get<lua_glue::Table>());
    table_sf__Sprite.raw_set("__nativeBases", native_bases_sf__Sprite);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Sprite", "sf.Drawable, sf.Transformable");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Sprite", "new", "fun(texture: sf.Texture, rectangle: sf.IntRect): sf.Sprite");
    LUASF_STUB_OVERLOAD("sf.Sprite", "new", "fun(texture: sf.Texture): sf.Sprite");
    lua_glue::BindCallable(type_sf__Sprite, "new",
        [](const sf::Texture& texture, const sf::IntRect& rectangle) {
            return lua_sf::makeLuaSharedObject<sf::Sprite>(texture, rectangle);
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__Sprite, "new",
        [](const sf::Texture& texture) {
            return lua_sf::makeLuaSharedObject<sf::Sprite>(texture);
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Sprite", "setPosition", "fun(self: sf.Sprite, position: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Sprite, "setPosition",
        [](sf::Sprite& self, sf::Vector2f position) {
            static_cast<sf::Transformable&>(self).setPosition(position);
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Sprite", "setRotation", "fun(self: sf.Sprite, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__Sprite, "setRotation",
        [](sf::Sprite& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).setRotation(angle);
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Sprite", "setScale", "fun(self: sf.Sprite, factors: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Sprite, "setScale",
        [](sf::Sprite& self, sf::Vector2f factors) {
            static_cast<sf::Transformable&>(self).setScale(factors);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Sprite", "setOrigin", "fun(self: sf.Sprite, origin: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Sprite, "setOrigin",
        [](sf::Sprite& self, sf::Vector2f origin) {
            static_cast<sf::Transformable&>(self).setOrigin(origin);
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getPosition", "fun(self: sf.Sprite): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Sprite, "getPosition",
        [](const sf::Sprite& self) -> sf::Vector2f {
            return static_cast<const sf::Transformable&>(self).getPosition();
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getRotation", "fun(self: sf.Sprite): sf.Angle");
    lua_glue::BindCallable(type_sf__Sprite, "getRotation",
        [](const sf::Sprite& self) -> sf::Angle {
            return static_cast<const sf::Transformable&>(self).getRotation();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getScale", "fun(self: sf.Sprite): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Sprite, "getScale",
        [](const sf::Sprite& self) -> sf::Vector2f {
            return static_cast<const sf::Transformable&>(self).getScale();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getOrigin", "fun(self: sf.Sprite): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Sprite, "getOrigin",
        [](const sf::Sprite& self) -> sf::Vector2f {
            return static_cast<const sf::Transformable&>(self).getOrigin();
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Sprite", "move", "fun(self: sf.Sprite, offset: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Sprite, "move",
        [](sf::Sprite& self, sf::Vector2f offset) {
            static_cast<sf::Transformable&>(self).move(offset);
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Sprite", "rotate", "fun(self: sf.Sprite, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__Sprite, "rotate",
        [](sf::Sprite& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).rotate(angle);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Sprite", "scale", "fun(self: sf.Sprite, factor: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Sprite, "scale",
        [](sf::Sprite& self, sf::Vector2f factor) {
            static_cast<sf::Transformable&>(self).scale(factor);
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getTransform", "fun(self: sf.Sprite): sf.Transform");
    lua_glue::BindCallable(type_sf__Sprite, "getTransform",
        [](const sf::Sprite& self) {
            return std::cref(static_cast<const sf::Transformable&>(self).getTransform());
        },
        docs[14],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getInverseTransform", "fun(self: sf.Sprite): sf.Transform");
    lua_glue::BindCallable(type_sf__Sprite, "getInverseTransform",
        [](const sf::Sprite& self) {
            return std::cref(static_cast<const sf::Transformable&>(self).getInverseTransform());
        },
        docs[15],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Sprite", "setTexture", "fun(self: sf.Sprite, texture: sf.Texture, resetRect?: boolean)");
    lua_glue::BindCallable(type_sf__Sprite, "setTexture",
        [](sf::Sprite& self, const sf::Texture& texture, bool resetRect) {
            self.setTexture(texture, resetRect);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Sprite", "setTextureRect", "fun(self: sf.Sprite, rectangle: sf.IntRect)");
    lua_glue::BindCallable(type_sf__Sprite, "setTextureRect",
        [](sf::Sprite& self, const sf::IntRect& rectangle) {
            self.setTextureRect(rectangle);
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.Sprite", "setColor", "fun(self: sf.Sprite, color: sf.Color)");
    lua_glue::BindCallable(type_sf__Sprite, "setColor",
        [](sf::Sprite& self, sf::Color color) {
            self.setColor(color);
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getTexture", "fun(self: sf.Sprite): sf.Texture");
    lua_glue::BindCallable(type_sf__Sprite, "getTexture",
        [](const sf::Sprite& self) {
            return std::cref(self.getTexture());
        },
        docs[19],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getTextureRect", "fun(self: sf.Sprite): sf.IntRect");
    lua_glue::BindCallable(type_sf__Sprite, "getTextureRect",
        [](const sf::Sprite& self) {
            return std::cref(self.getTextureRect());
        },
        docs[20],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getColor", "fun(self: sf.Sprite): sf.Color");
    lua_glue::BindCallable(type_sf__Sprite, "getColor",
        [](const sf::Sprite& self) -> sf::Color {
            return self.getColor();
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getLocalBounds", "fun(self: sf.Sprite): sf.FloatRect");
    lua_glue::BindCallable(type_sf__Sprite, "getLocalBounds",
        [](const sf::Sprite& self) -> sf::FloatRect {
            return self.getLocalBounds();
        },
        docs[22]
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.Sprite", "getGlobalBounds", "fun(self: sf.Sprite): sf.FloatRect");
    lua_glue::BindCallable(type_sf__Sprite, "getGlobalBounds",
        [](const sf::Sprite& self) -> sf::FloatRect {
            return self.getGlobalBounds();
        },
        docs[23]
    );
}
