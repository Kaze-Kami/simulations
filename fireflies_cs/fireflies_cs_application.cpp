/*
 * Created by Kami-Kaze on 4/11/2021.
 */

#include "fireflies_cs_application.h"

#include <random>
#include <chrono>
#include <glm/gtx/color_space.hpp>

#include <macros/gl_call.h>
#include <macros/assert.h>
#include <macros/no_op.h>
#include <core/logging/app_log.h>

/** Helpers */

template<class T>
T* readBuffer(GLenum target, GLuint location, int size) {
    GL_CALL(glBindBuffer(target, location));
    GL_CALL(void* ptr = glMapBuffer(target, GL_READ_ONLY));
    ASSERT(ptr != nullptr, "Mapping buffer failed!");
    T* arr = new T[size];
    memcpy(arr, ptr, sizeof(T) * size);
    GL_CALL(bool unmapSuccess = glUnmapBuffer(target));
    ASSERT(unmapSuccess, "Unmapping buffer failed!");
    GL_CALL(glBindBuffer(target, 0));
    return arr;
}

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
}

void FirefliesCsApplication::onContextAttach() {
    /** init data */
    //! use heap (stack is not big enough if we want a lot of fireflies!)
    _Firefly* fireflies = new _Firefly[NUM_FIREFLIES];

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
        _Firefly& firefly = fireflies[i];
        firefly.phase = phase;
        firefly.frequency = frequency;
        firefly.phi = 0;
        firefly.size = size;
        firefly.dPhase = 0;
        firefly.dFrequency = 0;
        firefly.dPhi = 0;
        firefly.position = position;
        firefly.color = color;
    }

    /** init opengl buffers and vertex array */

    // gen and init compute buffer
    GL_CALL(glCreateBuffers(1, &computeBufferId));
    GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, computeBufferId));
    GL_CALL(glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(_Firefly) * NUM_FIREFLIES, fireflies, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeBufferId));
    GL_CALL(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));

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

    renderShader->bind();
    rsDPhiLocation = renderShader->getUniformLocation("dPhi");
    ASSERT(rsDPhiLocation != -1, "Can't fetch location for uniform 'dPhi' from vertex shader");
    renderShader->unbind();

    // change compute shader
    shaderList.pushFromFile(
            ShaderType::COMPUTE,
            R"(C:\Development\Projects\simulations\fireflies_cs\resources\compute_shader.glsl)"
    );
    computeShader = ShaderProgram::fromShaderList(shaderList);
    shaderList.clear();

    // fetch dPhi location
    computeShader->bind();
    csDPhiLocation = computeShader->getUniformLocation("dPhi");
    ASSERT(csDPhiLocation != -1, "Can't fetch location for uniform 'dPhi' from compute shader");
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
    // push dPhi
    GL_CALL(glUniform1f(csDPhiLocation, dPhi));
    // calculate change
    GL_CALL(glDispatchCompute(NUM_FIREFLIES, 1, 1));
    // wait for completion
    GL_CALL(glMemoryBarrier(GL_SHADER_STORAGE_BUFFER));
    computeShader->unbind();

#if 0
    /* for testing: map buffers so we can read them */
    _Firefly* cb = readBuffer<_Firefly>(GL_SHADER_STORAGE_BUFFER, computeBufferId, NUM_FIREFLIES);
    _FireflyVertex* vb = readBuffer<_FireflyVertex>(GL_SHADER_STORAGE_BUFFER, vertexBufferId, 4 * NUM_FIREFLIES);
    NO_OP;

    // calculate avg phase, frequency and clock
    float avgPhase = 0;
    float avgFrequency = 0;
    float avgPhi = 0;
    for (int i = 0; i < NUM_FIREFLIES; i++) {
        const _Firefly& f = cb[i];
        avgPhase += f.phase;
        avgFrequency += f.frequency;
        avgPhi += f.phi;
    }
    avgPhase /= NUM_FIREFLIES;
    avgFrequency /= NUM_FIREFLIES;
    avgPhi /= NUM_FIREFLIES;

    float phaseDev = 0;
    float frequencyDev = 0;
    float phiDev = 0;

    for (int i = 0; i < NUM_FIREFLIES; i++) {
        const _Firefly& f = cb[i];
        phaseDev += glm::abs(avgPhase - f.phase);
        frequencyDev += glm::abs(avgFrequency - f.frequency);
        phiDev += glm::atan(glm::sin(avgPhi - f.phi), glm::cos(avgPhi - f.phi));
    }

    phaseDev /= NUM_FIREFLIES;
    frequencyDev /= NUM_FIREFLIES;
    phiDev /= NUM_FIREFLIES;

    LOG_INFO("phaseDev={}, frequencyDev={}, phiDev={}", phaseDev, frequencyDev, phiDev);

    delete[] cb;
    delete[] vb;
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