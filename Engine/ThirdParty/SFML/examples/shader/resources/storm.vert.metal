#include <metal_stdlib>
using namespace metal;

struct SFMLDraw
{
    float4x4 sf_ModelViewProjectionMatrix;
    float4 sf_TextureTransform;
    float4x4 sf_ModelViewMatrix;
    float4x4 sf_ProjectionMatrix;
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
    float size      [[point_size]];
};

struct Storm
{
    float2 storm_position;
    float storm_total_radius;
    float storm_inner_radius;
};

vertex VertexOut main0(VertexIn input [[stage_in]],
                       constant SFMLDraw& draw [[buffer(0)]],
                       constant Storm& storm [[buffer(1)]])
{
    float4 position = draw.sf_ModelViewMatrix * float4(input.position, 0.0, 1.0);
    float2 offset = position.xy - storm.storm_position;
    float len = length(offset);
    if (len < storm.storm_total_radius)
    {
        float push_distance = storm.storm_inner_radius + len / storm.storm_total_radius * (storm.storm_total_radius - storm.storm_inner_radius);
        position.xy = storm.storm_position + normalize(offset) * push_distance;
    }

    VertexOut output;
    output.position = draw.sf_ProjectionMatrix * position;
    output.texCoord = input.texCoord * draw.sf_TextureTransform.xy + draw.sf_TextureTransform.zw;
    output.color = input.color;
    output.size = 1.0;
    return output;
}
