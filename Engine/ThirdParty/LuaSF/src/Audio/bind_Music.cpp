#include "Audio/bind_Music.hpp"

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

namespace { constexpr std::array<std::string_view, 62> docs = {
    "\\brief Streamed music played from an audio file",
    "\\brief Default constructor\n\nConstruct an empty music that does not contain any data.",
    "\\brief Construct a music from an audio file\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather\nstreamed continuously, the file must remain accessible until\nthe `sf::Music` object loads a new music or is destroyed.\n\n\\param filename Path of the music file to open\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `openFromMemory`, `openFromStream`",
    "\\brief Construct a music from an audio file in memory\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather streamed\ncontinuously, the \\a data buffer must remain accessible until\nthe `sf::Music` object loads a new music or is destroyed. That is,\nyou can't deallocate the buffer right after calling this function.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `openFromFile`, `openFromStream`",
    "\\brief Construct a music from an audio file in a custom stream\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather\nstreamed continuously, the `stream` must remain accessible\nuntil the `sf::Music` object loads a new music or is destroyed.\n\n\\param stream Source stream to read from\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `openFromFile`, `openFromMemory`",
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
    "\\brief Open a music from an audio file\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather\nstreamed continuously, the file must remain accessible until\nthe `sf::Music` object loads a new music or is destroyed.\n\n\\param filename Path of the music file to open\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `openFromMemory`, `openFromStream`",
    "\\brief Open a music from an audio file in memory\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather streamed\ncontinuously, the `data` buffer must remain accessible until\nthe `sf::Music` object loads a new music or is destroyed. That is,\nyou can't deallocate the buffer right after calling this function.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `openFromFile`, `openFromStream`",
    "\\brief Open a music from an audio file in a custom stream\n\nThis function doesn't start playing the music (call `play()`\nto do so).\nSee the documentation of `sf::InputSoundFile` for the list\nof supported formats.\n\n\\warning Since the music is not loaded at once but rather\nstreamed continuously, the `stream` must remain accessible\nuntil the `sf::Music` object loads a new music or is destroyed.\n\n\\param stream Source stream to read from\n\n\\return `true` if loading succeeded, `false` if it failed\n\n\\see `openFromFile`, `openFromMemory`",
    "\\brief Get the total duration of the music\n\n\\return Music duration",
    "\\brief Get the positions of the of the sound's looping sequence\n\n\\return Loop Time position class.\n\n\\warning Since `setLoopPoints()` performs some adjustments on the\nprovided values and rounds them to internal samples, a call to\n`getLoopPoints()` is not guaranteed to return the same times passed\ninto a previous call to `setLoopPoints()`. However, it is guaranteed\nto return times that will map to the valid internal samples of\nthis Music if they are later passed to `setLoopPoints()`.\n\n\\see `setLoopPoints`",
    "\\brief Sets the beginning and duration of the sound's looping sequence using `sf::Time`\n\n`setLoopPoints()` allows for specifying the beginning offset and the duration of the loop such that,\nwhen the music is enabled for looping, it will seamlessly seek to the beginning whenever it\nencounters the end of the duration. Valid ranges for `timePoints.offset` and `timePoints.length` are\n[0, Dur) and (0, Dur-offset] respectively, where Dur is the value returned by `getDuration()`.\nNote that the EOF \"loop point\" from the end to the beginning of the stream is still honored,\nin case the caller seeks to a point after the end of the loop range. This function can be\nsafely called at any point after a stream is opened, and will be applied to a playing sound\nwithout affecting the current playing offset.\n\n\\warning Setting the loop points while the stream's status is Paused\nwill set its status to Stopped. The playing offset will be unaffected.\n\n\\param timePoints The definition of the loop. Can be any time points within the sound's length\n\n\\see `getLoopPoints`",
    "\\brief Structure defining a time range using the template type",
    "The beginning offset of the time range",
    "The length of the time range",
}; }

