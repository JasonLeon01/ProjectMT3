#include "Audio/bind_SoundFileWriter.hpp"

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

namespace { constexpr std::array<std::string_view, 3> docs = {
    "\\brief Abstract base class for sound file encoding",
    "\\brief Open a sound file for writing\n\n\\param filename     Path of the file to open\n\\param sampleRate   Sample rate of the sound\n\\param channelCount Number of channels of the sound\n\\param channelMap   Map of position in sample frame to sound channel\n\n\\return `true` if the file was successfully opened",
    "\\brief Write audio samples to the open file\n\n\\param samples Pointer to the sample array to write\n\\param count   Number of samples to write",
}; }

void bind_SoundFileWriter(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundFileWriter = lua_glue::BindClass<sf::SoundFileWriter>(sf, "SoundFileWriter");
    lua_glue::Table table_sf__SoundFileWriter = sf["SoundFileWriter"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::SoundFileWriter>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.SoundFileWriter");
    // sf::SoundFileWriter is abstract; constructor binding is omitted.
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.SoundFileWriter", "open", "fun(self: sf.SoundFileWriter, filename: string, sampleRate: integer, channelCount: integer, channelMap: sf.SoundChannel[]): boolean");
    lua_glue::BindCallable(type_sf__SoundFileWriter, "open",
        [](sf::SoundFileWriter& self, std::string filename, lua_sf::LuaIntegral<unsigned int> sampleRate, lua_sf::LuaIntegral<unsigned int> channelCount, lua_glue::Table channelMap) -> bool {
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return self.open(std::filesystem::path(filename), sampleRate.value(), channelCount.value(), channelMap_vector);
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.SoundFileWriter", "write", "fun(self: sf.SoundFileWriter, samples: any)");
    lua_glue::BindCallable(type_sf__SoundFileWriter, "write",
        [](sf::SoundFileWriter& self, lua_glue::Object samples) {
            auto samples_buffer = lua_sf::array_from_object<std::int16_t>(samples);
            self.write(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()));
        },
        docs[2]
    );
}
