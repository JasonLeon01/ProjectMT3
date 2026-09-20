#include "System/bind_Angle.hpp"

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

namespace { constexpr std::array<std::string_view, 9> docs = {
    "\\brief Represents an angle value.",
    "\\brief Default constructor\n\nSets the angle value to zero.",
    "\\brief Return the angle's value in degrees\n\n\\return Angle in degrees\n\n\\see `asRadians`",
    "\\brief Return the angle's value in radians\n\n\\return Angle in radians\n\n\\see `asDegrees`",
    "\\brief Wrap to a range such that -180\u00b0 <= angle < 180\u00b0\n\nSimilar to a modulo operation, this returns a copy of the angle\nconstrained to the range [-180\u00b0, 180\u00b0) == [-Pi, Pi).\nThe resulting angle represents a rotation which is equivalent to `*this`.\n\nThe name \"signed\" originates from the similarity to signed integers:\n<table>\n<tr>\n<th></th>\n<th>signed</th>\n<th>unsigned</th>\n</tr>\n<tr>\n<td>char</td>\n<td>[-128, 128)</td>\n<td>[0, 256)</td>\n</tr>\n<tr>\n<td>Angle</td>\n<td>[-180\u00b0, 180\u00b0)</td>\n<td>[0\u00b0, 360\u00b0)</td>\n</tr>\n</table>\n\n\\return Signed angle, wrapped to [-180\u00b0, 180\u00b0)\n\n\\see `wrapUnsigned`",
    "\\brief Wrap to a range such that 0\u00b0 <= angle < 360\u00b0\n\nSimilar to a modulo operation, this returns a copy of the angle\nconstrained to the range [0\u00b0, 360\u00b0) == [0, Tau) == [0, 2*Pi).\nThe resulting angle represents a rotation which is equivalent to `*this`.\n\nThe name \"unsigned\" originates from the similarity to unsigned integers:\n<table>\n<tr>\n<th></th>\n<th>signed</th>\n<th>unsigned</th>\n</tr>\n<tr>\n<td>char</td>\n<td>[-128, 128)</td>\n<td>[0, 256)</td>\n</tr>\n<tr>\n<td>Angle</td>\n<td>[-180\u00b0, 180\u00b0)</td>\n<td>[0\u00b0, 360\u00b0)</td>\n</tr>\n</table>\n\n\\return Unsigned angle, wrapped to [0\u00b0, 360\u00b0)\n\n\\see `wrapSigned`",
    "Predefined 0 degree angle value",
    "\\brief Construct an angle value from a number of degrees\n\n\\param angle Number of degrees\n\n\\return Angle value constructed from the number of degrees\n\n\\see `radians`",
    "\\brief Construct an angle value from a number of radians\n\n\\param angle Number of radians\n\n\\return Angle value constructed from the number of radians\n\n\\see `degrees`",
}; }

void bind_Angle(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Angle = lua_glue::BindStruct<sf::Angle>(sf, "Angle");
    lua_glue::Table table_sf__Angle = sf["Angle"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Angle");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Angle", "new", "fun(): sf.Angle");
    lua_glue::BindCallable(type_sf__Angle, "new",
        []() {
            return sf::Angle{};
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Angle", "asDegrees", "fun(self: sf.Angle): number");
    lua_glue::BindCallable(type_sf__Angle, "asDegrees",
        [](const sf::Angle& self) -> float {
            return self.asDegrees();
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Angle", "asRadians", "fun(self: sf.Angle): number");
    lua_glue::BindCallable(type_sf__Angle, "asRadians",
        [](const sf::Angle& self) -> float {
            return self.asRadians();
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Angle", "wrapSigned", "fun(self: sf.Angle): sf.Angle");
    lua_glue::BindCallable(type_sf__Angle, "wrapSigned",
        [](const sf::Angle& self) -> sf::Angle {
            return self.wrapSigned();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Angle", "wrapUnsigned", "fun(self: sf.Angle): sf.Angle");
    lua_glue::BindCallable(type_sf__Angle, "wrapUnsigned",
        [](const sf::Angle& self) -> sf::Angle {
            return self.wrapUnsigned();
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_VALUE("sf.Angle", "Zero", "sf.Angle");
    lua_glue::BindStaticAttr<const sf::Angle>(table_sf__Angle, "Zero", &sf::Angle::Zero);
    LUASF_STUB_FUNCTION("sf.Angle", "copy", "fun(self: sf.Angle): sf.Angle");
    LUASF_STUB_FUNCTION("sf.Angle", "deepcopy", "fun(self: sf.Angle): sf.Angle");
    lua_glue::Table sf_Literals = sf["Literals"].get_or_create<lua_glue::Table>();
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf", "degrees", "fun(angle: number): sf.Angle");
    lua_glue::BindCallable(sf, "degrees",
        [](float angle) -> sf::Angle {
            return sf::degrees(angle);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf", "radians", "fun(angle: number): sf.Angle");
    lua_glue::BindCallable(sf, "radians",
        [](float angle) -> sf::Angle {
            return sf::radians(angle);
        },
        docs[8]
    );
}
