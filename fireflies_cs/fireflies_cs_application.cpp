/*
 * Created by Kami-Kaze on 4/11/2021.
 */

#include "fireflies_cs_application.h"

#include <random>
#include <chrono>
#include <glm/gtx/color_space.hpp>
#include <core/logging/app_log.h>

/** Class implementation */

void FirefliesCsApplication::init() {
    Application::init();

    // todo: init
}

void FirefliesCsApplication::shutdown() {
    // todo: shutdown

    Application::shutdown();
}

void FirefliesCsApplication::setup(ApplicationProps& props) {
    WindowProps& windowProps = props.windowProps;

    // simple window
    props.name = "Fireflies with compute shaders";
    windowProps.name = "Fireflies";
    windowProps.width = 1000;
    windowProps.height = 1000;
    windowProps.center = true;
    windowProps.multisample = 16;

    /* log some stats */
    LOG_INFO("Num fireflies: {}", NUM_FIREFLIES);
}

void FirefliesCsApplication::onContextAttach() {
    /** init data */
    //! use heap (stack is not big enough if we want a lot of fireflies!)
    FireflyData* fireflies = new FireflyData[NUM_FIREFLIES];

    // fill arrays
    std::mt19937 engine;
    engine.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist01(0, 1);
    std::uniform_real_distribution<float> dist11(-1, 1);

    for (int i = 0; i < NUM_FIREFLIES; i++) {
        // firefly params
        float size = FIREFLY_SIZE;
        float phase = dist01(engine) * FIREFLY_MAX_PHASE;
        float frequency = dist01(engine) * FIREFLY_MAX_FREQUENCY;
        // firefly color

        float hue;
        if (0 < NUM_COLORS) {
            hue = glm::floor(dist01(engine) * NUM_COLORS) * 360.f / NUM_COLORS;
        } else {
            hue = dist01(engine) * 360.f;
        }
        float saturation = 1.f;
        float value = 1.f;
        glm::vec3 color = glm::rgbColor(glm::vec3(hue, saturation, value));
        // firefly position
        float x = dist11(engine) * (1.f - size);
        float y = dist11(engine) * (1.f - size);
        glm::vec2 position = glm::vec2(x, y);

        // init firefly data
        FireflyData& f = fireflies[i];
        f.position = position;
        f.color = color;
        f.size = size;
        f.phi = phase;
        f.frequency = frequency;
    }

    /** init opengl buffers and vertex array */

    // gen and init compute buffer
    computeBuffer = new Buffer<FireflyData>(
            BufferType::SHADER_STORAGE_BUFFER,
            BufferAccess::STATIC_DRAW,
            NUM_FIREFLIES,
            fireflies
    );
    computeBuffer->bind();
    computeBuffer->bindBase();
    computeBuffer->unbind();

    // our data arrays are not needed anymore now
    delete[] fireflies;

    /** load shaders */
    ShaderList shaderList;

    // quad render shader
    shaderList.pushFromFile(
            ShaderType::VERTEX,
            R"(C:\Development\Projects\simulations\fireflies_cs\resources\vertex_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::GEOMETRY,
            R"(C:\Development\Projects\simulations\fireflies_cs\resources\geometry_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::FRAGMENT,
            R"(C:\Development\Projects\simulations\fireflies_cs\resources\fragment_shader.glsl)"
    );
    renderShader = ShaderProgram::fromShaderList(shaderList);
    shaderList.clear();

    /* push uniforms */
    renderShader->bind();
    GL_CALL(glUniform1f(renderShader->getUniformLocation("muP"), muP));
    GL_CALL(glUniform1f(renderShader->getUniformLocation("muF"), muF));

    // fetch dPhi location
    rsDPhiLocation = renderShader->getUniformLocation("dPhi");
    renderShader->unbind();

    // change compute shader
    shaderList.pushFromFile(
            ShaderType::COMPUTE,
            R"(C:\Development\Projects\simulations\fireflies_cs\resources\compute_shader.glsl)"
    );
    computeShader = ShaderProgram::fromShaderList(shaderList);
    shaderList.clear();

    /* push uniforms */
    computeShader->bind();
    GL_CALL(glUniform1f(computeShader->getUniformLocation("epsilonV"), epsilonV));
    GL_CALL(glUniform1f(computeShader->getUniformLocation("epsilonC"), epsilonC));
    computeShader->unbind();

    // set blend
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void FirefliesCsApplication::onContextDetach() {
    // todo: shut down opengl
}

void FirefliesCsApplication::update(float dt) {
    // update phi
    const float dPhi = OMEGA * dt;

    /* update quad shader dPhi */
    renderShader->bind();
    GL_CALL(glUniform1f(rsDPhiLocation, dPhi));
    renderShader->unbind();

    /* calculate change */
    computeShader->bind();
    // calculate change
    GL_CALL(glDispatchCompute(NUM_FIREFLIES, 1, 1));
    // wait for completion
    GL_CALL(glMemoryBarrier(GL_SHADER_STORAGE_BUFFER));
    computeShader->unbind();

#if 0
    /* for testing: map buffers so we can read them */
    computeBuffer->bind();
    FireflyData* cb = computeBuffer->readData();
    computeBuffer->unbind();

    // calculate avg phase, frequency and clock
    float avgFrequency = 0;
    float avgPhase = 0;
    for (int i = 0; i < NUM_FIREFLIES; i++) {
        const FireflyData& f = cb[i];
        avgFrequency += f.frequency;
        avgPhase += f.phi;
    }
    avgFrequency /= NUM_FIREFLIES;
    avgPhase /= NUM_FIREFLIES;

    float frequencyDev = 0;
    float phaseDev = 0;

    for (int i = 0; i < NUM_FIREFLIES; i++) {
        const FireflyData& f = cb[i];
        frequencyDev += glm::abs(avgFrequency - f.frequency);
        phaseDev += glm::atan(glm::sin(avgPhase - f.phi), glm::cos(avgPhase - f.phi));
    }

    frequencyDev /= NUM_FIREFLIES;
    phaseDev /= NUM_FIREFLIES;

    delete[] cb;
#endif
}

void FirefliesCsApplication::render(Context* context) {
    // clear
    static constexpr unsigned int flags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    context->clear(0.f, 0.f, 0.f, flags);

    // draw
    renderShader->bind();
    GL_CALL(glDrawArraysInstanced(GL_POINTS, 0, 1, NUM_FIREFLIES));
    renderShader->unbind();
}