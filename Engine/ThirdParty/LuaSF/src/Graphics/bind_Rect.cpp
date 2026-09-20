#include "Graphics/bind_Rect.hpp"

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

namespace { constexpr std::array<std::string_view, 8> docs = {
    "\\brief Utility class for manipulating 2D axis aligned rectangles",
    "Position of the top-left corner of the rectangle",
    "Size of the rectangle",
    "\\brief Default constructor\n\nCreates an empty rectangle (it is equivalent to calling\n`Rect({0, 0}, {0, 0})`).",
    "\\brief Construct the rectangle from position and size\n\nBe careful, the last parameter is the size,\nnot the bottom-right corner!\n\n\\param position Position of the top-left corner of the rectangle\n\\param size     Size of the rectangle",
    "\\brief Check if a point is inside the rectangle's area\n\nThis check is non-inclusive. If the point lies on the\nedge of the rectangle, this function will return `false`.\n\n\\param point Point to test\n\n\\return `true` if the point is inside, `false` otherwise\n\n\\see `findIntersection`",
    "\\brief Check the intersection between two rectangles\n\n\\param rectangle Rectangle to test\n\n\\return Intersection rectangle if intersecting, `std::nullopt` otherwise\n\n\\see `contains`",
    "\\brief Get the position of the center of the rectangle\n\n\\return Center of rectangle",
}; }

