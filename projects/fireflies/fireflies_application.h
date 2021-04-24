#pragma once

/*
 * Created by Kami-Kaze on 4/11/2021.
 * Next Steps:
 *
 */

#include <glm/glm.hpp>

#include <core/application/application.h>
#include <core/events/mouse_events.h>
#include <core/events/key_events.h>

#include <core/renderer/shader_program/shader_program.h>
#include <core/renderer/buffer/buffer.h>
#include <core/renderer/camera/camera.h>
#include <core/renderer/camera/camera_controller.h>

using namespace Engine;

struct FireflyData {
    glm::vec2 position = glm::vec2(0.f);
    float _p3 = 0, _p4 = 0;                        // padding

    glm::vec3 color = glm::vec3(0.f);

    float size = 0;
    float phi = 0, frequency = 0;
    float nudgePhi = 0, nudgeFrequency = 0;
};

class FirefliesApplication : public Application {
    /* Inherited methods */
public:
    // those two are probably not needed
    void init() override;

    void shutdown() override;

    // set up opengl
    void onContextAttach(Context* context) override;

    // shut down opengl
    void onContextDetach(Context* context) override;

protected:
    void setup(ApplicationProps& props) override;

    void update(float dt) override;

    void render(Context* context) override;

    void renderImGui() override;

    void onEvent(Event& e) override;

private:
    /* constants */
    static constexpr float TWO_PI = glm::radians(360.f);

    /* todo: put this somewhere (aka get your ass up and add imgui support lol)*/

    // opengl config
    static constexpr int
            COMPUTE_CLUSTERS_X = 3,
            COMPUTE_CLUSTERS_Y = 2,
            COMPUTE_CLUSTERS_Z = 2,
            COMPUTE_CLUSTER_SIZE_BASE = 10,
    // sike! :')
    COMPUTE_CLUSTER_SIZE_X = COMPUTE_CLUSTER_SIZE_BASE,
            COMPUTE_CLUSTER_SIZE_Y = COMPUTE_CLUSTER_SIZE_BASE,
            COMPUTE_CLUSTER_SIZE_Z = COMPUTE_CLUSTER_SIZE_BASE,
            COMPUTE_CLUSTERS = COMPUTE_CLUSTERS_X * COMPUTE_CLUSTERS_Y * COMPUTE_CLUSTERS_Z,
            COMPUTE_CLUSTER_SIZE = COMPUTE_CLUSTER_SIZE_X * COMPUTE_CLUSTER_SIZE_Y * COMPUTE_CLUSTER_SIZE_Z,
            NUM_FIREFLIES = COMPUTE_CLUSTERS * COMPUTE_CLUSTER_SIZE;

    // initial config
    int numColors = 2, numColorsLoaded = numColors;         // number of colors (0 = continuous)
    bool* enableColors = nullptr;
    float fireflySize = .015f;                              // size of a firefly
    float fireflyMaxFrequency = 1.5f;                       // max frequency of a firefly

    // runtime config
    float simulationSpeed = 1.f;                    // simulation speed (frequency)
    float blinkThreshold = .6f;                     // percentage of one cycle a firefly is not lit

    float
            muP = 5e1f,                  // how much nearby fireflies phase effect a 'this' firefly
            muF = 5e1f,                  // how much nearby fireflies frequency effect a 'this' firefly
            epsilonV = 30.f,             // how 'fast' a fireflies vision 'decays'
            epsilonC = 30.f;             // how less 'interesting' different colors are


    CameraController cameraController = CameraController(glm::vec2(1.f), .1f, "view");
    ShaderProgram* renderShader = nullptr;
    ShaderProgram* computeShader = nullptr;
    Buffer<FireflyData>* computeBuffer;

    Uniform<float> uDPhi = Uniform<float>("dPhi", 0);

    bool onMouseButtonPressEvent(MouseButtonPressEvent& e);
    bool onKeyPressEvent(KeyPressEvent& e);

    void initFireflies();

};



