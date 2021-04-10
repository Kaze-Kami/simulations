/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include <core/entry_point.h>
#include <macros/gl_call.h>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

#include <core/renderer/shader_program/shader.h>
#include <core/renderer/shader_program/shader_program.h>
#include <core/renderer/buffer/buffer.h>
#include <macros/assert.h>

#include <iostream>
#include <core/renderer/vertex_array/vertex_array_layout.h>
#include <core/renderer/vertex_array/vertex_array.h>
#include <core/logging/app_log.h>

#include <cmath>

using namespace Engine;

int main() {
    return Engine::main();
}

const char* vertexShaderCode =
        "#version 450 core\n"
        "layout (location = 0) in vec2 i_position;"
        "layout (location = 1) in vec2 i_center;"
        "layout (location = 2) in float i_radius;"
        "layout (location = 3) in float i_brightness;"
        "out vec2 v_position;"
        "out vec2 v_center;"
        "out float v_radius;"
        "out float v_brightness;"
        "void main() {"
        "    v_position = i_position;"
        "    v_radius = i_radius;"
        "    v_brightness = i_brightness;"
        "    gl_Position = vec4(i_radius * i_position + i_center, 0.f, 1.f);"
        "}";

const char* fragmentShaderCode =
        "#version 450 core\n"
        "layout (location = 0) out vec3 o_Color;"
        "in vec2 v_position;"
        "in vec2 v_center;"
        "in float v_radius;"
        "in float v_brightness;"
        "void main() {"
        "    vec2 distanceV = v_center - v_position;"
        "    float distance = sqrt(dot(distanceV, distanceV));"
        "    if (v_radius < distance) {"
        "        discard;"
        "    }"
        "    o_Color = v_brightness * vec3(.3f, .8f, .6f);"
        "}";

struct Firefly {
public:
    float phase;
    glm::vec2 position;

    struct FfVertex {
        glm::vec2 position;         // position of vertex relative to center
        glm::vec2 center;
        float radius;
        float brightness;
    };
};

class SandboxApplication : public Application {
public:
    ShaderProgram* shaderProgram = nullptr;
    // GLuint quadVA = 0;

    VertexBuffer<Firefly::FfVertex>* vertexBuffer = nullptr;
    Buffer<int>* indexBuffer = nullptr;
    VertexArray* vertexArray = nullptr;

    // firefly settings
    static constexpr int nFireflies = 100;
    static constexpr float fireflySize = .1;
    static constexpr float fireflyBlinkF = 0.6f;
    static constexpr float fireflyBlinkThreshold = .75;

    // system params
    static constexpr float fireflyBaseMu = .02;        // controls how much a firefly is influenced by its neighbours
    static constexpr float fireflyEpsilon = 10.f;       // controls how 'far' a firefly can see (not absolute, multiplicative inverse)

    Firefly fireflies[nFireflies];

    float t = 0;

    inline void setup(ApplicationProps& props) override {
        WindowProps& windowProps = props.windowProps;

        // simple window
        props.name = "Sandbox application";
        windowProps.name = "Sandbox Window";
        windowProps.width = 1000;
        windowProps.height = 1000;
        windowProps.center = true;
        windowProps.multisample = 16;

        // init fireflies
        for (int i = 0; i < nFireflies; i++) {
            fireflies[i].phase = glm::radians(((double) rand() / (RAND_MAX)) * 360.f);
            fireflies[i].position = glm::linearRand(glm::vec2(-1.f), glm::vec2(1.f));
        }
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
        layout.pushAttribute<float>(1, false);

        vertexBuffer = new VertexBuffer<Firefly::FfVertex>(GL_DYNAMIC_DRAW, nFireflies * 4, nullptr);
        vertexBuffer->bind();

        vertexArray->bind();
        vertexArray->addBuffer(vertexBuffer, layout);

        int indices[6 * nFireflies];
        for (int i = 0; i < nFireflies; i++) {
            indices[6 * i + 0] = 4 * i + 0;
            indices[6 * i + 1] = 4 * i + 1;
            indices[6 * i + 2] = 4 * i + 2;
            indices[6 * i + 3] = 4 * i + 2;
            indices[6 * i + 4] = 4 * i + 3;
            indices[6 * i + 5] = 4 * i + 0;
        }
        indexBuffer = new Buffer<int>(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, 6 * nFireflies, indices);

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
        // update t (in °)
        t += dt * 360.f * fireflyBlinkF;
        // modulo
        while (360 < t) {
            t -= 360;
        }

        // update 'fireflies'
        float nudges[nFireflies];

        for (int i = 0; i < nFireflies; i++) {
            nudges[i] = 0;
        }

        // calculate nudges
        for (int i = 0; i < nFireflies - 1; i++) {
            const Firefly& f0 = fireflies[i];

            // figure out in which direction to nudge our phase
            for (int j = i + 1; j < nFireflies; j++) {
                const Firefly& f1 = fireflies[j];
                float dist = glm::distance(f0.position, f1.position);
                float mu = fireflyBaseMu / glm::exp(fireflyEpsilon * dist);
                float nudge = mu * (f1.phase - f0.phase);
                nudges[i] += nudge;
                nudges[j] -= nudge;
            }
        }

        // write nudges
        for (int i = 0; i < nFireflies; i++) {
            Firefly& f = fireflies[i];
            float nudge = nudges[i] / (nFireflies - 1);
            f.phase += nudge * dt;

            if (std::isnan(nudge)) {
                LOG_DEBUG("Nudge is nan");
            }
        }

    }

    inline void render(Context* context) override {
        // clear screen
        context->clear(0.f, 0.f, 0.f, .0f);

        // update the position
        Firefly::FfVertex vertices[nFireflies * 4];

        for (int i = 0; i < nFireflies; i++) {
            glm::vec2 position = fireflies[i].position;
            float brightness = (1.f + glm::sin(glm::radians(t) + fireflies[i].phase)) / 2.f;
            if (fireflyBlinkThreshold < brightness) {
                brightness = (brightness - fireflyBlinkThreshold) / (1.f - fireflyBlinkThreshold);
            } else {
                brightness = 0;
            }
            // update vertices
            vertices[4 * i + 0] = { glm::vec2(-fireflySize, -fireflySize), position, fireflySize, brightness};
            vertices[4 * i + 1] = { glm::vec2( fireflySize, -fireflySize), position, fireflySize, brightness};
            vertices[4 * i + 2] = { glm::vec2( fireflySize,  fireflySize), position, fireflySize, brightness};
            vertices[4 * i + 3] = { glm::vec2(-fireflySize,  fireflySize), position, fireflySize, brightness};
        }

        // bind everything
        shaderProgram->bind();
        vertexArray->bind();
        vertexBuffer->bind();
        indexBuffer->bind();

        // push data
        vertexBuffer->bufferData(vertices);

        // todo: render call via context
        // original call with 1 quad
        // GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        // non-instanced call with n quads
        GL_CALL(glDrawElements(GL_TRIANGLES, 6 * nFireflies, GL_UNSIGNED_INT, nullptr));
        // instanced call with n quads
        // GL_CALL(glDrawElementsInstanced(GL_TRIANGLES, 6 * nFireflies, GL_UNSIGNED_INT, nullptr, nFireflies));

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
