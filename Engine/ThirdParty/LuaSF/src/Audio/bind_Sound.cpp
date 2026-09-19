#include "Audio/bind_Sound.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Sound(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Sound = sf.new_usertype<sf::Sound>("Sound",
        sol::no_constructor,
        sol::base_classes, sol::bases<sf::SoundSource>()
    );
    sol::table table_sf__Sound = sf["Sound"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Sound>(lua);
    sol::table native_bases_sf__Sound = lua.create_table();
    native_bases_sf__Sound.add(lua["sf"]["SoundSource"].get<sol::table>());
    table_sf__Sound.raw_set("__nativeBases", native_bases_sf__Sound);
    LUASF_STUB_DOC("\\brief Regular sound that can be played in the audio environment");
    LUASF_STUB_CLASS("sf.Sound", "sf.SoundSource");
    LUASF_STUB_DOC("\\brief Construct the sound with a buffer\n\n\\param buffer Sound buffer containing the audio data to play with the sound");
    LUASF_STUB_FUNCTION("sf.Sound", "new", "fun(buffer: sf.SoundBuffer): sf.Sound");
    type_sf__Sound.set_function("new", sol::factories(
        [](const sf::SoundBuffer& buffer) {
            return lua_sf::makeLuaSharedObject<sf::Sound>(buffer);
        }
    ));
    LUASF_STUB_DOC("\\brief Set the pitch of the sound\n\nThe pitch represents the perceived fundamental frequency\nof a sound; thus you can make a sound more acute or grave\nby changing its pitch. A side effect of changing the pitch\nis to modify the playing speed of the sound as well.\nThe default value for the pitch is 1.\n\n\\param pitch New pitch to apply to the sound\n\n\\see `getPitch`");
    LUASF_STUB_FUNCTION("sf.Sound", "setPitch", "fun(self: sf.Sound, pitch: number)");
    type_sf__Sound.set_function("setPitch",
        [](sf::Sound& self, float pitch) {
            static_cast<sf::SoundSource&>(self).setPitch(pitch);
        }
    );
    LUASF_STUB_DOC("\\brief Set the pan of the sound\n\nUsing panning, a mono sound can be panned between\nstereo channels. When the pan is set to -1, the sound\nis played only on the left channel, when the pan is set\nto +1, the sound is played only on the right channel.\n\n\\param pan New pan to apply to the sound [-1, +1]\n\n\\see `getPan`");
    LUASF_STUB_FUNCTION("sf.Sound", "setPan", "fun(self: sf.Sound, pan: number)");
    type_sf__Sound.set_function("setPan",
        [](sf::Sound& self, float pan) {
            static_cast<sf::SoundSource&>(self).setPan(pan);
        }
    );
    LUASF_STUB_DOC("\\brief Set the volume of the sound\n\nThe volume is a value between 0 (mute) and 100 (full volume).\nThe default value for the volume is 100.\n\n\\param volume Volume of the sound\n\n\\see `getVolume`");
    LUASF_STUB_FUNCTION("sf.Sound", "setVolume", "fun(self: sf.Sound, volume: number)");
    type_sf__Sound.set_function("setVolume",
        [](sf::Sound& self, float volume) {
            static_cast<sf::SoundSource&>(self).setVolume(volume);
        }
    );
    LUASF_STUB_DOC("\\brief Set whether spatialization of the sound is enabled\n\nSpatialization is the application of various effects to\nsimulate a sound being emitted at a virtual position in\n3D space and exhibiting various physical phenomena such as\ndirectional attenuation and doppler shift.\n\n\\param enabled `true` to enable spatialization, `false` to disable\n\n\\see `isSpatializationEnabled`");
    LUASF_STUB_FUNCTION("sf.Sound", "setSpatializationEnabled", "fun(self: sf.Sound, enabled: boolean)");
    type_sf__Sound.set_function("setSpatializationEnabled",
        [](sf::Sound& self, bool enabled) {
            static_cast<sf::SoundSource&>(self).setSpatializationEnabled(enabled);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D position of the sound in the audio scene\n\nOnly sounds with one channel (mono sounds) can be\nspatialized.\nThe default position of a sound is (0, 0, 0).\n\n\\param position Position of the sound in the scene\n\n\\see `getPosition`");
    LUASF_STUB_FUNCTION("sf.Sound", "setPosition", "fun(self: sf.Sound, position: sf.Vector3f)");
    type_sf__Sound.set_function("setPosition",
        [](sf::Sound& self, const sf::Vector3f& position) {
            static_cast<sf::SoundSource&>(self).setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D direction of the sound in the audio scene\n\nThe direction defines where the sound source is facing\nin 3D space. It will affect how the sound is attenuated\nif facing away from the listener.\nThe default direction of a sound is (0, 0, -1).\n\n\\param direction Direction of the sound in the scene\n\n\\see `getDirection`");
    LUASF_STUB_FUNCTION("sf.Sound", "setDirection", "fun(self: sf.Sound, direction: sf.Vector3f)");
    type_sf__Sound.set_function("setDirection",
        [](sf::Sound& self, const sf::Vector3f& direction) {
            static_cast<sf::SoundSource&>(self).setDirection(direction);
        }
    );
    LUASF_STUB_DOC("\\brief Set the cone properties of the sound in the audio scene\n\nThe cone defines how directional attenuation is applied.\nThe default cone of a sound is (2 * PI, 2 * PI, 1).\n\n\\param cone Cone properties of the sound in the scene\n\n\\see `getCone`");
    LUASF_STUB_FUNCTION("sf.Sound", "setCone", "fun(self: sf.Sound, cone: sf.SoundSource.Cone)");
    type_sf__Sound.set_function("setCone",
        [](sf::Sound& self, const sf::SoundSource::Cone& cone) {
            static_cast<sf::SoundSource&>(self).setCone(cone);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D velocity of the sound in the audio scene\n\nThe velocity is used to determine how to doppler shift\nthe sound. Sounds moving towards the listener will be\nperceived to have a higher pitch and sounds moving away\nfrom the listener will be perceived to have a lower pitch.\n\n\\param velocity Velocity of the sound in the scene\n\n\\see `getVelocity`");
    LUASF_STUB_FUNCTION("sf.Sound", "setVelocity", "fun(self: sf.Sound, velocity: sf.Vector3f)");
    type_sf__Sound.set_function("setVelocity",
        [](sf::Sound& self, const sf::Vector3f& velocity) {
            static_cast<sf::SoundSource&>(self).setVelocity(velocity);
        }
    );
    LUASF_STUB_DOC("\\brief Set the doppler factor of the sound\n\nThe doppler factor determines how strong the doppler\nshift will be.\n\n\\param factor New doppler factor to apply to the sound\n\n\\see `getDopplerFactor`");
    LUASF_STUB_FUNCTION("sf.Sound", "setDopplerFactor", "fun(self: sf.Sound, factor: number)");
    type_sf__Sound.set_function("setDopplerFactor",
        [](sf::Sound& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDopplerFactor(factor);
        }
    );
    LUASF_STUB_DOC("\\brief Set the directional attenuation factor of the sound\n\nDepending on the virtual position of an output channel\nrelative to the listener (such as in surround sound\nsetups), sounds will be attenuated when emitting them\nfrom certain channels. This factor determines how strong\nthe attenuation based on output channel position\nrelative to the listener is.\n\n\\param factor New directional attenuation factor to apply to the sound\n\n\\see `getDirectionalAttenuationFactor`");
    LUASF_STUB_FUNCTION("sf.Sound", "setDirectionalAttenuationFactor", "fun(self: sf.Sound, factor: number)");
    type_sf__Sound.set_function("setDirectionalAttenuationFactor",
        [](sf::Sound& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDirectionalAttenuationFactor(factor);
        }
    );
    LUASF_STUB_DOC("\\brief Make the sound's position relative to the listener or absolute\n\nMaking a sound relative to the listener will ensure that it will always\nbe played the same way regardless of the position of the listener.\nThis can be useful for non-spatialized sounds, sounds that are\nproduced by the listener, or sounds attached to it.\nThe default value is `false` (position is absolute).\n\n\\param relative `true` to set the position relative, `false` to set it absolute\n\n\\see `isRelativeToListener`");
    LUASF_STUB_FUNCTION("sf.Sound", "setRelativeToListener", "fun(self: sf.Sound, relative: boolean)");
    type_sf__Sound.set_function("setRelativeToListener",
        [](sf::Sound& self, bool relative) {
            static_cast<sf::SoundSource&>(self).setRelativeToListener(relative);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum distance of the sound\n\nThe \"minimum distance\" of a sound is the maximum\ndistance at which it is heard at its maximum volume. Further\nthan the minimum distance, it will start to fade out according\nto its attenuation factor. A value of 0 (\"inside the head\nof the listener\") is an invalid value and is forbidden.\nThe default value of the minimum distance is 1.\n\n\\param distance New minimum distance of the sound\n\n\\see `getMinDistance`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.Sound", "setMinDistance", "fun(self: sf.Sound, distance: number)");
    type_sf__Sound.set_function("setMinDistance",
        [](sf::Sound& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMinDistance(distance);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum distance of the sound\n\nThe \"maximum distance\" of a sound is the minimum\ndistance at which it is heard at its minimum volume. Closer\nthan the maximum distance, it will start to fade in according\nto its attenuation factor.\nThe default value of the maximum distance is the maximum\nvalue a float can represent.\n\n\\param distance New maximum distance of the sound\n\n\\see `getMaxDistance`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.Sound", "setMaxDistance", "fun(self: sf.Sound, distance: number)");
    type_sf__Sound.set_function("setMaxDistance",
        [](sf::Sound& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMaxDistance(distance);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum gain of the sound\n\nWhen the sound is further away from the listener than\nthe \"maximum distance\" the attenuated gain is clamped\nso it cannot go below the minimum gain value.\n\n\\param gain New minimum gain of the sound\n\n\\see `getMinGain`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.Sound", "setMinGain", "fun(self: sf.Sound, gain: number)");
    type_sf__Sound.set_function("setMinGain",
        [](sf::Sound& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMinGain(gain);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum gain of the sound\n\nWhen the sound is closer from the listener than\nthe \"minimum distance\" the attenuated gain is clamped\nso it cannot go above the maximum gain value.\n\n\\param gain New maximum gain of the sound\n\n\\see `getMaxGain`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.Sound", "setMaxGain", "fun(self: sf.Sound, gain: number)");
    type_sf__Sound.set_function("setMaxGain",
        [](sf::Sound& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMaxGain(gain);
        }
    );
    LUASF_STUB_DOC("\\brief Set the attenuation factor of the sound\n\nThe attenuation is a multiplicative factor which makes\nthe sound more or less loud according to its distance\nfrom the listener. An attenuation of 0 will produce a\nnon-attenuated sound, i.e. its volume will always be the same\nwhether it is heard from near or from far. On the other hand,\nan attenuation value such as 100 will make the sound fade out\nvery quickly as it gets further from the listener.\nThe default value of the attenuation is 1.\n\n\\param attenuation New attenuation factor of the sound\n\n\\see `getAttenuation`, `setMinDistance`");
    LUASF_STUB_FUNCTION("sf.Sound", "setAttenuation", "fun(self: sf.Sound, attenuation: number)");
    type_sf__Sound.set_function("setAttenuation",
        [](sf::Sound& self, float attenuation) {
            static_cast<sf::SoundSource&>(self).setAttenuation(attenuation);
        }
    );
    LUASF_STUB_DOC("\\brief Set the effect processor to be applied to the sound\n\nThe effect processor is a callable that will be called\nwith sound data to be processed.\n\n\\param effectProcessor The effect processor to attach to this sound, attach an empty processor to disable processing");
    LUASF_STUB_FUNCTION("sf.Sound", "setEffectProcessor", "fun(self: sf.Sound, effectProcessor: sf.SoundSource.EffectProcessor|nil)");
    type_sf__Sound.set_function("setEffectProcessor",
        [](sf::Sound& self, sol::object effectProcessor) {
            self.setEffectProcessor(lua_sf::callback::from_object<sf::SoundSource::EffectProcessor, lua_sf::callback::InterleavedFloatTransformCodec>(effectProcessor, lua_sf::callback::CallbackOptions{"sf::Sound::setEffectProcessor.effectProcessor", true}));
        }
    );
    LUASF_STUB_DOC("\\brief Get the pitch of the sound\n\n\\return Pitch of the sound\n\n\\see `setPitch`");
    LUASF_STUB_FUNCTION("sf.Sound", "getPitch", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getPitch",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getPitch();
        }
    );
    LUASF_STUB_DOC("\\brief Get the pan of the sound\n\n\\return Pan of the sound\n\n\\see `setPan`");
    LUASF_STUB_FUNCTION("sf.Sound", "getPan", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getPan",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getPan();
        }
    );
    LUASF_STUB_DOC("\\brief Get the volume of the sound\n\n\\return Volume of the sound, in the range [0, 100]\n\n\\see `setVolume`");
    LUASF_STUB_FUNCTION("sf.Sound", "getVolume", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getVolume",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getVolume();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether spatialization of the sound is enabled\n\n\\return `true` if spatialization is enabled, `false` if it's disabled\n\n\\see `setSpatializationEnabled`");
    LUASF_STUB_FUNCTION("sf.Sound", "isSpatializationEnabled", "fun(self: sf.Sound): boolean");
    type_sf__Sound.set_function("isSpatializationEnabled",
        [](sf::Sound& self) -> bool {
            return static_cast<sf::SoundSource&>(self).isSpatializationEnabled();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D position of the sound in the audio scene\n\n\\return Position of the sound\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.Sound", "getPosition", "fun(self: sf.Sound): sf.Vector3f");
    type_sf__Sound.set_function("getPosition",
        [](sf::Sound& self) -> sf::Vector3f {
            return static_cast<sf::SoundSource&>(self).getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D direction of the sound in the audio scene\n\n\\return Direction of the sound\n\n\\see `setDirection`");
    LUASF_STUB_FUNCTION("sf.Sound", "getDirection", "fun(self: sf.Sound): sf.Vector3f");
    type_sf__Sound.set_function("getDirection",
        [](sf::Sound& self) -> sf::Vector3f {
            return static_cast<sf::SoundSource&>(self).getDirection();
        }
    );
    LUASF_STUB_DOC("\\brief Get the cone properties of the sound in the audio scene\n\n\\return Cone properties of the sound\n\n\\see `setCone`");
    LUASF_STUB_FUNCTION("sf.Sound", "getCone", "fun(self: sf.Sound): sf.SoundSource.Cone");
    type_sf__Sound.set_function("getCone",
        [](sf::Sound& self) -> sf::SoundSource::Cone {
            return static_cast<sf::SoundSource&>(self).getCone();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D velocity of the sound in the audio scene\n\n\\return Velocity of the sound\n\n\\see `setVelocity`");
    LUASF_STUB_FUNCTION("sf.Sound", "getVelocity", "fun(self: sf.Sound): sf.Vector3f");
    type_sf__Sound.set_function("getVelocity",
        [](sf::Sound& self) -> sf::Vector3f {
            return static_cast<sf::SoundSource&>(self).getVelocity();
        }
    );
    LUASF_STUB_DOC("\\brief Get the doppler factor of the sound\n\n\\return Doppler factor of the sound\n\n\\see `setDopplerFactor`");
    LUASF_STUB_FUNCTION("sf.Sound", "getDopplerFactor", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getDopplerFactor",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getDopplerFactor();
        }
    );
    LUASF_STUB_DOC("\\brief Get the directional attenuation factor of the sound\n\n\\return Directional attenuation factor of the sound\n\n\\see `setDirectionalAttenuationFactor`");
    LUASF_STUB_FUNCTION("sf.Sound", "getDirectionalAttenuationFactor", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getDirectionalAttenuationFactor",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getDirectionalAttenuationFactor();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the sound's position is relative to the\nlistener or is absolute\n\n\\return `true` if the position is relative, `false` if it's absolute\n\n\\see `setRelativeToListener`");
    LUASF_STUB_FUNCTION("sf.Sound", "isRelativeToListener", "fun(self: sf.Sound): boolean");
    type_sf__Sound.set_function("isRelativeToListener",
        [](sf::Sound& self) -> bool {
            return static_cast<sf::SoundSource&>(self).isRelativeToListener();
        }
    );
    LUASF_STUB_DOC("\\brief Get the minimum distance of the sound\n\n\\return Minimum distance of the sound\n\n\\see `setMinDistance`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.Sound", "getMinDistance", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getMinDistance",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMinDistance();
        }
    );
    LUASF_STUB_DOC("\\brief Get the maximum distance of the sound\n\n\\return Maximum distance of the sound\n\n\\see `setMaxDistance`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.Sound", "getMaxDistance", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getMaxDistance",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMaxDistance();
        }
    );
    LUASF_STUB_DOC("\\brief Get the minimum gain of the sound\n\n\\return Minimum gain of the sound\n\n\\see `setMinGain`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.Sound", "getMinGain", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getMinGain",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMinGain();
        }
    );
    LUASF_STUB_DOC("\\brief Get the maximum gain of the sound\n\n\\return Maximum gain of the sound\n\n\\see `setMaxGain`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.Sound", "getMaxGain", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getMaxGain",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getMaxGain();
        }
    );
    LUASF_STUB_DOC("\\brief Get the attenuation factor of the sound\n\n\\return Attenuation factor of the sound\n\n\\see `setAttenuation`, `getMinDistance`");
    LUASF_STUB_FUNCTION("sf.Sound", "getAttenuation", "fun(self: sf.Sound): number");
    type_sf__Sound.set_function("getAttenuation",
        [](sf::Sound& self) -> float {
            return static_cast<sf::SoundSource&>(self).getAttenuation();
        }
    );
    LUASF_STUB_DOC("\\brief Start or resume playing the sound\n\nThis function starts the stream if it was stopped, resumes\nit if it was paused, and restarts it from beginning if it\nwas it already playing.\nThis function uses its own thread so that it doesn't block\nthe rest of the program while the sound is played.\n\n\\see `pause`, `stop`");
    LUASF_STUB_FUNCTION("sf.Sound", "play", "fun(self: sf.Sound)");
    type_sf__Sound.set_function("play",
        [](sf::Sound& self) {
            self.play();
        }
    );
    LUASF_STUB_DOC("\\brief Pause the sound\n\nThis function pauses the sound if it was playing,\notherwise (sound already paused or stopped) it has no effect.\n\n\\see `play`, `stop`");
    LUASF_STUB_FUNCTION("sf.Sound", "pause", "fun(self: sf.Sound)");
    type_sf__Sound.set_function("pause",
        [](sf::Sound& self) {
            self.pause();
        }
    );
    LUASF_STUB_DOC("\\brief stop playing the sound\n\nThis function stops the sound if it was playing or paused,\nand does nothing if it was already stopped.\nIt also resets the playing position (unlike `pause()`).\n\n\\see `play`, `pause`");
    LUASF_STUB_FUNCTION("sf.Sound", "stop", "fun(self: sf.Sound)");
    type_sf__Sound.set_function("stop",
        [](sf::Sound& self) {
            self.stop();
        }
    );
    LUASF_STUB_DOC("\\brief Get the current status of the sound (stopped, paused, playing)\n\n\\return Current status of the sound");
    LUASF_STUB_FUNCTION("sf.Sound", "getStatus", "fun(self: sf.Sound): sf.SoundSource.Status");
    type_sf__Sound.set_function("getStatus",
        [](sf::Sound& self) -> sf::SoundSource::Status {
            return self.getStatus();
        }
    );
    LUASF_STUB_DOC("\\brief Set the source buffer containing the audio data to play\n\nIt is important to note that the sound buffer is not copied,\nthus the `sf::SoundBuffer` instance must remain alive as long\nas it is attached to the sound.\n\n\\param buffer Sound buffer to attach to the sound\n\n\\see `getBuffer`");
    LUASF_STUB_FUNCTION("sf.Sound", "setBuffer", "fun(self: sf.Sound, buffer: sf.SoundBuffer)");
    type_sf__Sound.set_function("setBuffer",
        [](sf::Sound& self, const sf::SoundBuffer& buffer) {
            self.setBuffer(buffer);
        }
    );
    LUASF_STUB_DOC("\\brief Set whether or not the sound should loop after reaching the end\n\nIf set, the sound will restart from beginning after\nreaching the end and so on, until it is stopped or\n`setLooping(false)` is called.\nThe default looping state for sound is `false`.\n\n\\param loop `true` to play in loop, `false` to play once\n\n\\see `isLooping`");
    LUASF_STUB_FUNCTION("sf.Sound", "setLooping", "fun(self: sf.Sound, loop: boolean)");
    type_sf__Sound.set_function("setLooping",
        [](sf::Sound& self, bool loop) {
            self.setLooping(loop);
        }
    );
    LUASF_STUB_DOC("\\brief Change the current playing position of the sound\n\nThe playing position can be changed when the sound is\neither paused or playing. Changing the playing position\nwhen the sound is stopped has no effect, since playing\nthe sound will reset its position.\n\n\\param timeOffset New playing position, from the beginning of the sound\n\n\\see `getPlayingOffset`");
    LUASF_STUB_FUNCTION("sf.Sound", "setPlayingOffset", "fun(self: sf.Sound, timeOffset: sf.Time)");
    type_sf__Sound.set_function("setPlayingOffset",
        [](sf::Sound& self, sf::Time timeOffset) {
            self.setPlayingOffset(timeOffset);
        }
    );
    LUASF_STUB_DOC("\\brief Get the audio buffer attached to the sound\n\n\\return Sound buffer attached to the sound");
    LUASF_STUB_FUNCTION("sf.Sound", "getBuffer", "fun(self: sf.Sound): sf.SoundBuffer");
    type_sf__Sound.set_function("getBuffer",
        sol::policies(
            [](sf::Sound& self) {
                return std::cref(self.getBuffer());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Tell whether or not the sound is in loop mode\n\n\\return `true` if the sound is looping, `false` otherwise\n\n\\see `setLooping`");
    LUASF_STUB_FUNCTION("sf.Sound", "isLooping", "fun(self: sf.Sound): boolean");
    type_sf__Sound.set_function("isLooping",
        [](sf::Sound& self) -> bool {
            return self.isLooping();
        }
    );
    LUASF_STUB_DOC("\\brief Get the current playing position of the sound\n\n\\return Current playing position, from the beginning of the sound\n\n\\see `setPlayingOffset`");
    LUASF_STUB_FUNCTION("sf.Sound", "getPlayingOffset", "fun(self: sf.Sound): sf.Time");
    type_sf__Sound.set_function("getPlayingOffset",
        [](sf::Sound& self) -> sf::Time {
            return self.getPlayingOffset();
        }
    );
}
