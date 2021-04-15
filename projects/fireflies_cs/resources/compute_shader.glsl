#version 450

#define LOCAL_SIZE 10
#define ONE_OVER_SQRT_3 0.577350269189625764509148780501

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

layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

layout (std430, binding = 0) buffer FireflyBufferLayout {
    Firefly fireflies[];
};

uniform float epsilonV;
uniform float epsilonC;

void main() {
    // get our id
    const uint index =
            gl_LocalInvocationIndex +
            LOCAL_SIZE * LOCAL_SIZE * LOCAL_SIZE *
            (
                gl_WorkGroupID.x +
                gl_WorkGroupID.y * gl_NumWorkGroups.x +
                gl_WorkGroupID.z * gl_NumWorkGroups.x * gl_NumWorkGroups.y
            )
    ;
    const int numFireflies = fireflies.length();

    // read our data
    Firefly fA = fireflies[index];
    const vec2 aPosition = fA.position;
    const vec3 aColor = fA.color;
    const float aPhi = fA.phi;
    const float aFrequency = fA.frequency;

    // reset update data
    fA.nudgePhi = 0;
    fA.nudgeFrequency = 0;

    const float c = 1.f / numFireflies;

    for (int i = 0; i < numFireflies; i++) {
        const Firefly fB = fireflies[i];

        // read other data
        const vec2 bPosition = fB.position;
        const vec3 bColor = fB.color;
        const float bPhi = fB.phi;
        const float bFrequency = fB.frequency;

        // calculate base mu
        const float dist = distance(aPosition, bPosition);
        // sim := pow(sqrt(3) - ||aCol - bCol||, eC);
        // vis := pow(1.f + ||aPos - bPos||, eV);
        // mu := c * sim / vis
        const float mu = c * pow(1.f - distance(aColor, bColor) * ONE_OVER_SQRT_3, epsilonC) / pow(1.f + dist, epsilonV);

        /* calculate nudge from this interaction */

        // phi
        fA.nudgePhi += mu * atan(sin(bPhi - aPhi), cos(bPhi - aPhi));

        // frequency
        fA.nudgeFrequency += mu * (bFrequency - aFrequency);
    }

    // write our data
    fireflies[index] = fA;
}
