cbuffer Blink : register(b1)
{
    float blink_alpha;
};

float4 main(float4 position : SV_POSITION, float4 color : COLOR0, float2 texCoord : TEXCOORD0) : SV_TARGET
{
    float4 pixel = color;
    pixel.a = blink_alpha;
    return pixel;
}
