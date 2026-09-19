#include "Audio/bind_SoundFileReader.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_SoundFileReader(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundFileReader = sf.new_usertype<sf::SoundFileReader>("SoundFileReader", sol::no_constructor);
    sol::table table_sf__SoundFileReader = sf["SoundFileReader"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::SoundFileReader>(lua);
    LUASF_STUB_DOC("\\brief Abstract base class for sound file decoding");
    LUASF_STUB_CLASS("sf.SoundFileReader");
    // sf::SoundFileReader is abstract; constructor binding is omitted.
    LUASF_STUB_DOC("\\brief Open a sound file for reading\n\nThe provided stream reference is valid as long as the\n`SoundFileReader` is alive, so it is safe to use/store it\nduring the whole lifetime of the reader.\n\n\\param stream Source stream to read from\n\n\\return Properties of the loaded sound if the file was successfully opened, `std::nullopt` otherwise");
    LUASF_STUB_FUNCTION("sf.SoundFileReader", "open", "fun(self: sf.SoundFileReader, stream: sf.InputStream): sf.SoundFileReader.Info|nil");
    type_sf__SoundFileReader.set_function("open",
        [lua](sf::SoundFileReader& self, sf::InputStream& stream) -> sol::object {
            return lua_sf::optional_to_object(lua, self.open(stream));
        }
    );
    LUASF_STUB_DOC("\\brief Change the current read position to the given sample offset\n\nThe sample offset takes the channels into account.\nIf you have a time offset instead, you can easily find\nthe corresponding sample offset with the following formula:\n`timeInSeconds * sampleRate * channelCount`\nIf the given offset exceeds to total number of samples,\nthis function must jump to the end of the file.\n\n\\param sampleOffset Index of the sample to jump to, relative to the beginning");
    LUASF_STUB_FUNCTION("sf.SoundFileReader", "seek", "fun(self: sf.SoundFileReader, sampleOffset: integer)");
    type_sf__SoundFileReader.set_function("seek",
        [](sf::SoundFileReader& self, lua_sf::LuaIntegral<std::uint64_t> sampleOffset) {
            self.seek(sampleOffset.value());
        }
    );
    LUASF_STUB_DOC("\\brief Read audio samples from the open file\n\n\\param samples  Pointer to the sample array to fill\n\\param maxCount Maximum number of samples to read\n\n\\return Number of samples actually read (may be less than \\a maxCount)");
    LUASF_STUB_FUNCTION("sf.SoundFileReader", "read", "fun(self: sf.SoundFileReader, maxCount: integer): integer, any");
    type_sf__SoundFileReader.set_function("read",
        [](sf::SoundFileReader& self, std::size_t maxCount) {
            std::vector<std::int16_t> samples_buffer(maxCount);
            auto result = self.read(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()));
            const auto samples_buffer_written = static_cast<std::size_t>(result);
            if (samples_buffer_written < samples_buffer.size())
                samples_buffer.resize(samples_buffer_written);
            return std::make_tuple(result, sol::as_table(samples_buffer));
        }
    );
    auto type_sf__SoundFileReader__Info = table_sf__SoundFileReader.new_usertype<sf::SoundFileReader::Info>("Info", sol::no_constructor);
    sol::table table_sf__SoundFileReader__Info = table_sf__SoundFileReader["Info"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::SoundFileReader::Info>(lua);
    LUASF_STUB_DOC("\\brief Structure holding the audio properties of a sound file");
    LUASF_STUB_CLASS("sf.SoundFileReader.Info");
    LUASF_STUB_DOC("Total number of samples in the file");
    LUASF_STUB_FIELD("sampleCount", "integer");
    LUASF_STUB_DOC("Number of channels of the sound");
    LUASF_STUB_FIELD("channelCount", "integer");
    LUASF_STUB_DOC("Samples rate of the sound, in samples per second");
    LUASF_STUB_FIELD("sampleRate", "integer");
    LUASF_STUB_DOC("Map of position in sample frame to sound channel");
    LUASF_STUB_FIELD("channelMap", "sf.SoundChannel[]");
    LUASF_STUB_FUNCTION("sf.SoundFileReader.Info", "new", "fun(): sf.SoundFileReader.Info");
    type_sf__SoundFileReader__Info.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::SoundFileReader::Info>();
        }
    ));
    type_sf__SoundFileReader__Info.set("sampleCount", sol::property(
        [](sf::SoundFileReader::Info& self) {
            return self.sampleCount;
        },
        [](sf::SoundFileReader::Info& self, lua_sf::LuaIntegral<std::uint64_t> value) {
            self.sampleCount = value.value();
        }
    ));
    type_sf__SoundFileReader__Info.set("channelCount", sol::property(
        [](sf::SoundFileReader::Info& self) {
            return self.channelCount;
        },
        [](sf::SoundFileReader::Info& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.channelCount = value.value();
        }
    ));
    type_sf__SoundFileReader__Info.set("sampleRate", sol::property(
        [](sf::SoundFileReader::Info& self) {
            return self.sampleRate;
        },
        [](sf::SoundFileReader::Info& self, lua_sf::LuaIntegral<unsigned int> value) {
            self.sampleRate = value.value();
        }
    ));
    type_sf__SoundFileReader__Info.set("channelMap", sol::property(
        [lua](sf::SoundFileReader::Info& self) {
            return lua_sf::vector_to_object(lua, self.channelMap);
        },
        [](sf::SoundFileReader::Info& self, sol::table value) {
            auto value_vector = lua_sf::array_from_object<sf::SoundChannel>(value);
            self.channelMap = value_vector;
        }
    ));
}
