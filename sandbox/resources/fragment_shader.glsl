#version 450 core

layout (location = 0) in vec2 i_texCoord;
layout (location = 1) in vec3 i_color;
layout (location = 2) in float i_brightness;

layout (location = 0) out vec4 o_color;

layout (binding = 0) uniform sampler2D fireflyTexture;

vec4 mixColor(vec3 color, vec4 light, float brightness) {
    vec3 baseColor = color * light.x + light.y;
    float baseAlpha = brightness * light.z;
    baseColor *= brightness;
    return vec4(baseColor, baseAlpha);
}

void main() {
    o_color = mixColor(i_color, texture(fireflyTexture, i_texCoord), i_brightness);
}