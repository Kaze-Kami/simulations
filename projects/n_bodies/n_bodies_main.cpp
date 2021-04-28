/*
 * Created by Kami-Kaze on 4/24/2021.
 */

#include <core/entry_point.h>

#include "n_bodies_application.h"

using namespace Engine;

int main() {
    return Engine::main();
}

Application* Application::createApplication() {
    return new NBodiesApplication();
}