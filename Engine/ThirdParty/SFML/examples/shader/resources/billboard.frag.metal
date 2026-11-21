#include <metal_stdlib>
using namespace metal;

fragment float4 main0(float2 pointCoord [[point_coord]],
                      texture2d<float> texture [[texture(0)]],
                      sampler textureSampler [[sampler(0)]])
{
    // Same orientation as the texture coordinates emitted by billboard.geom
    return texture.sample(textureSampler, float2(1.0 - pointCoord.x, pointCoord.y));
}
