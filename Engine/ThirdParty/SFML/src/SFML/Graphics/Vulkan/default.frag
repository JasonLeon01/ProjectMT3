#version 450

layout(set = 0, binding = 1) uniform sampler2D sf_Texture;
layout(location = 0) in vec4 sf_FrontColor;
layout(location = 1) in vec2 sf_TexCoord0;
layout(location = 0) out vec4 sf_FragColor;

void main()
{
    sf_FragColor = sf_FrontColor * texture(sf_Texture, sf_TexCoord0);
}
