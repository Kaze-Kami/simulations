#pragma once

/*
 * Created by Kami-Kaze on 4/10/2021.
 *
 */

#include <core/application/application.h>

#include <core/renderer/shader_program/shader_program.h>
#include <core/renderer/vertex_array/vertex_array.h>

#include <glm/glm.hpp>
#include <core/renderer/camera/camera_controller.h>

using namespace Engine;

struct Vertex {
    glm::vec2 position;
};

class SandboxApplication : public Application {
private:

    static constexpr float frequency = 1.f * 2.f * glm::pi<float>();

    bool showDemoWindow = true;

    ShaderProgram* renderShader = nullptr;
    VertexArray* vertexArray = nullptr;
    Buffer<Vertex>* vertexBuffer = nullptr;
    Buffer<unsigned int>* indexBuffer = nullptr;

    Uniform<glm::vec3> uColor = { "u_color", glm::vec3(.1f, .8f, .8f) };
    Uniform<float> uPhi = {"u_phi", 0.f};

    CameraController cameraController = CameraController(glm::vec2(1.f), .1f, "view");

public:
    void onContextAttach(Context* context) override;

    void onContextDetach(Context* context) override;

protected:
    void setup(Engine::ApplicationProps& props) override;

    void update(float dt) override;

    void render(Engine::Context* context) override;

    void onEvent(Event* e) override;
};



