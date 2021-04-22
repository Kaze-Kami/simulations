#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/window/window.h"
#include "core/events/event.h"

#include "opengl_context.h"
#include "glfw_input_controller.h"

namespace Engine {

    namespace internal::glfw {
        void setWindowHint(int name, bool enable);
    }

    class GlfwWindow : public Window {
    public:
        explicit GlfwWindow(const WindowProps& props, const EventFunction& eventFunction);

        virtual ~GlfwWindow();

        void swapBuffers() override;

        void setVsync(bool enable) override;

        void pollEvents() override;

        GLFWwindow* getGlfwWindow();
        void* getNativeWindow();
        void* getProcAddressFun();

        void makeContextCurrent();
        void releaseContext();

        Context* getContext() override;

        float getWidth() override;
        float getHeight() override;

        InputController* getInputController() override;

    private:
        GLFWwindow* window = nullptr;
        GlfwInputController* inputController = nullptr;
        OpenGlContext* context = nullptr;

        struct WindowData {
            int width = 0, height = 0, posX = 0, posY = 0;
            int vpWidth = 0, vpHeight = 0;

            // everything we need within the glfw callbacks
            EventFunction eventFunction;
            GLFWmonitor* monitor;
        };

        WindowData data;

        void init(const WindowProps& props, const EventFunction& eventFunction);

        void shutdown();
    };

}