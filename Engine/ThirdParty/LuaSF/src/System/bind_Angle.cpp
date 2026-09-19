#include "System/bind_Angle.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Angle(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Angle = sf.new_usertype<sf::Angle>("Angle", sol::no_constructor);
    sol::table table_sf__Angle = sf["Angle"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Angle>(lua);
    LUASF_STUB_DOC("\\brief Represents an angle value.");
    LUASF_STUB_CLASS("sf.Angle");
    LUASF_STUB_DOC("\\brief Default constructor\n\nSets the angle value to zero.");
    LUASF_STUB_FUNCTION("sf.Angle", "new", "fun(): sf.Angle");
    type_sf__Angle.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Angle>();
        }
    ));
    LUASF_STUB_DOC("\\brief Return the angle's value in degrees\n\n\\return Angle in degrees\n\n\\see `asRadians`");
    LUASF_STUB_FUNCTION("sf.Angle", "asDegrees", "fun(self: sf.Angle): number");
    type_sf__Angle.set_function("asDegrees",
        [](sf::Angle& self) -> float {
            return self.asDegrees();
        }
    );
    LUASF_STUB_DOC("\\brief Return the angle's value in radians\n\n\\return Angle in radians\n\n\\see `asDegrees`");
    LUASF_STUB_FUNCTION("sf.Angle", "asRadians", "fun(self: sf.Angle): number");
    type_sf__Angle.set_function("asRadians",
        [](sf::Angle& self) -> float {
            return self.asRadians();
        }
    );
    LUASF_STUB_DOC("\\brief Wrap to a range such that -180\u00b0 <= angle < 180\u00b0\n\nSimilar to a modulo operation, this returns a copy of the angle\nconstrained to the range [-180\u00b0, 180\u00b0) == [-Pi, Pi).\nThe resulting angle represents a rotation which is equivalent to `*this`.\n\nThe name \"signed\" originates from the similarity to signed integers:\n<table>\n<tr>\n<th></th>\n<th>signed</th>\n<th>unsigned</th>\n</tr>\n<tr>\n<td>char</td>\n<td>[-128, 128)</td>\n<td>[0, 256)</td>\n</tr>\n<tr>\n<td>Angle</td>\n<td>[-180\u00b0, 180\u00b0)</td>\n<td>[0\u00b0, 360\u00b0)</td>\n</tr>\n</table>\n\n\\return Signed angle, wrapped to [-180\u00b0, 180\u00b0)\n\n\\see `wrapUnsigned`");
    LUASF_STUB_FUNCTION("sf.Angle", "wrapSigned", "fun(self: sf.Angle): sf.Angle");
    type_sf__Angle.set_function("wrapSigned",
        [](sf::Angle& self) -> sf::Angle {
            return self.wrapSigned();
        }
    );
    LUASF_STUB_DOC("\\brief Wrap to a range such that 0\u00b0 <= angle < 360\u00b0\n\nSimilar to a modulo operation, this returns a copy of the angle\nconstrained to the range [0\u00b0, 360\u00b0) == [0, Tau) == [0, 2*Pi).\nThe resulting angle represents a rotation which is equivalent to `*this`.\n\nThe name \"unsigned\" originates from the similarity to unsigned integers:\n<table>\n<tr>\n<th></th>\n<th>signed</th>\n<th>unsigned</th>\n</tr>\n<tr>\n<td>char</td>\n<td>[-128, 128)</td>\n<td>[0, 256)</td>\n</tr>\n<tr>\n<td>Angle</td>\n<td>[-180\u00b0, 180\u00b0)</td>\n<td>[0\u00b0, 360\u00b0)</td>\n</tr>\n</table>\n\n\\return Unsigned angle, wrapped to [0\u00b0, 360\u00b0)\n\n\\see `wrapSigned`");
    LUASF_STUB_FUNCTION("sf.Angle", "wrapUnsigned", "fun(self: sf.Angle): sf.Angle");
    type_sf__Angle.set_function("wrapUnsigned",
        [](sf::Angle& self) -> sf::Angle {
            return self.wrapUnsigned();
        }
    );
    LUASF_STUB_DOC("Predefined 0 degree angle value");
    LUASF_STUB_VALUE("sf.Angle", "Zero", "sf.Angle");
    table_sf__Angle["Zero"] = sf::Angle::Zero;
    sol::table sf_Literals = sf["Literals"].get_or_create<sol::table>();
    LUASF_STUB_DOC("\\brief Construct an angle value from a number of degrees\n\n\\param angle Number of degrees\n\n\\return Angle value constructed from the number of degrees\n\n\\see `radians`");
    LUASF_STUB_FUNCTION("sf", "degrees", "fun(angle: number): sf.Angle");
    sf.set_function("degrees",
        [](float angle) -> sf::Angle {
            return sf::degrees(angle);
        }
    );
    LUASF_STUB_DOC("\\brief Construct an angle value from a number of radians\n\n\\param angle Number of radians\n\n\\return Angle value constructed from the number of radians\n\n\\see `degrees`");
    LUASF_STUB_FUNCTION("sf", "radians", "fun(angle: number): sf.Angle");
    sf.set_function("radians",
        [](float angle) -> sf::Angle {
            return sf::radians(angle);
        }
    );
}
