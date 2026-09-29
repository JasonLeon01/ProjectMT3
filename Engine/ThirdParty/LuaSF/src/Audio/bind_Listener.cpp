#include "Audio/bind_Listener.hpp"

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

namespace { constexpr std::array<std::string_view, 16> docs = {
    "\\brief Structure defining the properties of a directional cone\n\nSounds will play at gain 1 when they are positioned\nwithin the inner angle of the cone. Sounds will play\nat `outerGain` when they are positioned outside the\nouter angle of the cone. The gain declines linearly\nfrom 1 to `outerGain` as the sound moves from the inner\nangle to the outer angle.",
    "Inner angle",
    "Outer angle",
    "Outer gain",
    "\\brief Change the global volume of all the sounds and musics\n\n`volume` is a number between 0 and 100; it is combined\nwith the individual volume of each sound / music.\nThe default value for the volume is 100 (maximum).\n\n\\param volume New global volume, in the range [0, 100]\n\n\\see `getGlobalVolume`",
    "\\brief Get the current value of the global volume\n\n\\return Current global volume, in the range [0, 100]\n\n\\see `setGlobalVolume`",
    "\\brief Set the position of the listener in the scene\n\nThe default listener's position is (0, 0, 0).\n\n\\param position New listener's position\n\n\\see `getPosition`, `setDirection`",
    "\\brief Get the current position of the listener in the scene\n\n\\return Listener's position\n\n\\see `setPosition`",
    "\\brief Set the forward vector of the listener in the scene\n\nThe direction (also called \"at vector\") is the vector\npointing forward from the listener's perspective. Together\nwith the up vector, it defines the 3D orientation of the\nlistener in the scene. The direction vector doesn't\nhave to be normalized.\nThe default listener's direction is (0, 0, -1).\n\n\\param direction New listener's direction\n\n\\see `getDirection`, `setUpVector`, `setPosition`",
    "\\brief Get the current forward vector of the listener in the scene\n\n\\return Listener's forward vector (not normalized)\n\n\\see `setDirection`",
    "\\brief Set the velocity of the listener in the scene\n\nThe default listener's velocity is (0, 0, -1).\n\n\\param velocity New listener's velocity\n\n\\see `getVelocity`, `getDirection`, `setUpVector`, `setPosition`",
    "\\brief Get the current forward vector of the listener in the scene\n\n\\return Listener's velocity\n\n\\see `setVelocity`",
    "\\brief Set the cone properties of the listener in the audio scene\n\nThe cone defines how directional attenuation is applied.\nThe default cone of a sound is (2 * PI, 2 * PI, 1).\n\n\\param cone Cone properties of the listener in the scene\n\n\\see `getCone`",
    "\\brief Get the cone properties of the listener in the audio scene\n\n\\return Cone properties of the listener\n\n\\see `setCone`",
    "\\brief Set the upward vector of the listener in the scene\n\nThe up vector is the vector that points upward from the\nlistener's perspective. Together with the direction, it\ndefines the 3D orientation of the listener in the scene.\nThe up vector doesn't have to be normalized.\nThe default listener's up vector is (0, 1, 0). It is usually\nnot necessary to change it, especially in 2D scenarios.\n\n\\param upVector New listener's up vector\n\n\\see `getUpVector`, `setDirection`, `setPosition`",
    "\\brief Get the current upward vector of the listener in the scene\n\n\\return Listener's upward vector (not normalized)\n\n\\see `setUpVector`",
}; }

void bind_Listener(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    lua_glue::Table sf_Listener = sf["Listener"].get_or_create<lua_glue::Table>();
    auto type_sf__Listener__Cone = lua_glue::BindClass<sf::Listener::Cone>(sf_Listener, "Cone");
    lua_glue::Table table_sf__Listener__Cone = sf_Listener["Cone"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Listener::Cone>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Listener.Cone");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("innerAngle", "sf.Angle");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("outerAngle", "sf.Angle");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("outerGain", "number");
    LUASF_STUB_FUNCTION("sf.Listener.Cone", "new", "fun(): sf.Listener.Cone");
    lua_glue::BindCallable(type_sf__Listener__Cone, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Listener::Cone>();
        }
    );
    lua_glue::BindAttr<sf::Angle>(type_sf__Listener__Cone, "innerAngle", &sf::Listener::Cone::innerAngle);
    lua_glue::BindAttr<sf::Angle>(type_sf__Listener__Cone, "outerAngle", &sf::Listener::Cone::outerAngle);
    lua_glue::BindAttr<float>(type_sf__Listener__Cone, "outerGain", &sf::Listener::Cone::outerGain);
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Listener", "setGlobalVolume", "fun(volume: number)");
    lua_glue::BindCallable(sf_Listener, "setGlobalVolume",
        [](float volume) {
            sf::Listener::setGlobalVolume(volume);
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Listener", "getGlobalVolume", "fun(): number");
    lua_glue::BindCallable(sf_Listener, "getGlobalVolume",
        []() -> float {
            return sf::Listener::getGlobalVolume();
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Listener", "setPosition", "fun(position: sf.Vector3f)");
    lua_glue::BindCallable(sf_Listener, "setPosition",
        [](const sf::Vector3f& position) {
            sf::Listener::setPosition(position);
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Listener", "getPosition", "fun(): sf.Vector3f");
    lua_glue::BindCallable(sf_Listener, "getPosition",
        []() -> sf::Vector3f {
            return sf::Listener::getPosition();
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Listener", "setDirection", "fun(direction: sf.Vector3f)");
    lua_glue::BindCallable(sf_Listener, "setDirection",
        [](const sf::Vector3f& direction) {
            sf::Listener::setDirection(direction);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Listener", "getDirection", "fun(): sf.Vector3f");
    lua_glue::BindCallable(sf_Listener, "getDirection",
        []() -> sf::Vector3f {
            return sf::Listener::getDirection();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Listener", "setVelocity", "fun(velocity: sf.Vector3f)");
    lua_glue::BindCallable(sf_Listener, "setVelocity",
        [](const sf::Vector3f& velocity) {
            sf::Listener::setVelocity(velocity);
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Listener", "getVelocity", "fun(): sf.Vector3f");
    lua_glue::BindCallable(sf_Listener, "getVelocity",
        []() -> sf::Vector3f {
            return sf::Listener::getVelocity();
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Listener", "setCone", "fun(cone: sf.Listener.Cone)");
    lua_glue::BindCallable(sf_Listener, "setCone",
        [](const sf::Listener::Cone& cone) {
            sf::Listener::setCone(cone);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Listener", "getCone", "fun(): sf.Listener.Cone");
    lua_glue::BindCallable(sf_Listener, "getCone",
        []() -> sf::Listener::Cone {
            return sf::Listener::getCone();
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Listener", "setUpVector", "fun(upVector: sf.Vector3f)");
    lua_glue::BindCallable(sf_Listener, "setUpVector",
        [](const sf::Vector3f& upVector) {
            sf::Listener::setUpVector(upVector);
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Listener", "getUpVector", "fun(): sf.Vector3f");
    lua_glue::BindCallable(sf_Listener, "getUpVector",
        []() -> sf::Vector3f {
            return sf::Listener::getUpVector();
        },
        docs[15]
    );
}
