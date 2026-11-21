cbuffer Edge : register(b1)
{
    float edge_threshold;
};

Texture2D sf_Texture : register(t0);
SamplerState sf_TextureSampler : register(s0);

float4 main(float4 position : SV_POSITION, float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_TARGET
{
    const float offset = 1.0 / 512.0;
    float2 offx = float2(offset, 0.0);
    float2 offy = float2(0.0, offset);

    float4 hEdge = sf_Texture.Sample(sf_TextureSampler, texCoord - offy)        * -2.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offy)        *  2.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord - offx - offy) * -1.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord - offx + offy) *  1.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offx - offy) * -1.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offx + offy) *  1.0;

    float4 vEdge = sf_Texture.Sample(sf_TextureSampler, texCoord - offx)        *  2.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offx)        * -2.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord - offx - offy) *  1.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord - offx + offy) * -1.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offx - offy) *  1.0 +
                   sf_Texture.Sample(sf_TextureSampler, texCoord + offx + offy) * -1.0;

    float3 result = sqrt(hEdge.rgb * hEdge.rgb + vEdge.rgb * vEdge.rgb);
    float edge = length(result);
    float4 pixel = color * sf_Texture.Sample(sf_TextureSampler, texCoord);
    if (edge > (edge_threshold * 8.0))
        pixel.rgb = float3(0.0, 0.0, 0.0);
    else
        pixel.a = edge_threshold;
    return pixel;
}
