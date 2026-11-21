////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2026 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#pragma once

#include <string_view>

namespace sf::priv::MetalBuiltIn
{
inline constexpr std::string_view vertexFunction   = "sf_defaultVertex";
inline constexpr std::string_view fragmentFunction = "sf_defaultFragment";
inline constexpr std::string_view presentVertex    = "sf_presentVertex";
inline constexpr std::string_view presentFragment  = "sf_presentFragment";

inline constexpr std::string_view library = R"(
#include <metal_stdlib>
using namespace metal;

struct SFMLDraw
{
    float4x4 sf_ModelViewProjectionMatrix;
    float4   sf_TextureTransform;
};

struct SFMLVertexIn
{
    float2 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
};

struct SFMLVertexOut
{
    float4 position [[position]];
    float4 color    [[user(sf_color)]];
    float2 texCoord [[user(sf_texcoord)]];
    float  size     [[point_size]];
};

struct SFMLFragmentIn
{
    float4 color    [[user(sf_color)]];
    float2 texCoord [[user(sf_texcoord)]];
};

vertex SFMLVertexOut sf_defaultVertex(SFMLVertexIn input [[stage_in]], constant SFMLDraw& draw [[buffer(0)]])
{
    SFMLVertexOut output;
    output.position = draw.sf_ModelViewProjectionMatrix * float4(input.position, 0.0, 1.0);
    output.color    = input.color;
    output.texCoord = input.texCoord * draw.sf_TextureTransform.xy + draw.sf_TextureTransform.zw;
    output.size     = 1.0;
    return output;
}

fragment float4 sf_defaultFragment(SFMLFragmentIn input [[stage_in]],
                                   texture2d<float> sf_Texture [[texture(0)]],
                                   sampler sf_TextureSampler [[sampler(0)]])
{
    return sf_Texture.sample(sf_TextureSampler, input.texCoord) * input.color;
}

struct SFMLPresentOut
{
    float4 position [[position]];
    float2 texCoord;
};

vertex SFMLPresentOut sf_presentVertex(uint id [[vertex_id]])
{
    const float2 uv = float2((id << 1) & 2, id & 2);
    SFMLPresentOut output;
    output.position = float4(uv * float2(2.0, -2.0) + float2(-1.0, 1.0), 0.0, 1.0);
    output.texCoord = uv;
    return output;
}

fragment float4 sf_presentFragment(SFMLPresentOut input [[stage_in]], texture2d<float> image [[texture(0)]])
{
    constexpr sampler nearest(filter::nearest, address::clamp_to_edge);
    return image.sample(nearest, input.texCoord);
}
)";
} // namespace sf::priv::MetalBuiltIn
