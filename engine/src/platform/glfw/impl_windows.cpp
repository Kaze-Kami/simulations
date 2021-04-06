/*
 * Created by Kami-Kaze on 3/27/2021.
 */
#define GLFW_INCLUDE_NONE

#include "core/window/window.h"
#include "glfw_window.h"

namespace Engine {

    Window* Window::create(const WindowProps& props, MessageQueue* messageQueue) {
        return new WindowsWindow(props, messageQueue);
    }

}