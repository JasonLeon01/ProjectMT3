#include <metal_stdlib>
using namespace metal;

struct FragmentIn
{
    float4 color [[user(sf_color)]];
};

fragment float4 main0(FragmentIn input [[stage_in]], constant float& blink_alpha [[buffer(1)]])
{
    float4 pixel = input.color;
    pixel.a = blink_alpha;
    return pixel;
}
