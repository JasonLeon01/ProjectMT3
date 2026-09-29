#include "Audio/bind_SoundStream.hpp"

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

namespace { constexpr std::array<std::string_view, 52> docs = {
    "\\brief Abstract base class for streamed audio sources",
    "\\brief Set the pitch of the sound\n\nThe pitch represents the perceived fundamental frequency\nof a sound; thus you can make a sound more acute or grave\nby changing its pitch. A side effect of changing the pitch\nis to modify the playing speed of the sound as well.\nThe default value for the pitch is 1.\n\n\\param pitch New pitch to apply to the sound\n\n\\see `getPitch`",
    "\\brief Set the pan of the sound\n\nUsing panning, a mono sound can be panned between\nstereo channels. When the pan is set to -1, the sound\nis played only on the left channel, when the pan is set\nto +1, the sound is played only on the right channel.\n\n\\param pan New pan to apply to the sound [-1, +1]\n\n\\see `getPan`",
    "\\brief Set the volume of the sound\n\nThe volume is a value between 0 (mute) and 100 (full volume).\nThe default value for the volume is 100.\n\n\\param volume Volume of the sound\n\n\\see `getVolume`",
    "\\brief Set whether spatialization of the sound is enabled\n\nSpatialization is the application of various effects to\nsimulate a sound being emitted at a virtual position in\n3D space and exhibiting various physical phenomena such as\ndirectional attenuation and doppler shift.\n\n\\param enabled `true` to enable spatialization, `false` to disable\n\n\\see `isSpatializationEnabled`",
    "\\brief Set the 3D position of the sound in the audio scene\n\nOnly sounds with one channel (mono sounds) can be\nspatialized.\nThe default position of a sound is (0, 0, 0).\n\n\\param position Position of the sound in the scene\n\n\\see `getPosition`",
    "\\brief Set the 3D direction of the sound in the audio scene\n\nThe direction defines where the sound source is facing\nin 3D space. It will affect how the sound is attenuated\nif facing away from the listener.\nThe default direction of a sound is (0, 0, -1).\n\n\\param direction Direction of the sound in the scene\n\n\\see `getDirection`",
    "\\brief Set the cone properties of the sound in the audio scene\n\nThe cone defines how directional attenuation is applied.\nThe default cone of a sound is (2 * PI, 2 * PI, 1).\n\n\\param cone Cone properties of the sound in the scene\n\n\\see `getCone`",
    "\\brief Set the 3D velocity of the sound in the audio scene\n\nThe velocity is used to determine how to doppler shift\nthe sound. Sounds moving towards the listener will be\nperceived to have a higher pitch and sounds moving away\nfrom the listener will be perceived to have a lower pitch.\n\n\\param velocity Velocity of the sound in the scene\n\n\\see `getVelocity`",
    "\\brief Set the doppler factor of the sound\n\nThe doppler factor determines how strong the doppler\nshift will be.\n\n\\param factor New doppler factor to apply to the sound\n\n\\see `getDopplerFactor`",
    "\\brief Set the directional attenuation factor of the sound\n\nDepending on the virtual position of an output channel\nrelative to the listener (such as in surround sound\nsetups), sounds will be attenuated when emitting them\nfrom certain channels. This factor determines how strong\nthe attenuation based on output channel position\nrelative to the listener is.\n\n\\param factor New directional attenuation factor to apply to the sound\n\n\\see `getDirectionalAttenuationFactor`",
    "\\brief Make the sound's position relative to the listener or absolute\n\nMaking a sound relative to the listener will ensure that it will always\nbe played the same way regardless of the position of the listener.\nThis can be useful for non-spatialized sounds, sounds that are\nproduced by the listener, or sounds attached to it.\nThe default value is `false` (position is absolute).\n\n\\param relative `true` to set the position relative, `false` to set it absolute\n\n\\see `isRelativeToListener`",
    "\\brief Set the minimum distance of the sound\n\nThe \"minimum distance\" of a sound is the maximum\ndistance at which it is heard at its maximum volume. Further\nthan the minimum distance, it will start to fade out according\nto its attenuation factor. A value of 0 (\"inside the head\nof the listener\") is an invalid value and is forbidden.\nThe default value of the minimum distance is 1.\n\n\\param distance New minimum distance of the sound\n\n\\see `getMinDistance`, `setAttenuation`",
    "\\brief Set the maximum distance of the sound\n\nThe \"maximum distance\" of a sound is the minimum\ndistance at which it is heard at its minimum volume. Closer\nthan the maximum distance, it will start to fade in according\nto its attenuation factor.\nThe default value of the maximum distance is the maximum\nvalue a float can represent.\n\n\\param distance New maximum distance of the sound\n\n\\see `getMaxDistance`, `setAttenuation`",
    "\\brief Set the minimum gain of the sound\n\nWhen the sound is further away from the listener than\nthe \"maximum distance\" the attenuated gain is clamped\nso it cannot go below the minimum gain value.\n\n\\param gain New minimum gain of the sound\n\n\\see `getMinGain`, `setAttenuation`",
    "\\brief Set the maximum gain of the sound\n\nWhen the sound is closer from the listener than\nthe \"minimum distance\" the attenuated gain is clamped\nso it cannot go above the maximum gain value.\n\n\\param gain New maximum gain of the sound\n\n\\see `getMaxGain`, `setAttenuation`",
    "\\brief Set the attenuation factor of the sound\n\nThe attenuation is a multiplicative factor which makes\nthe sound more or less loud according to its distance\nfrom the listener. An attenuation of 0 will produce a\nnon-attenuated sound, i.e. its volume will always be the same\nwhether it is heard from near or from far. On the other hand,\nan attenuation value such as 100 will make the sound fade out\nvery quickly as it gets further from the listener.\nThe default value of the attenuation is 1.\n\n\\param attenuation New attenuation factor of the sound\n\n\\see `getAttenuation`, `setMinDistance`",
    "\\brief Set the effect processor to be applied to the sound\n\nThe effect processor is a callable that will be called\nwith sound data to be processed.\n\n\\param effectProcessor The effect processor to attach to this sound, attach an empty processor to disable processing",
    "\\brief Get the pitch of the sound\n\n\\return Pitch of the sound\n\n\\see `setPitch`",
    "\\brief Get the pan of the sound\n\n\\return Pan of the sound\n\n\\see `setPan`",
    "\\brief Get the volume of the sound\n\n\\return Volume of the sound, in the range [0, 100]\n\n\\see `setVolume`",
    "\\brief Tell whether spatialization of the sound is enabled\n\n\\return `true` if spatialization is enabled, `false` if it's disabled\n\n\\see `setSpatializationEnabled`",
    "\\brief Get the 3D position of the sound in the audio scene\n\n\\return Position of the sound\n\n\\see `setPosition`",
    "\\brief Get the 3D direction of the sound in the audio scene\n\n\\return Direction of the sound\n\n\\see `setDirection`",
    "\\brief Get the cone properties of the sound in the audio scene\n\n\\return Cone properties of the sound\n\n\\see `setCone`",
    "\\brief Get the 3D velocity of the sound in the audio scene\n\n\\return Velocity of the sound\n\n\\see `setVelocity`",
    "\\brief Get the doppler factor of the sound\n\n\\return Doppler factor of the sound\n\n\\see `setDopplerFactor`",
    "\\brief Get the directional attenuation factor of the sound\n\n\\return Directional attenuation factor of the sound\n\n\\see `setDirectionalAttenuationFactor`",
    "\\brief Tell whether the sound's position is relative to the\nlistener or is absolute\n\n\\return `true` if the position is relative, `false` if it's absolute\n\n\\see `setRelativeToListener`",
    "\\brief Get the minimum distance of the sound\n\n\\return Minimum distance of the sound\n\n\\see `setMinDistance`, `getAttenuation`",
    "\\brief Get the maximum distance of the sound\n\n\\return Maximum distance of the sound\n\n\\see `setMaxDistance`, `getAttenuation`",
    "\\brief Get the minimum gain of the sound\n\n\\return Minimum gain of the sound\n\n\\see `setMinGain`, `getAttenuation`",
    "\\brief Get the maximum gain of the sound\n\n\\return Maximum gain of the sound\n\n\\see `setMaxGain`, `getAttenuation`",
    "\\brief Get the attenuation factor of the sound\n\n\\return Attenuation factor of the sound\n\n\\see `setAttenuation`, `getMinDistance`",
    "\\brief Start or resume playing the sound source\n\nThis function starts the source if it was stopped, resumes\nit if it was paused, and restarts it from the beginning if\nit was already playing.\n\n\\see `pause`, `stop`",
    "\\brief Pause the sound source\n\nThis function pauses the source if it was playing,\notherwise (source already paused or stopped) it has no effect.\n\n\\see `play`, `stop`",
    "\\brief Stop playing the sound source\n\nThis function stops the source if it was playing or paused,\nand does nothing if it was already stopped.\nIt also resets the playing position (unlike `pause()`).\n\n\\see `play`, `pause`",
    "\\brief Get the current status of the sound (stopped, paused, playing)\n\n\\return Current status of the sound",
    "\\brief Start or resume playing the audio stream\n\nThis function starts the stream if it was stopped, resumes\nit if it was paused, and restarts it from the beginning if\nit was already playing.\nThis function uses its own thread so that it doesn't block\nthe rest of the program while the stream is played.\n\n\\see `pause`, `stop`",
    "\\brief Pause the audio stream\n\nThis function pauses the stream if it was playing,\notherwise (stream already paused or stopped) it has no effect.\n\n\\see `play`, `stop`",
    "\\brief Stop playing the audio stream\n\nThis function stops the stream if it was playing or paused,\nand does nothing if it was already stopped.\nIt also resets the playing position (unlike `pause()`).\n\n\\see `play`, `pause`",
    "\\brief Return the number of channels of the stream\n\n1 channel means a mono sound, 2 means stereo, etc.\n\n\\return Number of channels",
    "\\brief Get the stream sample rate of the stream\n\nThe sample rate is the number of audio samples played per\nsecond. The higher, the better the quality.\n\n\\return Sample rate, in number of samples per second",
    "\\brief Get the map of position in sample frame to sound channel\n\nThis is used to map a sample in the sample stream to a\nposition during spatialization.\n\n\\return Map of position in sample frame to sound channel",
    "\\brief Get the current status of the stream (stopped, paused, playing)\n\n\\return Current status",
    "\\brief Change the current playing position of the stream\n\nThe playing position can be changed when the stream is\neither paused or playing. Changing the playing position\nwhen the stream is stopped has no effect, since playing\nthe stream would reset its position.\n\n\\param timeOffset New playing position, from the beginning of the stream\n\n\\see `getPlayingOffset`",
    "\\brief Get the current playing position of the stream\n\n\\return Current playing position, from the beginning of the stream\n\n\\see `setPlayingOffset`",
    "\\brief Set whether or not the stream should loop after reaching the end\n\nIf set, the stream will restart from beginning after\nreaching the end and so on, until it is stopped or\n`setLooping(false)` is called.\nThe default looping state for streams is `false`.\n\n\\param loop `true` to play in loop, `false` to play once\n\n\\see `isLooping`",
    "\\brief Tell whether or not the stream is in loop mode\n\n\\return `true` if the stream is looping, `false` otherwise\n\n\\see `setLooping`",
    "\\brief Structure defining a chunk of audio data to stream",
    "Pointer to the audio samples",
    "Number of samples pointed by Samples",
}; }

