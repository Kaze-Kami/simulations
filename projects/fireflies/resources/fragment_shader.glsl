#version 450 core

struct Vertex {
    vec4 color;
    vec2 position;
    vec2 center;
    float size;
};

layout(location = 0) in Vertex i_vertex;
layout(location = 0) out vec4 o_color;

// test flags to rener either center, glow or both
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

vec3 mixColor(vec3 color, float mixLight, float mixDark) {
    // todo: this probably can be improved
    return mix(mix(color, vec3(1.f), mixLight), vec3(0.f), mixDark);
}

void main() {
    // calculate distance to center
    float d = distance(i_vertex.center, i_vertex.position);
    if (i_vertex.size < d) {
        // not within circle
        discard;
    }

    // normalize distance and scale to range [0, ..., 2]
    d *= 2.f / i_vertex.size;

    vec3 color = vec3(0.f);
    float alpha = 0.f;

    if (d < 1.f) {
        // calculate mix
        const float mixLight = centerLightBase * exp(-centerLightMu * d);
        const float mixDark = centerDarkBase + (1.f - centerDarkBase) * pow(d, centerDarkMu);
        // mix base color
        const vec3 basecolor = mixColor(i_vertex.color.rgb, mixLight, mixDark);
        // add actual brightness
        color = mix(basecolor, vec3(0.f), 1.f - i_vertex.color.a);
        // alpha *= renderCenter E [0, 1] (enable/disable color)
        alpha = renderCenter * i_vertex.color.a;
    } else {
        // rescale d from [1, ..., 2] -> [0, ..., 1]
        d -= 1.f;

        // calculate mix
        const float mixLight = -glowLightBase * pow(d, glowLightMu) + glowLightBase;
        const float mixDark = glowDarkBase + (1.f - glowDarkBase) * pow(d, glowDarkMu);
        // mix color
        color = mixColor(i_vertex.color.rgb, mixLight, mixDark);
        // alpha *= renderCenter E [0, 1] (enable/disable color)
        alpha = renderGlow * (1.f - mixLight) * i_vertex.color.a;
    }

    o_color = vec4(color.rgb, alpha);
}