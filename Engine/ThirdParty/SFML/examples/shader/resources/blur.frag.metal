#include <metal_stdlib>
using namespace metal;

struct FragmentIn
{
    float4 color    [[user(sf_color)]];
    float2 texCoord [[user(sf_texcoord)]];
};

fragment float4 main0(FragmentIn input [[stage_in]],
                      constant float& blur_radius [[buffer(1)]],
                      texture2d<float> texture [[texture(0)]],
                      sampler textureSampler [[sampler(0)]])
{
    float2 offx = float2(blur_radius, 0.0);
    float2 offy = float2(0.0, blur_radius);

    float4 pixel = texture.sample(textureSampler, input.texCoord)               * 4.0 +
                   texture.sample(textureSampler, input.texCoord - offx)        * 2.0 +
                   texture.sample(textureSampler, input.texCoord + offx)        * 2.0 +
                   texture.sample(textureSampler, input.texCoord - offy)        * 2.0 +
                   texture.sample(textureSampler, input.texCoord + offy)        * 2.0 +
                   texture.sample(textureSampler, input.texCoord - offx - offy) * 1.0 +
                   texture.sample(textureSampler, input.texCoord - offx + offy) * 1.0 +
                   texture.sample(textureSampler, input.texCoord + offx - offy) * 1.0 +
                   texture.sample(textureSampler, input.texCoord + offx + offy) * 1.0;

    return input.color * (pixel / 16.0);
}
