/*
 * Created by Kami-Kaze on 4/3/2021.
 */

#include "opengl_context.h"

#include "glfw_window.h"

#include "macros/assert.h"
#include "macros/gl_call.h"

#include <imgui/imgui.h>

#include "imgui_impl.h"
#include <imgui/backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

namespace Engine {

    namespace internal::opengl_context {

        static GL_LOG_LEVEL debugLogLevel = GL_LOG_LEVEL::None;

        void setGLDebugLogLevel(GL_LOG_LEVEL level) {
            debugLogLevel = level;
        }

        void GLAPIENTRY openGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
            switch (severity) {
                case GL_DEBUG_SEVERITY_HIGH:
                    if (debugLogLevel > GL_LOG_LEVEL::None) {
                        LOG_CORE_ERROR("[OpenGL Debug] {}", message);
                        CORE_ASSERT(debugLogLevel != GL_LOG_LEVEL::HighAssert, "GL_DEBUG_SEVERITY_HIGH");
                    }
                    break;
                case GL_DEBUG_SEVERITY_MEDIUM:
                    if (debugLogLevel > GL_LOG_LEVEL::High)
                        LOG_CORE_WARN("[OpenGL Debug] {}", message);
                    break;
                case GL_DEBUG_SEVERITY_LOW:
                    if (debugLogLevel > GL_LOG_LEVEL::Medium)
                        LOG_CORE_INFO("[OpenGL Debug] {}", message);
                    break;
                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    if (debugLogLevel > GL_LOG_LEVEL::Low)
                        LOG_CORE_TRACE("[OpenGL Debug] {}", message);
                    break;
            }
        }

        void enableGLDebugging(GL_LOG_LEVEL logLevel) {
            setGLDebugLogLevel(logLevel);
            GL_CALL(glDebugMessageCallback(openGLLogMessage, nullptr));
            GL_CALL(glEnable(GL_DEBUG_OUTPUT));
            GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
        }

        void setMultisample(bool enable) {
            if (enable) {
                GL_CALL(glEnable(GL_MULTISAMPLE));
            } else {
                GL_CALL(glDisable(GL_MULTISAMPLE));
            }

        }

        void setViewport(int x0, int y0, int width, int height) {
            GL_CALL(glViewport(x0, y0, width, height));
        }

    }

    OpenGlContext::OpenGlContext(GlfwWindow* window) : Context(window), window(window) {}

    void OpenGlContext::bind() {
        // make context current
        window->makeContextCurrent();

        // init glad
        int status = gladLoadGLLoader((GLADloadproc)window->getProcAddressFun());
        CORE_ASSERT(status, "Failed to initialize Glad!");

        LOG_CORE_INFO("OpenGL Info:");
        LOG_CORE_INFO("  Vendor: {}", glGetString(GL_VENDOR));
        LOG_CORE_INFO("  Renderer: {}", glGetString(GL_RENDERER));
        LOG_CORE_INFO("  Version: {}", glGetString(GL_VERSION));

        contextThreadId = std::this_thread::get_id();
        bound = true;

        // enable opengl debugging
        internal::opengl_context::enableGLDebugging(internal::opengl_context::GL_LOG_LEVEL::HighAssert);

        // set viewport and multisample and vsync
        internal::opengl_context::setMultisample(multisample);
        internal::opengl_context::setViewport(vpX0, vpY0, vpWidth, vpHeight);
        window->setVsync(vsync);

        // init imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        // io.ConfigViewportsNoAutoMerge = true;
        io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        // todo: cast
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 450 core");
    }

    void OpenGlContext::unbind() {
        // shut down imgui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // unbind context
        bound = false;
        contextThreadId = std::thread::id();
        window->releaseContext();
    }

    void OpenGlContext::beginFrame() {
        // clear screen
        GL_CALL(glClear(clearFlags));

        // begin imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void OpenGlContext::endFrame() {

        // render imgui
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(window->getWidth(), window->getHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void OpenGlContext::setVsync(bool enable) {
        vsync = enable;
        if (bound) {
            CORE_ASSERT(std::this_thread::get_id() == contextThreadId, "Calling context functions from unbound thread id!");
            window->setVsync(enable);
        }
    }

    void OpenGlContext::setViewport(int x0, int y0, int width, int height) {
        vpX0 = x0;
        vpY0 = y0;
        vpWidth = width;
        vpHeight = height;
        if (bound) {
            CORE_ASSERT(std::this_thread::get_id() == contextThreadId, "Calling context functions from unbound thread id!");
            internal::opengl_context::setViewport(x0, y0, width, height);
        }
    }

    void OpenGlContext::setMultisample(bool enable) {
        multisample = enable;
        if (bound) {
            CORE_ASSERT(std::this_thread::get_id() == contextThreadId, "Calling context functions from unbound thread id!");
            internal::opengl_context::setMultisample(enable);
        }
    }

    void OpenGlContext::setClearColor(float r, float g, float b, float a) {
        CORE_ASSERT(bound, "Calling context functions on unbound context!");
        CORE_ASSERT(std::this_thread::get_id() == contextThreadId, "Calling context functions from unbound thread id!");
        GL_CALL(glClearColor(r, g, b, a));
    }

    void OpenGlContext::setClearFlags(unsigned int flags) {
        CORE_ASSERT(bound, "Calling context functions on unbound context!");
        CORE_ASSERT(std::this_thread::get_id() == contextThreadId, "Calling context functions from unbound thread id!");
        clearFlags = flags;
    }

    bool OpenGlContext::isBound() {
        return bound;
    }

}