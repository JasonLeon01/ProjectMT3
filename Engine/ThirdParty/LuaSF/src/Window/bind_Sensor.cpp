#include "Window/bind_Sensor.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Sensor(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    sol::table sf_Sensor = sf["Sensor"].get_or_create<sol::table>();
    LUASF_STUB_DOC("\\brief Sensor type");
    LUASF_STUB_CLASS("sf.Sensor.Type");
    LUASF_STUB_DOC("Measures the raw acceleration (m/s^2)");
    LUASF_STUB_FIELD("Accelerometer", "sf.Sensor.Type");
    LUASF_STUB_DOC("Measures the raw rotation rates (radians/s)");
    LUASF_STUB_FIELD("Gyroscope", "sf.Sensor.Type");
    LUASF_STUB_DOC("Measures the ambient magnetic field (micro-teslas)");
    LUASF_STUB_FIELD("Magnetometer", "sf.Sensor.Type");
    LUASF_STUB_DOC("Measures the direction and intensity of gravity, independent of device acceleration (m/s^2)");
    LUASF_STUB_FIELD("Gravity", "sf.Sensor.Type");
    LUASF_STUB_DOC("Measures the direction and intensity of device acceleration, independent of the gravity (m/s^2)");
    LUASF_STUB_FIELD("UserAcceleration", "sf.Sensor.Type");
    LUASF_STUB_DOC("Measures the absolute 3D orientation (radians)");
    LUASF_STUB_FIELD("Orientation", "sf.Sensor.Type");
    sf_Sensor.new_enum("Type",
        "Accelerometer", sf::Sensor::Type::Accelerometer,
        "Gyroscope", sf::Sensor::Type::Gyroscope,
        "Magnetometer", sf::Sensor::Type::Magnetometer,
        "Gravity", sf::Sensor::Type::Gravity,
        "UserAcceleration", sf::Sensor::Type::UserAcceleration,
        "Orientation", sf::Sensor::Type::Orientation
    );
    LUASF_STUB_DOC("The total number of sensor types");
    LUASF_STUB_VALUE("sf.Sensor", "Count", "integer");
    sf_Sensor["Count"] = sf::Sensor::Count;
    LUASF_STUB_DOC("\\brief Check if a sensor is available on the underlying platform\n\n\\param sensor Sensor to check\n\n\\return `true` if the sensor is available, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Sensor", "isAvailable", "fun(sensor: sf.Sensor.Type): boolean");
    sf_Sensor.set_function("isAvailable",
        [](sf::Sensor::Type sensor) -> bool {
            return sf::Sensor::isAvailable(sensor);
        }
    );
    LUASF_STUB_DOC("\\brief Enable or disable a sensor\n\nAll sensors are disabled by default, to avoid consuming too\nmuch battery power. Once a sensor is enabled, it starts\nsending events of the corresponding type.\n\nThis function does nothing if the sensor is unavailable.\n\n\\param sensor  Sensor to enable\n\\param enabled `true` to enable, `false` to disable");
    LUASF_STUB_FUNCTION("sf.Sensor", "setEnabled", "fun(sensor: sf.Sensor.Type, enabled: boolean)");
    sf_Sensor.set_function("setEnabled",
        [](sf::Sensor::Type sensor, bool enabled) {
            sf::Sensor::setEnabled(sensor, enabled);
        }
    );
    LUASF_STUB_DOC("\\brief Get the current sensor value\n\n\\param sensor Sensor to read\n\n\\return The current sensor value");
    LUASF_STUB_FUNCTION("sf.Sensor", "getValue", "fun(sensor: sf.Sensor.Type): sf.Vector3f");
    sf_Sensor.set_function("getValue",
        [](sf::Sensor::Type sensor) -> sf::Vector3f {
            return sf::Sensor::getValue(sensor);
        }
    );
}
