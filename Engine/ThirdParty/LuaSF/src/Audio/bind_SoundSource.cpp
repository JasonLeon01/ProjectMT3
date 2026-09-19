#include "Audio/bind_SoundSource.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_SoundSource(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__SoundSource = sf.new_usertype<sf::SoundSource>("SoundSource", sol::no_constructor);
    sol::table table_sf__SoundSource = sf["SoundSource"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::SoundSource>(lua);
    LUASF_STUB_DOC("\\brief Base class defining a sound's properties");
    LUASF_STUB_CLASS("sf.SoundSource");
    // sf::SoundSource is abstract; constructor binding is omitted.
    LUASF_STUB_DOC("\\brief Set the pitch of the sound\n\nThe pitch represents the perceived fundamental frequency\nof a sound; thus you can make a sound more acute or grave\nby changing its pitch. A side effect of changing the pitch\nis to modify the playing speed of the sound as well.\nThe default value for the pitch is 1.\n\n\\param pitch New pitch to apply to the sound\n\n\\see `getPitch`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setPitch", "fun(self: sf.SoundSource, pitch: number)");
    type_sf__SoundSource.set_function("setPitch",
        [](sf::SoundSource& self, float pitch) {
            self.setPitch(pitch);
        }
    );
    LUASF_STUB_DOC("\\brief Set the pan of the sound\n\nUsing panning, a mono sound can be panned between\nstereo channels. When the pan is set to -1, the sound\nis played only on the left channel, when the pan is set\nto +1, the sound is played only on the right channel.\n\n\\param pan New pan to apply to the sound [-1, +1]\n\n\\see `getPan`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setPan", "fun(self: sf.SoundSource, pan: number)");
    type_sf__SoundSource.set_function("setPan",
        [](sf::SoundSource& self, float pan) {
            self.setPan(pan);
        }
    );
    LUASF_STUB_DOC("\\brief Set the volume of the sound\n\nThe volume is a value between 0 (mute) and 100 (full volume).\nThe default value for the volume is 100.\n\n\\param volume Volume of the sound\n\n\\see `getVolume`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setVolume", "fun(self: sf.SoundSource, volume: number)");
    type_sf__SoundSource.set_function("setVolume",
        [](sf::SoundSource& self, float volume) {
            self.setVolume(volume);
        }
    );
    LUASF_STUB_DOC("\\brief Set whether spatialization of the sound is enabled\n\nSpatialization is the application of various effects to\nsimulate a sound being emitted at a virtual position in\n3D space and exhibiting various physical phenomena such as\ndirectional attenuation and doppler shift.\n\n\\param enabled `true` to enable spatialization, `false` to disable\n\n\\see `isSpatializationEnabled`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setSpatializationEnabled", "fun(self: sf.SoundSource, enabled: boolean)");
    type_sf__SoundSource.set_function("setSpatializationEnabled",
        [](sf::SoundSource& self, bool enabled) {
            self.setSpatializationEnabled(enabled);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D position of the sound in the audio scene\n\nOnly sounds with one channel (mono sounds) can be\nspatialized.\nThe default position of a sound is (0, 0, 0).\n\n\\param position Position of the sound in the scene\n\n\\see `getPosition`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setPosition", "fun(self: sf.SoundSource, position: sf.Vector3f)");
    type_sf__SoundSource.set_function("setPosition",
        [](sf::SoundSource& self, const sf::Vector3f& position) {
            self.setPosition(position);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D direction of the sound in the audio scene\n\nThe direction defines where the sound source is facing\nin 3D space. It will affect how the sound is attenuated\nif facing away from the listener.\nThe default direction of a sound is (0, 0, -1).\n\n\\param direction Direction of the sound in the scene\n\n\\see `getDirection`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setDirection", "fun(self: sf.SoundSource, direction: sf.Vector3f)");
    type_sf__SoundSource.set_function("setDirection",
        [](sf::SoundSource& self, const sf::Vector3f& direction) {
            self.setDirection(direction);
        }
    );
    LUASF_STUB_DOC("\\brief Set the cone properties of the sound in the audio scene\n\nThe cone defines how directional attenuation is applied.\nThe default cone of a sound is (2 * PI, 2 * PI, 1).\n\n\\param cone Cone properties of the sound in the scene\n\n\\see `getCone`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setCone", "fun(self: sf.SoundSource, cone: sf.SoundSource.Cone)");
    type_sf__SoundSource.set_function("setCone",
        [](sf::SoundSource& self, const sf::SoundSource::Cone& cone) {
            self.setCone(cone);
        }
    );
    LUASF_STUB_DOC("\\brief Set the 3D velocity of the sound in the audio scene\n\nThe velocity is used to determine how to doppler shift\nthe sound. Sounds moving towards the listener will be\nperceived to have a higher pitch and sounds moving away\nfrom the listener will be perceived to have a lower pitch.\n\n\\param velocity Velocity of the sound in the scene\n\n\\see `getVelocity`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setVelocity", "fun(self: sf.SoundSource, velocity: sf.Vector3f)");
    type_sf__SoundSource.set_function("setVelocity",
        [](sf::SoundSource& self, const sf::Vector3f& velocity) {
            self.setVelocity(velocity);
        }
    );
    LUASF_STUB_DOC("\\brief Set the doppler factor of the sound\n\nThe doppler factor determines how strong the doppler\nshift will be.\n\n\\param factor New doppler factor to apply to the sound\n\n\\see `getDopplerFactor`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setDopplerFactor", "fun(self: sf.SoundSource, factor: number)");
    type_sf__SoundSource.set_function("setDopplerFactor",
        [](sf::SoundSource& self, float factor) {
            self.setDopplerFactor(factor);
        }
    );
    LUASF_STUB_DOC("\\brief Set the directional attenuation factor of the sound\n\nDepending on the virtual position of an output channel\nrelative to the listener (such as in surround sound\nsetups), sounds will be attenuated when emitting them\nfrom certain channels. This factor determines how strong\nthe attenuation based on output channel position\nrelative to the listener is.\n\n\\param factor New directional attenuation factor to apply to the sound\n\n\\see `getDirectionalAttenuationFactor`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setDirectionalAttenuationFactor", "fun(self: sf.SoundSource, factor: number)");
    type_sf__SoundSource.set_function("setDirectionalAttenuationFactor",
        [](sf::SoundSource& self, float factor) {
            self.setDirectionalAttenuationFactor(factor);
        }
    );
    LUASF_STUB_DOC("\\brief Make the sound's position relative to the listener or absolute\n\nMaking a sound relative to the listener will ensure that it will always\nbe played the same way regardless of the position of the listener.\nThis can be useful for non-spatialized sounds, sounds that are\nproduced by the listener, or sounds attached to it.\nThe default value is `false` (position is absolute).\n\n\\param relative `true` to set the position relative, `false` to set it absolute\n\n\\see `isRelativeToListener`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setRelativeToListener", "fun(self: sf.SoundSource, relative: boolean)");
    type_sf__SoundSource.set_function("setRelativeToListener",
        [](sf::SoundSource& self, bool relative) {
            self.setRelativeToListener(relative);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum distance of the sound\n\nThe \"minimum distance\" of a sound is the maximum\ndistance at which it is heard at its maximum volume. Further\nthan the minimum distance, it will start to fade out according\nto its attenuation factor. A value of 0 (\"inside the head\nof the listener\") is an invalid value and is forbidden.\nThe default value of the minimum distance is 1.\n\n\\param distance New minimum distance of the sound\n\n\\see `getMinDistance`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setMinDistance", "fun(self: sf.SoundSource, distance: number)");
    type_sf__SoundSource.set_function("setMinDistance",
        [](sf::SoundSource& self, float distance) {
            self.setMinDistance(distance);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum distance of the sound\n\nThe \"maximum distance\" of a sound is the minimum\ndistance at which it is heard at its minimum volume. Closer\nthan the maximum distance, it will start to fade in according\nto its attenuation factor.\nThe default value of the maximum distance is the maximum\nvalue a float can represent.\n\n\\param distance New maximum distance of the sound\n\n\\see `getMaxDistance`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setMaxDistance", "fun(self: sf.SoundSource, distance: number)");
    type_sf__SoundSource.set_function("setMaxDistance",
        [](sf::SoundSource& self, float distance) {
            self.setMaxDistance(distance);
        }
    );
    LUASF_STUB_DOC("\\brief Set the minimum gain of the sound\n\nWhen the sound is further away from the listener than\nthe \"maximum distance\" the attenuated gain is clamped\nso it cannot go below the minimum gain value.\n\n\\param gain New minimum gain of the sound\n\n\\see `getMinGain`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setMinGain", "fun(self: sf.SoundSource, gain: number)");
    type_sf__SoundSource.set_function("setMinGain",
        [](sf::SoundSource& self, float gain) {
            self.setMinGain(gain);
        }
    );
    LUASF_STUB_DOC("\\brief Set the maximum gain of the sound\n\nWhen the sound is closer from the listener than\nthe \"minimum distance\" the attenuated gain is clamped\nso it cannot go above the maximum gain value.\n\n\\param gain New maximum gain of the sound\n\n\\see `getMaxGain`, `setAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setMaxGain", "fun(self: sf.SoundSource, gain: number)");
    type_sf__SoundSource.set_function("setMaxGain",
        [](sf::SoundSource& self, float gain) {
            self.setMaxGain(gain);
        }
    );
    LUASF_STUB_DOC("\\brief Set the attenuation factor of the sound\n\nThe attenuation is a multiplicative factor which makes\nthe sound more or less loud according to its distance\nfrom the listener. An attenuation of 0 will produce a\nnon-attenuated sound, i.e. its volume will always be the same\nwhether it is heard from near or from far. On the other hand,\nan attenuation value such as 100 will make the sound fade out\nvery quickly as it gets further from the listener.\nThe default value of the attenuation is 1.\n\n\\param attenuation New attenuation factor of the sound\n\n\\see `getAttenuation`, `setMinDistance`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setAttenuation", "fun(self: sf.SoundSource, attenuation: number)");
    type_sf__SoundSource.set_function("setAttenuation",
        [](sf::SoundSource& self, float attenuation) {
            self.setAttenuation(attenuation);
        }
    );
    LUASF_STUB_DOC("\\brief Set the effect processor to be applied to the sound\n\nThe effect processor is a callable that will be called\nwith sound data to be processed.\n\n\\param effectProcessor The effect processor to attach to this sound, attach an empty processor to disable processing");
    LUASF_STUB_FUNCTION("sf.SoundSource", "setEffectProcessor", "fun(self: sf.SoundSource, effectProcessor: sf.SoundSource.EffectProcessor|nil)");
    type_sf__SoundSource.set_function("setEffectProcessor",
        [](sf::SoundSource& self, sol::object effectProcessor) {
            self.setEffectProcessor(lua_sf::callback::from_object<sf::SoundSource::EffectProcessor, lua_sf::callback::InterleavedFloatTransformCodec>(effectProcessor, lua_sf::callback::CallbackOptions{"sf::SoundSource::setEffectProcessor.effectProcessor", true}));
        }
    );
    LUASF_STUB_DOC("\\brief Get the pitch of the sound\n\n\\return Pitch of the sound\n\n\\see `setPitch`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getPitch", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getPitch",
        [](sf::SoundSource& self) -> float {
            return self.getPitch();
        }
    );
    LUASF_STUB_DOC("\\brief Get the pan of the sound\n\n\\return Pan of the sound\n\n\\see `setPan`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getPan", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getPan",
        [](sf::SoundSource& self) -> float {
            return self.getPan();
        }
    );
    LUASF_STUB_DOC("\\brief Get the volume of the sound\n\n\\return Volume of the sound, in the range [0, 100]\n\n\\see `setVolume`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getVolume", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getVolume",
        [](sf::SoundSource& self) -> float {
            return self.getVolume();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether spatialization of the sound is enabled\n\n\\return `true` if spatialization is enabled, `false` if it's disabled\n\n\\see `setSpatializationEnabled`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "isSpatializationEnabled", "fun(self: sf.SoundSource): boolean");
    type_sf__SoundSource.set_function("isSpatializationEnabled",
        [](sf::SoundSource& self) -> bool {
            return self.isSpatializationEnabled();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D position of the sound in the audio scene\n\n\\return Position of the sound\n\n\\see `setPosition`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getPosition", "fun(self: sf.SoundSource): sf.Vector3f");
    type_sf__SoundSource.set_function("getPosition",
        [](sf::SoundSource& self) -> sf::Vector3f {
            return self.getPosition();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D direction of the sound in the audio scene\n\n\\return Direction of the sound\n\n\\see `setDirection`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getDirection", "fun(self: sf.SoundSource): sf.Vector3f");
    type_sf__SoundSource.set_function("getDirection",
        [](sf::SoundSource& self) -> sf::Vector3f {
            return self.getDirection();
        }
    );
    LUASF_STUB_DOC("\\brief Get the cone properties of the sound in the audio scene\n\n\\return Cone properties of the sound\n\n\\see `setCone`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getCone", "fun(self: sf.SoundSource): sf.SoundSource.Cone");
    type_sf__SoundSource.set_function("getCone",
        [](sf::SoundSource& self) -> sf::SoundSource::Cone {
            return self.getCone();
        }
    );
    LUASF_STUB_DOC("\\brief Get the 3D velocity of the sound in the audio scene\n\n\\return Velocity of the sound\n\n\\see `setVelocity`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getVelocity", "fun(self: sf.SoundSource): sf.Vector3f");
    type_sf__SoundSource.set_function("getVelocity",
        [](sf::SoundSource& self) -> sf::Vector3f {
            return self.getVelocity();
        }
    );
    LUASF_STUB_DOC("\\brief Get the doppler factor of the sound\n\n\\return Doppler factor of the sound\n\n\\see `setDopplerFactor`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getDopplerFactor", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getDopplerFactor",
        [](sf::SoundSource& self) -> float {
            return self.getDopplerFactor();
        }
    );
    LUASF_STUB_DOC("\\brief Get the directional attenuation factor of the sound\n\n\\return Directional attenuation factor of the sound\n\n\\see `setDirectionalAttenuationFactor`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getDirectionalAttenuationFactor", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getDirectionalAttenuationFactor",
        [](sf::SoundSource& self) -> float {
            return self.getDirectionalAttenuationFactor();
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the sound's position is relative to the\nlistener or is absolute\n\n\\return `true` if the position is relative, `false` if it's absolute\n\n\\see `setRelativeToListener`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "isRelativeToListener", "fun(self: sf.SoundSource): boolean");
    type_sf__SoundSource.set_function("isRelativeToListener",
        [](sf::SoundSource& self) -> bool {
            return self.isRelativeToListener();
        }
    );
    LUASF_STUB_DOC("\\brief Get the minimum distance of the sound\n\n\\return Minimum distance of the sound\n\n\\see `setMinDistance`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getMinDistance", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getMinDistance",
        [](sf::SoundSource& self) -> float {
            return self.getMinDistance();
        }
    );
    LUASF_STUB_DOC("\\brief Get the maximum distance of the sound\n\n\\return Maximum distance of the sound\n\n\\see `setMaxDistance`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getMaxDistance", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getMaxDistance",
        [](sf::SoundSource& self) -> float {
            return self.getMaxDistance();
        }
    );
    LUASF_STUB_DOC("\\brief Get the minimum gain of the sound\n\n\\return Minimum gain of the sound\n\n\\see `setMinGain`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getMinGain", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getMinGain",
        [](sf::SoundSource& self) -> float {
            return self.getMinGain();
        }
    );
    LUASF_STUB_DOC("\\brief Get the maximum gain of the sound\n\n\\return Maximum gain of the sound\n\n\\see `setMaxGain`, `getAttenuation`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getMaxGain", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getMaxGain",
        [](sf::SoundSource& self) -> float {
            return self.getMaxGain();
        }
    );
    LUASF_STUB_DOC("\\brief Get the attenuation factor of the sound\n\n\\return Attenuation factor of the sound\n\n\\see `setAttenuation`, `getMinDistance`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getAttenuation", "fun(self: sf.SoundSource): number");
    type_sf__SoundSource.set_function("getAttenuation",
        [](sf::SoundSource& self) -> float {
            return self.getAttenuation();
        }
    );
    LUASF_STUB_DOC("\\brief Start or resume playing the sound source\n\nThis function starts the source if it was stopped, resumes\nit if it was paused, and restarts it from the beginning if\nit was already playing.\n\n\\see `pause`, `stop`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "play", "fun(self: sf.SoundSource)");
    type_sf__SoundSource.set_function("play",
        [](sf::SoundSource& self) {
            self.play();
        }
    );
    LUASF_STUB_DOC("\\brief Pause the sound source\n\nThis function pauses the source if it was playing,\notherwise (source already paused or stopped) it has no effect.\n\n\\see `play`, `stop`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "pause", "fun(self: sf.SoundSource)");
    type_sf__SoundSource.set_function("pause",
        [](sf::SoundSource& self) {
            self.pause();
        }
    );
    LUASF_STUB_DOC("\\brief Stop playing the sound source\n\nThis function stops the source if it was playing or paused,\nand does nothing if it was already stopped.\nIt also resets the playing position (unlike `pause()`).\n\n\\see `play`, `pause`");
    LUASF_STUB_FUNCTION("sf.SoundSource", "stop", "fun(self: sf.SoundSource)");
    type_sf__SoundSource.set_function("stop",
        [](sf::SoundSource& self) {
            self.stop();
        }
    );
    LUASF_STUB_DOC("\\brief Get the current status of the sound (stopped, paused, playing)\n\n\\return Current status of the sound");
    LUASF_STUB_FUNCTION("sf.SoundSource", "getStatus", "fun(self: sf.SoundSource): sf.SoundSource.Status");
    type_sf__SoundSource.set_function("getStatus",
        [](sf::SoundSource& self) -> sf::SoundSource::Status {
            return self.getStatus();
        }
    );
    LUASF_STUB_DOC("\\brief Enumeration of the sound source states");
    LUASF_STUB_CLASS("sf.SoundSource.Status");
    LUASF_STUB_DOC("Sound is not playing");
    LUASF_STUB_FIELD("Stopped", "sf.SoundSource.Status");
    LUASF_STUB_DOC("Sound is paused");
    LUASF_STUB_FIELD("Paused", "sf.SoundSource.Status");
    LUASF_STUB_DOC("Sound is playing");
    LUASF_STUB_FIELD("Playing", "sf.SoundSource.Status");
    table_sf__SoundSource.new_enum("Status",
        "Stopped", sf::SoundSource::Status::Stopped,
        "Paused", sf::SoundSource::Status::Paused,
        "Playing", sf::SoundSource::Status::Playing
    );
    auto type_sf__SoundSource__Cone = table_sf__SoundSource.new_usertype<sf::SoundSource::Cone>("Cone", sol::no_constructor);
    sol::table table_sf__SoundSource__Cone = table_sf__SoundSource["Cone"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::SoundSource::Cone>(lua);
    LUASF_STUB_DOC("\\brief Structure defining the properties of a directional cone\n\nSounds will play at gain 1 when the listener\nis positioned within the inner angle of the cone.\nSounds will play at `outerGain` when the listener is\npositioned outside the outer angle of the cone.\nThe gain declines linearly from 1 to `outerGain` as the\nlistener moves from the inner angle to the outer angle.");
    LUASF_STUB_CLASS("sf.SoundSource.Cone");
    LUASF_STUB_DOC("Inner angle");
    LUASF_STUB_FIELD("innerAngle", "sf.Angle");
    LUASF_STUB_DOC("Outer angle");
    LUASF_STUB_FIELD("outerAngle", "sf.Angle");
    LUASF_STUB_DOC("Outer gain");
    LUASF_STUB_FIELD("outerGain", "number");
    LUASF_STUB_FUNCTION("sf.SoundSource.Cone", "new", "fun(): sf.SoundSource.Cone");
    type_sf__SoundSource__Cone.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::SoundSource::Cone>();
        }
    ));
    type_sf__SoundSource__Cone["innerAngle"] = sol::policies(&sf::SoundSource::Cone::innerAngle, sol::self_dependency{});
    type_sf__SoundSource__Cone["outerAngle"] = sol::policies(&sf::SoundSource::Cone::outerAngle, sol::self_dependency{});
    type_sf__SoundSource__Cone["outerGain"] = sol::policies(&sf::SoundSource::Cone::outerGain, sol::self_dependency{});
    LUASF_STUB_DOC("\\brief Callable that is provided with sound data for processing\n\nWhen the audio engine sources sound data from sound\nsources it will pass the data through an effects\nprocessor if one is set. The sound data will already be\nconverted to the internal floating point format and have\nthe same sample rate as the audio device and engine. The\ndevice sample rate can differ from the sample rate of\nthe source data so keep this in mind when setting up\nprocessing that is dependent on the sample rate. The\nsample rate of the current playback device can be\nretrieved using `sf::PlaybackDevice::getDeviceSampleRate()`.\n\nSound data that is processed this way is provided in\nframes. Each frame contains 1 floating point sample per\nchannel. If e.g. the data source provides stereo data,\neach frame will contain 2 floats.\n\nThe effects processor function takes 4 parameters:\n- The input data frames, channels interleaved\n- The number of input data frames available\n- The buffer to write output data frames to, channels interleaved\n- The number of output data frames that the output buffer can hold\n- The channel count\n\nThe input and output frame counts are in/out parameters.\n\nWhen this function is called, the input count will\ncontain the number of frames available in the input\nbuffer. The output count will contain the size of the\noutput buffer i.e. the maximum number of frames that\ncan be written to the output buffer.\n\nAttempting to read more frames than the input frame\ncount or write more frames than the output frame count\nwill result in undefined behaviour.\n\nIt is important to note that the channel count of the\naudio engine currently sourcing data from this sound\nwill always be provided in `frameChannelCount`. This can\nbe different from the channel count of the sound source\nso make sure to size necessary processing buffers\naccording to the engine channel count and not the sound\nsource channel count.\n\nWhen done processing the frames, the input and output\nframe counts must be updated to reflect the actual\nnumber of frames that were read from the input and\nwritten to the output.\n\nThe processing function should always try to process as\nmuch sound data as possible i.e. always try to fill the\noutput buffer to the maximum. In certain situations for\nspecific effects it can be possible that the input frame\ncount and output frame count aren't equal. As long as\nthe frame counts are updated accordingly this is\nperfectly valid.\n\nIf the audio engine determines that no audio data is\navailable from the data source, the input data frames\npointer is set to `nullptr` and the input frame count is\nset to 0. In this case it is up to the function to\ndecide how to handle the situation. For specific effects\ne.g. Echo/Delay buffered data might still be able to be\nwritten to the output buffer even if there is no longer\nany input data.\n\nAn important thing to remember is that this function is\ndirectly called by the audio engine. Because the audio\nengine runs on an internal thread of its own, make sure\naccess to shared data is synchronized appropriately.\n\nBecause this function is stored by the `SoundSource`\nobject it will be able to be called as long as the\n`SoundSource` object hasn't yet been destroyed. Make sure\nthat any data this function references outlives the\nSoundSource object otherwise use-after-free errors will\noccur.");
    LUASF_STUB_ALIAS("sf.SoundSource.EffectProcessor", "fun(inputFrames: number[]|nil, inputFrameCount: integer, outputFrames: number[], outputFrameCount: integer, frameChannelCount: integer): {inputFrameCount: integer, outputFrameCount: integer, outputFrames: number[]?}");
}
