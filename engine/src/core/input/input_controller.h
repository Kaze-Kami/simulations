#pragma once

/*
 * Created by Kami-Kaze on 4/14/2021.
 *
 * todo: move implementations to .cpp
 */

#include <glm/glm.hpp>

#include "key_codes.h"
#include "mouse_codes.h"

namespace Engine {
    class InputController {
    public:
        inline virtual void init() {
            // set last and current pos
            lastMousePos = getMousePosImpl();
            lastGlobalMousePos = getGlobalMousePosition();
            currentMousePos = lastMousePos;
            currentGlobalMousePos = lastGlobalMousePos;

            // zero delta
            mouseDelta = glm::vec2(0.f);
            globalMouseDelta = glm::vec2(0.f);
            mouseWheelDelta = glm::vec2(0.f);
            mouseWheelBuffer = glm::vec2(0.f);
        }

        /* Mouse */
        virtual bool isMouseButtonDown(MouseCode code) const = 0;

        inline bool didMouseWheelScroll() const {
            return mouseWheelDelta != glm::vec2(0.f);
        }

        inline bool didMouseMove() const {
            return mouseDelta != glm::vec2(0.f);
        }

        // in screen coordinates
        inline glm::vec2 getGlobalMousePosition() const {
            return currentGlobalMousePos;
        };

        inline float getGlobalMouseX() const {
            return currentGlobalMousePos.x;
        };

        inline float getGlobalMouseY() const {
            return currentGlobalMousePos.y;
        };

        // in relative coordinates (x, y in [-1, ..., 1])
        inline glm::vec2 getMousePosition() const {
            return currentMousePos;
        };

        inline float getMouseX() const {
            return currentMousePos.x;
        };

        inline float getMouseY() const {
            return currentMousePos.y;
        };

        // mouse delta
        inline glm::vec2 getGlobalMouseDelta() const {
            return globalMouseDelta;
        }

        inline glm::vec2 getMouseDelta() const {
            return mouseDelta;
        }

        inline glm::vec2 getMouseWheelDelta() const {
            return mouseWheelDelta;
        }

        /* Buttons */
        virtual bool isKeyDown(const KeyCode code) const = 0;

        /* Update */
        inline void recordMouseWheelScroll(glm::vec2 delta) {
            mouseWheelBuffer += delta;
        }

        inline void update() {
            lastMousePos = currentMousePos;
            lastGlobalMousePos = currentGlobalMousePos;

            currentMousePos = getMousePosImpl();
            currentGlobalMousePos = getGlobalMousePosition();

            mouseDelta = currentMousePos - lastMousePos;
            globalMouseDelta = currentGlobalMousePos - lastGlobalMousePos;
            mouseWheelDelta = mouseWheelBuffer;

            // reset mouse wheel buffer
            mouseWheelBuffer = glm::vec2(0.f);
        }

        /* internal */
    protected:
        InputController() = default;

        virtual glm::vec2 getGlobalMousePosImpl() const = 0;

        virtual glm::vec2 getWindowSizeImpl() const = 0;

        inline glm::vec2 getMousePosImpl() const {
            glm::vec2 globalPos = getGlobalMousePosImpl();

            glm::vec2 windowSize = getWindowSizeImpl();
            glm::vec2 localPos = -1.f + globalPos / windowSize * 2.f;

            //! invert y axis (screen space is top left to bottom right, world space is bottom left to top right)
            localPos.y *= -1;
            return localPos;
        }

    private:
        glm::vec2 lastMousePos = glm::vec2(0.f), lastGlobalMousePos = glm::vec2(0.f);
        glm::vec2 currentMousePos = glm::vec2(0.f), currentGlobalMousePos = glm::vec2(0.f);
        glm::vec2
                mouseDelta = glm::vec2(0.f),
                globalMouseDelta = glm::vec2(0.f),
                mouseWheelDelta = glm::vec2(0.f),
                mouseWheelBuffer = glm::vec2(0.f);
    };
}