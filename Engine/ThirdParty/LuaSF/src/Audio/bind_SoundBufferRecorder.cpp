#include "Audio/bind_SoundBufferRecorder.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_SoundBufferRecorder(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundBufferRecorder = sf.new_usertype<sf::SoundBufferRecorder>("SoundBufferRecorder",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::SoundRecorder>()
    );
    sol::table table_sf__SoundBufferRecorder = sf["SoundBufferRecorder"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::SoundBufferRecorder>(lua);
    sol::table native_bases_sf__SoundBufferRecorder = lua.create_table();
    native_bases_sf__SoundBufferRecorder.add(lua["sf"]["SoundRecorder"].get<sol::table>());
    table_sf__SoundBufferRecorder.raw_set("__nativeBases", native_bases_sf__SoundBufferRecorder);
    LUASF_STUB_DOC("\\brief Specialized SoundRecorder which stores the captured\naudio data into a sound buffer");
    LUASF_STUB_CLASS("sf.SoundBufferRecorder", "sf.SoundRecorder");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "new", "fun(): sf.SoundBufferRecorder");
    type_sf__SoundBufferRecorder.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::SoundBufferRecorder>();
        }
    ));
    LUASF_STUB_DOC("\\brief Start the capture\n\nThe `sampleRate` parameter defines the number of audio samples\ncaptured per second. The higher, the better the quality\n(for example, 44100 samples/sec is CD quality).\nThis function uses its own thread so that it doesn't block\nthe rest of the program while the capture runs.\nPlease note that only one capture can happen at the same time.\nYou can select which capture device will be used by passing\nthe name to the `setDevice()` method. If none was selected\nbefore, the default capture device will be used. You can get a\nlist of the names of all available capture devices by calling\n`getAvailableDevices()`.\n\n\\param sampleRate Desired capture rate, in number of samples per second\n\n\\return `true`, if start of capture was successful\n\n\\see `stop`, `getAvailableDevices`");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "start", "fun(self: sf.SoundBufferRecorder, sampleRate: integer): boolean");
    LUASF_STUB_OVERLOAD("sf.SoundBufferRecorder", "start", "fun(self: sf.SoundBufferRecorder): boolean");
    type_sf__SoundBufferRecorder.set_function("start",
        sol::overload(
            [](sf::SoundBufferRecorder& self, lua_sf::LuaIntegral<unsigned int> sampleRate) -> bool {
                return static_cast<sf::SoundRecorder&>(self).start(sampleRate.value());
            },
            [](sf::SoundBufferRecorder& self) -> bool {
                return static_cast<sf::SoundRecorder&>(self).start();
            }
        )
    );
    LUASF_STUB_DOC("\\brief Stop the capture\n\n\\see `start`");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "stop", "fun(self: sf.SoundBufferRecorder)");
    type_sf__SoundBufferRecorder.set_function("stop",
        [](sf::SoundBufferRecorder& self) {
            static_cast<sf::SoundRecorder&>(self).stop();
        }
    );
    LUASF_STUB_DOC("\\brief Get the sample rate\n\nThe sample rate defines the number of audio samples\ncaptured per second. The higher, the better the quality\n(for example, 44100 samples/sec is CD quality).\n\n\\return Sample rate, in samples per second");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getSampleRate", "fun(self: sf.SoundBufferRecorder): integer");
    type_sf__SoundBufferRecorder.set_function("getSampleRate",
        [](sf::SoundBufferRecorder& self) -> unsigned int {
            return static_cast<sf::SoundRecorder&>(self).getSampleRate();
        }
    );
    LUASF_STUB_DOC("\\brief Get a list of the names of all available audio capture devices\n\nThis function returns a vector of strings, containing\nthe names of all available audio capture devices.\n\n\\return A vector of strings containing the names");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getAvailableDevices", "fun(): string[]");
    type_sf__SoundBufferRecorder.set_function("getAvailableDevices",
        [lua]() -> sol::object {
            return lua_sf::vector_to_object(lua, sf::SoundRecorder::getAvailableDevices());
        }
    );
    LUASF_STUB_DOC("\\brief Get the name of the default audio capture device\n\nThis function returns the name of the default audio\ncapture device. If none is available, an empty string\nis returned.\n\n\\return The name of the default audio capture device");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getDefaultDevice", "fun(): string");
    type_sf__SoundBufferRecorder.set_function("getDefaultDevice",
        []() -> std::string {
            return std::string(sf::SoundRecorder::getDefaultDevice());
        }
    );
    LUASF_STUB_DOC("\\brief Set the audio capture device\n\nThis function sets the audio capture device to the device\nwith the given `name`. It can be called on the fly (i.e:\nwhile recording). If you do so while recording and\nopening the device fails, it stops the recording.\n\n\\param name The name of the audio capture device\n\n\\return `true`, if it was able to set the requested device\n\n\\see `getAvailableDevices`, `getDefaultDevice`");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "setDevice", "fun(self: sf.SoundBufferRecorder, name: string): boolean");
    type_sf__SoundBufferRecorder.set_function("setDevice",
        [](sf::SoundBufferRecorder& self, std::string name) -> bool {
            return static_cast<sf::SoundRecorder&>(self).setDevice(name);
        }
    );
    LUASF_STUB_DOC("\\brief Get the name of the current audio capture device\n\n\\return The name of the current audio capture device");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getDevice", "fun(self: sf.SoundBufferRecorder): string");
    type_sf__SoundBufferRecorder.set_function("getDevice",
        sol::policies(
            [](sf::SoundBufferRecorder& self) -> std::string {
                return std::string(static_cast<sf::SoundRecorder&>(self).getDevice());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Set the channel count of the audio capture device\n\nThis method allows you to specify the number of channels\nused for recording. Currently only 16-bit mono and\n16-bit stereo are supported.\n\n\\param channelCount Number of channels. Currently only\nmono (1) and stereo (2) are supported.\n\n\\see `getChannelCount`");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "setChannelCount", "fun(self: sf.SoundBufferRecorder, channelCount: integer)");
    type_sf__SoundBufferRecorder.set_function("setChannelCount",
        [](sf::SoundBufferRecorder& self, lua_sf::LuaIntegral<unsigned int> channelCount) {
            static_cast<sf::SoundRecorder&>(self).setChannelCount(channelCount.value());
        }
    );
    LUASF_STUB_DOC("\\brief Get the number of channels used by this recorder\n\nCurrently only mono and stereo are supported, so the\nvalue is either 1 (for mono) or 2 (for stereo).\n\n\\return Number of channels\n\n\\see `setChannelCount`");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getChannelCount", "fun(self: sf.SoundBufferRecorder): integer");
    type_sf__SoundBufferRecorder.set_function("getChannelCount",
        [](sf::SoundBufferRecorder& self) -> unsigned int {
            return static_cast<sf::SoundRecorder&>(self).getChannelCount();
        }
    );
    LUASF_STUB_DOC("\\brief Get the map of position in sample frame to sound channel\n\nThis is used to map a sample in the sample stream to a\nposition during spatialization.\n\n\\return Map of position in sample frame to sound channel");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getChannelMap", "fun(self: sf.SoundBufferRecorder): sf.SoundChannel[]");
    type_sf__SoundBufferRecorder.set_function("getChannelMap",
        sol::policies(
            [lua](sf::SoundBufferRecorder& self) -> sol::object {
                return lua_sf::vector_to_object(lua, static_cast<sf::SoundRecorder&>(self).getChannelMap());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Check if the system supports audio capture\n\nThis function should always be called before using\nthe audio capture features. If it returns `false`, then\nany attempt to use `sf::SoundRecorder` or one of its derived\nclasses will fail.\n\n\\return `true` if audio capture is supported, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "isAvailable", "fun(): boolean");
    type_sf__SoundBufferRecorder.set_function("isAvailable",
        []() -> bool {
            return sf::SoundRecorder::isAvailable();
        }
    );
    LUASF_STUB_DOC("\\brief Get the sound buffer containing the captured audio data\n\nThe sound buffer is valid only after the capture has ended.\nThis function provides a read-only access to the internal\nsound buffer, but it can be copied if you need to\nmake any modification to it.\n\n\\return Read-only access to the sound buffer");
    LUASF_STUB_FUNCTION("sf.SoundBufferRecorder", "getBuffer", "fun(self: sf.SoundBufferRecorder): sf.SoundBuffer");
    type_sf__SoundBufferRecorder.set_function("getBuffer",
        sol::policies(
            [](sf::SoundBufferRecorder& self) {
                return std::cref(self.getBuffer());
            },
            sol::self_dependency{}
        )
    );
}
