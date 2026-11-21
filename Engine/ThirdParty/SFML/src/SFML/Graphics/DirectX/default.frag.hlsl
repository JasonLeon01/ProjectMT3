Texture2D sf_Texture : register(t0);
SamplerState sf_TextureSampler : register(s0);

float4 main(float4 position : SV_POSITION, float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_TARGET
{
    return sf_Texture.Sample(sf_TextureSampler, texCoord) * color;
}
