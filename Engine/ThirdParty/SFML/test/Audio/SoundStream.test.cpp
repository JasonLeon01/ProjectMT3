#include <SFML/Audio/SoundStream.hpp>

// Other 1st party headers
#include <SFML/Audio/PlaybackDevice.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <AudioUtil.hpp>
#include <SystemUtil.hpp>
#include <algorithm>
#include <array>
#include <atomic>
#include <type_traits>

namespace
{
class SoundStream : public sf::SoundStream
{
public:
    using sf::SoundStream::initialize;

private:
    [[nodiscard]] bool onGetData(Chunk& /* data */) override
    {
        return true;
    }

    void onSeek(sf::Time /* timeOffset */) override
    {
    }
};

class FiniteSoundStream : public sf::SoundStream
{
public:
    using sf::SoundStream::initialize;
    static constexpr unsigned int sampleCount = 17;

    ~FiniteSoundStream() override
    {
        stop();
    }

    std::atomic<sf::Time> lastSeek{sf::Time::Zero};

private:
    [[nodiscard]] bool onGetData(Chunk& data) override
    {
        data = {m_samples.data(), m_samples.size()};
        return false;
    }

    void onSeek(sf::Time offset) override
    {
        lastSeek = offset;
    }

    const std::array<std::int16_t, sampleCount> m_samples = []
    {
        std::array<std::int16_t, sampleCount> samples{};
        samples.back() = 16'384;
        return samples;
    }();
};
} // namespace

TEST_CASE("[Audio] sf::SoundStream queued seek and final audio block", runAudioDeviceTests())
{
    REQUIRE(sf::PlaybackDevice::setDeviceToNull());

    const auto pitch = GENERATE(1.f, 0.5f);
    CAPTURE(pitch);

    std::atomic<unsigned int> inputFrames{};
    std::atomic<bool>         heardLastSample{};
    std::atomic<bool>         reachedEffectTail{};
    FiniteSoundStream         stream;

    const auto sampleRate = sf::PlaybackDevice::getDeviceSampleRate();
    REQUIRE(sampleRate.has_value());
    stream.initialize(1, *sampleRate, {sf::SoundChannel::Mono});
    stream.setPitch(pitch);
    stream.setSpatializationEnabled(false);

    stream.setEffectProcessor(
        [&](const float* input, unsigned int& count, float*, unsigned int& outputCount, unsigned int channels)
        {
            if (input)
            {
                inputFrames += count;
                if (std::any_of(input, input + count * channels, [](float sample) { return sample > 0.01f; }))
                    heardLastSample = true;
            }
            else if (inputFrames > 0)
            {
                reachedEffectTail = true;
            }

            outputCount = 0;
        });

    stream.setPlayingOffset(sf::milliseconds(500));
    stream.setPlayingOffset(sf::seconds(1));
    CHECK(stream.getPlayingOffset() == sf::seconds(1));
    CHECK(stream.lastSeek.load() == sf::Time::Zero);
    stream.play();

    const sf::Clock clock;
    while (!reachedEffectTail && clock.getElapsedTime() < sf::seconds(2))
        sf::sleep(sf::milliseconds(1));
    stream.stop();

    CHECK(stream.lastSeek.load() == sf::seconds(1));
    CHECK(heardLastSample.load());
    CHECK(reachedEffectTail.load());
    CHECK(inputFrames.load() >= static_cast<unsigned int>(static_cast<float>(FiniteSoundStream::sampleCount) / pitch));
    CHECK(inputFrames.load() < 64);
}

