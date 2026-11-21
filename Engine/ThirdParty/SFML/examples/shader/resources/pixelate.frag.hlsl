cbuffer Pixelate : register(b1)
{
    float pixel_threshold;
};

Texture2D sf_Texture : register(t0);
SamplerState sf_TextureSampler : register(s0);

float4 main(float4 position : SV_POSITION, float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_TARGET
{
    float factor = 1.0 / (pixel_threshold + 0.001);
    float2 pos = floor(texCoord * factor + 0.5) / factor;
    return sf_Texture.Sample(sf_TextureSampler, pos) * color;
}
