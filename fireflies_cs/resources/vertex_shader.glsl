#version 450 core

#define TAU 6.2831853071795864769252867665590

struct Firefly {
    vec2 position;      /* x, y */
    vec3 color;         /* r, g, b */
    vec4 currentData;   /* phase, frequency, phi, size */
    vec3 updateData;    /* dPhase, dFrequency, dClock */
};

layout (std430, binding = 0) buffer FireflyBufferLayout {
    Firefly fireflies[];
};

uniform float dPhi;

layout (location = 0) out vec2 o_position;
layout (location = 1) out vec3 o_color;
layout (location = 2) out float o_brightness;
layout (location = 3) out float o_size;

/*
void main() {
    // get our id
    const uint index = gl_InstanceID;

    // get our data
    Firefly f = fireflies[index];

    // update phase, frequency and clock
    f.currentData.xyz += f.updateData;

    // update phi
    const float frequency = f.currentData.y;
    const float phi = mod(f.currentData.z + dPhi * frequency, TAU);
    f.currentData.z = phi;

    // write updated data
    fireflies[index] = f;

    // calculate brightness
    // const float phi = f.currentData.z;
    const float phase = f.currentData.x;
    const float size = f.currentData.w;

    const float bv = (1.f + sin(phi + phase)) / 2.f;
    const float brightness = bv < blinkThreshold ? 0 : (bv - blinkThreshold) / (1.f - blinkThreshold);

    o_color = vec4(f.color, brightness);

    gl_Position = vec4(size * i_vertexPosition + f.position, 0.f, 1.f);
}
*/
void main() {
    /* update data */

    // get our id
    const uint index = gl_InstanceID;

    // get our data
    Firefly f = fireflies[index];

    // update phase, frequency and clock
    f.currentData.xyz += f.updateData;

    // update phi
    const float frequency = f.currentData.y;
    const float phi = mod(f.currentData.z + dPhi * frequency, TAU);
    f.currentData.z = phi;

    // write updated data
    fireflies[index] = f;

    // calculate brightness
    const float phase = f.currentData.x;
    const float size = f.currentData.w;

    /* push data */
    //! real data
    o_color = f.color;
    o_brightness = (1.f + sin(phi + phase)) / 2.f;
    o_size = size;
    o_position = f.position;
}