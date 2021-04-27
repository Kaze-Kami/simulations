#pragma once

/*
 * Created by Kami-Kaze on 3/28/2021.
 */

#include <string>
#include <glm/glm.hpp>

namespace Engine {

    struct WindowProps {
        explicit inline WindowProps() = default;

        std::string name;

        // window size and refresh rate
        int width = 0, height = 0,
            positionX = 0, positionY = 0,
            multisample = 1;

        // fullscreen windows
        bool fullscreen = false, center=true;

        // vsync and framerate
        bool vsync = true;
        int targetFramerate = 60;

        // 'standard' msc flags
        bool decorated = true,
             maximized = false,
             centerCursor = false,
             resizable = true,
             focused = true;

        // 'cool' msc flags
        bool overlay = false,
             noTaskbar = false;

        // 'other' flags and data fields
        bool fpsCounterEnable = false;
        glm::vec4 fpsCounterColor = glm::vec4(1.f);
    };

}