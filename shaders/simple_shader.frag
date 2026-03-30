#version 450

// Only location and type (vec3) matter, the name doesn't need to match

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Pushdata
{
    mat4 transform; // projection * view * model
    mat4 normalMatrix;
} pushdata;


void main()
{
    // r g b a
    outColor = vec4(fragColor, 1.0);
}

