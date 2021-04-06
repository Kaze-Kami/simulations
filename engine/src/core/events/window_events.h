#pragma once

/*
 * Created by Kami-Kaze on 3/28/2021.
 */

#include "event.h"

namespace Engine {

    class WindowCloseEvent : public Event {

    public:
        EVENT_CLASS_TYPE(WindowClose);
    };

    class WindowResizeEvent : public Event {
    public:
        inline WindowResizeEvent(int width, int height) : width(width), height(height) {}

        const int width, height;

        EVENT_CLASS_TYPE(WindowResize);
    };

    class WindowMoveEvent : public Event {
    public:
        inline WindowMoveEvent(int posX, int posY) : posX(posX), posY(posY) {}

        const int posX, posY;

        EVENT_CLASS_TYPE(WindowMove);
    };

    class WindowViewportChangeEvent : public Event {
    public:
        inline WindowViewportChangeEvent(const int x, const int y, const int width, const int height)
                : x(x), y(y), width(width), height(height) {}

        const int x, y, width, height;

        EVENT_CLASS_TYPE(ViewportChange);
    };

}