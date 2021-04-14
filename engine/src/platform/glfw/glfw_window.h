#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/window/window.h"
#include "core/events/event.h"

#include "glfw_input_controller.h"

namespace Engine {

    namespace internal::glfw {
        void setWindowHint(int name, bool enable);
    }

    class GlfwWindow : public Window {
    public:
        explicit GlfwWindow(const WindowProps& props, MessageQueue* messageQueue);

        virtual ~GlfwWindow();

        void swapBuffers() override;

        void setVsync(bool enable) override;

        void pollEvents() override;
        void waitEvents() override;

        void* getProcAddressFun() override;
        void makeContextCurrent() override;

        Context* getContext() override;

        float getWidth() override;
        float getHeight() override;

        InputController* getInput() override;

    private:
        GLFWwindow* window = nullptr;
        GlfwInputController* inputController;

        struct WindowData {
            int width = 0, height = 0, posX = 0, posY = 0;
            int vpWidth = 0, vpHeight = 0;
            bool vsync = true, multisample = true;
            GLFWmonitor* monitor = nullptr;
            MessageQueue* messageQueue = nullptr;
            Context* context = nullptr;
        };

        WindowData data;

        void init(const WindowProps& props);

        void shutdown();
    };

}