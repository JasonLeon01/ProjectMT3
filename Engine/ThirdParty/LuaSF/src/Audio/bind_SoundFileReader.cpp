#include "Audio/bind_SoundFileReader.hpp"

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

namespace { constexpr std::array<std::string_view, 9> docs = {
    "\\brief Abstract base class for sound file decoding",
    "\\brief Open a sound file for reading\n\nThe provided stream reference is valid as long as the\n`SoundFileReader` is alive, so it is safe to use/store it\nduring the whole lifetime of the reader.\n\n\\param stream Source stream to read from\n\n\\return Properties of the loaded sound if the file was successfully opened, `std::nullopt` otherwise",
    "\\brief Change the current read position to the given sample offset\n\nThe sample offset takes the channels into account.\nIf you have a time offset instead, you can easily find\nthe corresponding sample offset with the following formula:\n`timeInSeconds * sampleRate * channelCount`\nIf the given offset exceeds to total number of samples,\nthis function must jump to the end of the file.\n\n\\param sampleOffset Index of the sample to jump to, relative to the beginning",
    "\\brief Read audio samples from the open file\n\n\\param samples  Pointer to the sample array to fill\n\\param maxCount Maximum number of samples to read\n\n\\return Number of samples actually read (may be less than \\a maxCount)",
    "\\brief Structure holding the audio properties of a sound file",
    "Total number of samples in the file",
    "Number of channels of the sound",
    "Samples rate of the sound, in samples per second",
    "Map of position in sample frame to sound channel",
}; }

void bind_SoundFileReader(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundFileReader = lua_glue::BindClass<sf::SoundFileReader>(sf, "SoundFileReader");
    lua_glue::Table table_sf__SoundFileReader = sf["SoundFileReader"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::SoundFileReader>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.SoundFileReader");
    // sf::SoundFileReader is abstract; constructor binding is omitted.
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.SoundFileReader", "open", "fun(self: sf.SoundFileReader, stream: sf.InputStream): sf.SoundFileReader.Info|nil");
    lua_glue::BindCallable(type_sf__SoundFileReader, "open",
        [lua](sf::SoundFileReader& self, sf::InputStream& stream) -> lua_glue::Object {
            return lua_sf::optional_to_object(lua, self.open(stream));
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.SoundFileReader", "seek", "fun(self: sf.SoundFileReader, sampleOffset: integer)");
    lua_glue::BindCallable(type_sf__SoundFileReader, "seek",
        [](sf::SoundFileReader& self, lua_sf::LuaIntegral<std::uint64_t> sampleOffset) {
            self.seek(sampleOffset.value());
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.SoundFileReader", "read", "fun(self: sf.SoundFileReader, maxCount: integer): integer, any");
    lua_glue::BindCallable(type_sf__SoundFileReader, "read",
        [](sf::SoundFileReader& self, std::size_t maxCount) {
            std::vector<std::int16_t> samples_buffer(maxCount);
            auto result = self.read(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()));
            const auto samples_buffer_written = static_cast<std::size_t>(result);
            if (samples_buffer_written < samples_buffer.size())
                samples_buffer.resize(samples_buffer_written);
            return std::make_tuple(result, lua_glue::AsTable(samples_buffer));
        },
        docs[3]
    );
    auto type_sf__SoundFileReader__Info = lua_glue::BindClass<sf::SoundFileReader::Info>(table_sf__SoundFileReader, "Info");
    lua_glue::Table table_sf__SoundFileReader__Info = table_sf__SoundFileReader["Info"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::SoundFileReader::Info>(lua);
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_CLASS("sf.SoundFileReader.Info");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("sampleCount", "integer");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("channelCount", "integer");
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FIELD("sampleRate", "integer");
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FIELD("channelMap", "sf.SoundChannel[]");
    LUASF_STUB_FUNCTION("sf.SoundFileReader.Info", "new", "fun(): sf.SoundFileReader.Info");
    lua_glue::BindCallable(type_sf__SoundFileReader__Info, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::SoundFileReader::Info>();
        }
    );
    lua_glue::BindProperty(type_sf__SoundFileReader__Info, "sampleCount",
        [](const sf::SoundFileReader::Info& self) {
            return self.sampleCount;
        },
        [](sf::SoundFileReader::Info& self, lua_sf::LuaIntegral<std::uint64_t> value) {
            self.sampleCount = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__SoundFileReader__Info, "channelCount",
        [](const sf::SoundFileReader::Info& self) {
            return self.channelCount;
        },
        [](sf::SoundFileReader::Info& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.channelCount = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__SoundFileReader__Info, "sampleRate",
        [](const sf::SoundFileReader::Info& self) {
            return self.sampleRate;
        },
        [](sf::SoundFileReader::Info& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.sampleRate = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__SoundFileReader__Info, "channelMap",
        [lua](const sf::SoundFileReader::Info& self) {
            return lua_sf::vector_to_object(lua, self.channelMap);
        },
        [](sf::SoundFileReader::Info& self, lua_glue::Table value) {
            auto value_vector = lua_sf::array_from_object<sf::SoundChannel>(value);
            self.channelMap = value_vector;
        }
    );
}
