#include "Graphics/bind_Rect.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Rect(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Rect_int_ = sf.new_usertype<sf::Rect<int>>("IntRect", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Rect_int_);
    sol::table table_sf__Rect_int_ = sf["IntRect"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Utility class for manipulating 2D axis aligned rectangles");
    LUASF_STUB_CLASS("sf.IntRect");
    LUASF_STUB_DOC("Position of the top-left corner of the rectangle");
    LUASF_STUB_FIELD("position", "sf.Vector2i");
    LUASF_STUB_DOC("Size of the rectangle");
    LUASF_STUB_FIELD("size", "sf.Vector2i");
    LUASF_STUB_DOC("\\brief Construct the rectangle from position and size\n\nBe careful, the last parameter is the size,\nnot the bottom-right corner!\n\n\\param position Position of the top-left corner of the rectangle\n\\param size     Size of the rectangle");
    LUASF_STUB_FUNCTION("sf.IntRect", "new", "fun(position: sf.Vector2i, size: sf.Vector2i): sf.IntRect");
    LUASF_STUB_OVERLOAD("sf.IntRect", "new", "fun(): sf.IntRect");
    LUASF_STUB_OVERLOAD("sf.IntRect", "new", "fun(x: integer, y: integer, width: integer, height: integer): sf.IntRect");
    type_sf__Rect_int_.set_function("new", sol::factories(
        [](sf::Vector2<int> position, sf::Vector2<int> size) {
            return sf::Rect<int>{position, size};
        },
        []() {
            return sf::Rect<int>{};
        },
        [](lua_sf::LuaIntegral<int> x, lua_sf::LuaIntegral<int> y, lua_sf::LuaIntegral<int> width, lua_sf::LuaIntegral<int> height) {
            return sf::Rect<int>{{x.value(), y.value()},
                               {width.value(), height.value()}};
        }
    ));
    type_sf__Rect_int_["position"] = sol::policies(&sf::Rect<int>::position, sol::self_dependency{});
    type_sf__Rect_int_["size"] = sol::policies(&sf::Rect<int>::size, sol::self_dependency{});
    LUASF_STUB_DOC("\\brief Check if a point is inside the rectangle's area\n\nThis check is non-inclusive. If the point lies on the\nedge of the rectangle, this function will return `false`.\n\n\\param point Point to test\n\n\\return `true` if the point is inside, `false` otherwise\n\n\\see `findIntersection`");
    LUASF_STUB_FUNCTION("sf.IntRect", "contains", "fun(self: sf.IntRect, point: sf.Vector2i): boolean");
    type_sf__Rect_int_.set_function("contains",
        [](sf::Rect<int>& self, sf::Vector2<int> point) -> bool {
            return self.contains(point);
        }
    );
    LUASF_STUB_DOC("\\brief Check the intersection between two rectangles\n\n\\param rectangle Rectangle to test\n\n\\return Intersection rectangle if intersecting, `std::nullopt` otherwise\n\n\\see `contains`");
    LUASF_STUB_FUNCTION("sf.IntRect", "findIntersection", "fun(self: sf.IntRect, rectangle: sf.IntRect): sf.IntRect|nil");
    type_sf__Rect_int_.set_function("findIntersection",
        [lua](sf::Rect<int>& self, const sf::Rect<int>& rectangle) -> sol::object {
            return lua_sf::optional_to_object(lua, self.findIntersection(rectangle));
        }
    );
    LUASF_STUB_DOC("\\brief Get the position of the center of the rectangle\n\n\\return Center of rectangle");
    LUASF_STUB_FUNCTION("sf.IntRect", "getCenter", "fun(self: sf.IntRect): sf.Vector2i");
    type_sf__Rect_int_.set_function("getCenter",
        [](sf::Rect<int>& self) -> sf::Vector2<int> {
            return self.getCenter();
        }
    );
    type_sf__Rect_int_[sol::meta_function::to_string] = [name = std::string("IntRect")](const sf::Rect<int>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.position.x << ", " << self.position.y << ", "
               << self.size.x << ", " << self.size.y << ")";
        return stream.str();
    };
    type_sf__Rect_int_[sol::meta_function::equal_to] = [](const sf::Rect<int>& left, const sf::Rect<int>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.IntRect", "eq(sf.IntRect): boolean");
    auto type_sf__Rect_float_ = sf.new_usertype<sf::Rect<float>>("FloatRect", sol::no_constructor);
    lua_sf::enableValueCopy(type_sf__Rect_float_);
    sol::table table_sf__Rect_float_ = sf["FloatRect"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Utility class for manipulating 2D axis aligned rectangles");
    LUASF_STUB_CLASS("sf.FloatRect");
    LUASF_STUB_DOC("Position of the top-left corner of the rectangle");
    LUASF_STUB_FIELD("position", "sf.Vector2f");
    LUASF_STUB_DOC("Size of the rectangle");
    LUASF_STUB_FIELD("size", "sf.Vector2f");
    LUASF_STUB_DOC("\\brief Construct the rectangle from position and size\n\nBe careful, the last parameter is the size,\nnot the bottom-right corner!\n\n\\param position Position of the top-left corner of the rectangle\n\\param size     Size of the rectangle");
    LUASF_STUB_FUNCTION("sf.FloatRect", "new", "fun(position: sf.Vector2f, size: sf.Vector2f): sf.FloatRect");
    LUASF_STUB_OVERLOAD("sf.FloatRect", "new", "fun(): sf.FloatRect");
    LUASF_STUB_OVERLOAD("sf.FloatRect", "new", "fun(x: number, y: number, width: number, height: number): sf.FloatRect");
    type_sf__Rect_float_.set_function("new", sol::factories(
        [](sf::Vector2<float> position, sf::Vector2<float> size) {
            return sf::Rect<float>{position, size};
        },
        []() {
            return sf::Rect<float>{};
        },
        [](float x, float y, float width, float height) {
            return sf::Rect<float>{{x, y},
                               {width, height}};
        }
    ));
    type_sf__Rect_float_["position"] = sol::policies(&sf::Rect<float>::position, sol::self_dependency{});
    type_sf__Rect_float_["size"] = sol::policies(&sf::Rect<float>::size, sol::self_dependency{});
    LUASF_STUB_DOC("\\brief Check if a point is inside the rectangle's area\n\nThis check is non-inclusive. If the point lies on the\nedge of the rectangle, this function will return `false`.\n\n\\param point Point to test\n\n\\return `true` if the point is inside, `false` otherwise\n\n\\see `findIntersection`");
    LUASF_STUB_FUNCTION("sf.FloatRect", "contains", "fun(self: sf.FloatRect, point: sf.Vector2f): boolean");
    type_sf__Rect_float_.set_function("contains",
        [](sf::Rect<float>& self, sf::Vector2<float> point) -> bool {
            return self.contains(point);
        }
    );
    LUASF_STUB_DOC("\\brief Check the intersection between two rectangles\n\n\\param rectangle Rectangle to test\n\n\\return Intersection rectangle if intersecting, `std::nullopt` otherwise\n\n\\see `contains`");
    LUASF_STUB_FUNCTION("sf.FloatRect", "findIntersection", "fun(self: sf.FloatRect, rectangle: sf.FloatRect): sf.FloatRect|nil");
    type_sf__Rect_float_.set_function("findIntersection",
        [lua](sf::Rect<float>& self, const sf::Rect<float>& rectangle) -> sol::object {
            return lua_sf::optional_to_object(lua, self.findIntersection(rectangle));
        }
    );
    LUASF_STUB_DOC("\\brief Get the position of the center of the rectangle\n\n\\return Center of rectangle");
    LUASF_STUB_FUNCTION("sf.FloatRect", "getCenter", "fun(self: sf.FloatRect): sf.Vector2f");
    type_sf__Rect_float_.set_function("getCenter",
        [](sf::Rect<float>& self) -> sf::Vector2<float> {
            return self.getCenter();
        }
    );
    type_sf__Rect_float_[sol::meta_function::to_string] = [name = std::string("FloatRect")](const sf::Rect<float>& self) {
        std::ostringstream stream;
        stream << name << "(" << self.position.x << ", " << self.position.y << ", "
               << self.size.x << ", " << self.size.y << ")";
        return stream.str();
    };
    type_sf__Rect_float_[sol::meta_function::equal_to] = [](const sf::Rect<float>& left, const sf::Rect<float>& right) { return left == right; };
    LUASF_STUB_OPERATOR("sf.FloatRect", "eq(sf.FloatRect): boolean");
}
