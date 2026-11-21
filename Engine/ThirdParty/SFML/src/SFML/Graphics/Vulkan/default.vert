#version 450

layout(location = 0) in vec2 sf_Vertex;
layout(location = 1) in vec4 sf_Color;
layout(location = 2) in vec2 sf_MultiTexCoord0;

layout(set = 0, binding = 0, std140) uniform Draw
{
    mat4 transform;
    vec4 textureTransform;
} sf_Draw;

layout(location = 0) out vec4 sf_FrontColor;
layout(location = 1) out vec2 sf_TexCoord0;

void main()
{
    gl_Position = sf_Draw.transform * vec4(sf_Vertex, 0.0, 1.0);
    gl_PointSize = 1.0;
    sf_FrontColor = sf_Color;
    sf_TexCoord0 = sf_MultiTexCoord0 * sf_Draw.textureTransform.xy + sf_Draw.textureTransform.zw;
}
