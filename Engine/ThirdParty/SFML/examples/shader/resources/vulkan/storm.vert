#version 450

layout(location = 0) in vec2 sf_Vertex;
layout(location = 1) in vec4 sf_Color;
layout(location = 2) in vec2 sf_MultiTexCoord0;

layout(set = 0, binding = 0, std140) uniform Draw
{
    mat4 transform;
    vec4 textureTransform;
    mat4 modelView;
    mat4 projection;
} sf_Draw;

layout(set = 1, binding = 0, std140) uniform Storm
{
    vec2 storm_position;
    float storm_total_radius;
    float storm_inner_radius;
};

layout(location = 0) out vec4 sf_FrontColor;
layout(location = 1) out vec2 sf_TexCoord0;

void main()
{
    vec4 vertex = sf_Draw.modelView * vec4(sf_Vertex, 0.0, 1.0);
    vec2 offset = vertex.xy - storm_position;
    float len = length(offset);
    if (len < storm_total_radius)
    {
        float push_distance = storm_inner_radius + len / storm_total_radius * (storm_total_radius - storm_inner_radius);
        vertex.xy = storm_position + normalize(offset) * push_distance;
    }

    gl_Position = sf_Draw.projection * vertex;
    gl_PointSize = 1.0;
    sf_TexCoord0 = sf_MultiTexCoord0 * sf_Draw.textureTransform.xy + sf_Draw.textureTransform.zw;
    sf_FrontColor = sf_Color;
}
