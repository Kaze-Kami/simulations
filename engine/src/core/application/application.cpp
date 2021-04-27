/*
 * Created by Kami-Kaze on 3/27/2021.
 */


#include <GLFW/glfw3.h>

#include "application.h"
#include "macros/assert.h"
#include "macros/bind.h"

#include "core/logging/engine_log.h"

#include <functional>
#include <core/events/event.h>

namespace Engine {

    Application* Application::instance = nullptr;

    Application::Application() {
        CORE_ASSERT(instance == nullptr, "Application already exists!");
        instance = this;
    }

    void Application::init() {
        CORE_ASSERT(!initialized, "Application already initialized!");
        ApplicationProps props;
        // run setup
        setup(props);

        // create window
        const WindowProps& windowProps = props.windowProps;
        window = Window::create(windowProps, BIND_FN(dispatchEvent));

        // set target frame time
        float targetFramerate = float(windowProps.targetFramerate);

        if (windowProps.vsync) {
            targetFramerate = window->getMonitorFramerate();
            LOG_CORE_INFO("Vsync enable, using monitor framerate as target: {}", int(targetFramerate));
        }

        targetFrameTime = 2.f / targetFramerate;

        initialized = true;
    }

    void Application::shutdown() {
        initialized = false;
        delete window;
    }

    void Application::onContextAttach(Context* context) {}

    void Application::onContextDetach(Context* context) {}

    void Application::run() {
        CORE_ASSERT(initialized, "Application not initialized!");

        // attach context
        Context* context = window->getContext();
        context->bind();
        onContextAttach(context);

        // clear window event queue
        window->pollEvents();

        // init input controller
        InputController* inputController = window->getInputController();
        inputController->init();

        // init last frame time
        lastFrameTime = (float) glfwGetTime();

        LOG_CORE_TRACE("Application started!");
        running = true;

        while (running) {
            // update events
            window->pollEvents();
            inputController->update();

            // calculate dt
            float t = float(glfwGetTime());
            float dt = t - lastFrameTime;

            /*
             * Subject: 27k fireflies
             * With dynamic updates
             *      28.3ms
             *
             * Without dynamic updates
             *      41.5ms
             */

            if (dt < targetFrameTime || didSkipFrame) {
                didSkipFrame = false;
                lastFrameTime = t;

                // update
                update(dt);

                // render pass
                context->beginFrame();
                render(context);
                context->endFrame();
            } else {
                LOG_CORE_INFO("Skipping frame! (delta = {:.4f}ms)", (dt - targetFrameTime) * 1e3);
                // we were to late for this frame, skip it.
                // instead just do a bigger update next frame.
                didSkipFrame = true;

                skipFrameUpdate();

                // context->beginFrame();
                // render(context);
                // context->endFrame();
            }

            // imgui render pass
            context->beginImGuiFrame();
            renderImGui();
            context->endImGuiFrame();

            // swap buffers
            context->swapBuffers();
        }

        LOG_CORE_TRACE("Application stopped!");

        // detach context
        onContextDetach(context);
        context->unbind();
    }

    void Application::stop() {
        running = false;
    }

    Window* Application::getWindow() {
        return window;
    }

    bool Application::onMouseWheelScrollEvent(MouseWheelScrollEvent& e) {
        //! update scroll on controller
        window->getInputController()->recordMouseWheelScroll(glm::vec2(e.offsetY, e.offsetY));
        return false;
    }

    bool Application::onWindowViewportChangeEvent(WindowViewportChangeEvent& e) {
        window->getContext()->setViewport(e.x, e.y, e.width, e.height);
        return true;
    }

    bool Application::onWindowCloseEvent(WindowCloseEvent& e) {
        // stop execution
        running = false;
        return true;
    }

    void Application::dispatchEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<WindowCloseEvent>(BIND_FN(Application::onWindowCloseEvent));
        dispatcher.dispatch<WindowViewportChangeEvent>(BIND_FN(Application::onWindowViewportChangeEvent));
        dispatcher.dispatch<MouseWheelScrollEvent>(BIND_FN(Application::onMouseWheelScrollEvent));

        if (!e.handled) {
            onEvent(e);
        }
    }

    Application* Application::get() {
        CORE_ASSERT(instance, "Application not created yet!");
        return instance;
    }

}