#version 450 core

#define SQRT_TWO 1.4142135623730950488016887242096

layout (points) in;

layout (location = 0) in vec2 i_position[1];
layout (location = 1) in vec3 i_color[1];
layout (location = 2) in float i_brightness[1];
layout (location = 3) in float i_size[1];
layout (location = 4) in int i_enable[1];

layout (triangle_strip, max_vertices=16) out;

struct Vertex {
    vec4 color;
    vec2 position;
    vec2 center;
    float size;
};

// layout (location = 0) out vec4 o_color;
layout (location = 0) out Vertex o_vertex;

uniform float fireflySize;
uniform float blinkThreshold;
uniform mat4 view;
uniform mat4 proj;

uniform float brightnessFalloff;

const mat4 mvp = view;

void _emitVertex(const vec2 center, const vec2 offset, const vec4 color, const float size) {
    // o_color = color;
    vec2 position = center + offset;
    o_vertex = Vertex(color, position, center, size);
    gl_Position = mvp * vec4(position, 0.f, 1.f);
    EmitVertex();
}

void _emitSquare(const vec2 position, const vec4 color, const float size) {
    // front face
    _emitVertex(position, vec2(-size, -size), color, size);           // bottom left
    _emitVertex(position, vec2( size, -size), color, size);           // bottom right
    _emitVertex(position, vec2(-size,  size), color, size);           // top left (triangle strip!)
    _emitVertex(position, vec2( size,  size), color, size);           // top right
    EndPrimitive();

}

float makeAlpha(highp float brightness) {
    // old
    // b' = x^4 - 2x^2 + 1; x = sqrt(2) (b - 1);
    // const highp float x = SQRT_TWO * (brightness - 1.f);
    // const highp float vPol = pow(x, 4.f) - 2.f * pow(x, 2.f) + 1.f;
    // return .3f + .7f * vPol;

    // new:
    // x = x * sqrt(c + 2) / sqrt(2)
    // b' = x^2 * (c - x^2) - 1/4 x^2 + 1;
    const highp float sx2 = pow(brightness, 2.f) * (brightnessFalloff * .5f + 1.f);
    return sx2 * (brightnessFalloff - sx2) - pow(brightnessFalloff, 2.f) * .25f + 1.f;
}

void main() {
    /*
     * Create a square.
     * Sidenote:
     *     Assuming counter clockwise numbering starting at the bottom left corner
     *     we need to output our vertices in the order 0, 1, 3, 2 to achieve this.
     */

    if (i_enable[0] == 0) return;

    highp float brightness = i_brightness[0];

    if (blinkThreshold < brightness) {
        const vec2 position = i_position[0];
        const vec3 color = i_color[0];
        const float size = i_size[0];

        // rescale brightness from [blinkThreshold, ..., 1] to [1, ..., 0]
        const highp float alpha = makeAlpha(1.f - (brightness - blinkThreshold) / (1.f - blinkThreshold));

        _emitSquare(position, vec4(color, alpha), size);
    }
}