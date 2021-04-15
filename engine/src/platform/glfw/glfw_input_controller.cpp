/*
 * Created by Kami-Kaze on 4/14/2021.
 */
#include "glfw_input_controller.h"

namespace Engine {

    GlfwInputController::GlfwInputController(GLFWwindow* window) : window(window) {}

    glm::vec2 GlfwInputController::getGlobalMousePosition() const {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return glm::vec2(float(x), float(y));
    }

    float GlfwInputController::getGlobalMouseX() const {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return float(x);
    }

    float GlfwInputController::getGlobalMouseY() const {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return float(y);
    }

    glm::vec2 GlfwInputController::getMousePosition() const {
        glm::vec2 globalPos = getGlobalMousePosition();

        glm::vec2 windowSize = getWindowSize();
        glm::vec2 localPos = -1.f + globalPos / windowSize * 2.f;

        //! invert y axis (screen space is top left to bottom right, world space is bottom left to top right)
        localPos.y *= -1;
        return localPos;
    }

    float GlfwInputController::getMouseX() const {
        return -1.f + getGlobalMouseX() / getWindowWidth() * 2.f;
    }

    float GlfwInputController::getMouseY() const {
        // invert y axis (screen space is top left to bottom right, world space is bottom left to top right)
        return 1.f - getGlobalMouseY() / getWindowHeight() * 2.f;
    }

    bool GlfwInputController::isKeyDown(KeyCode code) const {
        return glfwGetKey(window, code);
    }

    bool GlfwInputController::isMouseButtonDown(MouseCode code) const {
        return glfwGetMouseButton(window, code);
    }

    glm::vec2 GlfwInputController::getWindowSize() const {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        return glm::vec2(float(w), float(h));
    }

    float GlfwInputController::getWindowWidth() const {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        return float(w);
    }

    float GlfwInputController::getWindowHeight() const {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        return float(h);
    }
}