void bind_Rect(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Rect_int_ = lua_glue::BindStruct<sf::Rect<int>>(sf, "IntRect");
    lua_glue::Table table_sf__Rect_int_ = sf["IntRect"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.IntRect");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("position", "sf.Vector2i");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("size", "sf.Vector2i");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.IntRect", "new", "fun(position: sf.Vector2i, size: sf.Vector2i): sf.IntRect");
    LUASF_STUB_OVERLOAD("sf.IntRect", "new", "fun(): sf.IntRect");
    LUASF_STUB_OVERLOAD("sf.IntRect", "new", "fun(x: integer, y: integer, width: integer, height: integer): sf.IntRect");
    lua_glue::BindCallable(type_sf__Rect_int_, "new",
        [](sf::Vector2<int> position, sf::Vector2<int> size) {
            return sf::Rect<int>{position, size};
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Rect_int_, "new",
        []() {
            return sf::Rect<int>{};
        },
        docs[3]
    );
    lua_glue::BindCallable(type_sf__Rect_int_, "new",
        [](lua_sf::LuaIntegral<int> x, lua_sf::LuaIntegral<int> y, lua_sf::LuaIntegral<int> width, lua_sf::LuaIntegral<int> height) {
            return sf::Rect<int>{{x.value(), y.value()},
                               {width.value(), height.value()}};
        }
    );
    lua_glue::BindAttr<sf::Vector2<int>>(type_sf__Rect_int_, "position", &sf::Rect<int>::position);
    lua_glue::BindAttr<sf::Vector2<int>>(type_sf__Rect_int_, "size", &sf::Rect<int>::size);
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.IntRect", "contains", "fun(self: sf.IntRect, point: sf.Vector2i): boolean");
    lua_glue::BindCallable(type_sf__Rect_int_, "contains",
        [](const sf::Rect<int>& self, sf::Vector2<int> point) -> bool {
            return self.contains(point);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.IntRect", "findIntersection", "fun(self: sf.IntRect, rectangle: sf.IntRect): sf.IntRect|nil");
    lua_glue::BindCallable(type_sf__Rect_int_, "findIntersection",
        [lua](const sf::Rect<int>& self, const sf::Rect<int>& rectangle) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.findIntersection(rectangle));
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.IntRect", "getCenter", "fun(self: sf.IntRect): sf.Vector2i");
    lua_glue::BindCallable(type_sf__Rect_int_, "getCenter",
        [](const sf::Rect<int>& self) -> sf::Vector2<int> {
            return self.getCenter();
        },
        docs[7]
    );
    lua_glue::BindMetamethod(type_sf__Rect_int_, "__tostring", [name = std::string("IntRect")](const sf::Rect<int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.position.x << ", " << self.position.y << ", "
               << self.size.x << ", " << self.size.y << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Rect_int_, "__eq", [](const sf::Rect<int>& left, const sf::Rect<int>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.IntRect", "eq(sf.IntRect): boolean");
    LUASF_STUB_FUNCTION("sf.IntRect", "copy", "fun(self: sf.IntRect): sf.IntRect");
    LUASF_STUB_FUNCTION("sf.IntRect", "deepcopy", "fun(self: sf.IntRect): sf.IntRect");
    auto type_sf__Rect_float_ = lua_glue::BindStruct<sf::Rect<float>>(sf, "FloatRect");
    lua_glue::Table table_sf__Rect_float_ = sf["FloatRect"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.FloatRect");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("position", "sf.Vector2f");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("size", "sf.Vector2f");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.FloatRect", "new", "fun(position: sf.Vector2f, size: sf.Vector2f): sf.FloatRect");
    LUASF_STUB_OVERLOAD("sf.FloatRect", "new", "fun(): sf.FloatRect");
    LUASF_STUB_OVERLOAD("sf.FloatRect", "new", "fun(x: number, y: number, width: number, height: number): sf.FloatRect");
    lua_glue::BindCallable(type_sf__Rect_float_, "new",
        [](sf::Vector2<float> position, sf::Vector2<float> size) {
            return sf::Rect<float>{position, size};
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Rect_float_, "new",
        []() {
            return sf::Rect<float>{};
        },
        docs[3]
    );
    lua_glue::BindCallable(type_sf__Rect_float_, "new",
        [](float x, float y, float width, float height) {
            return sf::Rect<float>{{x, y},
                               {width, height}};
        }
    );
    lua_glue::BindAttr<sf::Vector2<float>>(type_sf__Rect_float_, "position", &sf::Rect<float>::position);
    lua_glue::BindAttr<sf::Vector2<float>>(type_sf__Rect_float_, "size", &sf::Rect<float>::size);
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.FloatRect", "contains", "fun(self: sf.FloatRect, point: sf.Vector2f): boolean");
    lua_glue::BindCallable(type_sf__Rect_float_, "contains",
        [](const sf::Rect<float>& self, sf::Vector2<float> point) -> bool {
            return self.contains(point);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.FloatRect", "findIntersection", "fun(self: sf.FloatRect, rectangle: sf.FloatRect): sf.FloatRect|nil");
    lua_glue::BindCallable(type_sf__Rect_float_, "findIntersection",
        [lua](const sf::Rect<float>& self, const sf::Rect<float>& rectangle) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.findIntersection(rectangle));
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.FloatRect", "getCenter", "fun(self: sf.FloatRect): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Rect_float_, "getCenter",
        [](const sf::Rect<float>& self) -> sf::Vector2<float> {
            return self.getCenter();
        },
        docs[7]
    );
    lua_glue::BindMetamethod(type_sf__Rect_float_, "__tostring", [name = std::string("FloatRect")](const sf::Rect<float>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.position.x << ", " << self.position.y << ", "
               << self.size.x << ", " << self.size.y << ")";
        return stream.str();
    });
    lua_glue::BindMetamethod(type_sf__Rect_float_, "__eq", [](const sf::Rect<float>& left, const sf::Rect<float>& right) { return left == right; });
    LUASF_STUB_OPERATOR("sf.FloatRect", "eq(sf.FloatRect): boolean");
    LUASF_STUB_FUNCTION("sf.FloatRect", "copy", "fun(self: sf.FloatRect): sf.FloatRect");
    LUASF_STUB_FUNCTION("sf.FloatRect", "deepcopy", "fun(self: sf.FloatRect): sf.FloatRect");
}
