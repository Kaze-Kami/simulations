#version 450 core

#define TAU 6.2831853071795864769252867665590

struct Firefly {
    vec2 position;      /* x, y + padding (0, 0) */
    vec3 color;         /* r, g, b */
    float size;
    float phi, frequency;
    float nudgePhi, nudgeFrequency;
};

layout (std430, binding = 0) buffer FireflyBufferLayout {
    Firefly fireflies[];
};

layout (location = 0) out vec2 o_position;
layout (location = 1) out vec3 o_color;
layout (location = 2) out float o_brightness;
layout (location = 3) out float o_size;

uniform float dPhi;

uniform float muP;
uniform float muF;

void main() {
    // get num fireflies
    const int numFireflies = fireflies.length();

    // get our id
    const uint index = gl_InstanceID;

    /* update data */

    // get our data
    Firefly f = fireflies[index];

    // update phase, frequency and clock
    f.phi += f.nudgePhi * muP * dPhi;
    f.frequency += f.nudgeFrequency * muF * dPhi;

    // update phi
    f.phi = mod(f.phi + dPhi * f.frequency, TAU);

    // write updated data
    fireflies[index] = f;

    /* push data */
    //! real data
    o_color = f.color;
    o_brightness = (1.f + sin(f.phi)) / 2.f;
    o_size = f.size;
    o_position = f.position;
}