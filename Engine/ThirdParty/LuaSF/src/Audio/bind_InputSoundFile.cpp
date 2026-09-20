#include "Audio/bind_InputSoundFile.hpp"

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

namespace { constexpr std::array<std::string_view, 19> docs = {
    "\\brief Provide read access to sound files",
    "\\brief Default constructor\n\nConstruct an input sound file that is not associated\nwith a file to read.",
    "\\brief Construct a sound file from the disk for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC, MP3.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param filename Path of the sound file to load\n\n\\throws sf::Exception if opening the file was unsuccessful",
    "\\brief Construct a sound file in memory for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\throws sf::Exception if opening the file was unsuccessful",
    "\\brief Construct a sound file from a custom stream for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param stream Source stream to read from\n\n\\throws sf::Exception if opening the file was unsuccessful",
    "\\brief Open a sound file from the disk for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC, MP3.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param filename Path of the sound file to load\n\n\\return `true` if the file was successfully opened",
    "\\brief Open a sound file in memory for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\return `true` if the file was successfully opened",
    "\\brief Open a sound file from a custom stream for reading\n\nThe supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC.\nThe supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.\n\n\\param stream Source stream to read from\n\n\\return `true` if the file was successfully opened",
    "\\brief Get the total number of audio samples in the file\n\n\\return Number of samples",
    "\\brief Get the number of channels used by the sound\n\n\\return Number of channels (1 = mono, 2 = stereo)",
    "\\brief Get the sample rate of the sound\n\n\\return Sample rate, in samples per second",
    "\\brief Get the map of position in sample frame to sound channel\n\nThis is used to map a sample in the sample stream to a\nposition during spatialization.\n\n\\return Map of position in sample frame to sound channel\n\n\\see `getSampleRate`, `getChannelCount`, `getDuration`",
    "\\brief Get the total duration of the sound file\n\nThis function is provided for convenience, the duration is\ndeduced from the other sound file attributes.\n\n\\return Duration of the sound file",
    "\\brief Get the read offset of the file in time\n\n\\return Time position",
    "\\brief Get the read offset of the file in samples\n\n\\return Sample position",
    "\\brief Change the current read position to the given sample offset\n\nThis function takes a sample offset to provide maximum\nprecision. If you need to jump to a given time, use the\nother overload.\n\nThe sample offset takes the channels into account.\nIf you have a time offset instead, you can easily find\nthe corresponding sample offset with the following formula:\n`timeInSeconds * sampleRate * channelCount`\nIf the given offset exceeds to total number of samples,\nthis function jumps to the end of the sound file.\n\n\\param sampleOffset Index of the sample to jump to, relative to the beginning",
    "\\brief Change the current read position to the given time offset\n\nUsing a time offset is handy but imprecise. If you need an accurate\nresult, consider using the overload which takes a sample offset.\n\nIf the given time exceeds to total duration, this function jumps\nto the end of the sound file.\n\n\\param timeOffset Time to jump to, relative to the beginning",
    "\\brief Read audio samples from the open file\n\n\\param samples  Pointer to the sample array to fill\n\\param maxCount Maximum number of samples to read\n\n\\return Number of samples actually read (may be less than \\a maxCount)",
    "\\brief Close the current file",
}; }

