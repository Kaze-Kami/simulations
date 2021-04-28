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
#include <core/renderer/texture/texture.h>

using namespace Engine;

struct Vertex {
    glm::vec2 position, texCoord;
};

class SandboxApplication : public Application {
private:

    float frequency = .3f * 2.f * glm::pi<float>();
    static constexpr int texShaderLocalSize = 16;

    int texDetail = 64;

    bool showDemoWindow = true;

    ShaderProgram* renderShader = nullptr, *computeShader = nullptr;
    VertexArray* vertexArray = nullptr;
    Buffer<Vertex>* vertexBuffer = nullptr;
    Buffer<unsigned int>* indexBuffer = nullptr;

    Uniform<float> phi = Uniform<float>("phi", 0.f);
    Uniform<glm::vec3> color = Uniform<glm::vec3>("color", glm::vec3(1.f, .01f, .01f));

    // texture
    bool needsTextureUpdate = true;
    unsigned int texId;

    Texture* texture = nullptr;

    // blinking
    Uniform<float> blinkThreshold = Uniform<float>("blinkThreshold", .65);
    Uniform<float> brightnessFalloff = Uniform<float>("brightnessFalloff", 1.685);

    // cente
    Uniform<int> renderCenter = Uniform<int>("renderCenter", 1);
    Uniform<float> centerLightBase = Uniform<float>("centerLightBase", .5f);
    Uniform<float> centerLightMu = Uniform<float>("centerLightMu", 2.f);
    Uniform<float> centerDarkBase = Uniform<float>("centerDarkBase", 0.f);
    Uniform<float> centerDarkMu = Uniform<float>("centerDarkMu", 15.f);

    // glow
    Uniform<int> renderGlow = Uniform<int>("renderGlow", 1);
    Uniform<float> glowLightBase = Uniform<float>("glowLightBase", .5f);
    Uniform<float> glowLightMu = Uniform<float>("glowLightMu", 1.25f);
    Uniform<float> glowDarkBase = Uniform<float>("glowDarkBase", .65f);
    Uniform<float> glowDarkMu = Uniform<float>("glowDarkMu", .5f);

    CameraController cameraController = CameraController(glm::vec2(1.f), .1f, "view");

public:
    void onContextAttach(Context* context) override;

    void onContextDetach(Context* context) override;

protected:
    void setup(Engine::ApplicationProps& props) override;

    void update(float dt) override;

    void render(Engine::Context* context) override;

    void renderImGui() override;

private:
    void updateTexture();
};



