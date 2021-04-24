/*
 * Created by Kami-Kaze on 4/3/2021.
 */

#include "opengl_context.h"

#include "glfw_window.h"

#include "macros/assert.h"
#include "macros/gl_call.h"

#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <macros/debug.h>

namespace Engine {

    namespace internal::opengl_context {

        static GL_LOG_LEVEL debugLogLevel = GL_LOG_LEVEL::None;

        void setGLDebugLogLevel(GL_LOG_LEVEL level) {
            debugLogLevel = level;
        }

        void GLAPIENTRY openGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                         const GLchar* message, const void* userParam) {
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
        int status = gladLoadGLLoader((GLADloadproc) window->getProcAddressFun());
        CORE_ASSERT(status, "Failed to initialize Glad!");

        LOG_CORE_INFO("OpenGL Info:");
        LOG_CORE_INFO("  Vendor: {}", glGetString(GL_VENDOR));
        LOG_CORE_INFO("  Renderer: {}", glGetString(GL_RENDERER));
        LOG_CORE_INFO("  Version: {}", glGetString(GL_VERSION));
        bound = true;

        // enable opengl debugging in debug builds
        DEBUG(internal::opengl_context::enableGLDebugging(internal::opengl_context::GL_LOG_LEVEL::HighAssert));

        // set viewport, multisample, vsync and clear color (might have been set before bind())
        window->setVsync(vsync);
        internal::opengl_context::setMultisample(multisample);
        internal::opengl_context::setViewport(vpX0, vpY0, vpWidth, vpHeight);
        GL_CALL(glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a));


        // init imgui
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window->getGLFWwindow(), true);
        ImGui_ImplOpenGL3_Init("#version 450 core");
    }

    void OpenGlContext::unbind() {
        // shut down imgui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // unbind context
        bound = false;
        window->releaseContext();
    }

    void OpenGlContext::beginFrame() {
        // clear screen
        GL_CALL(glClear(clearFlags));
    }

    void OpenGlContext::endFrame() {

    }

    void OpenGlContext::beginImGuiFrame() {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void OpenGlContext::endImGuiFrame() {
        // maybe append fps counter
        if (fpsCounterEnable) {
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);

            // see imgui.h: ImGuiWindowFlags_ (line 936+)
            // pretty much: remove everything to make it an 'overlay'
            static unsigned int flags = 0b00000000000101100011001111111111;
            ImGui::Begin("__fps__counter__", nullptr, flags);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(fpsCounterColor.r, fpsCounterColor.g, fpsCounterColor.b, fpsCounterColor.a));
            ImGui::Text("~%.2f ms/frame (%.0f fps)", 1e3f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::PopStyleColor();
            ImGui::End();
        }

        // render imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void OpenGlContext::setVsync(bool vsync) {
        this->vsync = vsync;
        if (bound) {
            window->setVsync(vsync);
        }
    }

    void OpenGlContext::setViewport(int x0, int y0, int width, int height) {
        vpX0 = x0;
        vpY0 = y0;
        vpWidth = width;
        vpHeight = height;
        if (bound) {
            internal::opengl_context::setViewport(x0, y0, width, height);
        }
    }

    void OpenGlContext::setMultisample(bool enable) {
        multisample = enable;
        if (bound) {
            internal::opengl_context::setMultisample(enable);
        }
    }

    void OpenGlContext::setClearColor(float r, float g, float b, float a) {
        clearColor = glm::vec4(r, g, b, a);
        if (bound) {
            GL_CALL(glClearColor(r, g, b, a));
        }
    }

    void OpenGlContext::setClearFlags(unsigned int flags) {
        clearFlags = flags;
    }

    bool OpenGlContext::isBound() {
        return bound;
    }

    void OpenGlContext::setFpsCounterEnable(bool enable) {
        fpsCounterEnable = enable;
    }

    void OpenGlContext::setFpsCounterColor(glm::vec4 color) {
        fpsCounterColor = color;
    }

}