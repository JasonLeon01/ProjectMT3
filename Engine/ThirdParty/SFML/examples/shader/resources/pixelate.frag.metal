#include <metal_stdlib>
using namespace metal;

struct FragmentIn
{
    float4 color    [[user(sf_color)]];
    float2 texCoord [[user(sf_texcoord)]];
};

fragment float4 main0(FragmentIn input [[stage_in]],
                      constant float& pixel_threshold [[buffer(1)]],
                      texture2d<float> texture [[texture(0)]],
                      sampler textureSampler [[sampler(0)]])
{
    float factor = 1.0 / (pixel_threshold + 0.001);
    float2 pos = floor(input.texCoord * factor + 0.5) / factor;
    return texture.sample(textureSampler, pos) * input.color;
}
