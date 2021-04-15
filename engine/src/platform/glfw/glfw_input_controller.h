#pragma once

/*
 * Created by Kami-Kaze on 4/14/2021.
 */

#include "core/input/input_controller.h"

#include <GLFW/glfw3.h>

namespace Engine {
    class GlfwInputController : public InputController {
    public:
        GlfwInputController(GLFWwindow* window);

        // global mouse pos (returns mouse position in screen coordinates
        glm::vec2 getGlobalMousePosition() const;
        float getGlobalMouseX() const;
        float getGlobalMouseY() const;

        // local mouse pos (returns mouse position in relative coordinates (x, y in [-1, ..., 1])
        glm::vec2 getMousePosition() const;
        float getMouseX() const;
        float getMouseY() const;

        // buttons
        bool isKeyDown(KeyCode code) const;
        bool isMouseButtonDown(MouseCode code) const;
    private:
        GLFWwindow* window;

        glm::vec2 getWindowSize() const;
        float getWindowWidth() const;
        float getWindowHeight() const;
    };
}



