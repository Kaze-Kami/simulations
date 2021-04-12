#version 450 core

layout (location = 0) out vec2 o_position;
layout (location = 1) out vec3 o_color;
layout (location = 2) out float o_brightness;
layout (location = 3) out float o_size;

void main() {
    //! fake data for testing
    o_color = vec3(1.f, 1.f, 0.f);
    o_brightness = 1.f;
    o_size = .5f;
    o_position = vec2(0f, 0.f);
}