#pragma once

/*
 * Created by Kami-Kaze on 4/11/2021.
 *
 *
 *
 * Next steps:
 */

#include <glm/glm.hpp>

#include <core/application/application.h>
#include <core/renderer/shader_program/shader_program.h>

using namespace Engine;

// this needs to 'fit' into std430 buffer format -> todo: figure out padding...
// padding should be [4 float][4 float] for vecX and [1 float] for floats
struct _Firefly {
    glm::vec2 position;
    float _p2 = 0, _p3 = 0; // match 4x padding (vec2 + 2 float)

    glm::vec3 color;
    float _c3 = 0;              // match 4x padding (vec3 + float)

    float phase, frequency, phi, size;
    float dPhase, dFrequency, dPhi = 0, _d2 = 0;
};

class FirefliesCsApplication : public Application {
    /* Inherited methods */
public:
    // those two are probably not needed
    void init() override;

    void shutdown() override;

    // set up opengl
    void onContextAttach() override;

    // shut down opengl
    void onContextDetach() override;

protected:
    // set up application
    void setup(ApplicationProps& props) override;

    // update and render
    void update(float dt) override;

    void render(Context* context) override;


private:
    /* const config;; todo: put this somewhere */
    static constexpr int NUM_FIREFLIES = 10000;
    static constexpr int NUM_COLORS = 5;
    static constexpr float TWO_PI = glm::radians(360.f);
    static constexpr float FIREFLY_SIZE = .002f;
    static constexpr float FIREFLY_MAX_FREQUENCY = 1.5f;
    static constexpr float FIREFLY_MAX_PHASE = TWO_PI;
    static constexpr float                                        // phi(t+dt) = (phi(t) + omega * dt) % phiMax
            FREQUENCY = 1.f,                                      // the speed of or simulation (number of ticks per second)
            OMEGA = FREQUENCY * TWO_PI;                           // update frequency matched to once per second

    /* Opengl */
    ShaderProgram* renderShader = nullptr;
    ShaderProgram* computeShader = nullptr;
    GLuint computeBufferId = 0;
    int csDPhiLocation = 0, rsDPhiLocation = 0;
};



