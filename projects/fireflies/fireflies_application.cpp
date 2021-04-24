/*
 * Created by Kami-Kaze on 4/11/2021.
 */

#include "fireflies_application.h"

#include <random>
#include <chrono>
#include <sstream>

#include <glm/gtx/color_space.hpp>

#include <imgui.h>

#include <core/logging/app_log.h>
#include <macros/bind.h>

#define DEBUG_GPU 0

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
    props.name = "Firefly simulation";
    std::ostringstream ss;
    ss << "Firefly simulation (" << NUM_FIREFLIES << " fireflies)";
    windowProps.name = ss.str();
    windowProps.width = 1000;
    windowProps.height = 1000;
    windowProps.center = true;
    windowProps.multisample = 16;
    // so we can see how much of a margin we got for increasing the number of fireflies
    windowProps.vsync = false;
    windowProps.fpsCounterEnable = true;
    windowProps.fpsCounterColor = glm::vec4(1.f);

    /* log some stats */
    LOG_INFO("Num fireflies: {}", NUM_FIREFLIES);
}

void FirefliesApplication::onContextAttach(Context* context) {
    /** init opengl buffers and vertex array */
    initFireflies();

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
    renderShader->uploadUniform(Uniform("brightnessFalloff", brightnessFalloff));
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
#if DEBUG_GPU
    {
        /* for testing: map buffers before update so we can read them */
        computeBuffer->bind();
        FireflyData* cb = computeBuffer->readData();
        computeBuffer->unbind();

        // useless float so we can 'do something' with f
        float _useless = 0;

        for (int i = 0; i < NUM_FIREFLIES; i++) {
            const FireflyData& f = cb[i];
            // we need to 'do something' with f so it's available in the debugger
            _useless += f.size;
        }

        delete[] cb;
    }
#endif

    const float rd = paused ? 0.f : dt * simulationSpeed;
    // update elapsed time
    tElapsed += rd;
    // update phi
    uDPhi.data = rd * TWO_PI;

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
#if DEBUG_GPU
    /* for testing: map buffers before update so we can read them */
    computeBuffer->bind();
    FireflyData* cb = computeBuffer->readData();
    computeBuffer->unbind();

    // useless float so we can 'do something' with f
    float _useless = 0;

    for (int i = 0; i < NUM_FIREFLIES; i++) {
        const FireflyData& f = cb[i];
        // we need to 'do something' with f so it's available in the debugger
        _useless += f.size;
    }

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

#if DEBUG_GPU
    /* for testing: map buffers after ('render') update so we can read them */
    computeBuffer->bind();
    FireflyData* cb = computeBuffer->readData();
    computeBuffer->unbind();

    colorBuffer->bind();
    int* ce = colorBuffer->readData();

    // useless float so we can 'do something' with f
    float _useless = 0;

    for (int i = 0; i < NUM_FIREFLIES; i++) {
        const FireflyData& f = cb[i];
        bool colorEnable = ce[f.colorIndex];
        // we need to 'do something' with f so it's available in the debugger
        _useless += f.size;
    }
    delete[] cb;
    delete[] ce;
#endif
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
    if (ImGui::Begin("Stats")) {
        ImGui::Text("Total phi:"); ImGui::SameLine(); ImGui::Text("%.4f", tElapsed);
        ImGui::Text("Compute clusters:");
        ImGui::Indent(4);
        ImGui::Text("%d x %d x %d x %d^3",
                    COMPUTE_CLUSTERS_X,
                    COMPUTE_CLUSTERS_Y,
                    COMPUTE_CLUSTERS_Z,
                    COMPUTE_CLUSTER_SIZE_BASE);
        ImGui::Text("= %d fireflies", NUM_FIREFLIES);
        ImGui::Unindent(4);
        ImGui::Text("Num colors:"); ImGui::SameLine(); ImGui::Text("%d", zeroColors ? 0 : numColorsLoaded);
        ImGui::Text("Paused: %s", paused ? "true" : "false");
        ImGui::Text("Holding: %s", holding ? "true" : "false");
    }
    ImGui::End();

    if (ImGui::Begin("Controls")) {
        ImGui::DragFloat("Simulation speed", &simulationSpeed, .01f);

        if (ImGui::Button("Restart")) {
            initFireflies();
        }
        ImGui::SameLine();
        if (ImGui::Button(paused ? "Resume" : "Pause")) {
            paused = !paused;
        }
        ImGui::SameLine();
        if (ImGui::Button(holding ? "Free" : "Hold")) {
            holding = !holding;
            if (holding) {
                // pause 'learning' of fireflies
                renderShader->use();
                renderShader->uploadUniform(Uniform("muP", 0.f));
                renderShader->uploadUniform(Uniform("muP", 0.f));
            } else {
                renderShader->use();
                renderShader->uploadUniform(Uniform("muP", muP));
                renderShader->uploadUniform(Uniform("muP", muF));
            }
        }
    }
    ImGui::End();

    if (ImGui::Begin("Settings")) {
        if (ImGui::CollapsingHeader("Parameters")) {
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

        if (ImGui::CollapsingHeader("Color")) {
            if (ImGui::DragFloat("Blink threshold", &blinkThreshold, .001f, 0.f, 1.f)) {
                renderShader->use();
                renderShader->uploadUniform(Uniform("blinkThreshold", blinkThreshold));
            }
            if (ImGui::DragFloat("Brightness falloff", &brightnessFalloff, .01f, 0.f, 2.f)) {
                renderShader->uploadUniform(Uniform("brightnessFalloff", brightnessFalloff));
            }

            if (!zeroColors) {
                ImGui::Text("Enabled Colors: ");
                ImGui::SameLine();
                for (int i = 0; i < numColorsLoaded; i++) {
                    const float hue = float(i) / float(numColorsLoaded);

                    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4) ImColor::HSV(hue, 1.f, 1.f));
                    ImGui::PushID(i);

                    bool enable = enableColors[i];
                    if (ImGui::Checkbox("", &enable)) {
                        // changed
                        enableColors[i] = enable;
                        colorBuffer->bind();
                        colorBuffer->bufferData(enableColors);
                        colorBuffer->unbind();
                    }
                    ImGui::PopID();
                    ImGui::PopStyleColor();

                    if (i < numColorsLoaded - 1) ImGui::SameLine();
                }
            }
        }

        if (ImGui::CollapsingHeader("Base")) {
            ImGui::DragInt("Num colors", &numColors, 1, 0, 20);
            ImGui::DragFloat("Firefly size", &fireflySize, .001f, 0.f, 1.f);
            ImGui::DragFloat("Max frequency", &fireflyMaxFrequency);
        }
    }
    ImGui::End();
}

