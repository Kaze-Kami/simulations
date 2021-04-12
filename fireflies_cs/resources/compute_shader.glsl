#version 450

#define TAU 6.2831853071795864769252867665590
#define PI  3.1415926535897932384626433832795

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
    vec2 position;      /* x, y */
    vec3 color;         /* r, g, b */
    vec4 currentData;   /* phase, frequency, phi, size */
    vec3 updateData;    /* dPhase, dFrequency, dClock */
};

layout (local_size_x = 1, local_size_y = 1) in;

layout (std430, binding = 0) buffer FireflyBufferLayout {
    Firefly fireflies[];
};

uniform float dPhi;

// todo: this should not be hardcoded here
const float muP =  0.3f;              // how much nearby fireflies phase effect a 'this' firefly
const float muF =  0.3f;              // how much nearby fireflies frequency effect a 'this' firefly
const float muC =  0.3f;              // how much nearby fireflies frequency effect a 'this' firefly
const float eVision = 2.f;            // how 'fast' a fireflies vision 'decays'
const float eColor = 10.f;            // how less 'interesting' different colors are

//! https://stackoverflow.com/questions/7428718/algorithm-or-formula-for-the-shortest-direction-of-travel-between-two-degrees-on?rq=1
float shortestDist(float start, float stop) {
    const float modDiff = mod(stop - start, TAU);
    const float shortestDistance = PI - abs(abs(modDiff) - PI);
    return mod(modDiff + TAU, TAU) < TAU ? shortestDistance  : -shortestDistance;
}

void main() {
    // get our id
    const uint index = gl_GlobalInvocationID.x;
    const int numFireflies = fireflies.length();

    // read our data
    Firefly fA = fireflies[index];
    const vec2 aPosition = fA.position;
    const vec2 aV = fA.currentData.xy;
    const float aClock = fA.currentData.z;
    const vec3 aColor = fA.color;

    // reset update data
    vec2 dV = vec2(0.f);
    float dClock = 0;
    for (int i = 0; i < numFireflies; i++) {
        const Firefly fB = fireflies[i];

        // read other data
        const vec2 bPosition = fB.position;
        const vec2 bV = fB.currentData.xy;
        const float bClock = fB.currentData.z;
        const vec3 bColor = fB.color;

        // calculate sim and mu
        const float dist = distance(aPosition, bPosition);
        const float sim = pow((2.f - distance(normalize(aColor), normalize(bColor))) / 2.f, eColor);
        const float baseMu = sim / pow(1.f + dist, eVision);

        // update dPhase, dFrequency and dClock
        dV += baseMu * (bV - aV);

        //! sidenote about dClock:
        //! we could in theory use dPhase to adjust 'our clock'
        //! but im lazy so i'll do it this way.
        // also: this calculation is quite bad in terms of timing (as far as i understand)
        dClock += baseMu * atan(sin(bClock - aClock), cos(bClock - aClock));
    }

    // scale dPhase, dFrequency and dClock
    dV *= vec2(muP, muF);
    dClock *= muC;

    // update firefly
    fA.updateData.xyz = vec3(dV.xy, dClock) * dPhi / numFireflies;

    // write our data
    fireflies[index] = fA;
}