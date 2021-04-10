#pragma once

/*
 * Created by Kami-Kaze on 4/10/2021.
 */

#include <core/application/application.h>
#include <glm/glm.hpp>

#include <core/renderer/shader_program/shader_program.h>
#include <core/renderer/vertex_array/vertex_array.h>

struct Firefly {
    float phase, size;
    glm::vec2 position;
    glm::vec3 color;

    // remaining 'fixed' color values
    static constexpr float colorSaturation = .9, colorValue = .9;
    // firefly size
    static constexpr float baseSize = .001f;
};

struct FireflyVertex {
    glm::vec2 position;             // position of the firefly vertex
    glm::vec3 color;                // base color of the firefly
};

class FirefliesApplication : public Engine::Application {
public:
    void onContextAttach() override;

    void onContextDetach() override;

protected:
    void setup(Engine::ApplicationProps& props) override;

    void update(float dt) override;

    void render(Engine::Context* context) override;

private:

    // config, here for now
    static constexpr int nFireflies = 1000;

    static constexpr float                                     // phi(t+dt) = (phi(t) + omega * dt) % phiMax
            frequency = .5f,                                   // how often we want to blink per second
            omega = frequency * glm::radians(360.f),    // angular velocity of clock
            phiMax = glm::radians(360.f);               // clock max angle

    static constexpr float blinkThreshold = .9;


    static constexpr float                          // nudge[i, j] = baseMu / exp(epsilon * |i - j|)
            baseMu = .10,                           // how much nearby fireflies effect a given firefly
            epsilon = 04.f;                         // how 'fast' a fireflies vision 'decays'



    Firefly fireflies[nFireflies];                      // firefly buffer
    FireflyVertex fireflyVertices[4 * nFireflies];      // firefly vertex buffer
    float nudges[nFireflies];                           // update loop buffer

    float phi = 0;

    Engine::ShaderProgram* shaderProgram = nullptr;
    Engine::VertexBuffer<FireflyVertex>* vertexBuffer = nullptr;
    Engine::Buffer<int>* indexBuffer = nullptr;
    Engine::VertexArray* vertexArray = nullptr;
};



