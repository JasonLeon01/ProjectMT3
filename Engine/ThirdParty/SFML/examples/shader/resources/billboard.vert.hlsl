cbuffer SFMLDraw : register(b0)
{
    column_major float4x4 sf_ModelViewProjectionMatrix;
    float4 sf_TextureTransform;
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
    // Transform the vertex position
    VertexOutput output;
    output.position = mul(sf_ModelViewProjectionMatrix, float4(input.position, 0.0, 1.0));
    output.color = input.color;
    output.texCoord = input.texCoord;
    return output;
}
