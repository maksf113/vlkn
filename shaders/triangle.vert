#version 450 core
layout(location = 0) in vec3 v_in_position;
layout(location = 1) in vec3 v_in_color;

layout(location = 0) out vec3 f_in_color;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    float time;
} ubo;

void main()
{
    gl_Position = vec4(v_in_position, 1.0);
    f_in_color = v_in_color * sin(ubo.time);
}