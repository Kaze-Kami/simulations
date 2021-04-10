/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include <core/entry_point.h>

#include "sandbox_application.h"

using namespace Engine;

int main() {
    return Engine::main();
}

Application* Application::createApplication() {
    return new SandboxApplication();
}
