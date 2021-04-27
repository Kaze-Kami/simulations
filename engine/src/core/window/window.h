#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include "window_props.h"

#include <functional>

#include "core/input/input_controller.h"
#include "core/events/event.h"
#include "core/renderer/renderSurface.h"
#include "core/renderer/context/context.h"

namespace Engine {

    class Window : public RenderSurface {
    public:
        using EventFunction = std::function<void (Event&)>;

        virtual ~Window() = default;

        virtual void pollEvents() = 0;

        virtual Context* getContext() = 0;

        virtual float getWidth() = 0;
        virtual float getHeight() = 0;

        virtual float getMonitorFramerate() = 0;

        inline glm::vec2 getSize() {
            return glm::vec2(getWidth(), getHeight());
        }

        virtual InputController* getInputController() = 0;

        static Window* create(const WindowProps& props, const EventFunction& eventFunction);
    };

}