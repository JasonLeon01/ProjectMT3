#version 450

layout(set = 0, binding = 1) uniform sampler2D sf_Texture;
layout(set = 1, binding = 0, std140) uniform Pixelate
{
    float pixel_threshold;
};

layout(location = 0) in vec4 sf_FrontColor;
layout(location = 1) in vec2 sf_TexCoord0;
layout(location = 0) out vec4 sf_FragColor;

void main()
{
    float factor = 1.0 / (pixel_threshold + 0.001);
    vec2 pos = floor(sf_TexCoord0 * factor + 0.5) / factor;
    sf_FragColor = texture(sf_Texture, pos) * sf_FrontColor;
}
