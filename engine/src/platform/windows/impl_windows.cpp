/*
 * Created by Kami-Kaze on 4/16/2021.
 */

#define GLFW_INCLUDE_NONE

#include "core/window/window.h"
#include "glfw_window.h"

#include "opengl_context.h"
#include "core/renderer/context/context.h"

namespace Engine {

    Window* Window::create(const WindowProps& props, MessageQueue* messageQueue) {
        return new GlfwWindow(props, messageQueue);
    }

    Context* Context::create(RenderSurface* renderSurface) {
        return new OpenGlContext(renderSurface);
    }


}