#include "Graphics/bind_Transformable.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Transformable(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Transformable = sf.new_usertype<sf::Transformable>("Transformable", sol::no_constructor);
    sol::table table_sf__Transformable = sf["Transformable"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Transformable>(lua);
    LUASF_STUB_DOC("\\brief Decomposed transform defined by a position, a rotation and a scale");
    LUASF_STUB_CLASS("sf.Transformable");
    LUASF_STUB_DOC("\\brief Default constructor");
    LUASF_STUB_FUNCTION("sf.Transformable", "new", "fun(): sf.Transformable");
    type_sf__Transformable.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Transformable>();
        }
    ));
    LUASF_STUB_DOC("\\brief set the position of the object\n\nThis function completely overwrites the previous position.\nSee the move function to apply an offset based on the previous position instead.\nThe default position of a transformable object is (0, 0).\n\nNote that `sf::Text` may appear offset when positioned.\nThis is because its local bounds are influenced by font metrics (e.g. tallest characters)\nto consistently align with the text's baseline. As such the `getGlobalBounds()`\nposition may not match the position you set.\n\nTo account for this offset, the local bounds need to be considered.\nEither by including it in the position calculation:\n\\code\ntext.setPosition(position - text.getLocalBounds().position);\n\\endcode\nOr by adjusting the text's origin:\n\\code\ntext.setOrigin(text.getLocalBounds().position);\ntext.setPosition(position);\n\\endcode\n\n\\param position New position\n\n\\see `move`, `getPosition`");
    LUASF_STUB_FUNCTION("sf.Transformable", "setPosition", "fun(self: sf.Transformable, position: sf.Vector2f)");
    type_sf__Transformable.set_function("setPosition",
        [](sf::Transformable& self, sf::Vector2f position) {
            self.setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief set the orientation of the object\n\nThis function completely overwrites the previous rotation.\nSee the rotate function to add an angle based on the previous rotation instead.\nThe default rotation of a transformable object is 0.\n\n\\param angle New rotation\n\n\\see `rotate`, `getRotation`");
    LUASF_STUB_FUNCTION("sf.Transformable", "setRotation", "fun(self: sf.Transformable, angle: sf.Angle)");
    type_sf__Transformable.set_function("setRotation",
        [](sf::Transformable& self, sf::Angle angle) {
            self.setRotation(angle);
        }
    );
    LUASF_STUB_DOC("\\brief set the scale factors of the object\n\nThis function completely overwrites the previous scale.\nSee the scale function to add a factor based on the previous scale instead.\nThe default scale of a transformable object is (1, 1).\n\n\\param factors New scale factors\n\n\\see `scale`, `getScale`");
    LUASF_STUB_FUNCTION("sf.Transformable", "setScale", "fun(self: sf.Transformable, factors: sf.Vector2f)");
    type_sf__Transformable.set_function("setScale",
        [](sf::Transformable& self, sf::Vector2f factors) {
            self.setScale(factors);
        }
    );
    LUASF_STUB_DOC("\\brief set the local origin of the object\n\nThe origin of an object defines the center point for\nall transformations (position, scale, rotation).\nThe coordinates of this point must be relative to the\ntop-left corner of the object, and ignore all\ntransformations (position, scale, rotation).\nThe default origin of a transformable object is (0, 0).\n\n\\param origin New origin\n\n\\see `getOrigin`");
    LUASF_STUB_FUNCTION("sf.Transformable", "setOrigin", "fun(self: sf.Transformable, origin: sf.Vector2f)");
    type_sf__Transformable.set_function("setOrigin",
        [](sf::Transformable& self, sf::Vector2f origin) {
            self.setOrigin(origin);
        }
    );
    LUASF_STUB_DOC("\\brief get the position of the object\n\n\\return Current position\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.Transformable", "getPosition", "fun(self: sf.Transformable): sf.Vector2f");
    type_sf__Transformable.set_function("getPosition",
        [](sf::Transformable& self) -> sf::Vector2f {
            return self.getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief get the orientation of the object\n\nThe rotation is always in the range [0, 360].\n\n\\return Current rotation\n\n\\see `setRotation`");
    LUASF_STUB_FUNCTION("sf.Transformable", "getRotation", "fun(self: sf.Transformable): sf.Angle");
    type_sf__Transformable.set_function("getRotation",
        [](sf::Transformable& self) -> sf::Angle {
            return self.getRotation();
        }
    );
    LUASF_STUB_DOC("\\brief get the current scale of the object\n\n\\return Current scale factors\n\n\\see `setScale`");
    LUASF_STUB_FUNCTION("sf.Transformable", "getScale", "fun(self: sf.Transformable): sf.Vector2f");
    type_sf__Transformable.set_function("getScale",
        [](sf::Transformable& self) -> sf::Vector2f {
            return self.getScale();
        }
    );
    LUASF_STUB_DOC("\\brief get the local origin of the object\n\n\\return Current origin\n\n\\see `setOrigin`");
    LUASF_STUB_FUNCTION("sf.Transformable", "getOrigin", "fun(self: sf.Transformable): sf.Vector2f");
    type_sf__Transformable.set_function("getOrigin",
        [](sf::Transformable& self) -> sf::Vector2f {
            return self.getOrigin();
        }
    );
    LUASF_STUB_DOC("\\brief Move the object by a given offset\n\nThis function adds to the current position of the object,\nunlike `setPosition` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setPosition(object.getPosition() + offset);\n\\endcode\n\n\\param offset Offset\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.Transformable", "move", "fun(self: sf.Transformable, offset: sf.Vector2f)");
    type_sf__Transformable.set_function("move",
        [](sf::Transformable& self, sf::Vector2f offset) {
            self.move(offset);
        }
    );
    LUASF_STUB_DOC("\\brief Rotate the object\n\nThis function adds to the current rotation of the object,\nunlike `setRotation` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setRotation(object.getRotation() + angle);\n\\endcode\n\n\\param angle Angle of rotation");
    LUASF_STUB_FUNCTION("sf.Transformable", "rotate", "fun(self: sf.Transformable, angle: sf.Angle)");
    type_sf__Transformable.set_function("rotate",
        [](sf::Transformable& self, sf::Angle angle) {
            self.rotate(angle);
        }
    );
    LUASF_STUB_DOC("\\brief Scale the object\n\nThis function multiplies the current scale of the object,\nunlike `setScale` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nsf::Vector2f scale = object.getScale();\nobject.setScale(scale.x * factor.x, scale.y * factor.y);\n\\endcode\n\n\\param factor Scale factors\n\n\\see `setScale`");
    LUASF_STUB_FUNCTION("sf.Transformable", "scale", "fun(self: sf.Transformable, factor: sf.Vector2f)");
    type_sf__Transformable.set_function("scale",
        [](sf::Transformable& self, sf::Vector2f factor) {
            self.scale(factor);
        }
    );
    LUASF_STUB_DOC("\\brief get the combined transform of the object\n\n\\return Transform combining the position/rotation/scale/origin of the object\n\n\\see `getInverseTransform`");
    LUASF_STUB_FUNCTION("sf.Transformable", "getTransform", "fun(self: sf.Transformable): sf.Transform");
    type_sf__Transformable.set_function("getTransform",
        sol::policies(
            [](sf::Transformable& self) {
                return std::cref(self.getTransform());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief get the inverse of the combined transform of the object\n\n\\return Inverse of the combined transformations applied to the object\n\n\\see `getTransform`");
    LUASF_STUB_FUNCTION("sf.Transformable", "getInverseTransform", "fun(self: sf.Transformable): sf.Transform");
    type_sf__Transformable.set_function("getInverseTransform",
        sol::policies(
            [](sf::Transformable& self) {
                return std::cref(self.getInverseTransform());
            },
            sol::self_dependency{}
        )
    );
}
