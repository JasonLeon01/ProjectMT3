#version 450

layout(set = 1, binding = 0, std140) uniform User
{
    vec4 shades[2];
} user;
layout(location = 0) out vec4 color;

void main()
{
    color = vec4(user.shades[0].r, user.shades[1].g, user.shades[0].b, 1.0);
}
