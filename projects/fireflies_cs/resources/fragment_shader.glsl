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

layout(location = 0) in Vertex i_vertex;

layout(location = 0) out vec4 o_color;

void main() {
    float d = distance(i_vertex.center, i_vertex.position);
    if (d < i_vertex.size) {
        const float c = i_vertex.color.a * d / i_vertex.size;
        o_color = vec4(i_vertex.color.xyz, c);
    } else {
        discard;
    }
}