void bind_InputSoundFile(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__InputSoundFile = lua_glue::BindClass<sf::InputSoundFile>(sf, "InputSoundFile");
    lua_glue::Table table_sf__InputSoundFile = sf["InputSoundFile"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::InputSoundFile>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.InputSoundFile");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "new", "fun(filename: string): sf.InputSoundFile");
    LUASF_STUB_OVERLOAD("sf.InputSoundFile", "new", "fun(stream: sf.InputStream): sf.InputSoundFile");
    LUASF_STUB_OVERLOAD("sf.InputSoundFile", "new", "fun(): sf.InputSoundFile");
    LUASF_STUB_OVERLOAD("sf.InputSoundFile", "new", "fun(data: any): sf.InputSoundFile");
    lua_glue::BindCallable(type_sf__InputSoundFile, "new",
        [](std::string filename) {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::InputSoundFile>(std::filesystem::path(filename)));
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__InputSoundFile, "new",
        [](lua_glue::Object stream) {
            auto& stream_ref = stream.as<sf::InputStream&>();
            auto object = lua_sf::makeLongLivedMemoryObject<sf::InputSoundFile>(stream_ref);
            lua_sf::rememberLongLivedStream(*object, std::move(stream));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__InputSoundFile, "new",
        []() {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::InputSoundFile>());
        },
        docs[1]
    );
    lua_glue::BindCallable(type_sf__InputSoundFile, "new",
        [](lua_glue::Object data) {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            auto object = lua_sf::makeLongLivedMemoryObject<sf::InputSoundFile>();
            if (!object->openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size())))
                throw std::runtime_error("Failed to open sf.InputSoundFile from memory");
            lua_sf::rememberLongLivedMemory(*object, std::move(data_buffer));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "openFromFile", "fun(self: sf.InputSoundFile, filename: string): boolean");
    lua_glue::BindCallable(type_sf__InputSoundFile, "openFromFile",
        [](sf::InputSoundFile& self, std::string filename) -> bool {
            auto result = self.openFromFile(std::filesystem::path(filename));
            if (result)
                lua_sf::releaseLongLivedResources(self);
            return result;
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "openFromMemory", "fun(self: sf.InputSoundFile, data: any): boolean");
    lua_glue::BindCallable(type_sf__InputSoundFile, "openFromMemory",
        [](sf::InputSoundFile& self, lua_glue::Object data) -> bool {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            const bool result = self.openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size()));
            if (result)
            {
                lua_sf::releaseLongLivedStream(self);
                lua_sf::rememberLongLivedMemory(self, std::move(data_buffer));
            }
            return result;
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "openFromStream", "fun(self: sf.InputSoundFile, stream: sf.InputStream): boolean");
    lua_glue::BindCallable(type_sf__InputSoundFile, "openFromStream",
        [](sf::InputSoundFile& self, lua_glue::Object stream) -> bool {
            auto& stream_ref = stream.as<sf::InputStream&>();
            const bool result = self.openFromStream(stream_ref);
            if (result)
            {
                lua_sf::releaseLongLivedMemory(self);
                lua_sf::rememberLongLivedStream(self, std::move(stream));
            }
            return result;
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getSampleCount", "fun(self: sf.InputSoundFile): integer");
    lua_glue::BindCallable(type_sf__InputSoundFile, "getSampleCount",
        [](const sf::InputSoundFile& self) -> std::uint64_t {
            return self.getSampleCount();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getChannelCount", "fun(self: sf.InputSoundFile): integer");
    lua_glue::BindCallable(type_sf__InputSoundFile, "getChannelCount",
        [](const sf::InputSoundFile& self) -> unsigned int {
            return self.getChannelCount();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getSampleRate", "fun(self: sf.InputSoundFile): integer");
    lua_glue::BindCallable(type_sf__InputSoundFile, "getSampleRate",
        [](const sf::InputSoundFile& self) -> unsigned int {
            return self.getSampleRate();
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getChannelMap", "fun(self: sf.InputSoundFile): sf.SoundChannel[]");
    lua_glue::BindCallable(type_sf__InputSoundFile, "getChannelMap",
        [lua](const sf::InputSoundFile& self) -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, self.getChannelMap());
        },
        docs[11],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getDuration", "fun(self: sf.InputSoundFile): sf.Time");
    lua_glue::BindCallable(type_sf__InputSoundFile, "getDuration",
        [](const sf::InputSoundFile& self) -> sf::Time {
            return self.getDuration();
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getTimeOffset", "fun(self: sf.InputSoundFile): sf.Time");
    lua_glue::BindCallable(type_sf__InputSoundFile, "getTimeOffset",
        [](const sf::InputSoundFile& self) -> sf::Time {
            return self.getTimeOffset();
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "getSampleOffset", "fun(self: sf.InputSoundFile): integer");
    lua_glue::BindCallable(type_sf__InputSoundFile, "getSampleOffset",
        [](const sf::InputSoundFile& self) -> std::uint64_t {
            return self.getSampleOffset();
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "seek", "fun(self: sf.InputSoundFile, sampleOffset: integer)");
    LUASF_STUB_OVERLOAD("sf.InputSoundFile", "seek", "fun(self: sf.InputSoundFile, timeOffset: sf.Time)");
    lua_glue::BindCallable(type_sf__InputSoundFile, "seek",
        [](sf::InputSoundFile& self, lua_sf::LuaIntegral<std::uint64_t> sampleOffset) {
            self.seek(sampleOffset.value());
        },
        docs[15]
    );
    lua_glue::BindCallable(type_sf__InputSoundFile, "seek",
        [](sf::InputSoundFile& self, sf::Time timeOffset) {
            self.seek(timeOffset);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "read", "fun(self: sf.InputSoundFile, maxCount: integer): integer, any");
    lua_glue::BindCallable(type_sf__InputSoundFile, "read",
        [](sf::InputSoundFile& self, std::size_t maxCount) {
            std::vector<std::int16_t> samples_buffer(maxCount);
            auto result = self.read(samples_buffer.data(), static_cast<std::uint64_t>(samples_buffer.size()));
            const auto samples_buffer_written = static_cast<std::size_t>(result);
            if (samples_buffer_written < samples_buffer.size())
                samples_buffer.resize(samples_buffer_written);
            return std::make_tuple(result, lua_glue::AsTable(samples_buffer));
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.InputSoundFile", "close", "fun(self: sf.InputSoundFile)");
    lua_glue::BindCallable(type_sf__InputSoundFile, "close",
        [](sf::InputSoundFile& self) {
            self.close();
            lua_sf::releaseLongLivedResources(self);
        },
        docs[18]
    );
}
