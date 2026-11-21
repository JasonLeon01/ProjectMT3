cbuffer SFMLDraw : register(b0)
{
    column_major float4x4 sf_ModelViewProjectionMatrix;
    float4 sf_TextureTransform;
    column_major float4x4 sf_ModelViewMatrix;
    column_major float4x4 sf_ProjectionMatrix;
};

cbuffer Storm : register(b1)
{
    float2 storm_position;
    float storm_total_radius;
    float storm_inner_radius;
};

struct VertexInput
{
    float2 position : POSITION;
    float4 color : COLOR0;
    float2 texCoord : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 texCoord : TEXCOORD0;
};

VertexOutput main(VertexInput input)
{
    float4 position = mul(sf_ModelViewMatrix, float4(input.position, 0.0, 1.0));
    float2 offset = position.xy - storm_position;
    float len = length(offset);
    if (len < storm_total_radius)
    {
        float push_distance = storm_inner_radius + len / storm_total_radius * (storm_total_radius - storm_inner_radius);
        position.xy = storm_position + normalize(offset) * push_distance;
    }

    VertexOutput output;
    output.position = mul(sf_ProjectionMatrix, position);
    output.color = input.color;
    output.texCoord = input.texCoord * sf_TextureTransform.xy + sf_TextureTransform.zw;
    return output;
}
