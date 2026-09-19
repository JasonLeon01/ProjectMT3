#include "System/bind_Version.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Version(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Version = sf.new_usertype<sf::Version>("Version", sol::no_constructor);
    sol::table table_sf__Version = sf["Version"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Version>(lua);
    LUASF_STUB_CLASS("sf.Version");
    LUASF_STUB_DOC("SFML major version");
    LUASF_STUB_FIELD("major", "integer");
    LUASF_STUB_DOC("SFML minor version");
    LUASF_STUB_FIELD("minor", "integer");
    LUASF_STUB_DOC("SFML patch version");
    LUASF_STUB_FIELD("patch", "integer");
    LUASF_STUB_DOC("`true` if this is a release version, `false` if this is a development version");
    LUASF_STUB_FIELD("isRelease", "boolean");
    LUASF_STUB_DOC("String representation of the SFML version, e.g. 3.1.0 or 3.1.0-dev");
    LUASF_STUB_FIELD("string", "string");
    type_sf__Version.set("major", sol::property(
        [](sf::Version& self) {
            return self.major;
        }
    ));
    type_sf__Version.set("minor", sol::property(
        [](sf::Version& self) {
            return self.minor;
        }
    ));
    type_sf__Version.set("patch", sol::property(
        [](sf::Version& self) {
            return self.patch;
        }
    ));
    type_sf__Version["isRelease"] = sol::policies(&sf::Version::isRelease, sol::self_dependency{});
    type_sf__Version.set("string", sol::property(
        [](sf::Version& self) {
            return std::string(self.string);
        }
    ));
    LUASF_STUB_DOC("\\brief Retrieve the runtime version of the SFML library\n\nThe SFML_VERSION_MAJOR, SFML_VERSION_MINOR,\nSFML_VERSION_PATCH and SFML_VERSION_IS_RELEASE defines\nonly provide a way to determine the SFML library version\nat compile time. If an application is dynamically linked\nto SFML, the version of the library that is loaded at\nruntime might not be the same as the version of the\nlibrary headers which the application included when it\nwas built. In order for an application to determine\nwhich SFML version is currently loaded at runtime, it\ncan use this function. This function relies on version\ninformation embedded into the loaded library when it was\nbuilt. This information can be useful when an application\nhas to determine the SFML version in order to diagnose\nproblems and be able to report them to the library\nmaintainers.\n\n\\return The version of the SFML library");
    LUASF_STUB_FUNCTION("sf", "version", "fun(): sf.Version");
    sf.set_function("version",
        []() {
            return std::cref(sf::version());
        }
    );
}
