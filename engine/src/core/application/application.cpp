/*
 * Created by Kami-Kaze on 3/27/2021.
 */


#include <GLFW/glfw3.h>

#include "application.h"
#include "macros/assert.h"
#include "macros/bind.h"
#include "core/logging/engine_log.h"

namespace Engine {

    Application* Application::instance = nullptr;

    Application::Application() : messageQueue(MessageQueue(BIND_FN(Application::dispatchEvent))) {
        CORE_ASSERT(instance == nullptr, "Application already exists!");
        instance = this;
    }

    void Application::init() {
        CORE_ASSERT(!initialized, "Application already initialized!");
        ApplicationProps props;
        // run setup
        setup(props);

        // create window and do other stuff
        window = Window::create(props.windowProps, &messageQueue);

        initialized = true;
    }

    void Application::shutdown() {
        initialized = false;
        delete window;
    }

    void Application::onContextAttach() {}

    void Application::onContextDetach() {}

    void Application::run() {
        CORE_ASSERT(initialized, "Application not initialized!");

        LOG_CORE_TRACE("Application started!");
        running = true;

        // start render loop
        renderThread = new std::thread(&Application::renderLoop, this);

        // start event loop
        while (running) {
            // update events
            window->waitEvents();
        }

        // wait for render loop to exit
        renderThread->join();
        delete renderThread;

        LOG_CORE_TRACE("Application stopped!");
    }

    void Application::stop() {
        running = false;
    }

    void Application::renderLoop() {
        // 'Worker' thread:
        // - Update
        // - Render
        // - Swap buffers

        Context* context = window->getContext();
        context->bind();
        onContextAttach();

        lastFrameTime = (float) glfwGetTime();

        while (running) {
            // poll events
            messageQueue.pump();

            // ToDo: This is only temporary and should be replaced with our own timing mechanism
            float t = (float) glfwGetTime();
            float dt = t - lastFrameTime;
            lastFrameTime = t;

            update(dt);

            context->beginFrame();
            render(context);
            context->endFrame();

            windowMutex.lock();
            if (running) {
                context->swapBuffers();
            }
            windowMutex.unlock();
        }
        onContextDetach();
        context->unbind();
    }

    bool Application::onWindowViewportChangeEvent(WindowViewportChangeEvent* e) {
        window->getContext()->setViewport(e->x, e->y, e->width, e->height);

        return true;
    }

    bool Application::onWindowCloseEvent(WindowCloseEvent*) {
        // stop execution
        windowMutex.lock();
        running = false;
        windowMutex.unlock();

        return true;
    }

    void Application::dispatchEvent(Event* e) {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<WindowCloseEvent>(BIND_FN(Application::onWindowCloseEvent));
        dispatcher.dispatch<WindowViewportChangeEvent>(BIND_FN(Application::onWindowViewportChangeEvent));
    }

    Application* Application::get() {
        CORE_ASSERT(instance, "Application not created yet!");
        return instance;
    }

}