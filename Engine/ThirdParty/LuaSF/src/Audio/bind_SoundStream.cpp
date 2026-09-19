#include "Audio/bind_SoundStream.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_SoundStream(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundStream = sf.new_usertype<sf::SoundStream>("SoundStream",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::SoundSource>()
    );
    sol::table table_sf__SoundStream = sf["SoundStream"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::SoundStream>(lua);
    sol::table native_bases_sf__SoundStream = lua.create_table();
    native_bases_sf__SoundStream.add(lua["sf"]["SoundSource"].get<sol::table>());
    table_sf__SoundStream.raw_set("__nativeBases", native_bases_sf__SoundStream);
    LUASF_STUB_DOC("\\brief Abstract base class for streamed audio sources");
    LUASF_STUB_CLASS("sf.SoundStream", "sf.SoundSource");
    // sf::SoundStream is abstract; constructor binding is omitted.
    LUASF_STUB_DOC("\\brief Set the pitch of the sound\n\nThe pitch represents the perceived fundamental frequency\nof a sound; thus you can make a sound more acute or grave\nby changing its pitch. A side effect of changing the pitch\nis to modify the playing speed of the sound as well.\nThe default value for the pitch is 1.\n\n\\param pitch New pitch to apply to the sound\n\n\\see `getPitch`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setPitch", "fun(self: sf.SoundStream, pitch: number)");
    type_sf__SoundStream.set_function("setPitch",
        [](sf::SoundStream& self, float pitch) {
            static_cast<sf::SoundSource&>(self).setPitch(pitch);
        }
    );
    LUASF_STUB_DOC("\\brief Set the pan of the sound\n\nUsing panning, a mono sound can be panned between\nstereo channels. When the pan is set to -1, the sound\nis played only on the left channel, when the pan is set\nto +1, the sound is played only on the right channel.\n\n\\param pan New pan to apply to the sound [-1, +1]\n\n\\see `getPan`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setPan", "fun(self: sf.SoundStream, pan: number)");
    type_sf__SoundStream.set_function("setPan",
        [](sf::SoundStream& self, float pan) {
            static_cast<sf::SoundSource&>(self).setPan(pan);
        }
    );
    LUASF_STUB_DOC("\\brief Set the volume of the sound\n\nThe volume is a value between 0 (mute) and 100 (full volume).\nThe default value for the volume is 100.\n\n\\param volume Volume of the sound\n\n\\see `getVolume`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setVolume", "fun(self: sf.SoundStream, volume: number)");
    type_sf__SoundStream.set_function("setVolume",
        [](sf::SoundStream& self, float volume) {
            static_cast<sf::SoundSource&>(self).setVolume(volume);
        }
    );
    LUASF_STUB_DOC("\\brief Set whether spatialization of the sound is enabled\n\nSpatialization is the application of various effects to\nsimulate a sound being emitted at a virtual position in\n3D space and exhibiting various physical phenomena such as\ndirectional attenuation and doppler shift.\n\n\\param enabled `true` to enable spatialization, `false` to disable\n\n\\see `isSpatializationEnabled`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setSpatializationEnabled", "fun(self: sf.SoundStream, enabled: boolean)");
    type_sf__SoundStream.set_function("setSpatializationEnabled",
        [](sf::SoundStream& self, bool enabled) {
            static_cast<sf::SoundSource&>(self).setSpatializationEnabled(enabled);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D position of the sound in the audio scene\n\nOnly sounds with one channel (mono sounds) can be\nspatialized.\nThe default position of a sound is (0, 0, 0).\n\n\\param position Position of the sound in the scene\n\n\\see `getPosition`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setPosition", "fun(self: sf.SoundStream, position: sf.Vector3f)");
    type_sf__SoundStream.set_function("setPosition",
        [](sf::SoundStream& self, const sf::Vector3f& position) {
            static_cast<sf::SoundSource&>(self).setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D direction of the sound in the audio scene\n\nThe direction defines where the sound source is facing\nin 3D space. It will affect how the sound is attenuated\nif facing away from the listener.\nThe default direction of a sound is (0, 0, -1).\n\n\\param direction Direction of the sound in the scene\n\n\\see `getDirection`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setDirection", "fun(self: sf.SoundStream, direction: sf.Vector3f)");
    type_sf__SoundStream.set_function("setDirection",
        [](sf::SoundStream& self, const sf::Vector3f& direction) {
            static_cast<sf::SoundSource&>(self).setDirection(direction);
        }
    );
    LUASF_STUB_DOC("\\brief Set the cone properties of the sound in the audio scene\n\nThe cone defines how directional attenuation is applied.\nThe default cone of a sound is (2 * PI, 2 * PI, 1).\n\n\\param cone Cone properties of the sound in the scene\n\n\\see `getCone`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setCone", "fun(self: sf.SoundStream, cone: sf.SoundSource.Cone)");
    type_sf__SoundStream.set_function("setCone",
        [](sf::SoundStream& self, const sf::SoundSource::Cone& cone) {
            static_cast<sf::SoundSource&>(self).setCone(cone);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D velocity of the sound in the audio scene\n\nThe velocity is used to determine how to doppler shift\nthe sound. Sounds moving towards the listener will be\nperceived to have a higher pitch and sounds moving away\nfrom the listener will be perceived to have a lower pitch.\n\n\\param velocity Velocity of the sound in the scene\n\n\\see `getVelocity`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setVelocity", "fun(self: sf.SoundStream, velocity: sf.Vector3f)");
    type_sf__SoundStream.set_function("setVelocity",
        [](sf::SoundStream& self, const sf::Vector3f& velocity) {
            static_cast<sf::SoundSource&>(self).setVelocity(velocity);
        }
    );
    LUASF_STUB_DOC("\\brief Set the doppler factor of the sound\n\nThe doppler factor determines how strong the doppler\nshift will be.\n\n\\param factor New doppler factor to apply to the sound\n\n\\see `getDopplerFactor`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setDopplerFactor", "fun(self: sf.SoundStream, factor: number)");
    type_sf__SoundStream.set_function("setDopplerFactor",
        [](sf::SoundStream& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDopplerFactor(factor);
        }
    );
    LUASF_STUB_DOC("\\brief Set the directional attenuation factor of the sound\n\nDepending on the virtual position of an output channel\nrelative to the listener (such as in surround sound\nsetups), sounds will be attenuated when emitting them\nfrom certain channels. This factor determines how strong\nthe attenuation based on output channel position\nrelative to the listener is.\n\n\\param factor New directional attenuation factor to apply to the sound\n\n\\see `getDirectionalAttenuationFactor`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setDirectionalAttenuationFactor", "fun(self: sf.SoundStream, factor: number)");
    type_sf__SoundStream.set_function("setDirectionalAttenuationFactor",
        [](sf::SoundStream& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDirectionalAttenuationFactor(factor);
        }
    );
    LUASF_STUB_DOC("\\brief Make the sound's position relative to the listener or absolute\n\nMaking a sound relative to the listener will ensure that it will always\nbe played the same way regardless of the position of the listener.\nThis can be useful for non-spatialized sounds, sounds that are\nproduced by the listener, or sounds attached to it.\nThe default value is `false` (position is absolute).\n\n\\param relative `true` to set the position relative, `false` to set it absolute\n\n\\see `isRelativeToListener`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setRelativeToListener", "fun(self: sf.SoundStream, relative: boolean)");
    type_sf__SoundStream.set_function("setRelativeToListener",
        [](sf::SoundStream& self, bool relative) {
            static_cast<sf::SoundSource&>(self).setRelativeToListener(relative);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum distance of the sound\n\nThe \"minimum distance\" of a sound is the maximum\ndistance at which it is heard at its maximum volume. Further\nthan the minimum distance, it will start to fade out according\nto its attenuation factor. A value of 0 (\"inside the head\nof the listener\") is an invalid value and is forbidden.\nThe default value of the minimum distance is 1.\n\n\\param distance New minimum distance of the sound\n\n\\see `getMinDistance`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setMinDistance", "fun(self: sf.SoundStream, distance: number)");
    type_sf__SoundStream.set_function("setMinDistance",
        [](sf::SoundStream& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMinDistance(distance);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum distance of the sound\n\nThe \"maximum distance\" of a sound is the minimum\ndistance at which it is heard at its minimum volume. Closer\nthan the maximum distance, it will start to fade in according\nto its attenuation factor.\nThe default value of the maximum distance is the maximum\nvalue a float can represent.\n\n\\param distance New maximum distance of the sound\n\n\\see `getMaxDistance`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setMaxDistance", "fun(self: sf.SoundStream, distance: number)");
    type_sf__SoundStream.set_function("setMaxDistance",
        [](sf::SoundStream& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMaxDistance(distance);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum gain of the sound\n\nWhen the sound is further away from the listener than\nthe \"maximum distance\" the attenuated gain is clamped\nso it cannot go below the minimum gain value.\n\n\\param gain New minimum gain of the sound\n\n\\see `getMinGain`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setMinGain", "fun(self: sf.SoundStream, gain: number)");
    type_sf__SoundStream.set_function("setMinGain",
        [](sf::SoundStream& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMinGain(gain);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum gain of the sound\n\nWhen the sound is closer from the listener than\nthe \"minimum distance\" the attenuated gain is clamped\nso it cannot go above the maximum gain value.\n\n\\param gain New maximum gain of the sound\n\n\\see `getMaxGain`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setMaxGain", "fun(self: sf.SoundStream, gain: number)");
    type_sf__SoundStream.set_function("setMaxGain",
        [](sf::SoundStream& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMaxGain(gain);
        }
    );
    LUASF_STUB_DOC("\\brief Set the attenuation factor of the sound\n\nThe attenuation is a multiplicative factor which makes\nthe sound more or less loud according to its distance\nfrom the listener. An attenuation of 0 will produce a\nnon-attenuated sound, i.e. its volume will always be the same\nwhether it is heard from near or from far. On the other hand,\nan attenuation value such as 100 will make the sound fade out\nvery quickly as it gets further from the listener.\nThe default value of the attenuation is 1.\n\n\\param attenuation New attenuation factor of the sound\n\n\\see `getAttenuation`, `setMinDistance`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setAttenuation", "fun(self: sf.SoundStream, attenuation: number)");
    type_sf__SoundStream.set_function("setAttenuation",
        [](sf::SoundStream& self, float attenuation) {
            static_cast<sf::SoundSource&>(self).setAttenuation(attenuation);
        }
    );
    LUASF_STUB_DOC("\\brief Set the effect processor to be applied to the sound\n\nThe effect processor is a callable that will be called\nwith sound data to be processed.\n\n\\param effectProcessor The effect processor to attach to this sound, attach an empty processor to disable processing");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setEffectProcessor", "fun(self: sf.SoundStream, effectProcessor: sf.SoundSource.EffectProcessor|nil)");
    type_sf__SoundStream.set_function("setEffectProcessor",
        [](sf::SoundStream& self, sol::object effectProcessor) {
            self.setEffectProcessor(lua_sf::callback::from_object<sf::SoundSource::EffectProcessor, lua_sf::callback::InterleavedFloatTransformCodec>(effectProcessor, lua_sf::callback::CallbackOptions{"sf::SoundStream::setEffectProcessor.effectProcessor", true}));
        }
    );
    LUASF_STUB_DOC("\\brief Get the pitch of the sound\n\n\\return Pitch of the sound\n\n\\see `setPitch`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getPitch", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getPitch",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getPitch();
        }
    );
    LUASF_STUB_DOC("\\brief Get the pan of the sound\n\n\\return Pan of the sound\n\n\\see `setPan`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getPan", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getPan",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getPan();
        }
    );
    LUASF_STUB_DOC("\\brief Get the volume of the sound\n\n\\return Volume of the sound, in the range [0, 100]\n\n\\see `setVolume`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getVolume", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getVolume",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getVolume();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether spatialization of the sound is enabled\n\n\\return `true` if spatialization is enabled, `false` if it's disabled\n\n\\see `setSpatializationEnabled`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "isSpatializationEnabled", "fun(self: sf.SoundStream): boolean");
    type_sf__SoundStream.set_function("isSpatializationEnabled",
        [](sf::SoundStream& self) -> bool {
            return static_cast<sf::SoundSource&>(self).isSpatializationEnabled();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D position of the sound in the audio scene\n\n\\return Position of the sound\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getPosition", "fun(self: sf.SoundStream): sf.Vector3f");
    type_sf__SoundStream.set_function("getPosition",
        [](sf::SoundStream& self) -> sf::Vector3f {
            return static_cast<sf::SoundSource&>(self).getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D direction of the sound in the audio scene\n\n\\return Direction of the sound\n\n\\see `setDirection`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getDirection", "fun(self: sf.SoundStream): sf.Vector3f");
    type_sf__SoundStream.set_function("getDirection",
        [](sf::SoundStream& self) -> sf::Vector3f {
            return static_cast<sf::SoundSource&>(self).getDirection();
        }
    );
    LUASF_STUB_DOC("\\brief Get the cone properties of the sound in the audio scene\n\n\\return Cone properties of the sound\n\n\\see `setCone`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getCone", "fun(self: sf.SoundStream): sf.SoundSource.Cone");
    type_sf__SoundStream.set_function("getCone",
        [](sf::SoundStream& self) -> sf::SoundSource::Cone {
            return static_cast<sf::SoundSource&>(self).getCone();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D velocity of the sound in the audio scene\n\n\\return Velocity of the sound\n\n\\see `setVelocity`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getVelocity", "fun(self: sf.SoundStream): sf.Vector3f");
    type_sf__SoundStream.set_function("getVelocity",
        [](sf::SoundStream& self) -> sf::Vector3f {
            return static_cast<sf::SoundSource&>(self).getVelocity();
        }
    );
    LUASF_STUB_DOC("\\brief Get the doppler factor of the sound\n\n\\return Doppler factor of the sound\n\n\\see `setDopplerFactor`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getDopplerFactor", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getDopplerFactor",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getDopplerFactor();
        }
    );
    LUASF_STUB_DOC("\\brief Get the directional attenuation factor of the sound\n\n\\return Directional attenuation factor of the sound\n\n\\see `setDirectionalAttenuationFactor`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getDirectionalAttenuationFactor", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getDirectionalAttenuationFactor",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getDirectionalAttenuationFactor();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the sound's position is relative to the\nlistener or is absolute\n\n\\return `true` if the position is relative, `false` if it's absolute\n\n\\see `setRelativeToListener`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "isRelativeToListener", "fun(self: sf.SoundStream): boolean");
    type_sf__SoundStream.set_function("isRelativeToListener",
        [](sf::SoundStream& self) -> bool {
            return static_cast<sf::SoundSource&>(self).isRelativeToListener();
        }
    );
    LUASF_STUB_DOC("\\brief Get the minimum distance of the sound\n\n\\return Minimum distance of the sound\n\n\\see `setMinDistance`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getMinDistance", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getMinDistance",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMinDistance();
        }
    );
    LUASF_STUB_DOC("\\brief Get the maximum distance of the sound\n\n\\return Maximum distance of the sound\n\n\\see `setMaxDistance`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getMaxDistance", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getMaxDistance",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMaxDistance();
        }
    );
    LUASF_STUB_DOC("\\brief Get the minimum gain of the sound\n\n\\return Minimum gain of the sound\n\n\\see `setMinGain`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getMinGain", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getMinGain",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMinGain();
        }
    );
    LUASF_STUB_DOC("\\brief Get the maximum gain of the sound\n\n\\return Maximum gain of the sound\n\n\\see `setMaxGain`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getMaxGain", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getMaxGain",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMaxGain();
        }
    );
    LUASF_STUB_DOC("\\brief Get the attenuation factor of the sound\n\n\\return Attenuation factor of the sound\n\n\\see `setAttenuation`, `getMinDistance`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getAttenuation", "fun(self: sf.SoundStream): number");
    type_sf__SoundStream.set_function("getAttenuation",
        [](sf::SoundStream& self) -> float {
            return static_cast<sf::SoundSource&>(self).getAttenuation();
        }
    );
    LUASF_STUB_DOC("\\brief Start or resume playing the audio stream\n\nThis function starts the stream if it was stopped, resumes\nit if it was paused, and restarts it from the beginning if\nit was already playing.\nThis function uses its own thread so that it doesn't block\nthe rest of the program while the stream is played.\n\n\\see `pause`, `stop`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "play", "fun(self: sf.SoundStream)");
    type_sf__SoundStream.set_function("play",
        [](sf::SoundStream& self) {
            self.play();
        }
    );
    LUASF_STUB_DOC("\\brief Pause the audio stream\n\nThis function pauses the stream if it was playing,\notherwise (stream already paused or stopped) it has no effect.\n\n\\see `play`, `stop`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "pause", "fun(self: sf.SoundStream)");
    type_sf__SoundStream.set_function("pause",
        [](sf::SoundStream& self) {
            self.pause();
        }
    );
    LUASF_STUB_DOC("\\brief Stop playing the audio stream\n\nThis function stops the stream if it was playing or paused,\nand does nothing if it was already stopped.\nIt also resets the playing position (unlike `pause()`).\n\n\\see `play`, `pause`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "stop", "fun(self: sf.SoundStream)");
    type_sf__SoundStream.set_function("stop",
        [](sf::SoundStream& self) {
            self.stop();
        }
    );
    LUASF_STUB_DOC("\\brief Get the current status of the stream (stopped, paused, playing)\n\n\\return Current status");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getStatus", "fun(self: sf.SoundStream): sf.SoundSource.Status");
    type_sf__SoundStream.set_function("getStatus",
        [](sf::SoundStream& self) -> sf::SoundSource::Status {
            return self.getStatus();
        }
    );
    LUASF_STUB_DOC("\\brief Return the number of channels of the stream\n\n1 channel means a mono sound, 2 means stereo, etc.\n\n\\return Number of channels");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getChannelCount", "fun(self: sf.SoundStream): integer");
    type_sf__SoundStream.set_function("getChannelCount",
        [](sf::SoundStream& self) -> unsigned int {
            return self.getChannelCount();
        }
    );
    LUASF_STUB_DOC("\\brief Get the stream sample rate of the stream\n\nThe sample rate is the number of audio samples played per\nsecond. The higher, the better the quality.\n\n\\return Sample rate, in number of samples per second");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getSampleRate", "fun(self: sf.SoundStream): integer");
    type_sf__SoundStream.set_function("getSampleRate",
        [](sf::SoundStream& self) -> unsigned int {
            return self.getSampleRate();
        }
    );
    LUASF_STUB_DOC("\\brief Get the map of position in sample frame to sound channel\n\nThis is used to map a sample in the sample stream to a\nposition during spatialization.\n\n\\return Map of position in sample frame to sound channel");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getChannelMap", "fun(self: sf.SoundStream): sf.SoundChannel[]");
    type_sf__SoundStream.set_function("getChannelMap",
        sol::policies(
            [lua](sf::SoundStream& self) -> sol::object {
                return lua_sf::vector_to_object(lua, self.getChannelMap());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Change the current playing position of the stream\n\nThe playing position can be changed when the stream is\neither paused or playing. Changing the playing position\nwhen the stream is stopped has no effect, since playing\nthe stream would reset its position.\n\n\\param timeOffset New playing position, from the beginning of the stream\n\n\\see `getPlayingOffset`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setPlayingOffset", "fun(self: sf.SoundStream, timeOffset: sf.Time)");
    type_sf__SoundStream.set_function("setPlayingOffset",
        [](sf::SoundStream& self, sf::Time timeOffset) {
            self.setPlayingOffset(timeOffset);
        }
    );
    LUASF_STUB_DOC("\\brief Get the current playing position of the stream\n\n\\return Current playing position, from the beginning of the stream\n\n\\see `setPlayingOffset`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "getPlayingOffset", "fun(self: sf.SoundStream): sf.Time");
    type_sf__SoundStream.set_function("getPlayingOffset",
        [](sf::SoundStream& self) -> sf::Time {
            return self.getPlayingOffset();
        }
    );
    LUASF_STUB_DOC("\\brief Set whether or not the stream should loop after reaching the end\n\nIf set, the stream will restart from beginning after\nreaching the end and so on, until it is stopped or\n`setLooping(false)` is called.\nThe default looping state for streams is `false`.\n\n\\param loop `true` to play in loop, `false` to play once\n\n\\see `isLooping`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "setLooping", "fun(self: sf.SoundStream, loop: boolean)");
    type_sf__SoundStream.set_function("setLooping",
        [](sf::SoundStream& self, bool loop) {
            self.setLooping(loop);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether or not the stream is in loop mode\n\n\\return `true` if the stream is looping, `false` otherwise\n\n\\see `setLooping`");
    LUASF_STUB_FUNCTION("sf.SoundStream", "isLooping", "fun(self: sf.SoundStream): boolean");
    type_sf__SoundStream.set_function("isLooping",
        [](sf::SoundStream& self) -> bool {
            return self.isLooping();
        }
    );
    auto type_sf__SoundStream__Chunk = table_sf__SoundStream.new_usertype<sf::SoundStream::Chunk>("Chunk", sol::no_constructor);
    sol::table table_sf__SoundStream__Chunk = table_sf__SoundStream["Chunk"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::SoundStream::Chunk>(lua);
    LUASF_STUB_DOC("\\brief Structure defining a chunk of audio data to stream");
    LUASF_STUB_CLASS("sf.SoundStream.Chunk");
    LUASF_STUB_DOC("Pointer to the audio samples");
    LUASF_STUB_FIELD("samples", "integer");
    LUASF_STUB_DOC("Number of samples pointed by Samples");
    LUASF_STUB_FIELD("sampleCount", "integer");
    LUASF_STUB_FUNCTION("sf.SoundStream.Chunk", "new", "fun(): sf.SoundStream.Chunk");
    type_sf__SoundStream__Chunk.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::SoundStream::Chunk>();
        }
    ));
    type_sf__SoundStream__Chunk["samples"] = sol::policies(&sf::SoundStream::Chunk::samples, sol::self_dependency{});
    type_sf__SoundStream__Chunk.set("sampleCount", sol::property(
        [](sf::SoundStream::Chunk& self) {
            return self.sampleCount;
        },
        [](sf::SoundStream::Chunk& self, lua_sf::LuaIntegral<std::size_t> value) {
            self.sampleCount = value.value();
        }
    ));
}
