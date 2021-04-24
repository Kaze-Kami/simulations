/*
 * Created by Kami-Kaze on 4/11/2021.
 */

#include "fireflies_application.h"

#include <random>
#include <chrono>

#include <glm/gtx/color_space.hpp>

#include <imgui.h>

#include <core/logging/app_log.h>
#include <macros/bind.h>

/** Class implementation */

void FirefliesApplication::init() {
    Application::init();

    // todo: init
}

void FirefliesApplication::shutdown() {
    // todo: shutdown

    Application::shutdown();
}

void FirefliesApplication::setup(ApplicationProps& props) {
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

void FirefliesApplication::onContextAttach(Context* context) {
    /** init opengl buffers and vertex array */
    initFireflies();

    computeBuffer->bind();
    computeBuffer->bindBase();
    computeBuffer->unbind();

    /** load shaders */
    ShaderList shaderList;

    // quad render shader
    shaderList.pushFromFile(
            ShaderType::VERTEX,
            R"(resources\vertex_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::GEOMETRY,
            R"(resources\geometry_shader.glsl)"
    );
    shaderList.pushFromFile(
            ShaderType::FRAGMENT,
            R"(resources\fragment_shader.glsl)"
    );
    renderShader = ShaderProgram::createProgram(shaderList);
    shaderList.clear();

    /* push uniforms */
    renderShader->use();
    renderShader->uploadUniform(Uniform("muP", muP));
    renderShader->uploadUniform(Uniform("muF", muF));
    renderShader->uploadUniform(Uniform("blinkThreshold", blinkThreshold));
    renderShader->uploadUniform(cameraController.getCamera().getUniform());

    // change compute shader
    shaderList.pushFromFile(
            ShaderType::COMPUTE,
            R"(resources\compute_shader.glsl)"
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

    // set clear color
    context->setClearColor(0.f, 0.f, 0.f, 0.f);
    context->setClearFlags(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FirefliesApplication::onContextDetach(Context* context) {
    // todo: shut down opengl
}

void FirefliesApplication::update(float dt) {
    // update phi
    uDPhi.data = dt * simulationSpeed * TWO_PI;

    /* update quad shader dPhi */
    renderShader->use();
    renderShader->uploadUniform(uDPhi);

    /* calculate change */
    computeShader->use();
    GL_CALL(glDispatchCompute(COMPUTE_CLUSTERS_X, COMPUTE_CLUSTERS_Y, COMPUTE_CLUSTERS_Z));
    GL_CALL(glMemoryBarrier(GL_SHADER_STORAGE_BUFFER));

    // update camera based on input
    InputController* controller = getWindow()->getInputController();
    cameraController.update(controller);

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

void FirefliesApplication::render(Context* context) {
    // bind shader
    renderShader->use();
    // maybe update view
    if (cameraController.needsUpload()) {
        renderShader->uploadUniform(cameraController.getCamera().getUniform());
        cameraController.setCameraUploaded();
    }
    // draw
    GL_CALL(glDrawArraysInstanced(GL_POINTS, 0, 1, NUM_FIREFLIES));
}

bool FirefliesApplication::onMouseButtonPressEvent(MouseButtonPressEvent& e) {
    if (e.code == Mouse::ButtonLeft) {
        Camera& camera = cameraController.getCamera();
        glm::vec4 worldAt = camera.applyInverse(glm::vec4(e.pos.x, e.pos.y, 0.f, 1.f));
        LOG_INFO("Mouse Position: ({}, {}) [({}, {})]", e.pos.x, e.pos.y, worldAt.x, worldAt.y);
    }
    return false;
}

bool FirefliesApplication::onKeyPressEvent(KeyPressEvent& e) {
    if (e.code == Key::C) {
        cameraController.resetCamera();
        return true;
    }
    return false;
}

void FirefliesApplication::onEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<MouseButtonPressEvent>(BIND_FN(FirefliesApplication::onMouseButtonPressEvent));
    dispatcher.dispatch<KeyPressEvent>(BIND_FN(FirefliesApplication::onKeyPressEvent));
}

void FirefliesApplication::renderImGui() {
    if (ImGui::Begin("Info")) {
        ImGui::Text("%d fireflies", NUM_FIREFLIES);
        ImGui::Text("~%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();

    if (ImGui::Begin("Settings")) {
        if (ImGui::CollapsingHeader("Setup")) {
            ImGui::DragInt("Num colors", &numColors, 1, 0);
            ImGui::DragFloat("Firefly size", &fireflySize, .001f, 0.f, 1.f);
            ImGui::DragFloat("Max frequency", &fireflyMaxFrequency);

            if (ImGui::Button("Restart")) {
                initFireflies();
            }

            for (int i = 0; i < numColorsLoaded; i++) {
                const float hue = float(i) / float(numColors);

                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(hue, 1.f, 1.f));

                ImGui::PushID(i);
                // todo: does nothing as of now
                ImGui::Checkbox("Show color", &enableColors[i]);
                ImGui::SameLine(); ImGui::Button("  ");
                ImGui::PopID();

                ImGui::PopStyleColor();
            }
        }

        if (ImGui::CollapsingHeader("Parameters")) {
            ImGui::DragFloat("Simulation speed", &simulationSpeed, .01f);
            if (ImGui::DragFloat("Blink threshold", &blinkThreshold, .05f, 0.f, 1.f)) {
                renderShader->use();
                renderShader->uploadUniform(Uniform("blinkThreshold", blinkThreshold));
            }
            if (ImGui::DragFloat("Mu phase", &muP, 1.f, 0.f)) {
                renderShader->use();
                renderShader->uploadUniform(Uniform("muP", muP));
            }
            if (ImGui::DragFloat("Mu frequency", &muF, 1.f, 0.f)) {
                renderShader->use();
                renderShader->uploadUniform(Uniform("muF", muF));
            }
            if (ImGui::DragFloat("Epsilon vision", &epsilonV, 1.f, 0.f)) {
                computeShader->use();
                computeShader->uploadUniform(Uniform("epsilonV", epsilonV));
            }
            if (ImGui::DragFloat("Epsilon color", &epsilonC, 1.f, 0.f)) {
                computeShader->use();
                computeShader->uploadUniform(Uniform("epsilonC", epsilonC));
            }
        }
    }
    ImGui::End();
}

void FirefliesApplication::initFireflies() {
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
        f.phi = .5f * glm::pi<float>();
        f.frequency = 1.f;
    } else {
        for (int i = 0; i < NUM_FIREFLIES; i++) {
            // firefly params
            float size = fireflySize; //  * (1.f + dist01(engine));
            float phase = dist01(engine) * TWO_PI;
            float frequency = dist01(engine) * fireflyMaxFrequency;
            // firefly color

            float hue;
            if (1 == numColors) {
                static float sHue = dist01(engine) * 360.f;
                hue = sHue;
            } else if (0 < numColors) {
                hue = glm::floor(dist01(engine) * float(numColors)) * 350.f / float(numColors);
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

    // create compute buffer
    delete computeBuffer;

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

    // update color stuff
    numColorsLoaded = numColors;
    delete enableColors;
    enableColors = new bool[numColors];
    for (int i = 0; i < numColors; i++) {
        enableColors[i] = true;
    }
}
