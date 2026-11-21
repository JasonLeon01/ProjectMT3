#include <metal_stdlib>
using namespace metal;

struct User
{
    float4 color;
};

fragment float4 main0(constant User& user [[buffer(1)]])
{
    return user.color;
}
