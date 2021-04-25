#version 450 core

#define TAU                 6.2831853071795864769252867665590
#define PI_HALF             1.5707963267948966192313216916397
#define ONE_OVER_2_SQRT_2   0.3535533905932737622004221810524

struct Firefly {
    vec2 position;      // x, y
    // 2x padding
    vec3 color;         // r, g, b
    // 1x padding
    float size;
    float phi, frequency;
    float nudgePhi, nudgeFrequency;
    // 3x padding
    int colorIndex;
};

layout (std430, binding = 0) buffer FireflyBufferLayout {
    Firefly fireflies[];
};

layout (std430, binding = 1) buffer ColorBufferLayout {
    int enableColors[];
};

layout (location = 0) out vec2 o_position;
layout (location = 1) out vec3 o_color;
layout (location = 2) out float o_brightness;
layout (location = 3) out float o_size;
layout (location = 4) out int o_enable;


uniform float dPhi;
uniform float epsilon;

uniform int attractorEnable;
uniform float attractorRange;
uniform float attractorStrength;
uniform vec2 attractorPosition;

void main() {
    // get num fireflies
    const int numFireflies = fireflies.length();

    // get our id
    const uint index = gl_InstanceID;

    /* update data */

    // get our data
    Firefly f = fireflies[index];

    // update phase, frequency and clock
    f.phi += f.nudgePhi * epsilon * dPhi;
    f.frequency += f.nudgeFrequency * epsilon * dPhi;

    // update phi
    f.phi = mod(f.phi + dPhi * f.frequency, TAU);
    
    const int colorEnable = enableColors[f.colorIndex];
    
    // maybe update attractor
    if (attractorEnable != 0 && colorEnable != 0) {
        f.phi *= 1.f - attractorStrength *
                (1.f + tanh(attractorRange - (distance(f.position, attractorPosition) * ONE_OVER_2_SQRT_2) * 20.f)) / 2.f;
    }

    // write updated data
    fireflies[index] = f;

    /* push data */
    o_color = f.color;
    o_brightness = (1.f + sin(f.phi - PI_HALF)) / 2.f;
    o_size = f.size;
    o_position = f.position;
    o_enable = colorEnable;
}