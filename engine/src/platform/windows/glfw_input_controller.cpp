/*
 * Created by Kami-Kaze on 4/14/2021.
 */

#include "glfw_input_controller.h"

#include "glfw_window.h"

namespace Engine {

    GlfwInputController::GlfwInputController(GlfwWindow* window)
        : InputController(), window(window) {}

    bool GlfwInputController::isKeyDown(KeyCode code) const {
        return glfwGetKey(window->getGlfwWindow(), code);
    }

    bool GlfwInputController::isMouseButtonDown(MouseCode code) const {
        return glfwGetMouseButton(window->getGlfwWindow(), code);
    }

    glm::vec2 GlfwInputController::getWindowSizeImpl() const {
        int w, h;
        glfwGetWindowSize(window->getGlfwWindow(), &w, &h);
        return glm::vec2(float(w), float(h));
    }

    glm::vec2 GlfwInputController::getGlobalMousePosImpl() const {
        double x, y;
        glfwGetCursorPos(window->getGlfwWindow(), &x, &y);
        return glm::vec2(float(x), float(y));
    }


}