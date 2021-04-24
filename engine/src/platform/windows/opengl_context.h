#pragma once

/*
 * Created by Kami-Kaze on 4/3/2021.
 */

#include "core/renderer/context/context.h"

#include <glad/glad.h>

#include <thread>

namespace Engine {

    namespace internal::opengl_context {

        enum GL_LOG_LEVEL {
            None = 0, HighAssert = 1, High = 2, Medium = 3, Low = 4, Notification = 5
        };

        void enableGLDebugging(GL_LOG_LEVEL logLevel);
        void setGLDebugLogLevel(GL_LOG_LEVEL level);
        void GLAPIENTRY openGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam);

        void setMultisample(bool enable);
        void setViewport(int x0, int y0, int width, int height);

    }

    class GlfwWindow;

    class OpenGlContext : public Context {
    public:
        explicit OpenGlContext(GlfwWindow* window);
        ~OpenGlContext() override = default;

        void bind() override;
        void unbind() override;
        bool isBound() override;

        void beginFrame() override;
        void endFrame() override;

        void beginImGuiFrame() override;
        void endImGuiFrame() override;

        void setVsync(bool vsync) override;
        void setViewport(int x0, int y0, int width, int height) override;
        void setMultisample(bool enable) override;

        void setFpsCounterEnable(bool enable) override;
        void setFpsCounterColor(glm::vec4 color) override;

        void setClearColor(float r, float g, float b, float a) override;
        void setClearFlags(unsigned int flags) override;

    private:
        GlfwWindow* window = nullptr;

        // flags
        bool bound = false, vsync = false, multisample = false;

        // viewport data
        int vpX0 = 0, vpY0 = 0, vpWidth = 1, vpHeight = 1;

        // fps counter
        bool fpsCounterEnable = false;
        glm::vec4 fpsCounterColor = glm::vec4(1.f);

        // defaults for clear: clear color with black
        glm::vec4 clearColor = glm::vec4(0.f, 0.f, 0.f, 1.f);
        unsigned int clearFlags = GL_COLOR_BUFFER_BIT;
    };

}