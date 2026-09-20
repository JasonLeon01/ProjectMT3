#include "System/bind_Version.hpp"

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

namespace { constexpr std::array<std::string_view, 6> docs = {
    "SFML major version",
    "SFML minor version",
    "SFML patch version",
    "`true` if this is a release version, `false` if this is a development version",
    "String representation of the SFML version, e.g. 3.1.0 or 3.1.0-dev",
    "\\brief Retrieve the runtime version of the SFML library\n\nThe SFML_VERSION_MAJOR, SFML_VERSION_MINOR,\nSFML_VERSION_PATCH and SFML_VERSION_IS_RELEASE defines\nonly provide a way to determine the SFML library version\nat compile time. If an application is dynamically linked\nto SFML, the version of the library that is loaded at\nruntime might not be the same as the version of the\nlibrary headers which the application included when it\nwas built. In order for an application to determine\nwhich SFML version is currently loaded at runtime, it\ncan use this function. This function relies on version\ninformation embedded into the loaded library when it was\nbuilt. This information can be useful when an application\nhas to determine the SFML version in order to diagnose\nproblems and be able to report them to the library\nmaintainers.\n\n\\return The version of the SFML library",
}; }

void bind_Version(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Version = lua_glue::BindClass<sf::Version>(sf, "Version");
    lua_glue::Table table_sf__Version = sf["Version"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Version>(lua);
    LUASF_STUB_CLASS("sf.Version");
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_FIELD("major", "integer");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("minor", "integer");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("patch", "integer");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("isRelease", "boolean");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("string", "string");
    lua_glue::BindProperty(type_sf__Version, "major",
        [](const sf::Version& self) {
            return self.major;
        }
    );
    lua_glue::BindProperty(type_sf__Version, "minor",
        [](const sf::Version& self) {
            return self.minor;
        }
    );
    lua_glue::BindProperty(type_sf__Version, "patch",
        [](const sf::Version& self) {
            return self.patch;
        }
    );
    lua_glue::BindAttr<const bool>(type_sf__Version, "isRelease", &sf::Version::isRelease);
    lua_glue::BindProperty(type_sf__Version, "string",
        [](const sf::Version& self) {
            return std::string(self.string);
        }
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf", "version", "fun(): sf.Version");
    lua_glue::BindCallable(sf, "version",
        []() {
            return std::cref(sf::version());
        },
        docs[5]
    );
}