void FirefliesApplication::initFireflies() {
    /** delete old data */
    delete computeBuffer;
    delete colorBuffer;
    delete[] enableColors;

    /** init data */
    //! use heap (stack is not big enough if we want a lot of fireflies!)
    auto* fireflies = new FireflyData[NUM_FIREFLIES];

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
        f.colorIndex = 0;
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
            int colorIndex;
            if (1 == numColors) {
                static float sHue = dist01(engine) * 360.f;
                colorIndex = 0;
                hue = sHue;
            } else if (0 < numColors) {
                colorIndex = glm::floor(dist01(engine) * float(numColors));
                hue = float(colorIndex) * 350.f / float(numColors);
            } else {
                colorIndex = 0;
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
            f.colorIndex = colorIndex;
            f.size = size;
            f.phi = phase;
            f.frequency = frequency;
        }
    }

    // create compute buffer
    computeBuffer = new Buffer<FireflyData>(
            BufferType::SHADER_STORAGE_BUFFER,
            BufferAccess::STATIC_DRAW,
            NUM_FIREFLIES,
            fireflies
    );

    // bind compute buffer base
    computeBuffer->bind();
    computeBuffer->bindBase(0);
    computeBuffer->unbind();

    // our data arrays are not needed anymore now
    delete[] fireflies;

    // update color stuff
    numColorsLoaded = numColors;
    // numColors == 0 <=> floating colors (treat all colors as the same)
    zeroColors = false;
    if (numColorsLoaded <= 1 || NUM_FIREFLIES == 1) {
        zeroColors = true;
        numColorsLoaded = 1;
    }

    enableColors = new int[numColorsLoaded];
    for (int i = 0; i < numColorsLoaded; i++) {
        enableColors[i] = 1;
    }

    // create buffer
    colorBuffer = new Buffer<int>(
            BufferType::SHADER_STORAGE_BUFFER,
            BufferAccess::DYNAMIC_DRAW,
            numColorsLoaded,
            enableColors
    );

    // bind color buffer base
    colorBuffer->bind();
    colorBuffer->bindBase(1);
    colorBuffer->unbind();
}
