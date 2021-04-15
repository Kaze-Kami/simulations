#version 450 core

/* from geometry shader:
 *     layout (location = 0) out vec4 o_color;
 */

struct Vertex {
    vec4 color;
    vec2 position;
    vec2 center;
    float size;
};

// layout(location = 0) in vec4 i_color;
layout(location = 0) in Vertex i_vertex;

layout(location = 0) out vec4 o_color;

void main() {
    float d = distance(i_vertex.center, i_vertex.position);
    if (d < i_vertex.size) {
        // o_color = i_color;
        o_color = i_vertex.color;
    } else {
        discard;
    }
}