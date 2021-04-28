#version 450

#define LOCAL_SIZE 1
#define ONE_OVER_SQRT_3 0.577350269189625764509148780501

struct Firefly {
    vec2 position;      // x, y
    // 2x padding
    vec3 color;         // r, g, b
    // 1x padding
    float size;
    float phi, frequency;
    float nudgePhi, nudgeFrequency;
    // 3x padding
    int colorIndex;
};

layout (local_size_x = LOCAL_SIZE, local_size_y = LOCAL_SIZE, local_size_z = LOCAL_SIZE) in;

layout (std430, binding = 0) buffer FireflyBufferLayout {
    Firefly fireflies[];
};

uniform float muVision;
uniform float muColor;

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
        const float mu = c * pow(1.f - distance(aColor, bColor) * ONE_OVER_SQRT_3, muColor) / pow(1.f + dist, muVision);

        /* calculate nudge from this interaction */

        // phi
        fA.nudgePhi += mu * atan(sin(bPhi - aPhi), cos(bPhi - aPhi));

        // frequency
        fA.nudgeFrequency += mu * (bFrequency - aFrequency);
    }

    // write our data
    fireflies[index] = fA;
}
