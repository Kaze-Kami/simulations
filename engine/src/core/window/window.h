#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include "window_props.h"
#include "input_controller.h"
#include "core/events/event.h"
#include "core/message_queue/message_queue.h"
#include "core/renderer/renderSurface.h"
#include "core/renderer/context/context.h"

namespace Engine {

    class Window : public RenderSurface {
    public:
        virtual ~Window() = default;

        virtual void pollEvents() = 0;
        virtual void waitEvents() = 0;

        virtual Context* getContext() = 0;

        virtual float getWidth() = 0;
        virtual float getHeight() = 0;

        inline glm::vec2 getSize() {
            return glm::vec2(getWidth(), getHeight());
        }

        virtual InputController* getInput() = 0;

        static Window* create(const WindowProps& props, MessageQueue* messageQueue);
    };

}