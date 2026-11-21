#include <metal_stdlib>
using namespace metal;

struct FragmentIn
{
    float4 color    [[user(sf_color)]];
    float2 texCoord [[user(sf_texcoord)]];
};

fragment float4 main0(FragmentIn input [[stage_in]],
                      constant float& edge_threshold [[buffer(1)]],
                      texture2d<float> texture [[texture(0)]],
                      sampler textureSampler [[sampler(0)]])
{
    const float offset = 1.0 / 512.0;
    float2 offx = float2(offset, 0.0);
    float2 offy = float2(0.0, offset);

    float4 hEdge = texture.sample(textureSampler, input.texCoord - offy)        * -2.0 +
                   texture.sample(textureSampler, input.texCoord + offy)        *  2.0 +
                   texture.sample(textureSampler, input.texCoord - offx - offy) * -1.0 +
                   texture.sample(textureSampler, input.texCoord - offx + offy) *  1.0 +
                   texture.sample(textureSampler, input.texCoord + offx - offy) * -1.0 +
                   texture.sample(textureSampler, input.texCoord + offx + offy) *  1.0;

    float4 vEdge = texture.sample(textureSampler, input.texCoord - offx)        *  2.0 +
                   texture.sample(textureSampler, input.texCoord + offx)        * -2.0 +
                   texture.sample(textureSampler, input.texCoord - offx - offy) *  1.0 +
                   texture.sample(textureSampler, input.texCoord - offx + offy) * -1.0 +
                   texture.sample(textureSampler, input.texCoord + offx - offy) *  1.0 +
                   texture.sample(textureSampler, input.texCoord + offx + offy) * -1.0;

    float3 result = sqrt(hEdge.rgb * hEdge.rgb + vEdge.rgb * vEdge.rgb);
    float edge = length(result);
    float4 pixel = input.color * texture.sample(textureSampler, input.texCoord);
    if (edge > (edge_threshold * 8.0))
        pixel.rgb = float3(0.0, 0.0, 0.0);
    else
        pixel.a = edge_threshold;
    return pixel;
}
