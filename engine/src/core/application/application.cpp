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

        // create window and do other stuff
        window = Window::create(props.windowProps, BIND_FN(dispatchEvent));

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

        LOG_CORE_TRACE("Application started!");
        running = true;

        Context* context = window->getContext();
        context->bind();
        onContextAttach(context);

        InputController* inputController = window->getInputController();
        inputController->init();

        lastFrameTime = (float) glfwGetTime();

        double sumSwapDuration = 0;
        long swaps = 0;

        while (running) {
            // update events
            window->pollEvents();
            inputController->update();


            float t = (float) glfwGetTime();
            float dt = t - lastFrameTime;
            lastFrameTime = t;

            update(dt);

            context->beginFrame();
            render(context);
            context->endFrame();

            //! todo: change to instrumentor
            auto start = std::chrono::high_resolution_clock::now();
            context->swapBuffers();
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            sumSwapDuration += elapsed.count();
            swaps++;
        }

        onContextDetach(context);
        context->unbind();

        //! todo: change to instrumentor as its implemented
        const double avgSwapDuration = sumSwapDuration / swaps;
        const double avgFrameRate = 1.f / avgSwapDuration;
        LOG_CORE_INFO("Avg swap duration: {:.6f} ms ({:.2f} fps)", avgSwapDuration * 1e3, avgFrameRate);

        LOG_CORE_TRACE("Application stopped!");
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