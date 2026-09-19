#include "Audio/bind_Listener.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Listener(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    sol::table sf_Listener = sf["Listener"].get_or_create<sol::table>();
    auto type_sf__Listener__Cone = sf_Listener.new_usertype<sf::Listener::Cone>("Cone", sol::no_constructor);
    sol::table table_sf__Listener__Cone = sf_Listener["Cone"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Listener::Cone>(lua);
    LUASF_STUB_DOC("\\brief Structure defining the properties of a directional cone\n\nSounds will play at gain 1 when they are positioned\nwithin the inner angle of the cone. Sounds will play\nat `outerGain` when they are positioned outside the\nouter angle of the cone. The gain declines linearly\nfrom 1 to `outerGain` as the sound moves from the inner\nangle to the outer angle.");
    LUASF_STUB_CLASS("sf.Listener.Cone");
    LUASF_STUB_DOC("Inner angle");
    LUASF_STUB_FIELD("innerAngle", "sf.Angle");
    LUASF_STUB_DOC("Outer angle");
    LUASF_STUB_FIELD("outerAngle", "sf.Angle");
    LUASF_STUB_DOC("Outer gain");
    LUASF_STUB_FIELD("outerGain", "number");
    LUASF_STUB_FUNCTION("sf.Listener.Cone", "new", "fun(): sf.Listener.Cone");
    type_sf__Listener__Cone.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Listener::Cone>();
        }
    ));
    type_sf__Listener__Cone["innerAngle"] = sol::policies(&sf::Listener::Cone::innerAngle, sol::self_dependency{});
    type_sf__Listener__Cone["outerAngle"] = sol::policies(&sf::Listener::Cone::outerAngle, sol::self_dependency{});
    type_sf__Listener__Cone["outerGain"] = sol::policies(&sf::Listener::Cone::outerGain, sol::self_dependency{});
    LUASF_STUB_DOC("\\brief Change the global volume of all the sounds and musics\n\n`volume` is a number between 0 and 100; it is combined\nwith the individual volume of each sound / music.\nThe default value for the volume is 100 (maximum).\n\n\\param volume New global volume, in the range [0, 100]\n\n\\see `getGlobalVolume`");
    LUASF_STUB_FUNCTION("sf.Listener", "setGlobalVolume", "fun(volume: number)");
    sf_Listener.set_function("setGlobalVolume",
        [](float volume) {
            sf::Listener::setGlobalVolume(volume);
        }
    );
    LUASF_STUB_DOC("\\brief Get the current value of the global volume\n\n\\return Current global volume, in the range [0, 100]\n\n\\see `setGlobalVolume`");
    LUASF_STUB_FUNCTION("sf.Listener", "getGlobalVolume", "fun(): number");
    sf_Listener.set_function("getGlobalVolume",
        []() -> float {
            return sf::Listener::getGlobalVolume();
        }
    );
    LUASF_STUB_DOC("\\brief Set the position of the listener in the scene\n\nThe default listener's position is (0, 0, 0).\n\n\\param position New listener's position\n\n\\see `getPosition`, `setDirection`");
    LUASF_STUB_FUNCTION("sf.Listener", "setPosition", "fun(position: sf.Vector3f)");
    sf_Listener.set_function("setPosition",
        [](const sf::Vector3f& position) {
            sf::Listener::setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief Get the current position of the listener in the scene\n\n\\return Listener's position\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.Listener", "getPosition", "fun(): sf.Vector3f");
    sf_Listener.set_function("getPosition",
        []() -> sf::Vector3f {
            return sf::Listener::getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief Set the forward vector of the listener in the scene\n\nThe direction (also called \"at vector\") is the vector\npointing forward from the listener's perspective. Together\nwith the up vector, it defines the 3D orientation of the\nlistener in the scene. The direction vector doesn't\nhave to be normalized.\nThe default listener's direction is (0, 0, -1).\n\n\\param direction New listener's direction\n\n\\see `getDirection`, `setUpVector`, `setPosition`");
    LUASF_STUB_FUNCTION("sf.Listener", "setDirection", "fun(direction: sf.Vector3f)");
    sf_Listener.set_function("setDirection",
        [](const sf::Vector3f& direction) {
            sf::Listener::setDirection(direction);
        }
    );
    LUASF_STUB_DOC("\\brief Get the current forward vector of the listener in the scene\n\n\\return Listener's forward vector (not normalized)\n\n\\see `setDirection`");
    LUASF_STUB_FUNCTION("sf.Listener", "getDirection", "fun(): sf.Vector3f");
    sf_Listener.set_function("getDirection",
        []() -> sf::Vector3f {
            return sf::Listener::getDirection();
        }
    );
    LUASF_STUB_DOC("\\brief Set the velocity of the listener in the scene\n\nThe default listener's velocity is (0, 0, -1).\n\n\\param velocity New listener's velocity\n\n\\see `getVelocity`, `getDirection`, `setUpVector`, `setPosition`");
    LUASF_STUB_FUNCTION("sf.Listener", "setVelocity", "fun(velocity: sf.Vector3f)");
    sf_Listener.set_function("setVelocity",
        [](const sf::Vector3f& velocity) {
            sf::Listener::setVelocity(velocity);
        }
    );
    LUASF_STUB_DOC("\\brief Get the current forward vector of the listener in the scene\n\n\\return Listener's velocity\n\n\\see `setVelocity`");
    LUASF_STUB_FUNCTION("sf.Listener", "getVelocity", "fun(): sf.Vector3f");
    sf_Listener.set_function("getVelocity",
        []() -> sf::Vector3f {
            return sf::Listener::getVelocity();
        }
    );
    LUASF_STUB_DOC("\\brief Set the cone properties of the listener in the audio scene\n\nThe cone defines how directional attenuation is applied.\nThe default cone of a sound is (2 * PI, 2 * PI, 1).\n\n\\param cone Cone properties of the listener in the scene\n\n\\see `getCone`");
    LUASF_STUB_FUNCTION("sf.Listener", "setCone", "fun(cone: sf.Listener.Cone)");
    sf_Listener.set_function("setCone",
        [](const sf::Listener::Cone& cone) {
            sf::Listener::setCone(cone);
        }
    );
    LUASF_STUB_DOC("\\brief Get the cone properties of the listener in the audio scene\n\n\\return Cone properties of the listener\n\n\\see `setCone`");
    LUASF_STUB_FUNCTION("sf.Listener", "getCone", "fun(): sf.Listener.Cone");
    sf_Listener.set_function("getCone",
        []() -> sf::Listener::Cone {
            return sf::Listener::getCone();
        }
    );
    LUASF_STUB_DOC("\\brief Set the upward vector of the listener in the scene\n\nThe up vector is the vector that points upward from the\nlistener's perspective. Together with the direction, it\ndefines the 3D orientation of the listener in the scene.\nThe up vector doesn't have to be normalized.\nThe default listener's up vector is (0, 1, 0). It is usually\nnot necessary to change it, especially in 2D scenarios.\n\n\\param upVector New listener's up vector\n\n\\see `getUpVector`, `setDirection`, `setPosition`");
    LUASF_STUB_FUNCTION("sf.Listener", "setUpVector", "fun(upVector: sf.Vector3f)");
    sf_Listener.set_function("setUpVector",
        [](const sf::Vector3f& upVector) {
            sf::Listener::setUpVector(upVector);
        }
    );
    LUASF_STUB_DOC("\\brief Get the current upward vector of the listener in the scene\n\n\\return Listener's upward vector (not normalized)\n\n\\see `setUpVector`");
    LUASF_STUB_FUNCTION("sf.Listener", "getUpVector", "fun(): sf.Vector3f");
    sf_Listener.set_function("getUpVector",
        []() -> sf::Vector3f {
            return sf::Listener::getUpVector();
        }
    );
}
