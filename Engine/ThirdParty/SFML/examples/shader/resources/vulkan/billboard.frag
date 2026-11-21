#version 450

layout(set = 0, binding = 1) uniform sampler2D sf_Texture;

layout(location = 1) in vec2 tex_coord;
layout(location = 0) out vec4 sf_FragColor;

void main()
{
    // Read and apply a color from the texture
    sf_FragColor = texture(sf_Texture, tex_coord);
}
