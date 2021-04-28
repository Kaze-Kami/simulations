/*
 * Created by Kami-Kaze on 4/10/2021.
 *  todo (next steps)
 *  -
 */

#include "sandbox_application.h"

#include <random>

#include <imgui.h>
#include <glm/gtx/color_space.hpp>

void SandboxApplication::onContextAttach(Context* context) {
    using namespace Engine;

    /** init opengl buffers and vertex array */

    // color
    std::mt19937 engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist01(0, 1);
    // color.data = glm::rgbColor(glm::vec3(dist01(engine) * 360.f, 1.f, 1.f));

    vertexArray = new VertexArray();
    vertexArray->bind();

    Vertex vertices[8] = {
            { glm::vec2(-.5f, -.3f), glm::vec2(0.f, 0.f)},
            { glm::vec2( .1f, -.3f), glm::vec2(1.f, 0.f)},
            { glm::vec2( .1f,  .3f), glm::vec2(1.f, 1.f)},
            { glm::vec2(-.5f,  .3f), glm::vec2(0.f, 1.f)},
            { glm::vec2(-.1f, -.3f), glm::vec2(0.f, 0.f)},
            { glm::vec2( .5f, -.3f), glm::vec2(1.f, 0.f)},
            { glm::vec2( .5f,  .3f), glm::vec2(1.f, 1.f)},
            { glm::vec2(-.1f,  .3f), glm::vec2(0.f, 1.f)},
    };
    vertexBuffer = new Buffer<Vertex>(BufferType::ARRAY_BUFFER, BufferAccess::STATIC_DRAW, 8, vertices);
    vertexBuffer->bind();

    VertexBufferLayout layout;
    layout.pushAttribute<float>(2, false);
    layout.pushAttribute<float>(2, false);
    vertexArray->addBuffer(vertexBuffer, layout);

    vertexArray->unbind();
    vertexBuffer->unbind();

    unsigned int indices[12] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
    };
    indexBuffer = new Buffer<unsigned int>(BufferType::ELEMENT_ARRAY_BUFFER, BufferAccess::STATIC_DRAW, 12, indices);

    // gen texture
    const int texSize = texDetail * texShaderLocalSize;
    texture = new Texture(
            TextureType::TEXTURE_2D, 0,
            texSize, texSize,
            GL_LINEAR, GL_LINEAR,
            GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER,
            TextureFormatInternal::RGBA32F);

    texture->bind();
    texture->bindImage(0, TextureAccess::READ_WRITE);
    texture->bindUnit(0);

    /** load shaders */
    ShaderList shaderList;

    // compute shader
    shaderList.pushFromFile(
            ShaderType::COMPUTE,
            R"(resources\texture_gen_compute_shader.glsl)"
    );
    computeShader = ShaderProgram::createProgram(shaderList);
    shaderList.clear();

    // quad render shader
    shaderList.pushFromFile(
            ShaderType::VERTEX,
            R"(resources\vertex_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::FRAGMENT,
            R"(resources\fragment_shader.glsl)"
    );
    renderShader = ShaderProgram::createProgram(shaderList);
    shaderList.clear();

    renderShader->use();
    renderShader->uploadUniform(phi);
    renderShader->uploadUniform(color);
    renderShader->uploadUniform(blinkThreshold);
    renderShader->uploadUniform(brightnessFalloff);

    context->setClearColor(.01f, .01f, .01f);

    // bind everything for rendering
    vertexArray->bind();
    vertexBuffer->bind();
    indexBuffer->bind();

    context->setClearColor(0.f, 0.f, 0.f, 1.f);
    context->setClearFlags(GL_COLOR_BUFFER_BIT);

    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void SandboxApplication::onContextDetach(Context* context) {

}

void SandboxApplication::setup(Engine::ApplicationProps& props) {
    using namespace Engine;

    WindowProps& windowProps = props.windowProps;

    // simple window
    props.name = "Sandbox application";
    windowProps.name = "Sandbox Window";
    windowProps.width = 1000;
    windowProps.height = 1000;
    windowProps.center = true;
    windowProps.multisample = 16;
    windowProps.vsync = true;
    windowProps.fpsCounterEnable = true;
    windowProps.fpsCounterColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
}

