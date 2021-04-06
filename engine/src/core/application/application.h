#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include <thread>
#include <mutex>

#include "core/window/window.h"
#include "core/message_queue/message_queue.h"
#include "core/events/window_events.h"

namespace Engine {

    struct ApplicationProps {
        std::string name;
        WindowProps windowProps;
    };

    class Application {
    public:
        Application();
        virtual ~Application() = default;

        virtual void init();
        virtual void shutdown();

        virtual void onContextAttach();
        virtual void onContextDetach();

        void run();
        void stop();

        static Application* get();
        static Application* createApplication();

    protected:
        inline virtual void setup(ApplicationProps& props) {};  // todo: improve naming
        inline virtual void update(float dt) {}
        inline virtual void render(Context* context) {}

        MessageQueue messageQueue;

    private:
        void dispatchEvent(Event* e);
        void renderLoop();

        bool onWindowViewportChangeEvent(WindowViewportChangeEvent* e);
        bool onWindowCloseEvent(WindowCloseEvent* e);

        bool initialized = false, running = false;
        float lastFrameTime = 0;
        Window* window = nullptr;
        std::thread* renderThread = nullptr;
        std::mutex windowMutex;

        static Application* instance;
    };
}