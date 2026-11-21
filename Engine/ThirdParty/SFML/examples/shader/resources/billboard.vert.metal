#include <metal_stdlib>
using namespace metal;

// Metal has no geometry shaders, so each point of the cloud is drawn as a point sprite instead

struct SFMLDraw
{
    float4x4 sf_ModelViewProjectionMatrix;
    float4 sf_TextureTransform;
};

struct VertexIn
{
    float2 position [[attribute(0)]];
};

struct VertexOut
{
    float4 position [[position]];
    float size      [[point_size]];
};

vertex VertexOut main0(VertexIn input [[stage_in]],
                       constant SFMLDraw& draw [[buffer(0)]],
                       constant float2& size [[buffer(1)]])
{
    VertexOut output;
    output.position = draw.sf_ModelViewProjectionMatrix * float4(input.position, 0.0, 1.0);

    // The GLSL billboards span size / resolution in normalized device coordinates,
    // which is half of size in pixels; point sprites are always square
    output.size = size.x / 2.0;
    return output;
}
