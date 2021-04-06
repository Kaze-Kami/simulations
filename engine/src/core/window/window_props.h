#pragma once

/*
 * Created by Kami-Kaze on 3/28/2021.
 */

#include <string>

namespace Engine {

    struct WindowProps {
        explicit inline WindowProps() = default;

        std::string name;

        // window size and refresh rate
        int width = 0, height = 0,
            positionX = 0, positionY = 0,
            multisample = 1;

        // fullscreen, vsync and multisample
        bool vsync = true,
             fullscreen = false, center=true;

        // 'standard' msc flags
        bool decorated = true,
             maximized = false,
             centerCursor = false,
             resizable = true,
             focused = true;

        // 'cool' msc flags
        bool overlay = false,
             noTaskbar = false;
    };

}