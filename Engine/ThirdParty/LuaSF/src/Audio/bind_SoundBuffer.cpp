#include "Audio/bind_SoundBuffer.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_SoundBuffer(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundBuffer = sf.new_usertype<sf::SoundBuffer>("SoundBuffer", sol::no_constructor);
    sol::table table_sf__SoundBuffer = sf["SoundBuffer"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::SoundBuffer>(lua);
    LUASF_STUB_DOC("\\brief Storage for audio samples defining a sound");
    LUASF_STUB_CLASS("sf.SoundBuffer");
    LUASF_STUB_DOC("\\brief Construct the sound buffer from a file\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param filename Path of the sound file to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromMemory`, `loadFromStream`, `loadFromSamples`, `saveToFile`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "new", "fun(filename: string): sf.SoundBuffer");
    LUASF_STUB_OVERLOAD("sf.SoundBuffer", "new", "fun(stream: sf.InputStream): sf.SoundBuffer");
    LUASF_STUB_OVERLOAD("sf.SoundBuffer", "new", "fun(): sf.SoundBuffer");
    LUASF_STUB_OVERLOAD("sf.SoundBuffer", "new", "fun(data: any): sf.SoundBuffer");
    LUASF_STUB_OVERLOAD("sf.SoundBuffer", "new", "fun(samples: any, channelCount: integer, sampleRate: integer, channelMap: sf.SoundChannel[]): sf.SoundBuffer");
    type_sf__SoundBuffer.set_function("new", sol::factories(
        [](std::string filename) {
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>(std::filesystem::path(filename));
        },
        [](sf::InputStream& stream) {
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>(stream);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>();
        },
        [](sol::object data) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
        },
        [](sol::object samples, lua_sf::LuaIntegral<unsigned int> channelCount, lua_sf::LuaIntegral<unsigned int> sampleRate, sol::table channelMap) {
            auto samples_buffer = lua_sf::array_from_object<std::int16_t>(samples);
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()), channelCount.value(), sampleRate.value(), channelMap_vector);
        }
    ));
    LUASF_STUB_DOC("\\brief Load the sound buffer from a file\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param filename Path of the sound file to load\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromMemory`, `loadFromStream`, `loadFromSamples`, `saveToFile`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "loadFromFile", "fun(self: sf.SoundBuffer, filename: string): boolean");
    type_sf__SoundBuffer.set_function("loadFromFile",
        [](sf::SoundBuffer& self, std::string filename) -> bool {
            return self.loadFromFile(std::filesystem::path(filename));
        }
    );
    LUASF_STUB_DOC("\\brief Load the sound buffer from a file in memory\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromStream`, `loadFromSamples`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "loadFromMemory", "fun(self: sf.SoundBuffer, data: any): boolean");
    type_sf__SoundBuffer.set_function("loadFromMemory",
        [](sf::SoundBuffer& self, sol::object data) -> bool {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return self.loadFromMemory(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
        }
    );
    LUASF_STUB_DOC("\\brief Load the sound buffer from a custom stream\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param stream Source stream to read from\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromSamples`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "loadFromStream", "fun(self: sf.SoundBuffer, stream: sf.InputStream): boolean");
    type_sf__SoundBuffer.set_function("loadFromStream",
        [](sf::SoundBuffer& self, sf::InputStream& stream) -> bool {
            return self.loadFromStream(stream);
        }
    );
    LUASF_STUB_DOC("\\brief Load the sound buffer from an array of audio samples\n\nThe assumed format of the audio samples is 16 bit signed integer.\n\n\\param samples      Pointer to the array of samples in memory\n\\param sampleCount  Number of samples in the array\n\\param channelCount Number of channels (1 = mono, 2 = stereo, ...)\n\\param sampleRate   Sample rate (number of samples to play per second)\n\\param channelMap   Map of position in sample frame to sound channel\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`, `saveToFile`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "loadFromSamples", "fun(self: sf.SoundBuffer, samples: any, channelCount: integer, sampleRate: integer, channelMap: sf.SoundChannel[]): boolean");
    type_sf__SoundBuffer.set_function("loadFromSamples",
        [](sf::SoundBuffer& self, sol::object samples, lua_sf::LuaIntegral<unsigned int> channelCount, lua_sf::LuaIntegral<unsigned int> sampleRate, sol::table channelMap) -> bool {
            auto samples_buffer = lua_sf::array_from_object<std::int16_t>(samples);
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return self.loadFromSamples(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()), channelCount.value(), sampleRate.value(), channelMap_vector);
        }
    );
    LUASF_STUB_DOC("\\brief Save the sound buffer to an audio file\n\nSee the documentation of `sf::OutputSoundFile` for the list\nof supported formats.\n\n\\param filename Path of the sound file to write\n\n\\return `true` if saving succeeded, `false` if it failed");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "saveToFile", "fun(self: sf.SoundBuffer, filename: string): boolean");
    type_sf__SoundBuffer.set_function("saveToFile",
        [](sf::SoundBuffer& self, std::string filename) -> bool {
            return self.saveToFile(std::filesystem::path(filename));
        }
    );
    LUASF_STUB_DOC("\\brief Get the array of audio samples stored in the buffer\n\nThe format of the returned samples is 16 bit signed integer.\nThe total number of samples in this array is given by the\n`getSampleCount()` function.\n\n\\return Read-only pointer to the array of sound samples\n\n\\see `getSampleCount`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getSamples", "fun(self: sf.SoundBuffer): integer[]");
    type_sf__SoundBuffer.set_function("getSamples",
        [](sf::SoundBuffer& self) {
            const auto* result = self.getSamples();
            if (!result)
                return sol::as_table(std::vector<std::int16_t>{});
            std::vector<std::int16_t> result_values(result, result + static_cast<std::size_t>(self.getSampleCount()));
            return sol::as_table(std::move(result_values));
        }
    );
    LUASF_STUB_DOC("\\brief Get the number of samples stored in the buffer\n\nThe array of samples can be accessed with the `getSamples()`\nfunction.\n\n\\return Number of samples\n\n\\see `getSamples`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getSampleCount", "fun(self: sf.SoundBuffer): integer");
    type_sf__SoundBuffer.set_function("getSampleCount",
        [](sf::SoundBuffer& self) -> std::uint64_t {
            return self.getSampleCount();
        }
    );
    LUASF_STUB_DOC("\\brief Get the sample rate of the sound\n\nThe sample rate is the number of samples played per second.\nThe higher, the better the quality (for example, 44100\nsamples/s is CD quality).\n\n\\return Sample rate (number of samples per second)\n\n\\see `getChannelCount`, `getChannelMap`, `getDuration`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getSampleRate", "fun(self: sf.SoundBuffer): integer");
    type_sf__SoundBuffer.set_function("getSampleRate",
        [](sf::SoundBuffer& self) -> unsigned int {
            return self.getSampleRate();
        }
    );
    LUASF_STUB_DOC("\\brief Get the number of channels used by the sound\n\nIf the sound is mono then the number of channels will\nbe 1, 2 for stereo, etc.\n\n\\return Number of channels\n\n\\see `getSampleRate`, `getChannelMap`, `getDuration`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getChannelCount", "fun(self: sf.SoundBuffer): integer");
    type_sf__SoundBuffer.set_function("getChannelCount",
        [](sf::SoundBuffer& self) -> unsigned int {
            return self.getChannelCount();
        }
    );
    LUASF_STUB_DOC("\\brief Get the map of position in sample frame to sound channel\n\nThis is used to map a sample in the sample stream to a\nposition during spatialization.\n\n\\return Map of position in sample frame to sound channel\n\n\\see `getSampleRate`, `getChannelCount`, `getDuration`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getChannelMap", "fun(self: sf.SoundBuffer): sf.SoundChannel[]");
    type_sf__SoundBuffer.set_function("getChannelMap",
        sol::policies(
            [lua](sf::SoundBuffer& self) -> sol::object {
                return lua_sf::vector_to_object(lua, self.getChannelMap());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the total duration of the sound\n\n\\return Sound duration\n\n\\see `getSampleRate`, `getChannelCount`, `getChannelMap`");
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getDuration", "fun(self: sf.SoundBuffer): sf.Time");
    type_sf__SoundBuffer.set_function("getDuration",
        [](sf::SoundBuffer& self) -> sf::Time {
            return self.getDuration();
        }
    );
}
