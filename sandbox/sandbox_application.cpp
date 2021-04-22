/*
 * Created by Kami-Kaze on 4/10/2021.
 *  todo (next steps)
 *  - get imgui to work
 */

#include <imgui.h>
#include "sandbox_application.h"

void SandboxApplication::onContextAttach(Context* context) {
    using namespace Engine;

    /** init opengl buffers and vertex array */

    vertexArray = new VertexArray();
    vertexArray->bind();

    glm::vec3 color = glm::vec3(.1f, 1.f, .8f);
    Vertex vertices[4] = {
            { glm::vec2(-.5f, -.5f)},
            { glm::vec2( .5f, -.5f)},
            { glm::vec2( .5f,  .5f)},
            { glm::vec2(-.5f,  .5f)},
    };
    vertexBuffer = new Buffer<Vertex>(BufferType::ARRAY_BUFFER, BufferAccess::STATIC_DRAW, 4, vertices);
    vertexBuffer->bind();

    VertexBufferLayout layout;
    layout.pushAttribute<float>(2, false);
    vertexArray->addBuffer(vertexBuffer, layout);

    vertexArray->unbind();
    vertexBuffer->unbind();

    unsigned int indices[6] = {
            0, 1, 2, 2, 3, 0,
    };
    indexBuffer = new Buffer<unsigned int>(BufferType::ELEMENT_ARRAY_BUFFER, BufferAccess::STATIC_DRAW, 6, indices);


    /** load shader */
    ShaderList shaderList;

    // quad render shader
    shaderList.pushFromFile(
            ShaderType::VERTEX,
            R"(C:\Development\Projects\simulations\sandbox\resources\vertex_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::FRAGMENT,
            R"(C:\Development\Projects\simulations\sandbox\resources\fragment_shader.glsl)"
    );
    renderShader = ShaderProgram::createProgram(shaderList);
    shaderList.clear();

    renderShader->use();
    renderShader->uploadUniform(uColor);
    renderShader->uploadUniform(uPhi);

    context->setClearColor(.01f, .01f, .01f);

    // bind everything for rendering
    vertexArray->bind();
    vertexBuffer->bind();
    indexBuffer->bind();
    renderShader->use();

    context->setClearColor(.05f, .05f, .05f);
    context->setClearFlags(GL_COLOR_BUFFER_BIT);
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
}

void SandboxApplication::update(float dt) {
    uPhi.data = glm::mod(uPhi.data + frequency * dt, 2.f * glm::pi<float>());
    renderShader->uploadUniform(uPhi);

    cameraController.update(getWindow()->getInputController());
}

void SandboxApplication::render(Engine::Context* context) {
    if (cameraController.needsUpload()) {
        renderShader->uploadUniform(cameraController.getCamera().getUniform());
    }
    GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

    ImGui::ShowDemoWindow(&showDemoWindow);

    ImGui::Begin("Info");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

}

void SandboxApplication::onEvent(Event* e) {
    Application::onEvent(e);
}