TEST_CASE("[Audio] sf::SoundStream", runAudioDeviceTests())
{
    [[maybe_unused]] auto result = sf::PlaybackDevice::setDeviceToNull();

    SECTION("Type traits")
    {
        STATIC_CHECK(!std::is_constructible_v<sf::SoundStream>);
        STATIC_CHECK(!std::is_copy_constructible_v<sf::SoundStream>);
        STATIC_CHECK(!std::is_copy_assignable_v<sf::SoundStream>);
        STATIC_CHECK(!std::is_nothrow_move_constructible_v<sf::SoundStream>);
        STATIC_CHECK(std::is_nothrow_move_assignable_v<sf::SoundStream>);
        STATIC_CHECK(std::has_virtual_destructor_v<sf::SoundStream>);
    }

    SECTION("Chunk")
    {
        const sf::SoundStream::Chunk chunk;
        CHECK(chunk.samples == nullptr);
        CHECK(chunk.sampleCount == 0);
    }

    SECTION("Construction")
    {
        const SoundStream soundStream;
        CHECK(soundStream.getChannelCount() == 0);
        CHECK(soundStream.getSampleRate() == 0);
        CHECK(soundStream.getChannelMap().empty());
        CHECK(soundStream.getStatus() == sf::SoundStream::Status::Stopped);
        CHECK(soundStream.getPlayingOffset() == sf::Time::Zero);
        CHECK(!soundStream.isLooping());

        // Inherited from sf::SoundStream
        CHECK(soundStream.getPitch() == 1);
        CHECK(soundStream.getPan() == 0);
        CHECK(soundStream.getVolume() == 100);
        CHECK(soundStream.isSpatializationEnabled());
        CHECK(soundStream.getPosition() == sf::Vector3f());
        CHECK(soundStream.getDirection() == sf::Vector3f(0, 0, -1));
        CHECK(soundStream.getCone().innerAngle == sf::degrees(360));
        CHECK(soundStream.getCone().innerAngle == sf::degrees(360));
        CHECK(soundStream.getCone().outerGain == 0);
        CHECK(soundStream.getVelocity() == sf::Vector3f());
        CHECK(soundStream.getDopplerFactor() == 1);
        CHECK(soundStream.getDirectionalAttenuationFactor() == 1);
        CHECK(!soundStream.isRelativeToListener());
        CHECK(soundStream.getMinDistance() == 1);
        CHECK(soundStream.getMaxDistance() == std::numeric_limits<float>::max());
        CHECK(soundStream.getMinGain() == 0);
        CHECK(soundStream.getMaxGain() == 1);
        CHECK(soundStream.getAttenuation() == 1);
    }

    SECTION("Set/get playing offset")
    {
        SoundStream soundStream;
        soundStream.setPlayingOffset(sf::milliseconds(100));
        CHECK(soundStream.getPlayingOffset() == sf::milliseconds(0));
    }

    SECTION("Set/get loop")
    {
        SoundStream soundStream;
        soundStream.setLooping(true);
        CHECK(soundStream.isLooping());
    }

    SECTION("initialize")
    {
        const std::vector channelMap{sf::SoundChannel::FrontLeft, sf::SoundChannel::FrontRight};
        SoundStream       soundStream;
        soundStream.initialize(2, 44100, channelMap);
        CHECK(soundStream.getChannelCount() == 2);
        CHECK(soundStream.getSampleRate() == 44100);
        CHECK(soundStream.getChannelMap() == channelMap);
        CHECK(soundStream.getStatus() == sf::SoundStream::Status::Stopped);
        CHECK(soundStream.getPlayingOffset() == sf::Time::Zero);
        CHECK(!soundStream.isLooping());
    }

    SECTION("Set/get pitch")
    {
        SoundStream soundStream;
        soundStream.setPitch(42);
        CHECK(soundStream.getPitch() == 42);
    }

    SECTION("Set/get pan")
    {
        SoundStream soundStream;
        soundStream.setPan(1);
        CHECK(soundStream.getPan() == 1);
        soundStream.setPan(2);
        CHECK(soundStream.getPan() == 1);
        soundStream.setPan(-2);
        CHECK(soundStream.getPan() == -1);
    }

    SECTION("Set/get volume")
    {
        SoundStream soundStream;
        soundStream.setVolume(50);
        CHECK(soundStream.getVolume() == 50);
    }

    SECTION("Set/get spatialization enabled")
    {
        SoundStream soundStream;
        soundStream.setSpatializationEnabled(true);
        CHECK(soundStream.isSpatializationEnabled());
    }

    SECTION("Set/get position")
    {
        SoundStream soundStream;
        soundStream.setPosition({1, 2, 3});
        CHECK(soundStream.getPosition() == sf::Vector3f(1, 2, 3));
    }

    SECTION("Set/get direction")
    {
        SoundStream soundStream;
        soundStream.setDirection({4, 5, 6});
        CHECK(soundStream.getDirection() == sf::Vector3f(4, 5, 6));
    }

    SECTION("Set/get cone")
    {
        SoundStream soundStream;
        soundStream.setCone({sf::radians(1), sf::radians(2), 3});
        CHECK(soundStream.getCone().innerAngle == sf::radians(1));
        CHECK(soundStream.getCone().outerAngle == sf::radians(2));
        CHECK(soundStream.getCone().outerGain == 3);
    }

    SECTION("Set/get velocity")
    {
        SoundStream soundStream;
        soundStream.setVelocity({7, 8, 9});
        CHECK(soundStream.getVelocity() == sf::Vector3f(7, 8, 9));
    }

    SECTION("Set/get doppler factor")
    {
        SoundStream soundStream;
        soundStream.setDopplerFactor(1);
        CHECK(soundStream.getDopplerFactor() == 1);
    }

    SECTION("Set/get directional attenuation factor")
    {
        SoundStream soundStream;
        soundStream.setDirectionalAttenuationFactor(1);
        CHECK(soundStream.getDirectionalAttenuationFactor() == 1);
    }

    SECTION("Set/get relative to listener")
    {
        SoundStream soundStream;
        soundStream.setRelativeToListener(true);
        CHECK(soundStream.isRelativeToListener());
    }

    SECTION("Set/get min distance")
    {
        SoundStream soundStream;
        soundStream.setMinDistance(12.34f);
        CHECK(soundStream.getMinDistance() == 12.34f);
    }

    SECTION("Set/get max distance")
    {
        SoundStream soundStream;
        soundStream.setMaxDistance(12.34f);
        CHECK(soundStream.getMaxDistance() == 12.34f);
    }

    SECTION("Set/get min gain")
    {
        SoundStream soundStream;
        soundStream.setMinGain(12.34f);
        CHECK(soundStream.getMinGain() == 12.34f);
    }

    SECTION("Set/get max gain")
    {
        SoundStream soundStream;
        soundStream.setMaxGain(12.34f);
        CHECK(soundStream.getMaxGain() == 12.34f);
    }

    SECTION("Set/get attenuation")
    {
        SoundStream soundStream;
        soundStream.setAttenuation(10);
        CHECK(soundStream.getAttenuation() == 10);
    }
}
