#include "Audio/bind_SoundBufferRecorder.hpp"

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

namespace { constexpr std::array<std::string_view, 13> docs = {
    "\\brief Specialized SoundRecorder which stores the captured\naudio data into a sound buffer",
    "\\brief Start the capture\n\nThe `sampleRate` parameter defines the number of audio samples\ncaptured per second. The higher, the better the quality\n(for example, 44100 samples/sec is CD quality).\nThis function uses its own thread so that it doesn't block\nthe rest of the program while the capture runs.\nPlease note that only one capture can happen at the same time.\nYou can select which capture device will be used by passing\nthe name to the `setDevice()` method. If none was selected\nbefore, the default capture device will be used. You can get a\nlist of the names of all available capture devices by calling\n`getAvailableDevices()`.\n\n\\param sampleRate Desired capture rate, in number of samples per second\n\n\\return `true`, if start of capture was successful\n\n\\see `stop`, `getAvailableDevices`",
    "\\brief Stop the capture\n\n\\see `start`",
    "\\brief Get the sample rate\n\nThe sample rate defines the number of audio samples\ncaptured per second. The higher, the better the quality\n(for example, 44100 samples/sec is CD quality).\n\n\\return Sample rate, in samples per second",
    "\\brief Get a list of the names of all available audio capture devices\n\nThis function returns a vector of strings, containing\nthe names of all available audio capture devices.\n\n\\return A vector of strings containing the names",
    "\\brief Get the name of the default audio capture device\n\nThis function returns the name of the default audio\ncapture device. If none is available, an empty string\nis returned.\n\n\\return The name of the default audio capture device",
    "\\brief Set the audio capture device\n\nThis function sets the audio capture device to the device\nwith the given `name`. It can be called on the fly (i.e:\nwhile recording). If you do so while recording and\nopening the device fails, it stops the recording.\n\n\\param name The name of the audio capture device\n\n\\return `true`, if it was able to set the requested device\n\n\\see `getAvailableDevices`, `getDefaultDevice`",
    "\\brief Get the name of the current audio capture device\n\n\\return The name of the current audio capture device",
    "\\brief Set the channel count of the audio capture device\n\nThis method allows you to specify the number of channels\nused for recording. Currently only 16-bit mono and\n16-bit stereo are supported.\n\n\\param channelCount Number of channels. Currently only\nmono (1) and stereo (2) are supported.\n\n\\see `getChannelCount`",
    "\\brief Get the number of channels used by this recorder\n\nCurrently only mono and stereo are supported, so the\nvalue is either 1 (for mono) or 2 (for stereo).\n\n\\return Number of channels\n\n\\see `setChannelCount`",
    "\\brief Get the map of position in sample frame to sound channel\n\nThis is used to map a sample in the sample stream to a\nposition during spatialization.\n\n\\return Map of position in sample frame to sound channel",
    "\\brief Check if the system supports audio capture\n\nThis function should always be called before using\nthe audio capture features. If it returns `false`, then\nany attempt to use `sf::SoundRecorder` or one of its derived\nclasses will fail.\n\n\\return `true` if audio capture is supported, `false` otherwise",
    "\\brief Get the sound buffer containing the captured audio data\n\nThe sound buffer is valid only after the capture has ended.\nThis function provides a read-only access to the internal\nsound buffer, but it can be copied if you need to\nmake any modification to it.\n\n\\return Read-only access to the sound buffer",
}; }

void bind_SoundBufferRecorder(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundBufferRecorder = lua_glue::BindClass<sf::SoundBufferRecorder>(sf, "SoundBufferRecorder");
    lua_glue::BindBase<sf::SoundBufferRecorder, sf::SoundRecorder>(type_sf__SoundBufferRecorder);
    lua_glue::Table table_sf__SoundBufferRecorder = sf["SoundBufferRecorder"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::SoundBufferRecorder>(lua);
    lua_glue::Table native_bases_sf__SoundBufferRecorder = lua.create_table();
    native_bases_sf__SoundBufferRecorder.add(lua["sf"]["SoundRecorder"].get<lua_glue::Table>());
    table_sf__SoundBufferRecorder.raw_set("__nativeBases", native_bases_sf__SoundBufferRecorder);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.SoundBufferRecorder", "sf.SoundRecorder");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "new", "fun(): sf.SoundBufferRecorder");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::SoundBufferRecorder>();
        }
    );
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "start", "fun(self: sf.SoundBufferRecorder, sampleRate?: integer): boolean");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "start",
        [](sf::SoundBufferRecorder& self, lua_sf::LuaIntegral<unsigned int> sampleRate) -> bool {
            return static_cast<sf::SoundRecorder&>(self).start(sampleRate.value());
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned int>(44100);
        }}},
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "stop", "fun(self: sf.SoundBufferRecorder)");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "stop",
        [](sf::SoundBufferRecorder& self) {
            static_cast<sf::SoundRecorder&>(self).stop();
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getSampleRate", "fun(self: sf.SoundBufferRecorder): integer");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "getSampleRate",
        [](const sf::SoundBufferRecorder& self) -> unsigned int {
            return static_cast<const sf::SoundRecorder&>(self).getSampleRate();
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getAvailableDevices", "fun(): string[]");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "getAvailableDevices",
        [lua]() -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, sf::SoundRecorder::getAvailableDevices());
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getDefaultDevice", "fun(): string");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "getDefaultDevice",
        []() -> std::string {
            return std::string(sf::SoundRecorder::getDefaultDevice());
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "setDevice", "fun(self: sf.SoundBufferRecorder, name: string): boolean");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "setDevice",
        [](sf::SoundBufferRecorder& self, std::string name) -> bool {
            return static_cast<sf::SoundRecorder&>(self).setDevice(name);
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getDevice", "fun(self: sf.SoundBufferRecorder): string");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "getDevice",
        [](const sf::SoundBufferRecorder& self) -> std::string {
            return std::string(static_cast<const sf::SoundRecorder&>(self).getDevice());
        },
        docs[7],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "setChannelCount", "fun(self: sf.SoundBufferRecorder, channelCount: integer)");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "setChannelCount",
        [](sf::SoundBufferRecorder& self, lua_sf::LuaIntegral<unsigned int> channelCount) {
            static_cast<sf::SoundRecorder&>(self).setChannelCount(channelCount.value());
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getChannelCount", "fun(self: sf.SoundBufferRecorder): integer");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "getChannelCount",
        [](const sf::SoundBufferRecorder& self) -> unsigned int {
            return static_cast<const sf::SoundRecorder&>(self).getChannelCount();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getChannelMap", "fun(self: sf.SoundBufferRecorder): sf.SoundChannel[]");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "getChannelMap",
        [lua](const sf::SoundBufferRecorder& self) -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, static_cast<const sf::SoundRecorder&>(self).getChannelMap());
        },
        docs[10],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "isAvailable", "fun(): boolean");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "isAvailable",
        []() -> bool {
            return sf::SoundRecorder::isAvailable();
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getBuffer", "fun(self: sf.SoundBufferRecorder): sf.SoundBuffer");
    lua_glue::BindCallable(type_sf__SoundBufferRecorder, "getBuffer",
        [](const sf::SoundBufferRecorder& self) {
            return std::cref(self.getBuffer());
        },
        docs[12],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
}
