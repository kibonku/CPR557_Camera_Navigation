#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

// Please note that the model is at (0, 0, -2.5) for now
// Put the light on the front left of the object
const vec3 DIRECTION_TO_LIGHT = normalize(vec3(-3.0, 10.0, 1.0));
const float AMBIENT = 0.02; // to simulate indirect illumination - directionless ambient light (try without it)

// Note 128 bytes can only contain 2 4x4 matrices, so we have run into the limitation
layout(push_constant) uniform Pushdata
{
    mat4 transform; // projetion * view * model
    mat4 modelMatrix;
} pushdata;


void main()
{
    gl_Position = pushdata.transform * vec4(position, 1.0);
  
    // Note: this ony works in certain condition
    // it is only correct if uniform scaling is applied (sz == sy == sz)
    vec3 normalWorldSpace = normalize(mat3(pushdata.modelMatrix) * normal);

    // If normal is away from the light source, the dot product can be negative. So we need to set the lower bound to 0
    float lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

    fragColor = lightIntensity * color;
}