void SandboxApplication::update(float dt) {
    phi.data = glm::mod(phi.data + frequency * dt, 2.f * glm::pi<float>());
    renderShader->uploadUniform(phi);

    cameraController.update(getWindow()->getInputController());

    if (needsTextureUpdate) updateTexture();
}

void SandboxApplication::render(Engine::Context* context) {
    renderShader->use();
    if (cameraController.needsUpload()) {
        renderShader->uploadUniform(cameraController.getCamera().getUniform());
    }
    GL_CALL(glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr));
}


bool UniformDragFloat(const char* name, Uniform<float>& u, ShaderProgram* shader,
                      float v_speed=1.f, float v_min=0.f, float v_max=0.f) {
    //! use u.name as id as uniform names are (or at least should be) unique.
    //! in case multiple elements respond to input on one element
    //! the error is most likely caused by non-unique uniform names.
    ImGui::PushID(u.name.c_str());
    if (ImGui::DragFloat(name, &u.data, v_speed, v_min, v_max)) {
        if (shader != nullptr) {
            shader->use();
            shader->uploadUniform(u);
        }
        ImGui::PopID();
        return true;
    }
    ImGui::PopID();
    return false;
}

bool UniformBool(const char* name, Uniform<int>& u, ShaderProgram* shader) {
    bool enable = u.data;
    if (ImGui::Checkbox(name, &enable)) {
        u.data = enable;
        if (shader != nullptr) {
            shader->use();
            shader->uploadUniform(u);
        }
        return true;
    }
    return false;
}

void SandboxApplication::renderImGui() {
    UniformDragFloat("Blink threshold", blinkThreshold, renderShader, .001f, 0.f, 1.f);
    UniformDragFloat("Brightness falloff", brightnessFalloff, renderShader, .001f, 0.f, 2.f);

    needsTextureUpdate |= UniformBool("Center", renderCenter, nullptr);

    ImGui::Indent(4.f);
    needsTextureUpdate |= UniformDragFloat("Light base", centerLightBase, nullptr, .001f, 0.f, 1.f);
    needsTextureUpdate |= UniformDragFloat("Light mu", centerLightMu, nullptr, .001f, 0.f, 20.f);

    needsTextureUpdate |= UniformDragFloat("Dark base", centerDarkBase, nullptr, .001f, 0.f, 1.f);
    needsTextureUpdate |= UniformDragFloat("Dark mu", centerDarkMu, nullptr, .001f, 0.f, 20.f);
    ImGui::Unindent(4.f);

    needsTextureUpdate |= UniformBool("Glow", renderGlow, nullptr);

    ImGui::Indent(4.f);
    needsTextureUpdate |= UniformDragFloat("Light base", glowLightBase, nullptr, .001f, 0.f, 1.f);
    needsTextureUpdate |= UniformDragFloat("Light mu", glowLightMu, nullptr, .001f, 0.f, 20.f);

    needsTextureUpdate |= UniformDragFloat("Dark base", glowDarkBase, nullptr, .001f, 0.f, 1.f);
    needsTextureUpdate |= UniformDragFloat("Dark mu", glowDarkMu, nullptr, .001f, 0.f, 20.f);
    ImGui::Unindent(4.f);
}

void SandboxApplication::updateTexture() {
    // push uniforms
    computeShader->use();
    computeShader->uploadUniform(renderCenter);
    computeShader->uploadUniform(centerLightBase);
    computeShader->uploadUniform(centerLightMu);
    computeShader->uploadUniform(centerDarkBase);
    computeShader->uploadUniform(centerDarkMu);

    computeShader->uploadUniform(renderGlow);
    computeShader->uploadUniform(glowLightBase);
    computeShader->uploadUniform(glowLightMu);
    computeShader->uploadUniform(glowDarkBase);
    computeShader->uploadUniform(glowDarkMu);

    GL_CALL(glDispatchCompute(texDetail, texDetail, 1));

    // wait for our compute shaders
    // this should be the right one - hopefully
    GL_CALL(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

    needsTextureUpdate = false;
}
