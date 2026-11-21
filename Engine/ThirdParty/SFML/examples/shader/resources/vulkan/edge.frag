#version 450

layout(set = 0, binding = 1) uniform sampler2D sf_Texture;
layout(set = 1, binding = 0, std140) uniform Edge
{
    float edge_threshold;
};

layout(location = 0) in vec4 sf_FrontColor;
layout(location = 1) in vec2 sf_TexCoord0;
layout(location = 0) out vec4 sf_FragColor;

void main()
{
    const float offset = 1.0 / 512.0;
    vec2 offx = vec2(offset, 0.0);
    vec2 offy = vec2(0.0, offset);

    vec4 hEdge = texture(sf_Texture, sf_TexCoord0 - offy)        * -2.0 +
                 texture(sf_Texture, sf_TexCoord0 + offy)        *  2.0 +
                 texture(sf_Texture, sf_TexCoord0 - offx - offy) * -1.0 +
                 texture(sf_Texture, sf_TexCoord0 - offx + offy) *  1.0 +
                 texture(sf_Texture, sf_TexCoord0 + offx - offy) * -1.0 +
                 texture(sf_Texture, sf_TexCoord0 + offx + offy) *  1.0;

    vec4 vEdge = texture(sf_Texture, sf_TexCoord0 - offx)        *  2.0 +
                 texture(sf_Texture, sf_TexCoord0 + offx)        * -2.0 +
                 texture(sf_Texture, sf_TexCoord0 - offx - offy) *  1.0 +
                 texture(sf_Texture, sf_TexCoord0 - offx + offy) * -1.0 +
                 texture(sf_Texture, sf_TexCoord0 + offx - offy) *  1.0 +
                 texture(sf_Texture, sf_TexCoord0 + offx + offy) * -1.0;

    vec3 result = sqrt(hEdge.rgb * hEdge.rgb + vEdge.rgb * vEdge.rgb);
    float edge = length(result);
    vec4 pixel = sf_FrontColor * texture(sf_Texture, sf_TexCoord0);
    if (edge > (edge_threshold * 8.0))
        pixel.rgb = vec3(0.0, 0.0, 0.0);
    else
        pixel.a = edge_threshold;
    sf_FragColor = pixel;
}
