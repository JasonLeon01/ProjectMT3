#include "Audio/bind_SoundChannel.hpp"

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

namespace { constexpr std::array<std::string_view, 1> docs = {
    "\\ingroup audio\n\\brief Types of sound channels that can be read/written from sound buffers/files\n\nIn multi-channel audio, each sound channel can be\nassigned a position. The position of the channel is\nused to determine where to place a sound when it\nis spatialized. Assigning an incorrect sound channel\nwill result in multi-channel audio being positioned\nincorrectly when using spatialization.",
}; }

void bind_SoundChannel(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.SoundChannel");
    LUASF_STUB_FIELD("Unspecified", "sf.SoundChannel");
    LUASF_STUB_FIELD("Mono", "sf.SoundChannel");
    LUASF_STUB_FIELD("FrontLeft", "sf.SoundChannel");
    LUASF_STUB_FIELD("FrontRight", "sf.SoundChannel");
    LUASF_STUB_FIELD("FrontCenter", "sf.SoundChannel");
    LUASF_STUB_FIELD("FrontLeftOfCenter", "sf.SoundChannel");
    LUASF_STUB_FIELD("FrontRightOfCenter", "sf.SoundChannel");
    LUASF_STUB_FIELD("LowFrequencyEffects", "sf.SoundChannel");
    LUASF_STUB_FIELD("BackLeft", "sf.SoundChannel");
    LUASF_STUB_FIELD("BackRight", "sf.SoundChannel");
    LUASF_STUB_FIELD("BackCenter", "sf.SoundChannel");
    LUASF_STUB_FIELD("SideLeft", "sf.SoundChannel");
    LUASF_STUB_FIELD("SideRight", "sf.SoundChannel");
    LUASF_STUB_FIELD("TopCenter", "sf.SoundChannel");
    LUASF_STUB_FIELD("TopFrontLeft", "sf.SoundChannel");
    LUASF_STUB_FIELD("TopFrontRight", "sf.SoundChannel");
    LUASF_STUB_FIELD("TopFrontCenter", "sf.SoundChannel");
    LUASF_STUB_FIELD("TopBackLeft", "sf.SoundChannel");
    LUASF_STUB_FIELD("TopBackRight", "sf.SoundChannel");
    LUASF_STUB_FIELD("TopBackCenter", "sf.SoundChannel");
    lua_glue::BindEnum<sf::SoundChannel>(sf, "SoundChannel", {
        {"Unspecified", sf::SoundChannel::Unspecified},
        {"Mono", sf::SoundChannel::Mono},
        {"FrontLeft", sf::SoundChannel::FrontLeft},
        {"FrontRight", sf::SoundChannel::FrontRight},
        {"FrontCenter", sf::SoundChannel::FrontCenter},
        {"FrontLeftOfCenter", sf::SoundChannel::FrontLeftOfCenter},
        {"FrontRightOfCenter", sf::SoundChannel::FrontRightOfCenter},
        {"LowFrequencyEffects", sf::SoundChannel::LowFrequencyEffects},
        {"BackLeft", sf::SoundChannel::BackLeft},
        {"BackRight", sf::SoundChannel::BackRight},
        {"BackCenter", sf::SoundChannel::BackCenter},
        {"SideLeft", sf::SoundChannel::SideLeft},
        {"SideRight", sf::SoundChannel::SideRight},
        {"TopCenter", sf::SoundChannel::TopCenter},
        {"TopFrontLeft", sf::SoundChannel::TopFrontLeft},
        {"TopFrontRight", sf::SoundChannel::TopFrontRight},
        {"TopFrontCenter", sf::SoundChannel::TopFrontCenter},
        {"TopBackLeft", sf::SoundChannel::TopBackLeft},
        {"TopBackRight", sf::SoundChannel::TopBackRight},
        {"TopBackCenter", sf::SoundChannel::TopBackCenter}
    });
}
