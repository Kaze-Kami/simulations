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

        void waitEvents() override;

        GLFWwindow* getGlfwWindow();
        void* getNativeWindow() override;
        void* getProcAddressFun() override;

        void makeContextCurrent() override;
        void releaseContext() override;

        Context* getContext() override;

        float getWidth() override;
        float getHeight() override;

        bool isFocused();
        InputController* getInputController() override;

    private:
        GLFWwindow* window = nullptr;
        GlfwInputController* inputController = nullptr;

        struct WindowData {
            int width = 0, height = 0, posX = 0, posY = 0;
            int vpWidth = 0, vpHeight = 0;
            bool focused = false;

            // everything we need within the glfw callbacks
            GLFWmonitor* monitor = nullptr;
            MessageQueue* messageQueue = nullptr;
            Context* context = nullptr;
        };

        WindowData data;

        void init(const WindowProps& props);

        void shutdown();
    };

}