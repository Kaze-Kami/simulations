/*
 * Created by Kami-Kaze on 4/10/2021.
 */

#include <core/entry_point.h>

#include "fireflies_application.h"

int main() {
    return Engine::main();
}


Engine::Application* Engine::Application::createApplication() {
    return new FirefliesApplication();
}