#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include <thread>

#include "core/window/window.h"
#include "core/events/window_events.h"
#include "core/events/mouse_events.h"

namespace Engine {

    struct ApplicationProps {
        std::string name = "";
        WindowProps windowProps;
    };

    class Application {
    public:
        Application();
        virtual ~Application() = default;

        virtual void init();
        virtual void shutdown();

        virtual void onContextAttach(Context* context);
        virtual void onContextDetach(Context* context);

        void run();
        void stop();

        Window* getWindow();

        void dispatchEvent(Event& e);

        static Application* get();
        static Application* createApplication();

    protected:
        inline virtual void setup(ApplicationProps& props) {}   // todo: improve naming
        inline virtual void skipFrameUpdate() {}                // called if we need to skip a frame
        inline virtual void update(float dt) {}
        inline virtual void render(Context* context) {}
        inline virtual void renderImGui() {}
        inline virtual void onEvent(Event& e) {}

    private:
        bool onMouseWheelScrollEvent(MouseWheelScrollEvent& e);
        bool onWindowViewportChangeEvent(WindowViewportChangeEvent& e);
        bool onWindowCloseEvent(WindowCloseEvent& e);

        bool initialized = false, running = false, didSkipFrame = false;
        float lastFrameTime = 0, targetFrameTime = 60;

        Window* window = nullptr;

        static Application* instance;
    };
}