#pragma once

/*
 * Created by Kami-Kaze on 3/28/2021.
 */

#include <glm/glm.hpp>

#include "core/renderer/renderSurface.h"

namespace Engine {

    class Context {
    public:
        explicit inline Context(RenderSurface* renderSurface) : renderSurface(renderSurface) {}
        virtual ~Context() = default;

        virtual void bind() = 0;
        virtual void unbind() = 0;

        virtual bool isBound() = 0;

        inline void swapBuffers() {
            renderSurface->swapBuffers();
        }

        // opengl config
        virtual void setVsync(bool vsync) = 0;
        virtual void setViewport(int x0, int y0, int width, int height) = 0;
        virtual void setMultisample(bool enable) = 0;

        // msc
        virtual void setFpsCounterEnable(bool enable) = 0;
        virtual void setFpsCounterColor(glm::vec4 color) = 0;

        // opengl util
        virtual void setClearColor(float r, float g, float b, float a) = 0;
        inline virtual void setClearColor(float r, float g, float b) { setClearColor(r, g, b, 1.f); };
        inline virtual void setClearColor(glm::vec3 color) { setClearColor(color.r, color.g, color.b, 1.f); }
        inline virtual void setClearColor(glm::vec4 color) { setClearColor(color.r, color.g, color.b, color.a); }
        virtual void setClearFlags(unsigned int flags = 0) = 0;

        // not used yet, but interesting for the future
        inline virtual void beginFrame() {}
        inline virtual void endFrame() {}

        inline virtual void beginImGuiFrame() {}
        inline virtual void endImGuiFrame() {}

    protected:
        RenderSurface* renderSurface;
    };

}