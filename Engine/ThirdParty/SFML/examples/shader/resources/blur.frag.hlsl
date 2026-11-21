cbuffer Blur : register(b1)
{
    float blur_radius;
};

Texture2D sf_Texture : register(t0);
SamplerState sf_TextureSampler : register(s0);

float4 main(float4 position : SV_POSITION, float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_TARGET
{
    float2 offx = float2(blur_radius, 0.0);
    float2 offy = float2(0.0, blur_radius);

    float4 pixel = sf_Texture.Sample(sf_TextureSampler, texCoord)               * 4.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord - offx)        * 2.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offx)        * 2.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord - offy)        * 2.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offy)        * 2.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord - offx - offy) * 1.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord - offx + offy) * 1.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offx - offy) * 1.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offx + offy) * 1.0;

    return color * (pixel / 16.0);
}
