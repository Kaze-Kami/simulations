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
    const float d = 2.f * distance(i_vertex.center, i_vertex.position);
    const float ra = i_vertex.color.a;
    if (d < i_vertex.size) {
        const float a = 1. - pow(d / i_vertex.size, 10.f);
        const float c = .5f + .5f * pow(d, 2);
        o_color = vec4(i_vertex.color.xyz * c, ra * a);
    } else if (d < 2.f * i_vertex.size) {
        // const float a = 1.f * (d - i_vertex.size);
        //o_color = vec4(i_vertex.color.xyz, d);
        o_color = vec4(1.f, 0.f, 0.f, d - i_vertex.size);
    } else {
        discard;
    }
}