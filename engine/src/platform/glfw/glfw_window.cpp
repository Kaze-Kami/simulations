/*
 * Created by Kami-Kaze on 3/27/2021.
 */


#include <glad/glad.h>

#include "glfw_window.h"
#include "macros/assert.h"
#include "macros/gl_call.h"
#include "core/logging/engine_log.h"
#include "core/events/window_events.h"
#include "core/events/mouse_events.h"

namespace Engine {

    namespace internal::glfw {
        static bool glfwInitialized = false;
        static void GLFWErrorCallback(int error, const char* description) {
            LOG_CORE_ERROR("GLFW Error: {} \"{}\"", error, description);
        }

        void setWindowHint(int name, bool enable) {
            if (enable) {
                glfwWindowHint(name, GLFW_TRUE);
            } else {
                glfwWindowHint(name, GLFW_FALSE);
            }
        }
    }

    GlfwWindow::GlfwWindow(const WindowProps &props, MessageQueue* messageQueue) {
        data.messageQueue = messageQueue;
        init(props);
    }

    GlfwWindow::~GlfwWindow() {
        shutdown();
    }

    void GlfwWindow::swapBuffers() {
        glfwSwapBuffers(window);
    }

    void GlfwWindow::setVsync(bool enable) {
        if (enable) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(0);
        }
    }

    void GlfwWindow::pollEvents() {
        glfwPollEvents();
    }

    void GlfwWindow::waitEvents() {
        glfwWaitEvents();
    }

    void* GlfwWindow::getProcAddressFun() {
        return (void*) glfwGetProcAddress;
    }

    void GlfwWindow::makeContextCurrent() {
        glfwMakeContextCurrent(window);
    }

    Context* GlfwWindow::getContext() {
        return data.context;
    }

    void GlfwWindow::init(const WindowProps& props) {
        using namespace internal::glfw;

        if (!glfwInitialized) {
            int glfwStatus = glfwInit();
            CORE_ASSERT(glfwStatus, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
            glfwInitialized = true;
        }

        // hide window till creation
        setWindowHint(GLFW_VISIBLE, false);

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

        if (props.fullscreen || props.maximized) {
            if (props.fullscreen) {
                data.monitor = primaryMonitor;
            }
            data.width = videoMode->width,
            data.height = videoMode->height;
            data.posX = 0;
            data.posY = 0;
        }
        else {
            data.width = props.width;
            data.height = props.height;
            if (props.center) {
                data.posX = (videoMode->width - props.width) / 2;
                data.posY = (videoMode->height - props.height) / 2;
            } else {
                data.posX = props.positionX;
                data.posY = props.positionY;
            }
        }

        if (props.multisample) {
            // enable multisample
            LOG_CORE_DEBUG("Multisample x{}", props.multisample);
            glfwWindowHint(GLFW_SAMPLES, props.multisample);
        }

        setWindowHint(GLFW_DECORATED, props.decorated);
        setWindowHint(GLFW_MAXIMIZED, props.maximized);
        setWindowHint(GLFW_CENTER_CURSOR, props.centerCursor);
        setWindowHint(GLFW_FOCUSED, props.focused);
        setWindowHint(GLFW_FOCUS_ON_SHOW, props.focused);
        setWindowHint(GLFW_RESIZABLE, props.resizable);

        window = glfwCreateWindow(data.width, data.height, props.name.c_str(), data.monitor, nullptr);
        CORE_ASSERT(window, "Could not create GLFW Window!");

        inputController = new GlfwInputController(window);

        if (!props.fullscreen) {
            glfwSetWindowPos(window, data.posX, data.posY);
        }

        // create context and push config
        data.context = Context::create(this);

        // multisample
        data.multisample = props.multisample;
        data.context->setMultisample(props.multisample);

        // viewport
        glfwGetFramebufferSize(window, &data.vpWidth, &data.vpHeight);
        data.context->setViewport(0, 0, data.vpWidth, data.vpHeight);

        // vsync
        data.vsync = props.vsync;
        data.context->setVsync(props.vsync);

        // set user point (window data)
        glfwSetWindowUserPointer(window, &data);


        // set window callbacks
        glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.messageQueue->dispatchImmediately(new WindowCloseEvent);
        });

        glfwSetWindowPosCallback(window, [](GLFWwindow* window, int xpos, int ypos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.posX = xpos;
            data.posY = ypos;

            data.messageQueue->dispatch(new WindowMoveEvent(xpos, ypos));
        });

        glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.width = width;
            data.height = height;

            // window size changed
            data.messageQueue->dispatch(new WindowResizeEvent(width, height));

            // viewport changed too
            glfwGetFramebufferSize(window, &data.vpWidth, &data.vpHeight);
            data.messageQueue->dispatch(new WindowViewportChangeEvent(0, 0, data.vpWidth, data.vpHeight));
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double posX,double posY) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            glm::vec2 globalPosition = glm::vec2(float(posX), float(posY));
            glm::vec2 localPosition = -1.f + globalPosition / glm::vec2(data.width, data.height) * 2.f;
            //! invert y axis
            localPosition.y *= -1;

            data.messageQueue->dispatch(new MouseMoveEvent(localPosition, globalPosition));
        });

        // set mouse callbacks
        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            glm::vec2 globalPosition = glm::vec2(float(x), float(y));
            glm::vec2 localPosition = -1.f + globalPosition / glm::vec2(data.width, data.height) * 2.f;
            //! invert y axis
            localPosition.y *= -1;

            if (action == GLFW_PRESS) {
                data.messageQueue->dispatch(new MouseButtonPressEvent(button, localPosition, globalPosition));
            } else {
                data.messageQueue->dispatch(new MouseButtonReleaseEvent(button, localPosition, globalPosition));
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, double offsetX, double offsetY) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.messageQueue->dispatch(new MouseWheelScrollEvent(float(offsetX), float(offsetY)));
        });

        // set key callbacks

        // show window
        glfwShowWindow(window);
    }

    void GlfwWindow::shutdown() {
        using namespace internal::glfw;
        delete data.context;
        glfwTerminate();
        glfwInitialized = false;
    }

    float GlfwWindow::getWidth() {
        return data.width;
    }

    float GlfwWindow::getHeight() {
        return data.height;
    }

    InputController* GlfwWindow::getInput() {
        return inputController;
    }

}