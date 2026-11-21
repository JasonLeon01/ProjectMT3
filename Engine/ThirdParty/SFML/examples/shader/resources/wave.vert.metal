#include <metal_stdlib>
using namespace metal;

struct SFMLDraw
{
    float4x4 sf_ModelViewProjectionMatrix;
    float4 sf_TextureTransform;
};

struct VertexIn
{
    float2 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
};

struct VertexOut
{
    float4 position [[position]];
    float4 color    [[user(sf_color)]];
    float2 texCoord [[user(sf_texcoord)]];
};

struct Wave
{
    float wave_phase;
    float2 wave_amplitude;
};

vertex VertexOut main0(VertexIn input [[stage_in]],
                       constant SFMLDraw& draw [[buffer(0)]],
                       constant Wave& wave [[buffer(1)]])
{
    float2 position = input.position;
    position.x += cos(input.position.y * 0.02 + wave.wave_phase * 3.8) * wave.wave_amplitude.x
                + sin(input.position.y * 0.02 + wave.wave_phase * 6.3) * wave.wave_amplitude.x * 0.3;
    position.y += sin(input.position.x * 0.02 + wave.wave_phase * 2.4) * wave.wave_amplitude.y
                + cos(input.position.x * 0.02 + wave.wave_phase * 5.2) * wave.wave_amplitude.y * 0.3;

    VertexOut output;
    output.position = draw.sf_ModelViewProjectionMatrix * float4(position, 0.0, 1.0);
    output.texCoord = input.texCoord * draw.sf_TextureTransform.xy + draw.sf_TextureTransform.zw;
    output.color = input.color;
    return output;
}
