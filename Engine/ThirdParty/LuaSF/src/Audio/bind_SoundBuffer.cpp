#include "Audio/bind_SoundBuffer.hpp"

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

namespace { constexpr std::array<std::string_view, 17> docs = {
    "\\brief Storage for audio samples defining a sound",
    "\\brief Default constructor\n\nConstruct an empty sound buffer that does not contain\nany samples.",
    "\\brief Construct the sound buffer from a file\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param filename Path of the sound file to load\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromMemory`, `loadFromStream`, `loadFromSamples`, `saveToFile`",
    "\\brief Construct the sound buffer from a file in memory\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromStream`, `loadFromSamples`",
    "\\brief Construct the sound buffer from a custom stream\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param stream Source stream to read from\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromSamples`",
    "\\brief Construct the sound buffer from an array of audio samples\n\nThe assumed format of the audio samples is 16 bit signed integer.\n\n\\param samples      Pointer to the array of samples in memory\n\\param sampleCount  Number of samples in the array\n\\param channelCount Number of channels (1 = mono, 2 = stereo, ...)\n\\param sampleRate   Sample rate (number of samples to play per second)\n\\param channelMap   Map of position in sample frame to sound channel\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `loadFromFile`, `loadFromMemory`, `saveToFile`",
    "\\brief Load the sound buffer from a file\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param filename Path of the sound file to load\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromMemory`, `loadFromStream`, `loadFromSamples`, `saveToFile`",
    "\\brief Load the sound buffer from a file in memory\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromStream`, `loadFromSamples`",
    "\\brief Load the sound buffer from a custom stream\n\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\param stream Source stream to read from\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`, `loadFromSamples`",
    "\\brief Load the sound buffer from an array of audio samples\n\nThe assumed format of the audio samples is 16 bit signed integer.\n\n\\param samples      Pointer to the array of samples in memory\n\\param sampleCount  Number of samples in the array\n\\param channelCount Number of channels (1 = mono, 2 = stereo, ...)\n\\param sampleRate   Sample rate (number of samples to play per second)\n\\param channelMap   Map of position in sample frame to sound channel\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `loadFromFile`, `loadFromMemory`, `saveToFile`",
    "\\brief Save the sound buffer to an audio file\n\nSee the documentation of `sf::OutputSoundFile` for the list\nof supported formats.\n\n\\param filename Path of the sound file to write\n\n\\return `true` if saving succeeded, `false` if it failed",
    "\\brief Get the array of audio samples stored in the buffer\n\nThe format of the returned samples is 16 bit signed integer.\nThe total number of samples in this array is given by the\n`getSampleCount()` function.\n\n\\return Read-only pointer to the array of sound samples\n\n\\see `getSampleCount`",
    "\\brief Get the number of samples stored in the buffer\n\nThe array of samples can be accessed with the `getSamples()`\nfunction.\n\n\\return Number of samples\n\n\\see `getSamples`",
    "\\brief Get the sample rate of the sound\n\nThe sample rate is the number of samples played per second.\nThe higher, the better the quality (for example, 44100\nsamples/s is CD quality).\n\n\\return Sample rate (number of samples per second)\n\n\\see `getChannelCount`, `getChannelMap`, `getDuration`",
    "\\brief Get the number of channels used by the sound\n\nIf the sound is mono then the number of channels will\nbe 1, 2 for stereo, etc.\n\n\\return Number of channels\n\n\\see `getSampleRate`, `getChannelMap`, `getDuration`",
    "\\brief Get the map of position in sample frame to sound channel\n\nThis is used to map a sample in the sample stream to a\nposition during spatialization.\n\n\\return Map of position in sample frame to sound channel\n\n\\see `getSampleRate`, `getChannelCount`, `getDuration`",
    "\\brief Get the total duration of the sound\n\n\\return Sound duration\n\n\\see `getSampleRate`, `getChannelCount`, `getChannelMap`",
}; }

