#version 450 core
layout(location = 0) in vec3 v_in_position;
layout(location = 1) in vec3 v_in_normal;
layout(location = 2) in vec3 v_in_tangent;
layout(location = 3) in vec2 v_in_uv;

layout(location = 0) out vec3 f_in_color;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 projection;
    mat4 view;
    mat4 model;
    float time;
} ubo;

void main()
{
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(v_in_position, 1.0);
    f_in_color = v_in_normal;
}