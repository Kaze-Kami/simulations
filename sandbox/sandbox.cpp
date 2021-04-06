/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include <core/entry_point.h>
#include <macros/gl_call.h>

#include <glm/glm.hpp>

#include <core/renderer/shader_program/shader.h>
#include <core/renderer/shader_program/shader_program.h>
#include <core/renderer/buffer/buffer.h>
#include <macros/assert.h>

#include <iostream>
#include <core/renderer/vertex_array/vertex_array_layout.h>
#include <core/renderer/vertex_array/vertex_array.h>

using namespace Engine;

int main() {
    return Engine::main();
}

const char* vertexShaderCode =
        "#version 450 core\n"
        "layout (location = 0) in vec2 i_position;"
        "layout (location = 1) in vec2 i_center;"
        "layout (location = 2) in float i_radius;"
        "out vec2 v_position;"
        "out vec2 v_center;"
        "out float v_radius;"
        "void main() {"
        "    v_position = i_position;"
        "    v_radius = i_radius;"
        "    gl_Position = vec4(i_radius * i_position + i_center, 0.f, 1.f);"
        "}";

const char* fragmentShaderCode =
        "#version 450 core\n"
        "layout (location = 0) out vec4 o_Color;"
        "in vec2 v_position;"
        "in vec2 v_center;"
        "in float v_radius;"
        "void main() {"
        "    vec2 distanceV = v_center - v_position;"
        "    float distance = sqrt(dot(distanceV, distanceV));"
        "    if (v_radius < distance) {"
        "        discard;"
        "    }"
        "    o_Color = vec4(.1f, .1f, .3f, 1.f);"
        "}";

class SandboxApplication : public Application {
private:
    struct vertex {
        glm::vec2 position;
        glm::vec2 center;
        float radius;
    };

public:
    ShaderProgram* shaderProgram = nullptr;
    // GLuint quadVA = 0;

    VertexBuffer<vertex>* vertexBuffer;
    Buffer<int>* indexBuffer;
    VertexArray* vertexArray;

    float t = 0;

    inline void setup(ApplicationProps& props) override {
        WindowProps& windowProps = props.windowProps;

        // simple window
        props.name = "Sandbox application";
        props.windowProps.name = "Sandbox Window";
        props.windowProps.width = 640 * 2;
        props.windowProps.height = 480 * 2;
        windowProps.center = true;
    }

    inline void onContextAttach() override {
        // load shader program
        std::vector<Shader*> shaders;
        Shader* vertexShader = Shader::fromString(ShaderType::VERTEX, vertexShaderCode);
        shaders.push_back(vertexShader);
        Shader* fragmentShader = Shader::fromString(ShaderType::FRAGMENT, fragmentShaderCode);
        shaders.push_back(fragmentShader);

        shaderProgram = ShaderProgram::fromShaderList(shaders);
        shaderProgram->bind();

        for (Shader* shader : shaders) {
            delete shader;
        }

        // create vertex array with data

        vertexArray = new VertexArray();

        VertexBufferLayout layout;
        layout.pushAttribute<float>(2, false);
        layout.pushAttribute<float>(2, false);
        layout.pushAttribute<float>(1, false);

        vertexBuffer = new VertexBuffer<vertex>(GL_DYNAMIC_DRAW, 4, nullptr);
        vertexBuffer->bind();

        vertexArray->bind();
        vertexArray->addBuffer(vertexBuffer, layout);

        int indices[6]{
                0, 1, 2,
                2, 3, 0
        };
        indexBuffer = new Buffer<int>(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, 6, indices);

        // clean up (our vertex and index buffer unbound themself after buffering the initial data
        vertexArray->unbind();
        vertexBuffer->unbind();
        indexBuffer->unbind();



        // todo: set blending via context
        GL_CALL(glEnable(GL_BLEND));
        GL_CALL(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
    }

    inline void onContextDetach() override {
        delete shaderProgram;
        delete vertexArray;
        delete vertexBuffer;
        delete indexBuffer;
    }

    inline void update(float dt) override {
        float f = 1.f / 5.f;
        t += dt * 360.f * f;
        // modulo
        while (360 < t) {
            t -= 360;
        }
    }

    inline void render(Context* context) override {
        // clear screen
        context->clear(0.f, 0.f, 0.f, .0f);

        float radians = glm::radians(t);
        float radius = .3f * (1.f + glm::cos(glm::radians(t)) / 2.f);
        glm::vec2 center = glm::vec2(glm::sin(radians) * .5, glm::cos(radians) * .3);

        // update the position
        vertex vertices[4] = {
                {glm::vec2(-1., -1.), center, radius},
                {glm::vec2(1., -1.),  center, radius},
                {glm::vec2(1., 1.),   center, radius},
                {glm::vec2(-1., 1.),  center, radius},
        };

        // bind everything
        shaderProgram->bind();
        vertexArray->bind();
        vertexBuffer->bind();
        indexBuffer->bind();

        // push data
        vertexBuffer->bufferData(vertices);

        // todo: render call via context
        GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        // unbind everything?
        shaderProgram->unbind();
        vertexArray->unbind();
        vertexBuffer->unbind();
        indexBuffer->unbind();
    }

};

Application* Application::createApplication() {
    return new SandboxApplication();
}
