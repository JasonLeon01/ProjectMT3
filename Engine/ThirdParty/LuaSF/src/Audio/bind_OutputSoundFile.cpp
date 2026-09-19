#include "Audio/bind_OutputSoundFile.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_OutputSoundFile(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__OutputSoundFile = sf.new_usertype<sf::OutputSoundFile>("OutputSoundFile", sol::no_constructor);
    sol::table table_sf__OutputSoundFile = sf["OutputSoundFile"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::OutputSoundFile>(lua);
    LUASF_STUB_DOC("\\brief Provide write access to sound files");
    LUASF_STUB_CLASS("sf.OutputSoundFile");
    LUASF_STUB_DOC("\\brief Default constructor\n\nConstruct an output sound file that is not associated\nwith a file to write.");
    LUASF_STUB_FUNCTION("sf.OutputSoundFile", "new", "fun(): sf.OutputSoundFile");
    LUASF_STUB_OVERLOAD("sf.OutputSoundFile", "new", "fun(filename: string, sampleRate: integer, channelCount: integer, channelMap: sf.SoundChannel[]): sf.OutputSoundFile");
    type_sf__OutputSoundFile.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::OutputSoundFile>();
        },
        [](std::string filename, lua_sf::LuaIntegral<unsigned int> sampleRate, lua_sf::LuaIntegral<unsigned int> channelCount, sol::table channelMap) {
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return lua_sf::makeLuaSharedObject<sf::OutputSoundFile>(std::filesystem::path(filename), sampleRate.value(), channelCount.value(), channelMap_vector);
        }
    ));
    LUASF_STUB_DOC("\\brief Open the sound file from the disk for writing\n\nThe supported audio formats are: WAV, OGG/Vorbis, FLAC.\n\n\\param filename     Path of the sound file to write\n\\param sampleRate   Sample rate of the sound\n\\param channelCount Number of channels in the sound\n\\param channelMap   Map of position in sample frame to sound channel\n\n\\return `true` if the file was successfully opened");
    LUASF_STUB_FUNCTION("sf.OutputSoundFile", "openFromFile", "fun(self: sf.OutputSoundFile, filename: string, sampleRate: integer, channelCount: integer, channelMap: sf.SoundChannel[]): boolean");
    type_sf__OutputSoundFile.set_function("openFromFile",
        [](sf::OutputSoundFile& self, std::string filename, lua_sf::LuaIntegral<unsigned int> sampleRate, lua_sf::LuaIntegral<unsigned int> channelCount, sol::table channelMap) -> bool {
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return self.openFromFile(std::filesystem::path(filename), sampleRate.value(), channelCount.value(), channelMap_vector);
        }
    );
    LUASF_STUB_DOC("\\brief Write audio samples to the file\n\n\\param samples     Pointer to the sample array to write\n\\param count       Number of samples to write");
    LUASF_STUB_FUNCTION("sf.OutputSoundFile", "write", "fun(self: sf.OutputSoundFile, samples: any)");
    type_sf__OutputSoundFile.set_function("write",
        [](sf::OutputSoundFile& self, sol::object samples) {
            auto samples_buffer = lua_sf::array_from_object<std::int16_t>(samples);
            self.write(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()));
        }
    );
    LUASF_STUB_DOC("\\brief Close the current file");
    LUASF_STUB_FUNCTION("sf.OutputSoundFile", "close", "fun(self: sf.OutputSoundFile)");
    type_sf__OutputSoundFile.set_function("close",
        [](sf::OutputSoundFile& self) {
            self.close();
        }
    );
}
