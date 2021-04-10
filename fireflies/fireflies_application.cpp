/*
 * Created by Kami-Kaze on 4/10/2021.
 */

#include "fireflies_application.h"

#include <glm/gtx/color_space.hpp>

const char* vertexShaderCode =
        "#version 450 core\n"
        /* inputs */
        "layout (location = 0) in vec2 i_position;"
        "layout (location = 1) in vec3 i_color;"
        /* outputs */
        "layout (location = 0) out vec3 o_color;"
        /* main */
        "void main() {"
        "    o_color = i_color;"
        "    gl_Position = vec4(i_position, 0.f, 1.f);"
        "}";

const char* fragmentShaderCode =
        "#version 450 core\n"
        /* inputs */
        "layout (location = 0) in vec3 i_color;"
        /* outputs */
        "layout (location = 0) out vec3 o_color;"
        /* main */
        "void main() {"
        "    o_color = i_color;"
        "}";

const char* computeShaderCode =
        "#version 450 core\n"
        "";

static float rnd(float min, float max) {
    return min + ((float) rand() / (RAND_MAX)) * (max - min);
}

static float rnd() {
    return rnd(0.f, 1.f);
}

void FirefliesApplication::onContextAttach() {
    using namespace Engine;
    /*
     * Create render chain
     */

    // load shaders
    std::vector<Shader*> shaders;
    shaders.push_back(Shader::fromString(ShaderType::VERTEX, vertexShaderCode));
    shaders.push_back(Shader::fromString(ShaderType::FRAGMENT, fragmentShaderCode));

    // create shader program
    shaderProgram = ShaderProgram::fromShaderList(shaders);

    // delete shaders
    for (Shader* shader : shaders) {
        delete shader;
    }

    // create vertex array
    vertexArray = new VertexArray();

    // create vertex array layout
    VertexBufferLayout layout;
    layout.pushAttribute<float>(2, false);
    layout.pushAttribute<float>(3, false);

    // create vertex buffer
    vertexBuffer = new VertexBuffer<FireflyVertex>(GL_DYNAMIC_DRAW, nFireflies * 4, nullptr);
    vertexBuffer->bind();

    // add vertex buffer to vertex array
    vertexArray->bind();
    vertexArray->addBuffer(vertexBuffer, layout);

    // generate indices
    int indices[6 * nFireflies];
    for (int i = 0; i < nFireflies; i++) {
        indices[6 * i + 0] = 4 * i + 0;
        indices[6 * i + 1] = 4 * i + 1;
        indices[6 * i + 2] = 4 * i + 2;
        indices[6 * i + 3] = 4 * i + 2;
        indices[6 * i + 4] = 4 * i + 3;
        indices[6 * i + 5] = 4 * i + 0;
    }

    // create index buffer
    indexBuffer = new Buffer<int>(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, 6 * nFireflies, indices);

    vertexArray->unbind();
    vertexBuffer->unbind();
    indexBuffer->unbind();

    /*
     * Create compute shader for updates
     */
}

void FirefliesApplication::onContextDetach() {
    delete shaderProgram;
    delete vertexArray;
    delete vertexBuffer;
    delete indexBuffer;
}

void FirefliesApplication::setup(Engine::ApplicationProps& props) {
    using namespace Engine;

    WindowProps& windowProps = props.windowProps;

    // simple window
    props.name = "Sandbox application";
    windowProps.name = "Sandbox Window";
    windowProps.width = 1000;
    windowProps.height = 1000;
    windowProps.center = true;
    windowProps.multisample = 16;

    glm::vec2 vertexPositions[4] = {
            glm::vec2(-1.f, -1.f),
            glm::vec2( 1.f, -1.f),
            glm::vec2( 1.f,  1.f),
            glm::vec2(-1.f,  1.f),
    };

    // init data arrays
    for (int i = 0; i < nFireflies; i++) {
        // zero nudges
        nudges[i] = 0;

        // init firefly
        Firefly& f = fireflies[i];
        f.phase = rnd(0.f, glm::radians(360.0));
        f.size = Firefly::baseSize;
        f.position = -1.f + glm::vec2(rnd(), rnd()) * 2.f;
        const float hue = rnd(0.f, 360.f);
        f.color = glm::rgbColor(glm::vec3(hue, Firefly::colorSaturation, Firefly::colorValue));

        // init vertices (4 per firefly)
        for (int j = 0; j < 4; j++) {
            FireflyVertex& fv = fireflyVertices[4 * i + j];
            fv.position = f.position + f.size * vertexPositions[j];
            fv.color = glm::vec3(0.f);
        }
    }

}

void FirefliesApplication::update(float dt) {
    // update time
    phi = glm::mod(phi + omega * dt, phiMax);

    // update fireflies (todo: compute shader)

    for (int i = 0; i < nFireflies; i++) {
        Firefly& fA = fireflies[i];
        float nudge = nudges[i];

        for (int j = i + 1; j < nFireflies; j++) {
            // calculate interaction between
            // fireflies i and j

            const Firefly& fB = fireflies[j];
            const float d = glm::distance(fA.position, fB.position);
            const float mu = baseMu / glm::exp(epsilon * d);
            const float dPhase = mu * (fB.phase - fA.phase);

            // update nudges
            nudge += dPhase;
            nudges[j] -= dPhase;
        }

        // update phase
        fA.phase += nudge / nFireflies * dt;

        // update current brightness
        float brightness = (1.f + glm::sin(phi + fA.phase)) / 2.f;
        if (blinkThreshold < brightness) {
            // we are 'blinking' -> scale brightness from [blinkThreshold, ..., 1] to [0, ..., 1]
            brightness = (brightness - blinkThreshold) / (1.f - blinkThreshold);
        } else {
            // we are 'off' -> brightness = 0
            brightness = 0;
        }

        const glm::vec3 color = fA.color * brightness;

        for (int j = 0; j < 4; j++) {
            FireflyVertex& fv = fireflyVertices[4 * i + j];
            fv.color = color;
        }

        // clear buffer
        nudges[i] = 0;
    }
}

void FirefliesApplication::render(Engine::Context* context) {
    context->clear(0.f, 0.f, 0.f, 1.f);

    // set gpu state (binding order is important actually...)
    // always bind the vertex array first.
    // then bind the index and vertex buffer.
    // shader does not matter
    shaderProgram->bind();
    vertexArray->bind();
    vertexBuffer->bind();
    indexBuffer->bind();

    // push data to opengl
    vertexBuffer->bufferData(fireflyVertices);

    // draw call
    GL_CALL(glDrawElements(GL_TRIANGLES, 6 * nFireflies, GL_UNSIGNED_INT, nullptr));

    // clean up gpu state (binding order still matters as far as we know)
    // always unbind vertex array before index and vertex buffer.
    // shader does not matter, again
    shaderProgram->unbind();
    vertexArray->unbind();
    vertexBuffer->unbind();
    indexBuffer->unbind();

}
