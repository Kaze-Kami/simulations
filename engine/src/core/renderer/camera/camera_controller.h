#pragma once

/*
 * Created by Kami-Kaze on 4/15/2021.
 *
 * todo: move implementations to .cpp
 */

#include "camera.h"
#include "core/input/input_controller.h"
#include "core/input/mouse_codes.h"

#include <glm/glm.hpp>

namespace Engine {
    class CameraController {
    public:
        inline explicit CameraController(glm::vec2 vTrans, float fScale, const std::string& uniformName)
                : camera(Camera(uniformName)), vTrans(vTrans), fScale(1.f + fScale) {}

        inline ~CameraController() {}

        inline void update(const InputController* ic) {
            if (ic->didMouseMove() && ic->isMouseButtonDown(Mouse::ButtonRight)) {
                camera.translate(vTrans * ic->getMouseDelta());
                cameraChanged = true;
            }
            if (ic->didMouseWheelScroll()) {
                float f = glm::pow(fScale, ic->getMouseWheelDelta().y);
                camera.scale(glm::vec2(f), ic->getMousePosition());
                cameraChanged = true;
            }
        }
        inline void resetCamera() {
            camera.reset();
            cameraChanged = true;
        }

        Camera& getCamera() {
            return camera;
        }

        inline bool needsUpload() const {
            return cameraChanged;
        }
        inline void setCameraUploaded() {
            cameraChanged = false;
        }

    private:
        glm::vec2 vTrans;
        float fScale;

        Camera camera;
        bool cameraChanged = true;
    };
}