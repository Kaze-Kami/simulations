#version 450

/*
 * Hint: all values are padded as vec4.
 * We can use those unused floats to return debug data:
 * 'actual' member    | 'used' member  | 'free' floats
 * vec4 position;     |  vec2          |  2
 * vec4 color;        |  vec3          |  1
 * vec4 currentData;  |  vec4          |  0
 * vec4 updateData;   |  vec3          |  1
 */

struct Firefly {
    vec2 position;      /* x, y + padding (0, 0) */
    vec3 color;         /* r, g, b */
    float size;
    float phi, frequency;
    float nudgePhi, nudgeFrequency;
};

layout (local_size_x = 1, local_size_y = 1) in;

layout (std430, binding = 0) buffer FireflyBufferLayout {
    Firefly fireflies[];
};

uniform float epsilonV;
uniform float epsilonC;

void main() {
    // get our id
    const uint index = gl_GlobalInvocationID.x;
    const int numFireflies = fireflies.length();

    // read our data
    Firefly fA = fireflies[index];
    const vec2 aPosition = fA.position;
    const vec3 aColor = fA.color;
    const float aPhi = fA.phi;
    const float aFrequency = fA.frequency;

    // reset update data
    float nudgePhi = 0;
    float nudgeFrequency = 0;

    for (int i = 0; i < numFireflies; i++) {
        const Firefly fB = fireflies[i];

        // read other data
        const vec2 bPosition = fB.position;
        const vec3 bColor = fB.color;
        const float bPhi = fB.phi;
        const float bFrequency = fB.frequency;

        // calculate base mu
        const float dist = distance(aPosition, bPosition);
        const float sim = pow((2.f - distance(normalize(aColor), normalize(bColor))) / 2.f, epsilonC);
        const float mu = sim / pow(1.f + dist, epsilonV);

        /* calculate nudge from this interaction */

        // phi
        nudgePhi += mu * atan(sin(bPhi - aPhi), cos(bPhi - aPhi));

        // frequency
        nudgeFrequency += mu * (bFrequency - aFrequency);
    }

    // update firefly
    fA.nudgePhi = nudgePhi / numFireflies;
    fA.nudgeFrequency = nudgeFrequency / numFireflies;

    // write our data
    fireflies[index] = fA;
}
