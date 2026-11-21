cbuffer Billboard : register(b1)
{
    // The render target's resolution (used for scaling)
    float2 resolution;

    // The billboards' size
    float2 size;
};

struct Vertex
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 texCoord : TEXCOORD0;
};

// Input is the passed point cloud, the output consists of triangle strips with four vertices each
[maxvertexcount(4)]
void main(point Vertex input[1], inout TriangleStream<Vertex> output)
{
    // Calculate the half width/height of the billboards
    float2 half_size = size / 2.0;

    // Scale the size based on resolution (1 would be full width/height)
    half_size /= resolution;

    // Retrieve the passed vertex position
    float2 pos = input[0].position.xy;

    Vertex corner;
    corner.color = input[0].color;

    // Bottom left vertex
    corner.position = float4(pos - half_size, 0.0, 1.0);
    corner.texCoord = float2(1.0, 1.0);
    output.Append(corner);

    // Bottom right vertex
    corner.position = float4(pos.x + half_size.x, pos.y - half_size.y, 0.0, 1.0);
    corner.texCoord = float2(0.0, 1.0);
    output.Append(corner);

    // Top left vertex
    corner.position = float4(pos.x - half_size.x, pos.y + half_size.y, 0.0, 1.0);
    corner.texCoord = float2(1.0, 0.0);
    output.Append(corner);

    // Top right vertex
    corner.position = float4(pos + half_size, 0.0, 1.0);
    corner.texCoord = float2(0.0, 0.0);
    output.Append(corner);

    // And finalize the primitive
    output.RestartStrip();
}
