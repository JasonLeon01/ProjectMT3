#include "Graphics/bind_Transformable.hpp"

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

namespace { constexpr std::array<std::string_view, 15> docs = {
    "\\brief Decomposed transform defined by a position, a rotation and a scale",
    "\\brief Default constructor",
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
}; }

void bind_Transformable(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Transformable = lua_glue::BindClass<sf::Transformable>(sf, "Transformable");
    lua_glue::Table table_sf__Transformable = sf["Transformable"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Transformable>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Transformable");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Transformable", "new", "fun(): sf.Transformable");
    lua_glue::BindCallable(type_sf__Transformable, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Transformable>();
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Transformable", "setPosition", "fun(self: sf.Transformable, position: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Transformable, "setPosition",
        [](sf::Transformable& self, sf::Vector2f position) {
            self.setPosition(position);
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Transformable", "setRotation", "fun(self: sf.Transformable, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__Transformable, "setRotation",
        [](sf::Transformable& self, sf::Angle angle) {
            self.setRotation(angle);
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Transformable", "setScale", "fun(self: sf.Transformable, factors: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Transformable, "setScale",
        [](sf::Transformable& self, sf::Vector2f factors) {
            self.setScale(factors);
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Transformable", "setOrigin", "fun(self: sf.Transformable, origin: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Transformable, "setOrigin",
        [](sf::Transformable& self, sf::Vector2f origin) {
            self.setOrigin(origin);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Transformable", "getPosition", "fun(self: sf.Transformable): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Transformable, "getPosition",
        [](const sf::Transformable& self) -> sf::Vector2f {
            return self.getPosition();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Transformable", "getRotation", "fun(self: sf.Transformable): sf.Angle");
    lua_glue::BindCallable(type_sf__Transformable, "getRotation",
        [](const sf::Transformable& self) -> sf::Angle {
            return self.getRotation();
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Transformable", "getScale", "fun(self: sf.Transformable): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Transformable, "getScale",
        [](const sf::Transformable& self) -> sf::Vector2f {
            return self.getScale();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Transformable", "getOrigin", "fun(self: sf.Transformable): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Transformable, "getOrigin",
        [](const sf::Transformable& self) -> sf::Vector2f {
            return self.getOrigin();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Transformable", "move", "fun(self: sf.Transformable, offset: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Transformable, "move",
        [](sf::Transformable& self, sf::Vector2f offset) {
            self.move(offset);
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Transformable", "rotate", "fun(self: sf.Transformable, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__Transformable, "rotate",
        [](sf::Transformable& self, sf::Angle angle) {
            self.rotate(angle);
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Transformable", "scale", "fun(self: sf.Transformable, factor: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Transformable, "scale",
        [](sf::Transformable& self, sf::Vector2f factor) {
            self.scale(factor);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Transformable", "getTransform", "fun(self: sf.Transformable): sf.Transform");
    lua_glue::BindCallable(type_sf__Transformable, "getTransform",
        [](const sf::Transformable& self) {
            return std::cref(self.getTransform());
        },
        docs[13],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Transformable", "getInverseTransform", "fun(self: sf.Transformable): sf.Transform");
    lua_glue::BindCallable(type_sf__Transformable, "getInverseTransform",
        [](const sf::Transformable& self) {
            return std::cref(self.getInverseTransform());
        },
        docs[14],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
}
