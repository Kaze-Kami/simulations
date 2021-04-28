#version 450 core

layout (location = 0) in vec2 i_position;
layout (location = 1) in vec2 i_texCoord;

layout (location = 0) out vec2 o_texCoord;
layout (location = 1) out vec3 o_color;
layout (location = 2) out float o_brightness;

uniform vec3 color;
uniform float phi;

uniform float blinkThreshold;
uniform float brightnessFalloff;

uniform mat4 view;

float makeAlpha(highp float brightness) {
    const highp float sx2 = pow(brightness, 2.f) * (brightnessFalloff * .5f + 1.f);
    return sx2 * (brightnessFalloff - sx2) - pow(brightnessFalloff, 2.f) * .25f + 1.f;
}

void main() {
    // calculate brightness
    highp float brightness = (1.f + sin(phi)) / 2.f;
    if (blinkThreshold < brightness) {
        brightness = makeAlpha(1.f - (brightness - blinkThreshold) / (1.f - blinkThreshold));
    } else {
        brightness = 0;
    }

    // pass data
    o_texCoord = i_texCoord;
    o_color = color;
    o_brightness = brightness;
    gl_Position = view * vec4(i_position, 0.f, 1.f);
}