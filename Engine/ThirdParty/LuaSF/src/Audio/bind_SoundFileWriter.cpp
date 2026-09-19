#include "Audio/bind_SoundFileWriter.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_SoundFileWriter(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundFileWriter = sf.new_usertype<sf::SoundFileWriter>("SoundFileWriter", sol::no_constructor);
    sol::table table_sf__SoundFileWriter = sf["SoundFileWriter"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::SoundFileWriter>(lua);
    LUASF_STUB_DOC("\\brief Abstract base class for sound file encoding");
    LUASF_STUB_CLASS("sf.SoundFileWriter");
    // sf::SoundFileWriter is abstract; constructor binding is omitted.
    LUASF_STUB_DOC("\\brief Open a sound file for writing\n\n\\param filename     Path of the file to open\n\\param sampleRate   Sample rate of the sound\n\\param channelCount Number of channels of the sound\n\\param channelMap   Map of position in sample frame to sound channel\n\n\\return `true` if the file was successfully opened");
    LUASF_STUB_FUNCTION("sf.SoundFileWriter", "open", "fun(self: sf.SoundFileWriter, filename: string, sampleRate: integer, channelCount: integer, channelMap: sf.SoundChannel[]): boolean");
    type_sf__SoundFileWriter.set_function("open",
        [](sf::SoundFileWriter& self, std::string filename, lua_sf::LuaIntegral<unsigned int> sampleRate, lua_sf::LuaIntegral<unsigned int> channelCount, sol::table channelMap) -> bool {
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return self.open(std::filesystem::path(filename), sampleRate.value(), channelCount.value(), channelMap_vector);
        }
    );
    LUASF_STUB_DOC("\\brief Write audio samples to the open file\n\n\\param samples Pointer to the sample array to write\n\\param count   Number of samples to write");
    LUASF_STUB_FUNCTION("sf.SoundFileWriter", "write", "fun(self: sf.SoundFileWriter, samples: any)");
    type_sf__SoundFileWriter.set_function("write",
        [](sf::SoundFileWriter& self, sol::object samples) {
            auto samples_buffer = lua_sf::array_from_object<std::int16_t>(samples);
            self.write(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()));
        }
    );
}
