/*
 * Created by Kami-Kaze on 3/27/2021.
 */


#include "glfw_window.h"

#include <imgui.h>

#include "macros/assert.h"
#include "macros/gl_call.h"

#include "core/logging/engine_log.h"

#include "core/events/window_events.h"
#include "core/events/mouse_events.h"
#include "core/events/key_events.h"

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

    GlfwWindow::GlfwWindow(const WindowProps &props, const EventFunction& eventFunction) {
        init(props, eventFunction);
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

    GLFWwindow* GlfwWindow::getGLFWwindow() {
        return window;
    }

    void* GlfwWindow::getNativeWindow() {
        return static_cast<void*>(window);
    }

    void* GlfwWindow::getProcAddressFun() {
        return (void*) glfwGetProcAddress;
    }

    void GlfwWindow::makeContextCurrent() {
        glfwMakeContextCurrent(window);
    }

    void GlfwWindow::releaseContext() {
        glfwMakeContextCurrent(nullptr);
    }

    Context* GlfwWindow::getContext() {
        return context;
    }

    void GlfwWindow::init(const WindowProps& props, const EventFunction& eventFunction) {
        using namespace internal::glfw;
        data.eventFunction = eventFunction;

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
        data.monitor = nullptr;

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

        inputController = new GlfwInputController(this);

        if (!props.fullscreen) {
            glfwSetWindowPos(window, data.posX, data.posY);
        }

        // create context and push config
        context = new OpenGlContext(this);

        // multisample
        context->setMultisample(props.multisample);

        // viewport
        glfwGetFramebufferSize(window, &data.vpWidth, &data.vpHeight);
        context->setViewport(0, 0, data.vpWidth, data.vpHeight);

        // vsync
        context->setVsync(props.vsync);

        // set user point (window data)
        glfwSetWindowUserPointer(window, &data);


        // set window callbacks
        glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent e;
            data.eventFunction(e);
        });

        glfwSetWindowPosCallback(window, [](GLFWwindow* window, int xpos, int ypos) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.posX = xpos;
            data.posY = ypos;

            WindowMoveEvent e(xpos, ypos);
            data.eventFunction(e);
        });

        glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.width = width;
            data.height = height;

            // window size changed
            WindowResizeEvent eResize(width, height);
            data.eventFunction(eResize);

            // viewport changed too
            glfwGetFramebufferSize(window, &data.vpWidth, &data.vpHeight);

            WindowViewportChangeEvent eViewport(0, 0, data.vpWidth, data.vpHeight);
            data.eventFunction(eViewport);
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double posX,double posY) {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            glm::vec2 globalPosition = glm::vec2(float(posX), float(posY));
            glm::vec2 localPosition = -1.f + globalPosition / glm::vec2(data.width, data.height) * 2.f;
            //! invert y axis
            localPosition.y *= -1;

            MouseMoveEvent e(localPosition, globalPosition);
            data.eventFunction(e);
        });

        // set mouse callbacks
        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            if (ImGui::GetIO().WantCaptureMouse) return;

            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            glm::vec2 globalPosition = glm::vec2(float(x), float(y));
            glm::vec2 localPosition = -1.f + globalPosition / glm::vec2(data.width, data.height) * 2.f;
            //! invert y axis
            localPosition.y *= -1;

            if (action == GLFW_PRESS) {
                MouseButtonPressEvent e(button, localPosition, globalPosition);
                data.eventFunction(e);
            } else {
                MouseButtonReleaseEvent e(button, localPosition, globalPosition);
                data.eventFunction(e);
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, double offsetX, double offsetY) {
            if (ImGui::GetIO().WantCaptureMouse) return;

            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseWheelScrollEvent e((float(offsetX)), float(offsetY));
            data.eventFunction(e);
        });

        // set key callbacks
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scanCode, int action, int mods) {
            if (ImGui::GetIO().WantCaptureKeyboard) return;

            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            if (action == GLFW_PRESS) {
                KeyPressEvent e(key, mods);
                data.eventFunction(e);
            } else {
                KeyReleaseEvent e(key, mods);
                data.eventFunction(e);
            }
        });

        // show window
        glfwShowWindow(window);
    }

    void GlfwWindow::shutdown() {
        using namespace internal::glfw;
        delete context;
        glfwTerminate();
        glfwInitialized = false;
    }

    float GlfwWindow::getWidth() {
        return data.width;
    }

    float GlfwWindow::getHeight() {
        return data.height;
    }

    InputController* GlfwWindow::getInputController() {
        return inputController;
    }

}