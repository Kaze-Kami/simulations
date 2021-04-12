#pragma once

/*
 * Created by Kami-Kaze on 4/10/2021.
 *
 */

#include <core/application/application.h>

#include <core/renderer/shader_program/shader_program.h>
#include <core/renderer/vertex_array/vertex_array.h>

static constexpr int NUM_FIREFLIES = 1;
static constexpr float FIREFLY_SIZE = .001f;

struct Firefly {
    float size;
    glm::vec2 position;

    float phase, frequency;
};

struct FireflyVertex {
    glm::vec4 position;
    glm::vec4 color;
};

class SandboxApplication : public Engine::Application {
public:
    void onContextAttach() override;

    void onContextDetach() override;

protected:
    void setup(Engine::ApplicationProps& props) override;

    void update(float dt) override;

    void render(Engine::Context* context) override;

private:
    Engine::ShaderProgram* renderShader = nullptr;
    GLuint vertexArrayId = 0, vertexBufferId = 0, indexBufferId;
};

//! https://youtu.be/DVoylVUL7J0
//! https://youtu.be/OqvVFd1W3Lg



