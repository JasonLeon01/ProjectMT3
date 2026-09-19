#include "Audio/bind_Music.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Music(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Music = sf.new_usertype<sf::Music>("Music",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::SoundStream, sf::SoundSource>()
    );
    sol::table table_sf__Music = sf["Music"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Music>(lua);
    sol::table native_bases_sf__Music = lua.create_table();
    native_bases_sf__Music.add(lua["sf"]["SoundStream"].get<sol::table>());
    table_sf__Music.raw_set("__nativeBases", native_bases_sf__Music);
    LUASF_STUB_DOC("\\brief Streamed music played from an audio file");
    LUASF_STUB_CLASS("sf.Music", "sf.SoundStream, sf.SoundSource");
    LUASF_STUB_DOC("\\brief Construct a music from an audio file\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather\nstreamed continuously, the file must remain accessible until\nthe `sf::Music` object loads a new music or is destroyed.\n\n\\param filename Path of the music file to open\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `openFromMemory`, `openFromStream`");
    LUASF_STUB_FUNCTION("sf.Music", "new", "fun(filename: string): sf.Music");
    LUASF_STUB_OVERLOAD("sf.Music", "new", "fun(stream: sf.InputStream): sf.Music");
    LUASF_STUB_OVERLOAD("sf.Music", "new", "fun(): sf.Music");
    LUASF_STUB_OVERLOAD("sf.Music", "new", "fun(data: any): sf.Music");
    type_sf__Music.set_function("new", sol::factories(
        [](std::string filename) {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::Music>(std::filesystem::path(filename)));
        },
        [](sol::object stream) {
            auto& stream_ref = stream.as<sf::InputStream&>();
            auto object = lua_sf::makeLongLivedMemoryObject<sf::Music>(stream_ref);
            lua_sf::rememberLongLivedStream(*object, std::move(stream));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        []() {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::Music>());
        },
        [](sol::object data) {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            auto object = lua_sf::makeLongLivedMemoryObject<sf::Music>();
            if (!object->openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size())))
                throw std::runtime_error("Failed to open sf.Music from memory");
            lua_sf::rememberLongLivedMemory(*object, std::move(data_buffer));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        }
    ));
    LUASF_STUB_DOC("\\brief Set the pitch of the sound\n\nThe pitch represents the perceived fundamental frequency\nof a sound; thus you can make a sound more acute or grave\nby changing its pitch. A side effect of changing the pitch\nis to modify the playing speed of the sound as well.\nThe default value for the pitch is 1.\n\n\\param pitch New pitch to apply to the sound\n\n\\see `getPitch`");
    LUASF_STUB_FUNCTION("sf.Music", "setPitch", "fun(self: sf.Music, pitch: number)");
    type_sf__Music.set_function("setPitch",
        [](sf::Music& self, float pitch) {
            static_cast<sf::SoundSource&>(self).setPitch(pitch);
        }
    );
    LUASF_STUB_DOC("\\brief Set the pan of the sound\n\nUsing panning, a mono sound can be panned between\nstereo channels. When the pan is set to -1, the sound\nis played only on the left channel, when the pan is set\nto +1, the sound is played only on the right channel.\n\n\\param pan New pan to apply to the sound [-1, +1]\n\n\\see `getPan`");
    LUASF_STUB_FUNCTION("sf.Music", "setPan", "fun(self: sf.Music, pan: number)");
    type_sf__Music.set_function("setPan",
        [](sf::Music& self, float pan) {
            static_cast<sf::SoundSource&>(self).setPan(pan);
        }
    );
    LUASF_STUB_DOC("\\brief Set the volume of the sound\n\nThe volume is a value between 0 (mute) and 100 (full volume).\nThe default value for the volume is 100.\n\n\\param volume Volume of the sound\n\n\\see `getVolume`");
    LUASF_STUB_FUNCTION("sf.Music", "setVolume", "fun(self: sf.Music, volume: number)");
    type_sf__Music.set_function("setVolume",
        [](sf::Music& self, float volume) {
            static_cast<sf::SoundSource&>(self).setVolume(volume);
        }
    );
    LUASF_STUB_DOC("\\brief Set whether spatialization of the sound is enabled\n\nSpatialization is the application of various effects to\nsimulate a sound being emitted at a virtual position in\n3D space and exhibiting various physical phenomena such as\ndirectional attenuation and doppler shift.\n\n\\param enabled `true` to enable spatialization, `false` to disable\n\n\\see `isSpatializationEnabled`");
    LUASF_STUB_FUNCTION("sf.Music", "setSpatializationEnabled", "fun(self: sf.Music, enabled: boolean)");
    type_sf__Music.set_function("setSpatializationEnabled",
        [](sf::Music& self, bool enabled) {
            static_cast<sf::SoundSource&>(self).setSpatializationEnabled(enabled);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D position of the sound in the audio scene\n\nOnly sounds with one channel (mono sounds) can be\nspatialized.\nThe default position of a sound is (0, 0, 0).\n\n\\param position Position of the sound in the scene\n\n\\see `getPosition`");
    LUASF_STUB_FUNCTION("sf.Music", "setPosition", "fun(self: sf.Music, position: sf.Vector3f)");
    type_sf__Music.set_function("setPosition",
        [](sf::Music& self, const sf::Vector3f& position) {
            static_cast<sf::SoundSource&>(self).setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D direction of the sound in the audio scene\n\nThe direction defines where the sound source is facing\nin 3D space. It will affect how the sound is attenuated\nif facing away from the listener.\nThe default direction of a sound is (0, 0, -1).\n\n\\param direction Direction of the sound in the scene\n\n\\see `getDirection`");
    LUASF_STUB_FUNCTION("sf.Music", "setDirection", "fun(self: sf.Music, direction: sf.Vector3f)");
    type_sf__Music.set_function("setDirection",
        [](sf::Music& self, const sf::Vector3f& direction) {
            static_cast<sf::SoundSource&>(self).setDirection(direction);
        }
    );
    LUASF_STUB_DOC("\\brief Set the cone properties of the sound in the audio scene\n\nThe cone defines how directional attenuation is applied.\nThe default cone of a sound is (2 * PI, 2 * PI, 1).\n\n\\param cone Cone properties of the sound in the scene\n\n\\see `getCone`");
    LUASF_STUB_FUNCTION("sf.Music", "setCone", "fun(self: sf.Music, cone: sf.SoundSource.Cone)");
    type_sf__Music.set_function("setCone",
        [](sf::Music& self, const sf::SoundSource::Cone& cone) {
            static_cast<sf::SoundSource&>(self).setCone(cone);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D velocity of the sound in the audio scene\n\nThe velocity is used to determine how to doppler shift\nthe sound. Sounds moving towards the listener will be\nperceived to have a higher pitch and sounds moving away\nfrom the listener will be perceived to have a lower pitch.\n\n\\param velocity Velocity of the sound in the scene\n\n\\see `getVelocity`");
    LUASF_STUB_FUNCTION("sf.Music", "setVelocity", "fun(self: sf.Music, velocity: sf.Vector3f)");
    type_sf__Music.set_function("setVelocity",
        [](sf::Music& self, const sf::Vector3f& velocity) {
            static_cast<sf::SoundSource&>(self).setVelocity(velocity);
        }
    );
    LUASF_STUB_DOC("\\brief Set the doppler factor of the sound\n\nThe doppler factor determines how strong the doppler\nshift will be.\n\n\\param factor New doppler factor to apply to the sound\n\n\\see `getDopplerFactor`");
    LUASF_STUB_FUNCTION("sf.Music", "setDopplerFactor", "fun(self: sf.Music, factor: number)");
    type_sf__Music.set_function("setDopplerFactor",
        [](sf::Music& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDopplerFactor(factor);
        }
    );
    LUASF_STUB_DOC("\\brief Set the directional attenuation factor of the sound\n\nDepending on the virtual position of an output channel\nrelative to the listener (such as in surround sound\nsetups), sounds will be attenuated when emitting them\nfrom certain channels. This factor determines how strong\nthe attenuation based on output channel position\nrelative to the listener is.\n\n\\param factor New directional attenuation factor to apply to the sound\n\n\\see `getDirectionalAttenuationFactor`");
    LUASF_STUB_FUNCTION("sf.Music", "setDirectionalAttenuationFactor", "fun(self: sf.Music, factor: number)");
    type_sf__Music.set_function("setDirectionalAttenuationFactor",
        [](sf::Music& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDirectionalAttenuationFactor(factor);
        }
    );
    LUASF_STUB_DOC("\\brief Make the sound's position relative to the listener or absolute\n\nMaking a sound relative to the listener will ensure that it will always\nbe played the same way regardless of the position of the listener.\nThis can be useful for non-spatialized sounds, sounds that are\nproduced by the listener, or sounds attached to it.\nThe default value is `false` (position is absolute).\n\n\\param relative `true` to set the position relative, `false` to set it absolute\n\n\\see `isRelativeToListener`");
    LUASF_STUB_FUNCTION("sf.Music", "setRelativeToListener", "fun(self: sf.Music, relative: boolean)");
    type_sf__Music.set_function("setRelativeToListener",
        [](sf::Music& self, bool relative) {
            static_cast<sf::SoundSource&>(self).setRelativeToListener(relative);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum distance of the sound\n\nThe \"minimum distance\" of a sound is the maximum\ndistance at which it is heard at its maximum volume. Further\nthan the minimum distance, it will start to fade out according\nto its attenuation factor. A value of 0 (\"inside the head\nof the listener\") is an invalid value and is forbidden.\nThe default value of the minimum distance is 1.\n\n\\param distance New minimum distance of the sound\n\n\\see `getMinDistance`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.Music", "setMinDistance", "fun(self: sf.Music, distance: number)");
    type_sf__Music.set_function("setMinDistance",
        [](sf::Music& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMinDistance(distance);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum distance of the sound\n\nThe \"maximum distance\" of a sound is the minimum\ndistance at which it is heard at its minimum volume. Closer\nthan the maximum distance, it will start to fade in according\nto its attenuation factor.\nThe default value of the maximum distance is the maximum\nvalue a float can represent.\n\n\\param distance New maximum distance of the sound\n\n\\see `getMaxDistance`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.Music", "setMaxDistance", "fun(self: sf.Music, distance: number)");
    type_sf__Music.set_function("setMaxDistance",
        [](sf::Music& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMaxDistance(distance);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum gain of the sound\n\nWhen the sound is further away from the listener than\nthe \"maximum distance\" the attenuated gain is clamped\nso it cannot go below the minimum gain value.\n\n\\param gain New minimum gain of the sound\n\n\\see `getMinGain`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.Music", "setMinGain", "fun(self: sf.Music, gain: number)");
    type_sf__Music.set_function("setMinGain",
        [](sf::Music& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMinGain(gain);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum gain of the sound\n\nWhen the sound is closer from the listener than\nthe \"minimum distance\" the attenuated gain is clamped\nso it cannot go above the maximum gain value.\n\n\\param gain New maximum gain of the sound\n\n\\see `getMaxGain`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.Music", "setMaxGain", "fun(self: sf.Music, gain: number)");
    type_sf__Music.set_function("setMaxGain",
        [](sf::Music& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMaxGain(gain);
        }
    );
    LUASF_STUB_DOC("\\brief Set the attenuation factor of the sound\n\nThe attenuation is a multiplicative factor which makes\nthe sound more or less loud according to its distance\nfrom the listener. An attenuation of 0 will produce a\nnon-attenuated sound, i.e. its volume will always be the same\nwhether it is heard from near or from far. On the other hand,\nan attenuation value such as 100 will make the sound fade out\nvery quickly as it gets further from the listener.\nThe default value of the attenuation is 1.\n\n\\param attenuation New attenuation factor of the sound\n\n\\see `getAttenuation`, `setMinDistance`");
    LUASF_STUB_FUNCTION("sf.Music", "setAttenuation", "fun(self: sf.Music, attenuation: number)");
    type_sf__Music.set_function("setAttenuation",
        [](sf::Music& self, float attenuation) {
            static_cast<sf::SoundSource&>(self).setAttenuation(attenuation);
        }
    );
    LUASF_STUB_DOC("\\brief Set the effect processor to be applied to the sound\n\nThe effect processor is a callable that will be called\nwith sound data to be processed.\n\n\\param effectProcessor The effect processor to attach to this sound, attach an empty processor to disable processing");
    LUASF_STUB_FUNCTION("sf.Music", "setEffectProcessor", "fun(self: sf.Music, effectProcessor: sf.SoundSource.EffectProcessor|nil)");
    type_sf__Music.set_function("setEffectProcessor",
        [](sf::Music& self, sol::object effectProcessor) {
            static_cast<sf::SoundSource&>(self).setEffectProcessor(lua_sf::callback::from_object<sf::SoundSource::EffectProcessor, lua_sf::callback::InterleavedFloatTransformCodec>(effectProcessor, lua_sf::callback::CallbackOptions{"sf::SoundSource::setEffectProcessor.effectProcessor", true}));
        }
    );
    LUASF_STUB_DOC("\\brief Get the pitch of the sound\n\n\\return Pitch of the sound\n\n\\see `setPitch`");
    LUASF_STUB_FUNCTION("sf.Music", "getPitch", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getPitch",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getPitch();
        }
    );
    LUASF_STUB_DOC("\\brief Get the pan of the sound\n\n\\return Pan of the sound\n\n\\see `setPan`");
    LUASF_STUB_FUNCTION("sf.Music", "getPan", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getPan",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getPan();
        }
    );
    LUASF_STUB_DOC("\\brief Get the volume of the sound\n\n\\return Volume of the sound, in the range [0, 100]\n\n\\see `setVolume`");
    LUASF_STUB_FUNCTION("sf.Music", "getVolume", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getVolume",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getVolume();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether spatialization of the sound is enabled\n\n\\return `true` if spatialization is enabled, `false` if it's disabled\n\n\\see `setSpatializationEnabled`");
    LUASF_STUB_FUNCTION("sf.Music", "isSpatializationEnabled", "fun(self: sf.Music): boolean");
    type_sf__Music.set_function("isSpatializationEnabled",
        [](sf::Music& self) -> bool {
            return static_cast<sf::SoundSource&>(self).isSpatializationEnabled();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D position of the sound in the audio scene\n\n\\return Position of the sound\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.Music", "getPosition", "fun(self: sf.Music): sf.Vector3f");
    type_sf__Music.set_function("getPosition",
        [](sf::Music& self) -> sf::Vector3f {
            return static_cast<sf::SoundSource&>(self).getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D direction of the sound in the audio scene\n\n\\return Direction of the sound\n\n\\see `setDirection`");
    LUASF_STUB_FUNCTION("sf.Music", "getDirection", "fun(self: sf.Music): sf.Vector3f");
    type_sf__Music.set_function("getDirection",
        [](sf::Music& self) -> sf::Vector3f {
            return static_cast<sf::SoundSource&>(self).getDirection();
        }
    );
    LUASF_STUB_DOC("\\brief Get the cone properties of the sound in the audio scene\n\n\\return Cone properties of the sound\n\n\\see `setCone`");
    LUASF_STUB_FUNCTION("sf.Music", "getCone", "fun(self: sf.Music): sf.SoundSource.Cone");
    type_sf__Music.set_function("getCone",
        [](sf::Music& self) -> sf::SoundSource::Cone {
            return static_cast<sf::SoundSource&>(self).getCone();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D velocity of the sound in the audio scene\n\n\\return Velocity of the sound\n\n\\see `setVelocity`");
    LUASF_STUB_FUNCTION("sf.Music", "getVelocity", "fun(self: sf.Music): sf.Vector3f");
    type_sf__Music.set_function("getVelocity",
        [](sf::Music& self) -> sf::Vector3f {
            return static_cast<sf::SoundSource&>(self).getVelocity();
        }
    );
    LUASF_STUB_DOC("\\brief Get the doppler factor of the sound\n\n\\return Doppler factor of the sound\n\n\\see `setDopplerFactor`");
    LUASF_STUB_FUNCTION("sf.Music", "getDopplerFactor", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getDopplerFactor",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getDopplerFactor();
        }
    );
    LUASF_STUB_DOC("\\brief Get the directional attenuation factor of the sound\n\n\\return Directional attenuation factor of the sound\n\n\\see `setDirectionalAttenuationFactor`");
    LUASF_STUB_FUNCTION("sf.Music", "getDirectionalAttenuationFactor", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getDirectionalAttenuationFactor",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getDirectionalAttenuationFactor();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the sound's position is relative to the\nlistener or is absolute\n\n\\return `true` if the position is relative, `false` if it's absolute\n\n\\see `setRelativeToListener`");
    LUASF_STUB_FUNCTION("sf.Music", "isRelativeToListener", "fun(self: sf.Music): boolean");
    type_sf__Music.set_function("isRelativeToListener",
        [](sf::Music& self) -> bool {
            return static_cast<sf::SoundSource&>(self).isRelativeToListener();
        }
    );
    LUASF_STUB_DOC("\\brief Get the minimum distance of the sound\n\n\\return Minimum distance of the sound\n\n\\see `setMinDistance`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.Music", "getMinDistance", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getMinDistance",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMinDistance();
        }
    );
    LUASF_STUB_DOC("\\brief Get the maximum distance of the sound\n\n\\return Maximum distance of the sound\n\n\\see `setMaxDistance`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.Music", "getMaxDistance", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getMaxDistance",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMaxDistance();
        }
    );
    LUASF_STUB_DOC("\\brief Get the minimum gain of the sound\n\n\\return Minimum gain of the sound\n\n\\see `setMinGain`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.Music", "getMinGain", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getMinGain",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMinGain();
        }
    );
    LUASF_STUB_DOC("\\brief Get the maximum gain of the sound\n\n\\return Maximum gain of the sound\n\n\\see `setMaxGain`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.Music", "getMaxGain", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getMaxGain",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMaxGain();
        }
    );
    LUASF_STUB_DOC("\\brief Get the attenuation factor of the sound\n\n\\return Attenuation factor of the sound\n\n\\see `setAttenuation`, `getMinDistance`");
    LUASF_STUB_FUNCTION("sf.Music", "getAttenuation", "fun(self: sf.Music): number");
    type_sf__Music.set_function("getAttenuation",
        [](sf::Music& self) -> float {
            return static_cast<sf::SoundSource&>(self).getAttenuation();
        }
    );
    LUASF_STUB_DOC("\\brief Start or resume playing the sound source\n\nThis function starts the source if it was stopped, resumes\nit if it was paused, and restarts it from the beginning if\nit was already playing.\n\n\\see `pause`, `stop`");
    LUASF_STUB_FUNCTION("sf.Music", "play", "fun(self: sf.Music)");
    type_sf__Music.set_function("play",
        [](sf::Music& self) {
            static_cast<sf::SoundSource&>(self).play();
        }
    );
    LUASF_STUB_DOC("\\brief Pause the sound source\n\nThis function pauses the source if it was playing,\notherwise (source already paused or stopped) it has no effect.\n\n\\see `play`, `stop`");
    LUASF_STUB_FUNCTION("sf.Music", "pause", "fun(self: sf.Music)");
    type_sf__Music.set_function("pause",
        [](sf::Music& self) {
            static_cast<sf::SoundSource&>(self).pause();
        }
    );
    LUASF_STUB_DOC("\\brief Stop playing the sound source\n\nThis function stops the source if it was playing or paused,\nand does nothing if it was already stopped.\nIt also resets the playing position (unlike `pause()`).\n\n\\see `play`, `pause`");
    LUASF_STUB_FUNCTION("sf.Music", "stop", "fun(self: sf.Music)");
    type_sf__Music.set_function("stop",
        [](sf::Music& self) {
            static_cast<sf::SoundSource&>(self).stop();
        }
    );
    LUASF_STUB_DOC("\\brief Get the current status of the sound (stopped, paused, playing)\n\n\\return Current status of the sound");
    LUASF_STUB_FUNCTION("sf.Music", "getStatus", "fun(self: sf.Music): sf.SoundSource.Status");
    type_sf__Music.set_function("getStatus",
        [](sf::Music& self) -> sf::SoundSource::Status {
            return static_cast<sf::SoundSource&>(self).getStatus();
        }
    );
    LUASF_STUB_DOC("\\brief Return the number of channels of the stream\n\n1 channel means a mono sound, 2 means stereo, etc.\n\n\\return Number of channels");
    LUASF_STUB_FUNCTION("sf.Music", "getChannelCount", "fun(self: sf.Music): integer");
    type_sf__Music.set_function("getChannelCount",
        [](sf::Music& self) -> unsigned int {
            return static_cast<sf::SoundStream&>(self).getChannelCount();
        }
    );
    LUASF_STUB_DOC("\\brief Get the stream sample rate of the stream\n\nThe sample rate is the number of audio samples played per\nsecond. The higher, the better the quality.\n\n\\return Sample rate, in number of samples per second");
    LUASF_STUB_FUNCTION("sf.Music", "getSampleRate", "fun(self: sf.Music): integer");
    type_sf__Music.set_function("getSampleRate",
        [](sf::Music& self) -> unsigned int {
            return static_cast<sf::SoundStream&>(self).getSampleRate();
        }
    );
    LUASF_STUB_DOC("\\brief Get the map of position in sample frame to sound channel\n\nThis is used to map a sample in the sample stream to a\nposition during spatialization.\n\n\\return Map of position in sample frame to sound channel");
    LUASF_STUB_FUNCTION("sf.Music", "getChannelMap", "fun(self: sf.Music): sf.SoundChannel[]");
    type_sf__Music.set_function("getChannelMap",
        sol::policies(
            [lua](sf::Music& self) -> sol::object {
                return lua_sf::vector_to_object(lua, static_cast<sf::SoundStream&>(self).getChannelMap());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Change the current playing position of the stream\n\nThe playing position can be changed when the stream is\neither paused or playing. Changing the playing position\nwhen the stream is stopped has no effect, since playing\nthe stream would reset its position.\n\n\\param timeOffset New playing position, from the beginning of the stream\n\n\\see `getPlayingOffset`");
    LUASF_STUB_FUNCTION("sf.Music", "setPlayingOffset", "fun(self: sf.Music, timeOffset: sf.Time)");
    type_sf__Music.set_function("setPlayingOffset",
        [](sf::Music& self, sf::Time timeOffset) {
            static_cast<sf::SoundStream&>(self).setPlayingOffset(timeOffset);
        }
    );
    LUASF_STUB_DOC("\\brief Get the current playing position of the stream\n\n\\return Current playing position, from the beginning of the stream\n\n\\see `setPlayingOffset`");
    LUASF_STUB_FUNCTION("sf.Music", "getPlayingOffset", "fun(self: sf.Music): sf.Time");
    type_sf__Music.set_function("getPlayingOffset",
        [](sf::Music& self) -> sf::Time {
            return static_cast<sf::SoundStream&>(self).getPlayingOffset();
        }
    );
    LUASF_STUB_DOC("\\brief Set whether or not the stream should loop after reaching the end\n\nIf set, the stream will restart from beginning after\nreaching the end and so on, until it is stopped or\n`setLooping(false)` is called.\nThe default looping state for streams is `false`.\n\n\\param loop `true` to play in loop, `false` to play once\n\n\\see `isLooping`");
    LUASF_STUB_FUNCTION("sf.Music", "setLooping", "fun(self: sf.Music, loop: boolean)");
    type_sf__Music.set_function("setLooping",
        [](sf::Music& self, bool loop) {
            static_cast<sf::SoundStream&>(self).setLooping(loop);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether or not the stream is in loop mode\n\n\\return `true` if the stream is looping, `false` otherwise\n\n\\see `setLooping`");
    LUASF_STUB_FUNCTION("sf.Music", "isLooping", "fun(self: sf.Music): boolean");
    type_sf__Music.set_function("isLooping",
        [](sf::Music& self) -> bool {
            return static_cast<sf::SoundStream&>(self).isLooping();
        }
    );
    LUASF_STUB_DOC("\\brief Open a music from an audio file\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather\nstreamed continuously, the file must remain accessible until\nthe `sf::Music` object loads a new music or is destroyed.\n\n\\param filename Path of the music file to open\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `openFromMemory`, `openFromStream`");
    LUASF_STUB_FUNCTION("sf.Music", "openFromFile", "fun(self: sf.Music, filename: string): boolean");
    type_sf__Music.set_function("openFromFile",
        [](sf::Music& self, std::string filename) -> bool {
            auto result = self.openFromFile(std::filesystem::path(filename));
            if (result)
                lua_sf::releaseLongLivedResources(self);
            return result;
        }
    );
    LUASF_STUB_DOC("\\brief Open a music from an audio file in memory\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather streamed\ncontinuously, the `data` buffer must remain accessible until\nthe `sf::Music` object loads a new music or is destroyed. That is,\nyou can't deallocate the buffer right after calling this function.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `openFromFile`, `openFromStream`");
    LUASF_STUB_FUNCTION("sf.Music", "openFromMemory", "fun(self: sf.Music, data: any): boolean");
    type_sf__Music.set_function("openFromMemory",
        [](sf::Music& self, sol::object data) -> bool {
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
    LUASF_STUB_DOC("\\brief Open a music from an audio file in a custom stream\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather\nstreamed continuously, the `stream` must remain accessible\nuntil the `sf::Music` object loads a new music or is destroyed.\n\n\\param stream Source stream to read from\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `openFromFile`, `openFromMemory`");
    LUASF_STUB_FUNCTION("sf.Music", "openFromStream", "fun(self: sf.Music, stream: sf.InputStream): boolean");
    type_sf__Music.set_function("openFromStream",
        [](sf::Music& self, sol::object stream) -> bool {
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
    LUASF_STUB_DOC("\\brief Get the total duration of the music\n\n\\return Music duration");
    LUASF_STUB_FUNCTION("sf.Music", "getDuration", "fun(self: sf.Music): sf.Time");
    type_sf__Music.set_function("getDuration",
        [](sf::Music& self) -> sf::Time {
            return self.getDuration();
        }
    );
    LUASF_STUB_DOC("\\brief Get the positions of the of the sound's looping sequence\n\n\\return Loop Time position class.\n\n\\warning Since `setLoopPoints()` performs some adjustments on the\nprovided values and rounds them to internal samples, a call to\n`getLoopPoints()` is not guaranteed to return the same times passed\ninto a previous call to `setLoopPoints()`. However, it is guaranteed\nto return times that will map to the valid internal samples of\nthis Music if they are later passed to `setLoopPoints()`.\n\n\\see `setLoopPoints`");
    LUASF_STUB_FUNCTION("sf.Music", "getLoopPoints", "fun(self: sf.Music): sf.Music.TimeSpan");
    type_sf__Music.set_function("getLoopPoints",
        [](sf::Music& self) -> sf::Music::TimeSpan {
            return self.getLoopPoints();
        }
    );
    LUASF_STUB_DOC("\\brief Sets the beginning and duration of the sound's looping sequence using `sf::Time`\n\n`setLoopPoints()` allows for specifying the beginning offset and the duration of the loop such that,\nwhen the music is enabled for looping, it will seamlessly seek to the beginning whenever it\nencounters the end of the duration. Valid ranges for `timePoints.offset` and `timePoints.length` are\n[0, Dur) and (0, Dur-offset] respectively, where Dur is the value returned by `getDuration()`.\nNote that the EOF \"loop point\" from the end to the beginning of the stream is still honored,\nin case the caller seeks to a point after the end of the loop range. This function can be\nsafely called at any point after a stream is opened, and will be applied to a playing sound\nwithout affecting the current playing offset.\n\n\\warning Setting the loop points while the stream's status is Paused\nwill set its status to Stopped. The playing offset will be unaffected.\n\n\\param timePoints The definition of the loop. Can be any time points within the sound's length\n\n\\see `getLoopPoints`");
    LUASF_STUB_FUNCTION("sf.Music", "setLoopPoints", "fun(self: sf.Music, timePoints: sf.Music.TimeSpan)");
    type_sf__Music.set_function("setLoopPoints",
        [](sf::Music& self, sf::Music::TimeSpan timePoints) {
            self.setLoopPoints(timePoints);
        }
    );
    auto type_sf__Music__Span_sf__Time_ = table_sf__Music.new_usertype<sf::Music::Span<sf::Time>>("TimeSpan", sol::no_constructor);
    sol::table table_sf__Music__Span_sf__Time_ = table_sf__Music["TimeSpan"].get<sol::table>();
    LUASF_STUB_DOC("\\brief Structure defining a time range using the template type");
    LUASF_STUB_CLASS("sf.Music.TimeSpan");
    LUASF_STUB_DOC("The beginning offset of the time range");
    LUASF_STUB_FIELD("offset", "sf.Time");
    LUASF_STUB_DOC("The length of the time range");
    LUASF_STUB_FIELD("length", "sf.Time");
    LUASF_STUB_FUNCTION("sf.Music.TimeSpan", "new", "fun(): sf.Music.TimeSpan");
    type_sf__Music__Span_sf__Time_.set_function("new", sol::factories(
        []() {
            return sf::Music::Span<sf::Time>{};
        }
    ));
    type_sf__Music__Span_sf__Time_["offset"] = sol::policies(&sf::Music::Span<sf::Time>::offset, sol::self_dependency{});
    type_sf__Music__Span_sf__Time_["length"] = sol::policies(&sf::Music::Span<sf::Time>::length, sol::self_dependency{});
}
