#version 450

// Binding 0 of set 1 is the vertex stage's Storm block
layout(set = 1, binding = 1, std140) uniform Blink
{
    float blink_alpha;
};

layout(location = 0) in vec4 sf_FrontColor;
layout(location = 0) out vec4 sf_FragColor;

void main()
{
    vec4 pixel = sf_FrontColor;
    pixel.a = blink_alpha;
    sf_FragColor = pixel;
}
