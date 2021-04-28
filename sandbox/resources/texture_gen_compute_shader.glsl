#version 450 core

#define ONE_OVER_SQRT_2 0.7071067811865475244008443621048490392848359376884740365883398689


/*
 * Compute shader to generate firefly textures from given parametes
 *
 */

#define LOCAL_SIZE 16
layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform writeonly restrict image2D outTex;

uniform int renderCenter;
uniform int renderGlow;

uniform float centerLightBase;
uniform float centerDarkBase;
uniform float centerLightMu;
uniform float centerDarkMu;

uniform float glowLightBase;
uniform float glowDarkBase;
uniform float glowLightMu;
uniform float glowDarkMu;

void main() {
    const ivec2 texPos = ivec2(gl_LocalInvocationID) + ivec2(gl_WorkGroupID) * LOCAL_SIZE;
    const vec2 position = -1.f + 2.f * vec2(texPos) / (gl_NumWorkGroups.xy * LOCAL_SIZE);

    float dist = 2.f * length(position);

    float l = 0, d = 0, a = 0;

    // calculate mix
    if (dist < 1.f) {
        l = centerLightBase * exp(-centerLightMu * dist);
        d = centerDarkBase + (1.f - centerDarkBase) * pow(dist, centerDarkMu);
        a = renderCenter;
    } else if (dist < 2.f) {
        dist -= 1.f;
        l = -glowLightBase * pow(dist, glowLightMu) + glowLightBase;
        d = glowDarkBase + (1.f - glowDarkBase) * pow(dist, glowDarkMu);
        a = renderGlow * (1.f - l);
    }

    // col' = col(1 + (l - 1)d - l) + (1 - d)l
    //    '= col * c0 + c1
    const float c0 = (1.f - l) * (1.f - d);
    const float c1 = (1.f - d) * l;

    // save to texture
    imageStore(outTex, texPos, vec4(c0, c1, a, 0.f));
}
