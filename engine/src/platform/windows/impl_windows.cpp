/*
 * Created by Kami-Kaze on 4/16/2021.
 */

#define GLFW_INCLUDE_NONE

#include "core/window/window.h"
#include "glfw_window.h"

namespace Engine {

    Window* Window::create(const WindowProps& props, const EventFunction& eventFunction) {
        return new GlfwWindow(props, eventFunction);
    }

}