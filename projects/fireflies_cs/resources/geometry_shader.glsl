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

        _emitSquare(position, vec4(color, brightness), size);
    }
}