/*
 * Created by Kami-Kaze on 4/10/2021.
 */

#include "sandbox_application.h"

void SandboxApplication::onContextAttach() {

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
    windowProps.multisample = 16;
}

void SandboxApplication::update(float dt) {
    Application::update(dt);
}

void SandboxApplication::render(Engine::Context* context) {
    Application::render(context);
}
