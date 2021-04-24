/*
 * Created by Kami-Kaze on 4/11/2021.
 */

#include <core/entry_point.h>

#include "fireflies_application.h"

using namespace Engine;

int main() {
    return Engine::main();
}

Application* Application::createApplication() {
    return new FirefliesApplication();
}