void bind_SoundStream(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundStream = lua_glue::BindClass<sf::SoundStream>(sf, "SoundStream");
    lua_glue::BindBase<sf::SoundStream, sf::SoundSource>(type_sf__SoundStream);
    lua_glue::Table table_sf__SoundStream = sf["SoundStream"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::SoundStream>(lua);
    lua_glue::Table native_bases_sf__SoundStream = lua.create_table();
    native_bases_sf__SoundStream.add(lua["sf"]["SoundSource"].get<lua_glue::Table>());
    table_sf__SoundStream.raw_set("__nativeBases", native_bases_sf__SoundStream);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.SoundStream", "sf.SoundSource");
    // sf::SoundStream is abstract; constructor binding is omitted.
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setPitch", "fun(self: sf.SoundStream, pitch: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setPitch",
        [](sf::SoundStream& self, float pitch) {
            static_cast<sf::SoundSource&>(self).setPitch(pitch);
        },
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setPan", "fun(self: sf.SoundStream, pan: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setPan",
        [](sf::SoundStream& self, float pan) {
            static_cast<sf::SoundSource&>(self).setPan(pan);
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setVolume", "fun(self: sf.SoundStream, volume: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setVolume",
        [](sf::SoundStream& self, float volume) {
            static_cast<sf::SoundSource&>(self).setVolume(volume);
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setSpatializationEnabled", "fun(self: sf.SoundStream, enabled: boolean)");
    lua_glue::BindCallable(type_sf__SoundStream, "setSpatializationEnabled",
        [](sf::SoundStream& self, bool enabled) {
            static_cast<sf::SoundSource&>(self).setSpatializationEnabled(enabled);
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setPosition", "fun(self: sf.SoundStream, position: sf.Vector3f)");
    lua_glue::BindCallable(type_sf__SoundStream, "setPosition",
        [](sf::SoundStream& self, const sf::Vector3f& position) {
            static_cast<sf::SoundSource&>(self).setPosition(position);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setDirection", "fun(self: sf.SoundStream, direction: sf.Vector3f)");
    lua_glue::BindCallable(type_sf__SoundStream, "setDirection",
        [](sf::SoundStream& self, const sf::Vector3f& direction) {
            static_cast<sf::SoundSource&>(self).setDirection(direction);
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setCone", "fun(self: sf.SoundStream, cone: sf.SoundSource.Cone)");
    lua_glue::BindCallable(type_sf__SoundStream, "setCone",
        [](sf::SoundStream& self, const sf::SoundSource::Cone& cone) {
            static_cast<sf::SoundSource&>(self).setCone(cone);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setVelocity", "fun(self: sf.SoundStream, velocity: sf.Vector3f)");
    lua_glue::BindCallable(type_sf__SoundStream, "setVelocity",
        [](sf::SoundStream& self, const sf::Vector3f& velocity) {
            static_cast<sf::SoundSource&>(self).setVelocity(velocity);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setDopplerFactor", "fun(self: sf.SoundStream, factor: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setDopplerFactor",
        [](sf::SoundStream& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDopplerFactor(factor);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setDirectionalAttenuationFactor", "fun(self: sf.SoundStream, factor: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setDirectionalAttenuationFactor",
        [](sf::SoundStream& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDirectionalAttenuationFactor(factor);
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setRelativeToListener", "fun(self: sf.SoundStream, relative: boolean)");
    lua_glue::BindCallable(type_sf__SoundStream, "setRelativeToListener",
        [](sf::SoundStream& self, bool relative) {
            static_cast<sf::SoundSource&>(self).setRelativeToListener(relative);
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setMinDistance", "fun(self: sf.SoundStream, distance: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setMinDistance",
        [](sf::SoundStream& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMinDistance(distance);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setMaxDistance", "fun(self: sf.SoundStream, distance: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setMaxDistance",
        [](sf::SoundStream& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMaxDistance(distance);
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setMinGain", "fun(self: sf.SoundStream, gain: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setMinGain",
        [](sf::SoundStream& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMinGain(gain);
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setMaxGain", "fun(self: sf.SoundStream, gain: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setMaxGain",
        [](sf::SoundStream& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMaxGain(gain);
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setAttenuation", "fun(self: sf.SoundStream, attenuation: number)");
    lua_glue::BindCallable(type_sf__SoundStream, "setAttenuation",
        [](sf::SoundStream& self, float attenuation) {
            static_cast<sf::SoundSource&>(self).setAttenuation(attenuation);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setEffectProcessor", "fun(self: sf.SoundStream, effectProcessor: sf.SoundSource.EffectProcessor|nil)");
    lua_glue::BindCallable(type_sf__SoundStream, "setEffectProcessor",
        [](sf::SoundStream& self, lua_glue::Object effectProcessor) {
            self.setEffectProcessor(lua_sf::callback::from_object<sf::SoundSource::EffectProcessor, lua_sf::callback::InterleavedFloatTransformCodec>(effectProcessor, lua_sf::callback::CallbackOptions{"sf::SoundStream::setEffectProcessor.effectProcessor", true}));
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getPitch", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getPitch",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getPitch();
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getPan", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getPan",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getPan();
        },
        docs[19]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getVolume", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getVolume",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getVolume();
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "isSpatializationEnabled", "fun(self: sf.SoundStream): boolean");
    lua_glue::BindCallable(type_sf__SoundStream, "isSpatializationEnabled",
        [](const sf::SoundStream& self) -> bool {
            return static_cast<const sf::SoundSource&>(self).isSpatializationEnabled();
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getPosition", "fun(self: sf.SoundStream): sf.Vector3f");
    lua_glue::BindCallable(type_sf__SoundStream, "getPosition",
        [](const sf::SoundStream& self) -> sf::Vector3f {
            return static_cast<const sf::SoundSource&>(self).getPosition();
        },
        docs[22]
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getDirection", "fun(self: sf.SoundStream): sf.Vector3f");
    lua_glue::BindCallable(type_sf__SoundStream, "getDirection",
        [](const sf::SoundStream& self) -> sf::Vector3f {
            return static_cast<const sf::SoundSource&>(self).getDirection();
        },
        docs[23]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getCone", "fun(self: sf.SoundStream): sf.SoundSource.Cone");
    lua_glue::BindCallable(type_sf__SoundStream, "getCone",
        [](const sf::SoundStream& self) -> sf::SoundSource::Cone {
            return static_cast<const sf::SoundSource&>(self).getCone();
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getVelocity", "fun(self: sf.SoundStream): sf.Vector3f");
    lua_glue::BindCallable(type_sf__SoundStream, "getVelocity",
        [](const sf::SoundStream& self) -> sf::Vector3f {
            return static_cast<const sf::SoundSource&>(self).getVelocity();
        },
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getDopplerFactor", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getDopplerFactor",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getDopplerFactor();
        },
        docs[26]
    );
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getDirectionalAttenuationFactor", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getDirectionalAttenuationFactor",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getDirectionalAttenuationFactor();
        },
        docs[27]
    );
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "isRelativeToListener", "fun(self: sf.SoundStream): boolean");
    lua_glue::BindCallable(type_sf__SoundStream, "isRelativeToListener",
        [](const sf::SoundStream& self) -> bool {
            return static_cast<const sf::SoundSource&>(self).isRelativeToListener();
        },
        docs[28]
    );
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getMinDistance", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getMinDistance",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getMinDistance();
        },
        docs[29]
    );
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getMaxDistance", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getMaxDistance",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getMaxDistance();
        },
        docs[30]
    );
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getMinGain", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getMinGain",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getMinGain();
        },
        docs[31]
    );
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getMaxGain", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getMaxGain",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getMaxGain();
        },
        docs[32]
    );
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getAttenuation", "fun(self: sf.SoundStream): number");
    lua_glue::BindCallable(type_sf__SoundStream, "getAttenuation",
        [](const sf::SoundStream& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getAttenuation();
        },
        docs[33]
    );
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "play", "fun(self: sf.SoundStream)");
    lua_glue::BindCallable(type_sf__SoundStream, "play",
        [](sf::SoundStream& self) {
            self.play();
        },
        docs[38]
    );
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "pause", "fun(self: sf.SoundStream)");
    lua_glue::BindCallable(type_sf__SoundStream, "pause",
        [](sf::SoundStream& self) {
            self.pause();
        },
        docs[39]
    );
    LUASF_STUB_DOC(docs[40]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "stop", "fun(self: sf.SoundStream)");
    lua_glue::BindCallable(type_sf__SoundStream, "stop",
        [](sf::SoundStream& self) {
            self.stop();
        },
        docs[40]
    );
    LUASF_STUB_DOC(docs[44]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getStatus", "fun(self: sf.SoundStream): sf.SoundSource.Status");
    lua_glue::BindCallable(type_sf__SoundStream, "getStatus",
        [](const sf::SoundStream& self) -> sf::SoundSource::Status {
            return self.getStatus();
        },
        docs[44]
    );
    LUASF_STUB_DOC(docs[41]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getChannelCount", "fun(self: sf.SoundStream): integer");
    lua_glue::BindCallable(type_sf__SoundStream, "getChannelCount",
        [](const sf::SoundStream& self) -> unsigned int {
            return self.getChannelCount();
        },
        docs[41]
    );
    LUASF_STUB_DOC(docs[42]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getSampleRate", "fun(self: sf.SoundStream): integer");
    lua_glue::BindCallable(type_sf__SoundStream, "getSampleRate",
        [](const sf::SoundStream& self) -> unsigned int {
            return self.getSampleRate();
        },
        docs[42]
    );
    LUASF_STUB_DOC(docs[43]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getChannelMap", "fun(self: sf.SoundStream): sf.SoundChannel[]");
    lua_glue::BindCallable(type_sf__SoundStream, "getChannelMap",
        [lua](const sf::SoundStream& self) -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, self.getChannelMap());
        },
        docs[43],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[45]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setPlayingOffset", "fun(self: sf.SoundStream, timeOffset: sf.Time)");
    lua_glue::BindCallable(type_sf__SoundStream, "setPlayingOffset",
        [](sf::SoundStream& self, sf::Time timeOffset) {
            self.setPlayingOffset(timeOffset);
        },
        docs[45]
    );
    LUASF_STUB_DOC(docs[46]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "getPlayingOffset", "fun(self: sf.SoundStream): sf.Time");
    lua_glue::BindCallable(type_sf__SoundStream, "getPlayingOffset",
        [](const sf::SoundStream& self) -> sf::Time {
            return self.getPlayingOffset();
        },
        docs[46]
    );
    LUASF_STUB_DOC(docs[47]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "setLooping", "fun(self: sf.SoundStream, loop: boolean)");
    lua_glue::BindCallable(type_sf__SoundStream, "setLooping",
        [](sf::SoundStream& self, bool loop) {
            self.setLooping(loop);
        },
        docs[47]
    );
    LUASF_STUB_DOC(docs[48]);
    LUASF_STUB_FUNCTION("sf.SoundStream", "isLooping", "fun(self: sf.SoundStream): boolean");
    lua_glue::BindCallable(type_sf__SoundStream, "isLooping",
        [](const sf::SoundStream& self) -> bool {
            return self.isLooping();
        },
        docs[48]
    );
    auto type_sf__SoundStream__Chunk = lua_glue::BindClass<sf::SoundStream::Chunk>(table_sf__SoundStream, "Chunk");
    lua_glue::Table table_sf__SoundStream__Chunk = table_sf__SoundStream["Chunk"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::SoundStream::Chunk>(lua);
    LUASF_STUB_DOC(docs[49]);
    LUASF_STUB_CLASS("sf.SoundStream.Chunk");
    LUASF_STUB_DOC(docs[50]);
    LUASF_STUB_FIELD("samples", "integer");
    LUASF_STUB_DOC(docs[51]);
    LUASF_STUB_FIELD("sampleCount", "integer");
    LUASF_STUB_FUNCTION("sf.SoundStream.Chunk", "new", "fun(): sf.SoundStream.Chunk");
    lua_glue::BindCallable(type_sf__SoundStream__Chunk, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::SoundStream::Chunk>();
        }
    );
    lua_glue::BindAttr<const std::int16_t*>(type_sf__SoundStream__Chunk, "samples", &sf::SoundStream::Chunk::samples);
    lua_glue::BindProperty(type_sf__SoundStream__Chunk, "sampleCount",
        [](const sf::SoundStream::Chunk& self) {
            return self.sampleCount;
        },
        [](sf::SoundStream::Chunk& self, lua_sf::LuaIntegral<std::size_t> value) {
            self.sampleCount = value.value();
        }
    );
}
