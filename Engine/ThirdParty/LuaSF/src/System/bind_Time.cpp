#include "System/bind_Time.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Time(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Time = sf.new_usertype<sf::Time>("Time", sol::no_constructor);
    sol::table table_sf__Time = sf["Time"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Time>(lua);
    LUASF_STUB_DOC("\\brief Represents a time value");
    LUASF_STUB_CLASS("sf.Time");
    LUASF_STUB_DOC("\\brief Default constructor\n\nSets the time value to zero.");
    LUASF_STUB_FUNCTION("sf.Time", "new", "fun(): sf.Time");
    type_sf__Time.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Time>();
        }
    ));
    LUASF_STUB_DOC("\\brief Return the time value as a number of seconds\n\n\\return Time in seconds\n\n\\see `asMilliseconds`, `asMicroseconds`");
    LUASF_STUB_FUNCTION("sf.Time", "asSeconds", "fun(self: sf.Time): number");
    type_sf__Time.set_function("asSeconds",
        [](sf::Time& self) -> float {
            return self.asSeconds();
        }
    );
    LUASF_STUB_DOC("\\brief Return the time value as a number of milliseconds\n\n\\return Time in milliseconds\n\n\\see `asSeconds`, `asMicroseconds`");
    LUASF_STUB_FUNCTION("sf.Time", "asMilliseconds", "fun(self: sf.Time): integer");
    type_sf__Time.set_function("asMilliseconds",
        [](sf::Time& self) -> std::int32_t {
            return self.asMilliseconds();
        }
    );
    LUASF_STUB_DOC("\\brief Return the time value as a number of microseconds\n\n\\return Time in microseconds\n\n\\see `asSeconds`, `asMilliseconds`");
    LUASF_STUB_FUNCTION("sf.Time", "asMicroseconds", "fun(self: sf.Time): integer");
    type_sf__Time.set_function("asMicroseconds",
        [](sf::Time& self) -> std::int64_t {
            return self.asMicroseconds();
        }
    );
    LUASF_STUB_DOC("\\brief Return the time value as a `std::chrono::duration`\n\n\\return Time in microseconds");
    LUASF_STUB_FUNCTION("sf.Time", "toDuration", "fun(self: sf.Time): any");
    type_sf__Time.set_function("toDuration",
        [](sf::Time& self) -> std::chrono::microseconds {
            return self.toDuration();
        }
    );
    LUASF_STUB_DOC("Predefined \"zero\" time value");
    LUASF_STUB_VALUE("sf.Time", "Zero", "sf.Time");
    table_sf__Time["Zero"] = sf::Time::Zero;
    LUASF_STUB_DOC("\\relates Time\n\\brief Construct a time value from a number of seconds\n\n\\param amount Number of seconds\n\n\\return Time value constructed from the amount of seconds\n\n\\see `milliseconds`, `microseconds`");
    LUASF_STUB_FUNCTION("sf", "seconds", "fun(amount: number): sf.Time");
    sf.set_function("seconds",
        [](float amount) -> sf::Time {
            return sf::seconds(amount);
        }
    );
    LUASF_STUB_DOC("\\relates Time\n\\brief Construct a time value from a number of milliseconds\n\n\\param amount Number of milliseconds\n\n\\return Time value constructed from the amount of milliseconds\n\n\\see `seconds`, `microseconds`");
    LUASF_STUB_FUNCTION("sf", "milliseconds", "fun(amount: integer): sf.Time");
    sf.set_function("milliseconds",
        [](lua_sf::LuaIntegral<std::int32_t> amount) -> sf::Time {
            return sf::milliseconds(amount.value());
        }
    );
    LUASF_STUB_DOC("\\relates Time\n\\brief Construct a time value from a number of microseconds\n\n\\param amount Number of microseconds\n\n\\return Time value constructed from the amount of microseconds\n\n\\see `seconds`, `milliseconds`");
    LUASF_STUB_FUNCTION("sf", "microseconds", "fun(amount: integer): sf.Time");
    sf.set_function("microseconds",
        [](lua_sf::LuaIntegral<std::int64_t> amount) -> sf::Time {
            return sf::microseconds(amount.value());
        }
    );
}
