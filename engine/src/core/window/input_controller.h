#pragma once

/*
 * Created by Kami-Kaze on 4/14/2021.
 */

#include <glm/glm.hpp>

namespace Engine {
    class InputController {
    public:
        // global mouse pos (returns mouse position in screen coordinates
        virtual glm::vec2 getGlobalMousePosition() const = 0;
        virtual float getGlobalMouseX() const = 0;
        virtual float getGlobalMouseY() const = 0;

        // local mouse pos (returns mouse position in relative coordinates (x, y in [-1, ..., 1])
        virtual glm::vec2 getMousePosition() const = 0;
        virtual float getMouseX() const = 0;
        virtual float getMouseY() const = 0;

        // buttons
        virtual bool isKeyDown(int key) const = 0;
        virtual bool isMouseButtonDown(int button) const = 0;
    };
}