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

        bool isKeyDown(KeyCode code) const override;
        bool isMouseButtonDown(MouseCode code) const override;

    protected:
        glm::vec2 getWindowSizeImpl() const override;
        glm::vec2 getGlobalMousePosImpl() const override;

    private:
        GLFWwindow* window;
    };
}



