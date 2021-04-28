/*
 * Created by Kami-Kaze on 4/24/2021.
 */

#include "n_bodies_application.h"

void NBodiesApplication::init() {
    Application::init();
    // todo: init?
}

void NBodiesApplication::shutdown() {
    // todo: shutdown?
    Application::shutdown();
}

void NBodiesApplication::setup(ApplicationProps& props) {
    WindowProps& windowProps = props.windowProps;

    // simple window
    props.name = "N-Body-Problem";
    windowProps.name = "N-Bodies";
    windowProps.width = 1000;
    windowProps.height = 1000;
    windowProps.center = true;
    windowProps.multisample = 16;
    windowProps.fpsCounterEnable = true;
    windowProps.fpsCounterColor = glm::vec4(1.f, .1f, .1f, 1.f);
}

void NBodiesApplication::update(float dt) {
    // todo: update
}

void NBodiesApplication::render(Context* context) {
    // todo: render
}

void NBodiesApplication::renderImGui() {
    // todo: render imgui
}
