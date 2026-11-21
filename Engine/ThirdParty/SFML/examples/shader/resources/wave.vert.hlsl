cbuffer SFMLDraw : register(b0)
{
    column_major float4x4 sf_ModelViewProjectionMatrix;
    float4 sf_TextureTransform;
};

cbuffer Wave : register(b1)
{
    float wave_phase;
    float2 wave_amplitude;
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
    float2 position = input.position;
    position.x += cos(input.position.y * 0.02 + wave_phase * 3.8) * wave_amplitude.x
                + sin(input.position.y * 0.02 + wave_phase * 6.3) * wave_amplitude.x * 0.3;
    position.y += sin(input.position.x * 0.02 + wave_phase * 2.4) * wave_amplitude.y
                + cos(input.position.x * 0.02 + wave_phase * 5.2) * wave_amplitude.y * 0.3;

    VertexOutput output;
    output.position = mul(sf_ModelViewProjectionMatrix, float4(position, 0.0, 1.0));
    output.color = input.color;
    output.texCoord = input.texCoord * sf_TextureTransform.xy + sf_TextureTransform.zw;
    return output;
}
