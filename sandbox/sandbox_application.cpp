/*
 * Created by Kami-Kaze on 4/10/2021.
 */

#include "sandbox_application.h"

static float rnd(float min, float max) {
    return min + ((float) rand() / (RAND_MAX)) * (max - min);
}

static float rnd() {
    return rnd(0.f, 1.f);
}

void SandboxApplication::onContextAttach() {
    using namespace Engine;

    /** init opengl buffers and vertex array */

    // gen vertex array
    GL_CALL(glCreateVertexArrays(1, &vertexArrayId));
    GL_CALL(glBindVertexArray(vertexArrayId));

    // gen and init vertex buffer
    GL_CALL(glCreateBuffers(1, &vertexBufferId));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId));

    float vertex[1] = {0};
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float), vertex, GL_STATIC_DRAW));

    GL_CALL(glEnableVertexArrayAttrib(vertexArrayId, 0));
    GL_CALL(glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr));

    // gen and init index buffer
    GL_CALL(glCreateBuffers(1, &indexBufferId));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId));

    unsigned int index[1] = {0};
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int), index, GL_STATIC_DRAW));

    // unbind vertex array and buffers
    GL_CALL(glBindVertexArray(0));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    /** load shader */
    ShaderList shaderList;

    // quad render shader
    shaderList.pushFromFile(
            ShaderType::VERTEX,
            R"(C:\Development\Projects\simulations\sandbox\resources\vertex_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::GEOMETRY,
            R"(C:\Development\Projects\simulations\sandbox\resources\geometry_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::FRAGMENT,
            R"(C:\Development\Projects\simulations\sandbox\resources\fragment_shader.glsl)"
    );
    renderShader = ShaderProgram::fromShaderList(shaderList);
    shaderList.clear();

    // bind everything for rendering
    GL_CALL(glBindVertexArray(vertexArrayId));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId));
    renderShader->bind();
}

void SandboxApplication::onContextDetach() {

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
    windowProps.multisample = 4;
    windowProps.vsync = true;
}

void SandboxApplication::update(float dt) {
    // nothing really to do here either for now
}

void SandboxApplication::render(Engine::Context* context) {
    static constexpr unsigned int flags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    context->clear(0.f, 0.f, 0.f, flags);

    // renderShader->bind();

    GL_CALL(glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, nullptr));

    // renderShader->unbind();
}
