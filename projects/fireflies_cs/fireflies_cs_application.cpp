/*
 * Created by Kami-Kaze on 4/11/2021.
 */

#include "fireflies_cs_application.h"

#include <random>
#include <chrono>

#include <glm/gtx/color_space.hpp>
#include <core/logging/app_log.h>
#include <macros/bind.h>

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
    // windowProps.vsync = false;

    /* log some stats */
    LOG_INFO("Num fireflies: {}", NUM_FIREFLIES);
}

void FirefliesCsApplication::onContextAttach(Context* context) {
    /** init data */
    //! use heap (stack is not big enough if we want a lot of fireflies!)
    FireflyData* fireflies = new FireflyData[NUM_FIREFLIES];

    // fill arrays
    std::mt19937 engine;
    engine.seed(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dist01(0, 1);
    std::uniform_real_distribution<float> dist11(-1, 1);

    if (NUM_FIREFLIES == 1) {
        // init firefly data
        FireflyData& f = fireflies[0];
        f.position = glm::vec2(0.f);
        f.color = glm::rgbColor(glm::vec3(dist01(engine) * 360.f, 1.f, 1.f));
        f.size = .3;
        f.phi = 0;
        f.frequency = 0.f;
    } else {
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
            R"(C:\Development\Projects\simulations\projects\fireflies_cs\resources\vertex_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::GEOMETRY,
            R"(C:\Development\Projects\simulations\projects\fireflies_cs\resources\geometry_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::FRAGMENT,
            R"(C:\Development\Projects\simulations\projects\fireflies_cs\resources\fragment_shader.glsl)"
    );
    renderShader = ShaderProgram::createProgram(shaderList);
    shaderList.clear();

    /* push uniforms */
    renderShader->use();
    renderShader->uploadUniform(Uniform("muP", muP));
    renderShader->uploadUniform(Uniform("muF", muF));
    renderShader->uploadUniform(Uniform("blinkThreshold", BLINK_THRESHOLD));

    // change compute shader
    shaderList.pushFromFile(
            ShaderType::COMPUTE,
            R"(C:\Development\Projects\simulations\projects\fireflies_cs\resources\compute_shader.glsl)"
    );
    computeShader = ShaderProgram::createProgram(shaderList);
    shaderList.clear();

    /* push uniforms */
    computeShader->use();
    computeShader->uploadUniform(Uniform("epsilonV", epsilonV));
    computeShader->uploadUniform(Uniform("epsilonC", epsilonC));

    // set blend
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // init camera
    resetCamera();
    context->setClearColor(0.f, 0.f, 0.f, 0.f);
}

void FirefliesCsApplication::onContextDetach(Context* context) {
    // todo: shut down opengl
}

void FirefliesCsApplication::update(float dt) {
    // update timing
    // update phi
    uDPhi.data = OMEGA * dt;

    /* update quad shader dPhi */
    renderShader->use();
    renderShader->uploadUniform(uDPhi);

    /* calculate change */
    computeShader->use();
    GL_CALL(glDispatchCompute(COMPUTE_CLUSTERS_X, COMPUTE_CLUSTERS_Y, COMPUTE_CLUSTERS_Z));
    GL_CALL(glMemoryBarrier(GL_SHADER_STORAGE_BUFFER));

    // update camera based on input
    InputController* controller = getWindow()->getInputController();
    if (controller->isKeyDown(Key::C)) {
        resetCamera();
    }
    if (controller->isMouseButtonDown(Mouse::ButtonRight) && controller->didMouseMove()) {
        cameraOffset += controller->getMouseDelta();
        updateCamera();
    }
    if (controller->didMouseWheelScroll()) {
        const float f = glm::pow(CAMERA_SCALE_SPEED, controller->getMouseWheelDelta().x);

        glm::vec2 mouse = getWindow()->getInputController()->getMousePosition();
        glm::vec4 at0 = glm::vec4(mouse.x, mouse.y, 0.f, 1.f);

        glm::vec4 atScreen0 = cameraInverse * at0;

        cameraScale *= f;
        updateCamera();

        glm::vec4 atScreen1 = cameraInverse * at0;
        glm::vec4 offsetScreen = atScreen0 - atScreen1;
        glm::vec4 offset = camera * offsetScreen;

        cameraOffset -= glm::vec2(offset.x, offset.y);

        updateCamera();
    }

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

    LOG_INFO("dev frequency = {:.4f}, dev phase = {:.4f}", frequencyDev, phaseDev);

    delete[] cb;
#endif
}

void FirefliesCsApplication::render(Context* context) {
    // clear
    context->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw
    renderShader->use();
    if (cameraChanged) {
        uploadCamera();
    }
    GL_CALL(glDrawArraysInstanced(GL_POINTS, 0, 1, NUM_FIREFLIES));
}

bool FirefliesCsApplication::onMouseButtonPressEvent(MouseButtonPressEvent* e) {
    if (e->code == Mouse::ButtonLeft) {
        glm::vec4 worldAt = cameraInverse * glm::vec4(e->pos.x, e->pos.y, 0.f, 1.f);
        LOG_INFO("Mouse Position: ({}, {}) [({}, {})]", e->pos.x, e->pos.y, worldAt.x, worldAt.y);
    }
    return false;
}

void FirefliesCsApplication::onEvent(Event* e) {
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<MouseButtonPressEvent>(BIND_FN(FirefliesCsApplication::onMouseButtonPressEvent));
}

void FirefliesCsApplication::resetCamera() {
    cameraOffset = glm::vec2(0.f, 0.f);
    cameraScale = 1.f;
    updateCamera();
}

void FirefliesCsApplication::updateCamera() {
    // calculate new camera
    camera = glm::mat4(1.f);
    camera = glm::translate(camera, glm::vec3(cameraOffset.x, cameraOffset.y, 0.f));
    camera = glm::scale(camera, glm::vec3(cameraScale, cameraScale, 1.f));

    cameraInverse = glm::mat4(1.f);
    cameraInverse = glm::scale(cameraInverse, glm::vec3(1.f / cameraScale, 1.f / cameraScale, 1.f));
    cameraInverse = glm::translate(cameraInverse, glm::vec3(-cameraOffset.x, -cameraOffset.y, 0.f));

    cameraChanged = true;
}

void FirefliesCsApplication::uploadCamera() {
    uView.data = camera;
    renderShader->use();
    renderShader->uploadUniform(uView);
    cameraChanged = false;
}
