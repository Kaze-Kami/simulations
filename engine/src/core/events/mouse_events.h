#pragma once

/*
 * Created by Kami-Kaze on 4/14/2021.
 */

#include "event.h"
#include "core/input/mouse_codes.h"

#include <glm/glm.hpp>

namespace Engine {

    class MouseMoveEvent : public Event {
    public:
        const glm::vec2 pos, globalPos;

        EVENT_CLASS_TYPE(MouseMove);

        inline MouseMoveEvent(const glm::vec2 pos, const glm::vec2 globalPos) : pos(pos), globalPos(globalPos) {}
    };

    class MouseButtonPressEvent : public Event {
    public:
        const MouseCode code;
        const glm::vec2 pos, globalPos;

        EVENT_CLASS_TYPE(MouseButtonPress);

        inline explicit MouseButtonPressEvent(const MouseCode code, const glm::vec2 pos, const glm::vec2 globalPos)
                : code(code), pos(pos), globalPos(globalPos) {}
    };

    class MouseButtonReleaseEvent : public Event {
    public:
        const MouseCode code;
        const glm::vec2 pos, globalPos;

        EVENT_CLASS_TYPE(MouseButtonRelease);

        inline explicit MouseButtonReleaseEvent(const MouseCode code, const glm::vec2 pos, const glm::vec2 globalPos)
                : code(code), pos(pos), globalPos(globalPos) {}
    };

    class MouseWheelScrollEvent : public Event {
    public:
        const float offsetX, offsetY;

        EVENT_CLASS_TYPE(MouseWheelScroll);

        inline MouseWheelScrollEvent(float offsetX, float offsetY) : offsetX(offsetX), offsetY(offsetY) {}
    };

}