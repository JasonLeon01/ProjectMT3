#version 450

layout(location = 0) in vec2 sf_Vertex;
layout(location = 1) in vec4 sf_Color;
layout(location = 2) in vec2 sf_MultiTexCoord0;

layout(set = 0, binding = 0, std140) uniform Draw
{
    mat4 transform;
    vec4 textureTransform;
} sf_Draw;

layout(set = 1, binding = 0, std140) uniform Wave
{
    float wave_phase;
    vec2 wave_amplitude;
};

layout(location = 0) out vec4 sf_FrontColor;
layout(location = 1) out vec2 sf_TexCoord0;

void main()
{
    vec4 vertex = vec4(sf_Vertex, 0.0, 1.0);
    vertex.x += cos(sf_Vertex.y * 0.02 + wave_phase * 3.8) * wave_amplitude.x
              + sin(sf_Vertex.y * 0.02 + wave_phase * 6.3) * wave_amplitude.x * 0.3;
    vertex.y += sin(sf_Vertex.x * 0.02 + wave_phase * 2.4) * wave_amplitude.y
              + cos(sf_Vertex.x * 0.02 + wave_phase * 5.2) * wave_amplitude.y * 0.3;

    gl_Position = sf_Draw.transform * vertex;
    sf_TexCoord0 = sf_MultiTexCoord0 * sf_Draw.textureTransform.xy + sf_Draw.textureTransform.zw;
    sf_FrontColor = sf_Color;
}