void bind_SoundBuffer(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundBuffer = lua_glue::BindClass<sf::SoundBuffer>(sf, "SoundBuffer");
    lua_glue::Table table_sf__SoundBuffer = sf["SoundBuffer"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::SoundBuffer>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.SoundBuffer");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "new", "fun(filename: string): sf.SoundBuffer");
    LUASF_STUB_OVERLOAD("sf.SoundBuffer", "new", "fun(stream: sf.InputStream): sf.SoundBuffer");
    LUASF_STUB_OVERLOAD("sf.SoundBuffer", "new", "fun(): sf.SoundBuffer");
    LUASF_STUB_OVERLOAD("sf.SoundBuffer", "new", "fun(data: any): sf.SoundBuffer");
    LUASF_STUB_OVERLOAD("sf.SoundBuffer", "new", "fun(samples: any, channelCount: integer, sampleRate: integer, channelMap: sf.SoundChannel[]): sf.SoundBuffer");
    lua_glue::BindCallable(type_sf__SoundBuffer, "new",
        [](std::string filename) {
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>(std::filesystem::path(filename));
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__SoundBuffer, "new",
        [](sf::InputStream& stream) {
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>(stream);
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__SoundBuffer, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>();
        },
        docs[1]
    );
    lua_glue::BindCallable(type_sf__SoundBuffer, "new",
        [](lua_glue::Object data) {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
        },
        docs[3]
    );
    lua_glue::BindCallable(type_sf__SoundBuffer, "new",
        [](lua_glue::Object samples, lua_sf::LuaIntegral<unsigned int> channelCount, lua_sf::LuaIntegral<unsigned int> sampleRate, lua_glue::Table channelMap) {
            auto samples_buffer = lua_sf::array_from_object<std::int16_t>(samples);
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return lua_sf::makeLuaSharedObject<sf::SoundBuffer>(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()), channelCount.value(), sampleRate.value(), channelMap_vector);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "loadFromFile", "fun(self: sf.SoundBuffer, filename: string): boolean");
    lua_glue::BindCallable(type_sf__SoundBuffer, "loadFromFile",
        [](sf::SoundBuffer& self, std::string filename) -> bool {
            return self.loadFromFile(std::filesystem::path(filename));
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "loadFromMemory", "fun(self: sf.SoundBuffer, data: any): boolean");
    lua_glue::BindCallable(type_sf__SoundBuffer, "loadFromMemory",
        [](sf::SoundBuffer& self, lua_glue::Object data) -> bool {
            auto data_buffer = lua_sf::array_from_object<std::byte>(data);
            return self.loadFromMemory(data_buffer.data(), static_cast<std::size_t>(data_buffer.size()));
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "loadFromStream", "fun(self: sf.SoundBuffer, stream: sf.InputStream): boolean");
    lua_glue::BindCallable(type_sf__SoundBuffer, "loadFromStream",
        [](sf::SoundBuffer& self, sf::InputStream& stream) -> bool {
            return self.loadFromStream(stream);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "loadFromSamples", "fun(self: sf.SoundBuffer, samples: any, channelCount: integer, sampleRate: integer, channelMap: sf.SoundChannel[]): boolean");
    lua_glue::BindCallable(type_sf__SoundBuffer, "loadFromSamples",
        [](sf::SoundBuffer& self, lua_glue::Object samples, lua_sf::LuaIntegral<unsigned int> channelCount, lua_sf::LuaIntegral<unsigned int> sampleRate, lua_glue::Table channelMap) -> bool {
            auto samples_buffer = lua_sf::array_from_object<std::int16_t>(samples);
            auto channelMap_vector = lua_sf::array_from_object<sf::SoundChannel>(channelMap);
            return self.loadFromSamples(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()), channelCount.value(), sampleRate.value(), channelMap_vector);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "saveToFile", "fun(self: sf.SoundBuffer, filename: string): boolean");
    lua_glue::BindCallable(type_sf__SoundBuffer, "saveToFile",
        [](const sf::SoundBuffer& self, std::string filename) -> bool {
            return self.saveToFile(std::filesystem::path(filename));
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getSamples", "fun(self: sf.SoundBuffer): integer[]");
    lua_glue::BindCallable(type_sf__SoundBuffer, "getSamples",
        [](const sf::SoundBuffer& self) {
            const auto* result = self.getSamples();
            if (!result)
                return lua_glue::AsTable(std::vector<std::int16_t>{});
            std::vector<std::int16_t> result_values(result, result + static_cast<std::size_t>(self.getSampleCount()));
            return lua_glue::AsTable(std::move(result_values));
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getSampleCount", "fun(self: sf.SoundBuffer): integer");
    lua_glue::BindCallable(type_sf__SoundBuffer, "getSampleCount",
        [](const sf::SoundBuffer& self) -> std::uint64_t {
            return self.getSampleCount();
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getSampleRate", "fun(self: sf.SoundBuffer): integer");
    lua_glue::BindCallable(type_sf__SoundBuffer, "getSampleRate",
        [](const sf::SoundBuffer& self) -> unsigned int {
            return self.getSampleRate();
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getChannelCount", "fun(self: sf.SoundBuffer): integer");
    lua_glue::BindCallable(type_sf__SoundBuffer, "getChannelCount",
        [](const sf::SoundBuffer& self) -> unsigned int {
            return self.getChannelCount();
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getChannelMap", "fun(self: sf.SoundBuffer): sf.SoundChannel[]");
    lua_glue::BindCallable(type_sf__SoundBuffer, "getChannelMap",
        [lua](const sf::SoundBuffer& self) -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, self.getChannelMap());
        },
        docs[15],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.SoundBuffer", "getDuration", "fun(self: sf.SoundBuffer): sf.Time");
    lua_glue::BindCallable(type_sf__SoundBuffer, "getDuration",
        [](const sf::SoundBuffer& self) -> sf::Time {
            return self.getDuration();
        },
        docs[16]
    );
}
