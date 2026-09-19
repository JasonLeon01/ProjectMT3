#include "Audio/bind_InputSoundFile.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_InputSoundFile(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__InputSoundFile = sf.new_usertype<sf::InputSoundFile>("InputSoundFile", sol::no_constructor);
    sol::table table_sf__InputSoundFile = sf["InputSoundFile"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::InputSoundFile>(lua);
    LUASF_STUB_DOC("\\brief Provide read access to sound files");
    LUASF_STUB_CLASS("sf.InputSoundFile");
    LUASF_STUB_DOC("\\brief Construct a sound file from the disk for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC, MP3.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param filename Path of the sound file to load\n\n\\throws sf::Exception if opening the file was unsuccessful");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "new", "fun(filename: string): sf.InputSoundFile");
    LUASF_STUB_OVERLOAD("sf.InputSoundFile", "new", "fun(stream: sf.InputStream): sf.InputSoundFile");
    LUASF_STUB_OVERLOAD("sf.InputSoundFile", "new", "fun(): sf.InputSoundFile");
    LUASF_STUB_OVERLOAD("sf.InputSoundFile", "new", "fun(data: any): sf.InputSoundFile");
    type_sf__InputSoundFile.set_function("new", sol::factories(
        [](std::string filename) {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::InputSoundFile>(std::filesystem::path(filename)));
        },
        [](sol::object stream) {
            auto& stream_ref = stream.as<sf::InputStream&>();
            auto object = lua_sf::makeLongLivedMemoryObject<sf::InputSoundFile>(stream_ref);
            lua_sf::rememberLongLivedStream(*object, std::move(stream));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        []() {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::InputSoundFile>());
        },
        [](sol::object data) {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            auto object = lua_sf::makeLongLivedMemoryObject<sf::InputSoundFile>();
            if (!object->openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size())))
                throw std::runtime_error("Failed to open sf.InputSoundFile from memory");
            lua_sf::rememberLongLivedMemory(*object, std::move(data_buffer));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        }
    ));
    LUASF_STUB_DOC("\\brief Open a sound file from the disk for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC, MP3.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param filename Path of the sound file to load\n\n\\return `true` if the file was successfully opened");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "openFromFile", "fun(self: sf.InputSoundFile, filename: string): boolean");
    type_sf__InputSoundFile.set_function("openFromFile",
        [](sf::InputSoundFile& self, std::string filename) -> bool {
            auto result = self.openFromFile(std::filesystem::path(filename));
            if (result)
                lua_sf::releaseLongLivedResources(self);
            return result;
        }
    );
    LUASF_STUB_DOC("\\brief Open a sound file in memory for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\return `true` if the file was successfully opened");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "openFromMemory", "fun(self: sf.InputSoundFile, data: any): boolean");
    type_sf__InputSoundFile.set_function("openFromMemory",
        [](sf::InputSoundFile& self, sol::object data) -> bool {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            const bool result = self.openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size()));
            if (result)
            {
                lua_sf::releaseLongLivedStream(self);
                lua_sf::rememberLongLivedMemory(self, std::move(data_buffer));
            }
            return result;
        }
    );
    LUASF_STUB_DOC("\\brief Open a sound file from a custom stream for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param stream Source stream to read from\n\n\\return `true` if the file was successfully opened");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "openFromStream", "fun(self: sf.InputSoundFile, stream: sf.InputStream): boolean");
    type_sf__InputSoundFile.set_function("openFromStream",
        [](sf::InputSoundFile& self, sol::object stream) -> bool {
            auto& stream_ref = stream.as<sf::InputStream&>();
            const bool result = self.openFromStream(stream_ref);
            if (result)
            {
                lua_sf::releaseLongLivedMemory(self);
                lua_sf::rememberLongLivedStream(self, std::move(stream));
            }
            return result;
        }
    );
    LUASF_STUB_DOC("\\brief Get the total number of audio samples in the file\n\n\\return Number of samples");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getSampleCount", "fun(self: sf.InputSoundFile): integer");
    type_sf__InputSoundFile.set_function("getSampleCount",
        [](sf::InputSoundFile& self) -> std::uint64_t {
            return self.getSampleCount();
        }
    );
    LUASF_STUB_DOC("\\brief Get the number of channels used by the sound\n\n\\return Number of channels (1 = mono, 2 = stereo)");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getChannelCount", "fun(self: sf.InputSoundFile): integer");
    type_sf__InputSoundFile.set_function("getChannelCount",
        [](sf::InputSoundFile& self) -> unsigned int {
            return self.getChannelCount();
        }
    );
    LUASF_STUB_DOC("\\brief Get the sample rate of the sound\n\n\\return Sample rate, in samples per second");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getSampleRate", "fun(self: sf.InputSoundFile): integer");
    type_sf__InputSoundFile.set_function("getSampleRate",
        [](sf::InputSoundFile& self) -> unsigned int {
            return self.getSampleRate();
        }
    );
    LUASF_STUB_DOC("\\brief Get the map of position in sample frame to sound channel\n\nThis is used to map a sample in the sample stream to a\nposition during spatialization.\n\n\\return Map of position in sample frame to sound channel\n\n\\see `getSampleRate`, `getChannelCount`, `getDuration`");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getChannelMap", "fun(self: sf.InputSoundFile): sf.SoundChannel[]");
    type_sf__InputSoundFile.set_function("getChannelMap",
        sol::policies(
            [lua](sf::InputSoundFile& self) -> sol::object {
                return lua_sf::vector_to_object(lua, self.getChannelMap());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Get the total duration of the sound file\n\nThis function is provided for convenience, the duration is\ndeduced from the other sound file attributes.\n\n\\return Duration of the sound file");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getDuration", "fun(self: sf.InputSoundFile): sf.Time");
    type_sf__InputSoundFile.set_function("getDuration",
        [](sf::InputSoundFile& self) -> sf::Time {
            return self.getDuration();
        }
    );
    LUASF_STUB_DOC("\\brief Get the read offset of the file in time\n\n\\return Time position");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getTimeOffset", "fun(self: sf.InputSoundFile): sf.Time");
    type_sf__InputSoundFile.set_function("getTimeOffset",
        [](sf::InputSoundFile& self) -> sf::Time {
            return self.getTimeOffset();
        }
    );
    LUASF_STUB_DOC("\\brief Get the read offset of the file in samples\n\n\\return Sample position");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getSampleOffset", "fun(self: sf.InputSoundFile): integer");
    type_sf__InputSoundFile.set_function("getSampleOffset",
        [](sf::InputSoundFile& self) -> std::uint64_t {
            return self.getSampleOffset();
        }
    );
    LUASF_STUB_DOC("\\brief Change the current read position to the given sample offset\n\nThis function takes a sample offset to provide maximum\nprecision. If you need to jump to a given time, use the\nother overload.\n\nThe sample offset takes the channels into account.\nIf you have a time offset instead, you can easily find\nthe corresponding sample offset with the following formula:\n`timeInSeconds * sampleRate * channelCount`\nIf the given offset exceeds to total number of samples,\nthis function jumps to the end of the sound file.\n\n\\param sampleOffset Index of the sample to jump to, relative to the beginning");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "seek", "fun(self: sf.InputSoundFile, sampleOffset: integer)");
    LUASF_STUB_OVERLOAD("sf.InputSoundFile", "seek", "fun(self: sf.InputSoundFile, timeOffset: sf.Time)");
    type_sf__InputSoundFile.set_function("seek",
        sol::overload(
            [](sf::InputSoundFile& self, lua_sf::LuaIntegral<std::uint64_t> sampleOffset) {
                self.seek(sampleOffset.value());
            },
            [](sf::InputSoundFile& self, sf::Time timeOffset) {
                self.seek(timeOffset);
            }
        )
    );
    LUASF_STUB_DOC("\\brief Read audio samples from the open file\n\n\\param samples  Pointer to the sample array to fill\n\\param maxCount Maximum number of samples to read\n\n\\return Number of samples actually read (may be less than \\a maxCount)");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "read", "fun(self: sf.InputSoundFile, maxCount: integer): integer, any");
    type_sf__InputSoundFile.set_function("read",
        [](sf::InputSoundFile& self, std::size_t maxCount) {
            std::vector<std::int16_t> samples_buffer(maxCount);
            auto result = self.read(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()));
            const auto samples_buffer_written = static_cast<std::size_t>(result);
            if (samples_buffer_written < samples_buffer.size())
                samples_buffer.resize(samples_buffer_written);
            return std::make_tuple(result, sol::as_table(samples_buffer));
        }
    );
    LUASF_STUB_DOC("\\brief Close the current file");
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "close", "fun(self: sf.InputSoundFile)");
    type_sf__InputSoundFile.set_function("close",
        [](sf::InputSoundFile& self) {
            self.close();
            lua_sf::releaseLongLivedResources(self);
        }
    );
}
