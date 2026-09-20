#include "Audio/bind_OutputSoundFile.hpp"

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
    "\\brief Provide write access to sound files",
    "\\brief Default constructor\n\nConstruct an output sound file that is not associated\nwith a file to write.",
    "\\brief Construct the sound file from the disk for writing\n\nThe supported audio formats are: WAV, OGG/Vorbis, FLAC.\n\n\\param filename     Path of the sound file to write\n\\param sampleRate   Sample rate of the sound\n\\param channelCount Number of channels in the sound\n\\param channelMap   Map of position in sample frame to sound channel\n\n\\throws sf::Exception if the file could not be opened successfully",
    "\\brief Open the sound file from the disk for writing\n\nThe supported audio formats are: WAV, OGG/Vorbis, FLAC.\n\n\\param filename     Path of the sound file to write\n\\param sampleRate   Sample rate of the sound\n\\param channelCount Number of channels in the sound\n\\param channelMap   Map of position in sample frame to sound channel\n\n\\return `true` if the file was successfully opened",
    "\\brief Write audio samples to the file\n\n\\param samples     Pointer to the sample array to write\n\\param count       Number of samples to write",
    "\\brief Close the current file",
}; }

void bind_OutputSoundFile(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__OutputSoundFile = lua_glue::BindClass<sf::OutputSoundFile>(sf, "OutputSoundFile");
    lua_glue::Table table_sf__OutputSoundFile = sf["OutputSoundFile"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::OutputSoundFile>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.OutputSoundFile");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.OutputSoundFile", "new", "fun(): sf.OutputSoundFile");
    LUASF_STUB_OVERLOAD("sf.OutputSoundFile", "new", "fun(filename: string, sampleRate: integer, channelCount: integer, channelMap: sf.SoundChannel[]): sf.OutputSoundFile");
    lua_glue::BindCallable(type_sf__OutputSoundFile, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::OutputSoundFile>();
        },
        docs[1]
    );
    lua_glue::BindCallable(type_sf__OutputSoundFile, "new",
        [](std::string filename, lua_sf::LuaIntegral<unsigned int> sampleRate, lua_sf::LuaIntegral<unsigned int> channelCount, lua_glue::Table channelMap) {
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return lua_sf::makeLuaSharedObject<sf::OutputSoundFile>(std::filesystem::path(filename), sampleRate.value(), channelCount.value(), channelMap_vector);
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.OutputSoundFile", "openFromFile", "fun(self: sf.OutputSoundFile, filename: string, sampleRate: integer, channelCount: integer, channelMap: sf.SoundChannel[]): boolean");
    lua_glue::BindCallable(type_sf__OutputSoundFile, "openFromFile",
        [](sf::OutputSoundFile& self, std::string filename, lua_sf::LuaIntegral<unsigned int> sampleRate, lua_sf::LuaIntegral<unsigned int> channelCount, lua_glue::Table channelMap) -> bool {
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return self.openFromFile(std::filesystem::path(filename), sampleRate.value(), channelCount.value(), channelMap_vector);
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.OutputSoundFile", "write", "fun(self: sf.OutputSoundFile, samples: any)");
    lua_glue::BindCallable(type_sf__OutputSoundFile, "write",
        [](sf::OutputSoundFile& self, lua_glue::Object samples) {
            auto samples_buffer = lua_sf::array_from_object<std::int16_t>(samples);
            self.write(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()));
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.OutputSoundFile", "close", "fun(self: sf.OutputSoundFile)");
    lua_glue::BindCallable(type_sf__OutputSoundFile, "close",
        [](sf::OutputSoundFile& self) {
            self.close();
        },
        docs[5]
    );
}
