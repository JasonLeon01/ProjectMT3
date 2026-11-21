#version 450

layout(location = 0) in vec2 sf_Vertex;

layout(set = 0, binding = 0, std140) uniform Draw
{
    mat4 transform;
    vec4 textureTransform;
} sf_Draw;

void main()
{
    // Transform the vertex position
    gl_Position = sf_Draw.transform * vec4(sf_Vertex, 0.0, 1.0);
}