void bind_Music(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Music = lua_glue::BindClass<sf::Music>(sf, "Music");
    lua_glue::BindBase<sf::Music, sf::SoundStream>(type_sf__Music);
    lua_glue::BindBase<sf::Music, sf::SoundSource>(type_sf__Music);
    lua_glue::Table table_sf__Music = sf["Music"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Music>(lua);
    lua_glue::Table native_bases_sf__Music = lua.create_table();
    native_bases_sf__Music.add(lua["sf"]["SoundStream"].get<lua_glue::Table>());
    table_sf__Music.raw_set("__nativeBases", native_bases_sf__Music);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Music", "sf.SoundStream, sf.SoundSource");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Music", "new", "fun(filename: string): sf.Music");
    LUASF_STUB_OVERLOAD("sf.Music", "new", "fun(stream: sf.InputStream): sf.Music");
    LUASF_STUB_OVERLOAD("sf.Music", "new", "fun(): sf.Music");
    LUASF_STUB_OVERLOAD("sf.Music", "new", "fun(data: any): sf.Music");
    lua_glue::BindCallable(type_sf__Music, "new",
        [](std::string filename) {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::Music>(std::filesystem::path(filename)));
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__Music, "new",
        [](lua_glue::Object stream) {
            auto& stream_ref = stream.as<sf::InputStream&>();
            auto object = lua_sf::makeLongLivedMemoryObject<sf::Music>(stream_ref);
            lua_sf::rememberLongLivedStream(*object, std::move(stream));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Music, "new",
        []() {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::Music>());
        },
        docs[1]
    );
    lua_glue::BindCallable(type_sf__Music, "new",
        [](lua_glue::Object data) {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            auto object = lua_sf::makeLongLivedMemoryObject<sf::Music>();
            if (!object->openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size())))
                throw std::runtime_error("Failed to open sf.Music from memory");
            lua_sf::rememberLongLivedMemory(*object, std::move(data_buffer));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Music", "setPitch", "fun(self: sf.Music, pitch: number)");
    lua_glue::BindCallable(type_sf__Music, "setPitch",
        [](sf::Music& self, float pitch) {
            static_cast<sf::SoundSource&>(self).setPitch(pitch);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Music", "setPan", "fun(self: sf.Music, pan: number)");
    lua_glue::BindCallable(type_sf__Music, "setPan",
        [](sf::Music& self, float pan) {
            static_cast<sf::SoundSource&>(self).setPan(pan);
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Music", "setVolume", "fun(self: sf.Music, volume: number)");
    lua_glue::BindCallable(type_sf__Music, "setVolume",
        [](sf::Music& self, float volume) {
            static_cast<sf::SoundSource&>(self).setVolume(volume);
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Music", "setSpatializationEnabled", "fun(self: sf.Music, enabled: boolean)");
    lua_glue::BindCallable(type_sf__Music, "setSpatializationEnabled",
        [](sf::Music& self, bool enabled) {
            static_cast<sf::SoundSource&>(self).setSpatializationEnabled(enabled);
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Music", "setPosition", "fun(self: sf.Music, position: sf.Vector3f)");
    lua_glue::BindCallable(type_sf__Music, "setPosition",
        [](sf::Music& self, const sf::Vector3f& position) {
            static_cast<sf::SoundSource&>(self).setPosition(position);
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Music", "setDirection", "fun(self: sf.Music, direction: sf.Vector3f)");
    lua_glue::BindCallable(type_sf__Music, "setDirection",
        [](sf::Music& self, const sf::Vector3f& direction) {
            static_cast<sf::SoundSource&>(self).setDirection(direction);
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Music", "setCone", "fun(self: sf.Music, cone: sf.SoundSource.Cone)");
    lua_glue::BindCallable(type_sf__Music, "setCone",
        [](sf::Music& self, const sf::SoundSource::Cone& cone) {
            static_cast<sf::SoundSource&>(self).setCone(cone);
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Music", "setVelocity", "fun(self: sf.Music, velocity: sf.Vector3f)");
    lua_glue::BindCallable(type_sf__Music, "setVelocity",
        [](sf::Music& self, const sf::Vector3f& velocity) {
            static_cast<sf::SoundSource&>(self).setVelocity(velocity);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Music", "setDopplerFactor", "fun(self: sf.Music, factor: number)");
    lua_glue::BindCallable(type_sf__Music, "setDopplerFactor",
        [](sf::Music& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDopplerFactor(factor);
        },
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Music", "setDirectionalAttenuationFactor", "fun(self: sf.Music, factor: number)");
    lua_glue::BindCallable(type_sf__Music, "setDirectionalAttenuationFactor",
        [](sf::Music& self, float factor) {
            static_cast<sf::SoundSource&>(self).setDirectionalAttenuationFactor(factor);
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Music", "setRelativeToListener", "fun(self: sf.Music, relative: boolean)");
    lua_glue::BindCallable(type_sf__Music, "setRelativeToListener",
        [](sf::Music& self, bool relative) {
            static_cast<sf::SoundSource&>(self).setRelativeToListener(relative);
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Music", "setMinDistance", "fun(self: sf.Music, distance: number)");
    lua_glue::BindCallable(type_sf__Music, "setMinDistance",
        [](sf::Music& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMinDistance(distance);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Music", "setMaxDistance", "fun(self: sf.Music, distance: number)");
    lua_glue::BindCallable(type_sf__Music, "setMaxDistance",
        [](sf::Music& self, float distance) {
            static_cast<sf::SoundSource&>(self).setMaxDistance(distance);
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.Music", "setMinGain", "fun(self: sf.Music, gain: number)");
    lua_glue::BindCallable(type_sf__Music, "setMinGain",
        [](sf::Music& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMinGain(gain);
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Music", "setMaxGain", "fun(self: sf.Music, gain: number)");
    lua_glue::BindCallable(type_sf__Music, "setMaxGain",
        [](sf::Music& self, float gain) {
            static_cast<sf::SoundSource&>(self).setMaxGain(gain);
        },
        docs[19]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Music", "setAttenuation", "fun(self: sf.Music, attenuation: number)");
    lua_glue::BindCallable(type_sf__Music, "setAttenuation",
        [](sf::Music& self, float attenuation) {
            static_cast<sf::SoundSource&>(self).setAttenuation(attenuation);
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Music", "setEffectProcessor", "fun(self: sf.Music, effectProcessor: sf.SoundSource.EffectProcessor|nil)");
    lua_glue::BindCallable(type_sf__Music, "setEffectProcessor",
        [](sf::Music& self, lua_glue::Object effectProcessor) {
            static_cast<sf::SoundSource&>(self).setEffectProcessor(lua_sf::callback::from_object<sf::SoundSource::EffectProcessor, lua_sf::callback::InterleavedFloatTransformCodec>(effectProcessor, lua_sf::callback::CallbackOptions{"sf::SoundSource::setEffectProcessor.effectProcessor", true}));
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Music", "getPitch", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getPitch",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getPitch();
        },
        docs[22]
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.Music", "getPan", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getPan",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getPan();
        },
        docs[23]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.Music", "getVolume", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getVolume",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getVolume();
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.Music", "isSpatializationEnabled", "fun(self: sf.Music): boolean");
    lua_glue::BindCallable(type_sf__Music, "isSpatializationEnabled",
        [](const sf::Music& self) -> bool {
            return static_cast<const sf::SoundSource&>(self).isSpatializationEnabled();
        },
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.Music", "getPosition", "fun(self: sf.Music): sf.Vector3f");
    lua_glue::BindCallable(type_sf__Music, "getPosition",
        [](const sf::Music& self) -> sf::Vector3f {
            return static_cast<const sf::SoundSource&>(self).getPosition();
        },
        docs[26]
    );
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FUNCTION("sf.Music", "getDirection", "fun(self: sf.Music): sf.Vector3f");
    lua_glue::BindCallable(type_sf__Music, "getDirection",
        [](const sf::Music& self) -> sf::Vector3f {
            return static_cast<const sf::SoundSource&>(self).getDirection();
        },
        docs[27]
    );
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FUNCTION("sf.Music", "getCone", "fun(self: sf.Music): sf.SoundSource.Cone");
    lua_glue::BindCallable(type_sf__Music, "getCone",
        [](const sf::Music& self) -> sf::SoundSource::Cone {
            return static_cast<const sf::SoundSource&>(self).getCone();
        },
        docs[28]
    );
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_FUNCTION("sf.Music", "getVelocity", "fun(self: sf.Music): sf.Vector3f");
    lua_glue::BindCallable(type_sf__Music, "getVelocity",
        [](const sf::Music& self) -> sf::Vector3f {
            return static_cast<const sf::SoundSource&>(self).getVelocity();
        },
        docs[29]
    );
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FUNCTION("sf.Music", "getDopplerFactor", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getDopplerFactor",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getDopplerFactor();
        },
        docs[30]
    );
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FUNCTION("sf.Music", "getDirectionalAttenuationFactor", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getDirectionalAttenuationFactor",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getDirectionalAttenuationFactor();
        },
        docs[31]
    );
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FUNCTION("sf.Music", "isRelativeToListener", "fun(self: sf.Music): boolean");
    lua_glue::BindCallable(type_sf__Music, "isRelativeToListener",
        [](const sf::Music& self) -> bool {
            return static_cast<const sf::SoundSource&>(self).isRelativeToListener();
        },
        docs[32]
    );
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FUNCTION("sf.Music", "getMinDistance", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getMinDistance",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getMinDistance();
        },
        docs[33]
    );
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FUNCTION("sf.Music", "getMaxDistance", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getMaxDistance",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getMaxDistance();
        },
        docs[34]
    );
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FUNCTION("sf.Music", "getMinGain", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getMinGain",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getMinGain();
        },
        docs[35]
    );
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FUNCTION("sf.Music", "getMaxGain", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getMaxGain",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getMaxGain();
        },
        docs[36]
    );
    LUASF_STUB_DOC(docs[37]);
    LUASF_STUB_FUNCTION("sf.Music", "getAttenuation", "fun(self: sf.Music): number");
    lua_glue::BindCallable(type_sf__Music, "getAttenuation",
        [](const sf::Music& self) -> float {
            return static_cast<const sf::SoundSource&>(self).getAttenuation();
        },
        docs[37]
    );
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FUNCTION("sf.Music", "play", "fun(self: sf.Music)");
    lua_glue::BindCallable(type_sf__Music, "play",
        [](sf::Music& self) {
            static_cast<sf::SoundSource&>(self).play();
        },
        docs[38]
    );
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FUNCTION("sf.Music", "pause", "fun(self: sf.Music)");
    lua_glue::BindCallable(type_sf__Music, "pause",
        [](sf::Music& self) {
            static_cast<sf::SoundSource&>(self).pause();
        },
        docs[39]
    );
    LUASF_STUB_DOC(docs[40]);
    LUASF_STUB_FUNCTION("sf.Music", "stop", "fun(self: sf.Music)");
    lua_glue::BindCallable(type_sf__Music, "stop",
        [](sf::Music& self) {
            static_cast<sf::SoundSource&>(self).stop();
        },
        docs[40]
    );
    LUASF_STUB_DOC(docs[41]);
    LUASF_STUB_FUNCTION("sf.Music", "getStatus", "fun(self: sf.Music): sf.SoundSource.Status");
    lua_glue::BindCallable(type_sf__Music, "getStatus",
        [](const sf::Music& self) -> sf::SoundSource::Status {
            return static_cast<const sf::SoundSource&>(self).getStatus();
        },
        docs[41]
    );
    LUASF_STUB_DOC(docs[45]);
    LUASF_STUB_FUNCTION("sf.Music", "getChannelCount", "fun(self: sf.Music): integer");
    lua_glue::BindCallable(type_sf__Music, "getChannelCount",
        [](const sf::Music& self) -> unsigned int {
            return static_cast<const sf::SoundStream&>(self).getChannelCount();
        },
        docs[45]
    );
    LUASF_STUB_DOC(docs[46]);
    LUASF_STUB_FUNCTION("sf.Music", "getSampleRate", "fun(self: sf.Music): integer");
    lua_glue::BindCallable(type_sf__Music, "getSampleRate",
        [](const sf::Music& self) -> unsigned int {
            return static_cast<const sf::SoundStream&>(self).getSampleRate();
        },
        docs[46]
    );
    LUASF_STUB_DOC(docs[47]);
    LUASF_STUB_FUNCTION("sf.Music", "getChannelMap", "fun(self: sf.Music): sf.SoundChannel[]");
    lua_glue::BindCallable(type_sf__Music, "getChannelMap",
        [lua](const sf::Music& self) -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, static_cast<const sf::SoundStream&>(self).getChannelMap());
        },
        docs[47],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[49]);
    LUASF_STUB_FUNCTION("sf.Music", "setPlayingOffset", "fun(self: sf.Music, timeOffset: sf.Time)");
    lua_glue::BindCallable(type_sf__Music, "setPlayingOffset",
        [](sf::Music& self, sf::Time timeOffset) {
            static_cast<sf::SoundStream&>(self).setPlayingOffset(timeOffset);
        },
        docs[49]
    );
    LUASF_STUB_DOC(docs[50]);
    LUASF_STUB_FUNCTION("sf.Music", "getPlayingOffset", "fun(self: sf.Music): sf.Time");
    lua_glue::BindCallable(type_sf__Music, "getPlayingOffset",
        [](const sf::Music& self) -> sf::Time {
            return static_cast<const sf::SoundStream&>(self).getPlayingOffset();
        },
        docs[50]
    );
    LUASF_STUB_DOC(docs[51]);
    LUASF_STUB_FUNCTION("sf.Music", "setLooping", "fun(self: sf.Music, loop: boolean)");
    lua_glue::BindCallable(type_sf__Music, "setLooping",
        [](sf::Music& self, bool loop) {
            static_cast<sf::SoundStream&>(self).setLooping(loop);
        },
        docs[51]
    );
    LUASF_STUB_DOC(docs[52]);
    LUASF_STUB_FUNCTION("sf.Music", "isLooping", "fun(self: sf.Music): boolean");
    lua_glue::BindCallable(type_sf__Music, "isLooping",
        [](const sf::Music& self) -> bool {
            return static_cast<const sf::SoundStream&>(self).isLooping();
        },
        docs[52]
    );
    LUASF_STUB_DOC(docs[53]);
    LUASF_STUB_FUNCTION("sf.Music", "openFromFile", "fun(self: sf.Music, filename: string): boolean");
    lua_glue::BindCallable(type_sf__Music, "openFromFile",
        [](sf::Music& self, std::string filename) -> bool {
            auto result = self.openFromFile(std::filesystem::path(filename));
            if (result)
                lua_sf::releaseLongLivedResources(self);
            return result;
        },
        docs[53]
    );
    LUASF_STUB_DOC(docs[54]);
    LUASF_STUB_FUNCTION("sf.Music", "openFromMemory", "fun(self: sf.Music, data: any): boolean");
    lua_glue::BindCallable(type_sf__Music, "openFromMemory",
        [](sf::Music& self, lua_glue::Object data) -> bool {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            const bool result = self.openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size()));
            if (result)
            {
                lua_sf::releaseLongLivedStream(self);
                lua_sf::rememberLongLivedMemory(self, std::move(data_buffer));
            }
            return result;
        },
        docs[54]
    );
    LUASF_STUB_DOC(docs[55]);
    LUASF_STUB_FUNCTION("sf.Music", "openFromStream", "fun(self: sf.Music, stream: sf.InputStream): boolean");
    lua_glue::BindCallable(type_sf__Music, "openFromStream",
        [](sf::Music& self, lua_glue::Object stream) -> bool {
            auto& stream_ref = stream.as<sf::InputStream&>();
            const bool result = self.openFromStream(stream_ref);
            if (result)
            {
                lua_sf::releaseLongLivedMemory(self);
                lua_sf::rememberLongLivedStream(self, std::move(stream));
            }
            return result;
        },
        docs[55]
    );
    LUASF_STUB_DOC(docs[56]);
    LUASF_STUB_FUNCTION("sf.Music", "getDuration", "fun(self: sf.Music): sf.Time");
    lua_glue::BindCallable(type_sf__Music, "getDuration",
        [](const sf::Music& self) -> sf::Time {
            return self.getDuration();
        },
        docs[56]
    );
    LUASF_STUB_DOC(docs[57]);
    LUASF_STUB_FUNCTION("sf.Music", "getLoopPoints", "fun(self: sf.Music): sf.Music.TimeSpan");
    lua_glue::BindCallable(type_sf__Music, "getLoopPoints",
        [](const sf::Music& self) -> sf::Music::TimeSpan {
            return self.getLoopPoints();
        },
        docs[57]
    );
    LUASF_STUB_DOC(docs[58]);
    LUASF_STUB_FUNCTION("sf.Music", "setLoopPoints", "fun(self: sf.Music, timePoints: sf.Music.TimeSpan)");
    lua_glue::BindCallable(type_sf__Music, "setLoopPoints",
        [](sf::Music& self, sf::Music::TimeSpan timePoints) {
            self.setLoopPoints(timePoints);
        },
        docs[58]
    );
    auto type_sf__Music__Span_sf__Time_ = lua_glue::BindStruct<sf::Music::Span<sf::Time>>(table_sf__Music, "TimeSpan");
    lua_glue::Table table_sf__Music__Span_sf__Time_ = table_sf__Music["TimeSpan"].get<lua_glue::Table>();
    LUASF_STUB_DOC(docs[59]);
    LUASF_STUB_CLASS("sf.Music.TimeSpan");
    LUASF_STUB_DOC(docs[60]);
    LUASF_STUB_FIELD("offset", "sf.Time");
    LUASF_STUB_DOC(docs[61]);
    LUASF_STUB_FIELD("length", "sf.Time");
    LUASF_STUB_FUNCTION("sf.Music.TimeSpan", "new", "fun(): sf.Music.TimeSpan");
    lua_glue::BindCallable(type_sf__Music__Span_sf__Time_, "new",
        []() {
            return sf::Music::Span<sf::Time>{};
        }
    );
    lua_glue::BindAttr<sf::Time>(type_sf__Music__Span_sf__Time_, "offset", &sf::Music::Span<sf::Time>::offset);
    lua_glue::BindAttr<sf::Time>(type_sf__Music__Span_sf__Time_, "length", &sf::Music::Span<sf::Time>::length);
    LUASF_STUB_FUNCTION("sf.Music.TimeSpan", "copy", "fun(self: sf.Music.TimeSpan): sf.Music.TimeSpan");
    LUASF_STUB_FUNCTION("sf.Music.TimeSpan", "deepcopy", "fun(self: sf.Music.TimeSpan): sf.Music.TimeSpan");
}
