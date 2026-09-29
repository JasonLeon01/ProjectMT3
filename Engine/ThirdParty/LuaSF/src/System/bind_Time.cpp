#include "System/bind_Time.hpp"

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

namespace { constexpr std::array<std::string_view, 10> docs = {
    "\\brief Represents a time value",
    "\\brief Default constructor\n\nSets the time value to zero.",
    "\\brief Return the time value as a number of seconds\n\n\\return Time in seconds\n\n\\see `asMilliseconds`, `asMicroseconds`",
    "\\brief Return the time value as a number of milliseconds\n\n\\return Time in milliseconds\n\n\\see `asSeconds`, `asMicroseconds`",
    "\\brief Return the time value as a number of microseconds\n\n\\return Time in microseconds\n\n\\see `asSeconds`, `asMilliseconds`",
    "\\brief Return the time value as a `std::chrono::duration`\n\n\\return Time in microseconds",
    "Predefined \"zero\" time value",
    "\\relates Time\n\\brief Construct a time value from a number of seconds\n\n\\param amount Number of seconds\n\n\\return Time value constructed from the amount of seconds\n\n\\see `milliseconds`, `microseconds`",
    "\\relates Time\n\\brief Construct a time value from a number of milliseconds\n\n\\param amount Number of milliseconds\n\n\\return Time value constructed from the amount of milliseconds\n\n\\see `seconds`, `microseconds`",
    "\\relates Time\n\\brief Construct a time value from a number of microseconds\n\n\\param amount Number of microseconds\n\n\\return Time value constructed from the amount of microseconds\n\n\\see `seconds`, `milliseconds`",
}; }

void bind_Time(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Time = lua_glue::BindStruct<sf::Time>(sf, "Time");
    lua_glue::Table table_sf__Time = sf["Time"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Time");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Time", "new", "fun(): sf.Time");
    lua_glue::BindCallable(type_sf__Time, "new",
        []() {
            return sf::Time{};
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Time", "asSeconds", "fun(self: sf.Time): number");
    lua_glue::BindCallable(type_sf__Time, "asSeconds",
        [](const sf::Time& self) -> float {
            return self.asSeconds();
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Time", "asMilliseconds", "fun(self: sf.Time): integer");
    lua_glue::BindCallable(type_sf__Time, "asMilliseconds",
        [](const sf::Time& self) -> std::int32_t {
            return self.asMilliseconds();
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Time", "asMicroseconds", "fun(self: sf.Time): integer");
    lua_glue::BindCallable(type_sf__Time, "asMicroseconds",
        [](const sf::Time& self) -> std::int64_t {
            return self.asMicroseconds();
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Time", "toDuration", "fun(self: sf.Time): any");
    lua_glue::BindCallable(type_sf__Time, "toDuration",
        [](const sf::Time& self) -> std::chrono::microseconds {
            return self.toDuration();
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_VALUE("sf.Time", "Zero", "sf.Time");
    lua_glue::BindStaticAttr<const sf::Time>(table_sf__Time, "Zero", &sf::Time::Zero);
    LUASF_STUB_FUNCTION("sf.Time", "copy", "fun(self: sf.Time): sf.Time");
    LUASF_STUB_FUNCTION("sf.Time", "deepcopy", "fun(self: sf.Time): sf.Time");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf", "seconds", "fun(amount: number): sf.Time");
    lua_glue::BindCallable(sf, "seconds",
        [](float amount) -> sf::Time {
            return sf::seconds(amount);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf", "milliseconds", "fun(amount: integer): sf.Time");
    lua_glue::BindCallable(sf, "milliseconds",
        [](lua_sf::LuaIntegral<std::int32_t> amount) -> sf::Time {
            return sf::milliseconds(amount.value());
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf", "microseconds", "fun(amount: integer): sf.Time");
    lua_glue::BindCallable(sf, "microseconds",
        [](lua_sf::LuaIntegral<std::int64_t> amount) -> sf::Time {
            return sf::microseconds(amount.value());
        },
        docs[9]
    );
}
