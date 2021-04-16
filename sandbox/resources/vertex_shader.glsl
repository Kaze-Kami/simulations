#version 450 core

layout (location = 0) in vec2 i_position;
layout (location = 0) out vec3 o_color;

uniform vec3 u_color;
uniform float u_phi;

uniform mat4 view;

void main() {
    o_color = u_color * (1.f + sin(u_phi)) / 2.f;
    gl_Position = view * vec4(i_position, 0.f, 1.f);
}