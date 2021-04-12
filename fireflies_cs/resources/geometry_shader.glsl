#version 450 core

layout (points) in;

/* from vertex shader:
 *     layout (location = 0) out vec2 o_position;
 *     layout (location = 1) out vec3 o_color;
 *     layout (location = 2) out float o_brightness;
 *     layout (location = 3) out float o_size;
 */

layout (location = 0) in vec2 i_position[1];
layout (location = 1) in vec3 i_color[1];
layout (location = 2) in float i_brightness[1];
layout (location = 3) in float i_size[1];

//! intersting questing: do we want 2 dimensional fireflies
//! or do we want '0-dimensional' points?
layout (triangle_strip, max_vertices=4) out;

layout (location = 0) out vec4 o_color;

// todo: this should not be hardcoded here
const float blinkThreshold = .9;

void _emitVertex(const vec2 position, const vec4 color) {
    o_color = color;
    gl_Position = vec4(position, 0.f, 1.f);
    EmitVertex();
}

void _emitSquare(const vec2 position, const float size, const vec4 color) {
    _emitVertex(position + vec2(-size, -size), color);           // bottom left
    _emitVertex(position + vec2( size, -size), color);           // bottom right
    _emitVertex(position + vec2(-size,  size), color);           // top left (triangle strip!)
    _emitVertex(position + vec2( size,  size), color);           // top right
    EndPrimitive();
}

void main() {
    /*
     * Create a square.
     * Sidenote:
     *     Assuming counter clockwise numbering starting at the bottom left corner
     *     we need to output our vertices in the order 0, 1, 3, 2 to achieve this.
     */

    float brightness = i_brightness[0];

    if (blinkThreshold < brightness) {
        const vec2 position = i_position[0];
        const vec3 color = i_color[0];
        const float size = i_size[0];
        brightness = (brightness - blinkThreshold) / (1.f - blinkThreshold);

        _emitSquare(position, size, vec4(color, brightness));
    }
}