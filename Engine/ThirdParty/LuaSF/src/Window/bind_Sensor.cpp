#include "Window/bind_Sensor.hpp"

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

namespace { constexpr std::array<std::string_view, 11> docs = {
    "\\brief Sensor type",
    "Measures the raw acceleration (m/s^2)",
    "Measures the raw rotation rates (radians/s)",
    "Measures the ambient magnetic field (micro-teslas)",
    "Measures the direction and intensity of gravity, independent of device acceleration (m/s^2)",
    "Measures the direction and intensity of device acceleration, independent of the gravity (m/s^2)",
    "Measures the absolute 3D orientation (radians)",
    "The total number of sensor types",
    "\\brief Check if a sensor is available on the underlying platform\n\n\\param sensor Sensor to check\n\n\\return `true` if the sensor is available, `false` otherwise",
    "\\brief Enable or disable a sensor\n\nAll sensors are disabled by default, to avoid consuming too\nmuch battery power. Once a sensor is enabled, it starts\nsending events of the corresponding type.\n\nThis function does nothing if the sensor is unavailable.\n\n\\param sensor  Sensor to enable\n\\param enabled `true` to enable, `false` to disable",
    "\\brief Get the current sensor value\n\n\\param sensor Sensor to read\n\n\\return The current sensor value",
}; }

void bind_Sensor(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    lua_glue::Table sf_Sensor = sf["Sensor"].get_or_create<lua_glue::Table>();
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Sensor.Type");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("Accelerometer", "sf.Sensor.Type");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("Gyroscope", "sf.Sensor.Type");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("Magnetometer", "sf.Sensor.Type");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("Gravity", "sf.Sensor.Type");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("UserAcceleration", "sf.Sensor.Type");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("Orientation", "sf.Sensor.Type");
    lua_glue::BindEnum<sf::Sensor::Type>(sf_Sensor, "Type", {
        {"Accelerometer", sf::Sensor::Type::Accelerometer},
        {"Gyroscope", sf::Sensor::Type::Gyroscope},
        {"Magnetometer", sf::Sensor::Type::Magnetometer},
        {"Gravity", sf::Sensor::Type::Gravity},
        {"UserAcceleration", sf::Sensor::Type::UserAcceleration},
        {"Orientation", sf::Sensor::Type::Orientation}
    });
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_VALUE("sf.Sensor", "Count", "integer");
    lua_glue::BindStaticAttr<const unsigned int>(sf_Sensor, "Count", &sf::Sensor::Count);
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Sensor", "isAvailable", "fun(sensor: sf.Sensor.Type): boolean");
    lua_glue::BindCallable(sf_Sensor, "isAvailable",
        [](sf::Sensor::Type sensor) -> bool {
            return sf::Sensor::isAvailable(sensor);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Sensor", "setEnabled", "fun(sensor: sf.Sensor.Type, enabled: boolean)");
    lua_glue::BindCallable(sf_Sensor, "setEnabled",
        [](sf::Sensor::Type sensor, bool enabled) {
            sf::Sensor::setEnabled(sensor, enabled);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Sensor", "getValue", "fun(sensor: sf.Sensor.Type): sf.Vector3f");
    lua_glue::BindCallable(sf_Sensor, "getValue",
        [](sf::Sensor::Type sensor) -> sf::Vector3f {
            return sf::Sensor::getValue(sensor);
        },
        docs[10]
    );
}
