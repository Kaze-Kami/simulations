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
#include <core/renderer/buffer/buffer.h>

using namespace Engine;

struct FireflyData {
    glm::vec2 position;
    float _p3 = 0, _p4 = 0;         // padding

    glm::vec3 color;

    float size;
    float phi, frequency;
    float nudgePhi = 0, nudgeFrequency = 0;
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
    /* constants */
    static constexpr float TWO_PI = glm::radians(360.f);

    /* todo: put this somewhere (aka get your ass up and add imgui support lol)*/

    // general config
    static constexpr int NUM_FIREFLIES = 5000;
    static constexpr int NUM_COLORS = 5;
    static constexpr float SIMULATION_SPEED = 1.f;

    static constexpr float FIREFLY_SIZE = .002f;
    static constexpr float FIREFLY_MAX_PHASE = TWO_PI;
    static constexpr float FIREFLY_MAX_FREQUENCY = 1.5f;

    static constexpr float
            muP = 1.f,              // how much nearby fireflies phase effect a 'this' firefly
            muF = 1.f,              // how much nearby fireflies frequency effect a 'this' firefly
            epsilonV = 15.f,        // how 'fast' a fireflies vision 'decays'
            epsilonC = 20.f;        // how less 'interesting' different colors are

    // internal
    static constexpr float OMEGA = SIMULATION_SPEED * TWO_PI;

    /* Opengl */
    ShaderProgram* renderShader = nullptr;
    ShaderProgram* computeShader = nullptr;
    Buffer<FireflyData>* computeBuffer;

    int rsDPhiLocation = 0;
};



