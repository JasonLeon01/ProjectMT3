#version 450

layout(set = 0, binding = 1) uniform sampler2D sf_Texture;
// Binding 0 of set 1 is the vertex stage's Wave block
layout(set = 1, binding = 1, std140) uniform Blur
{
    float blur_radius;
};

layout(location = 0) in vec4 sf_FrontColor;
layout(location = 1) in vec2 sf_TexCoord0;
layout(location = 0) out vec4 sf_FragColor;

void main()
{
    vec2 offx = vec2(blur_radius, 0.0);
    vec2 offy = vec2(0.0, blur_radius);

    vec4 pixel = texture(sf_Texture, sf_TexCoord0)               * 4.0 +
                 texture(sf_Texture, sf_TexCoord0 - offx)        * 2.0 +
                 texture(sf_Texture, sf_TexCoord0 + offx)        * 2.0 +
                 texture(sf_Texture, sf_TexCoord0 - offy)        * 2.0 +
                 texture(sf_Texture, sf_TexCoord0 + offy)        * 2.0 +
                 texture(sf_Texture, sf_TexCoord0 - offx - offy) * 1.0 +
                 texture(sf_Texture, sf_TexCoord0 - offx + offy) * 1.0 +
                 texture(sf_Texture, sf_TexCoord0 + offx - offy) * 1.0 +
                 texture(sf_Texture, sf_TexCoord0 + offx + offy) * 1.0;

    sf_FragColor = sf_FrontColor * (pixel / 16.0);
}
