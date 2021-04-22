/*
 * Created by Kami-Kaze on 4/14/2021.
 */

#include "glfw_input_controller.h"

#include "glfw_window.h"
#include "imgui.h"

namespace Engine {

    GlfwInputController::GlfwInputController(GlfwWindow* window)
        : InputController(), window(window) {}

    bool GlfwInputController::isKeyDown(KeyCode code) const {
        return !ImGui::GetIO().WantCaptureKeyboard && glfwGetKey(window->getGLFWwindow(), code);
    }

    bool GlfwInputController::isMouseButtonDown(MouseCode code) const {
        return !ImGui::GetIO().WantCaptureMouse && glfwGetMouseButton(window->getGLFWwindow(), code);
    }

    glm::vec2 GlfwInputController::getWindowSizeImpl() const {
        int w, h;
        glfwGetWindowSize(window->getGLFWwindow(), &w, &h);
        return glm::vec2(float(w), float(h));
    }

    glm::vec2 GlfwInputController::getGlobalMousePosImpl() const {
        double x, y;
        glfwGetCursorPos(window->getGLFWwindow(), &x, &y);
        return glm::vec2(float(x), float(y));
    }


}