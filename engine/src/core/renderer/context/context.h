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

        inline void swapBuffers() {
            renderSurface->swapBuffers();
        }

        // opengl config
        virtual void setVsync(bool enable) = 0;
        virtual void setViewport(int x0, int y0, int width, int height) = 0;
        virtual void setMultisample(bool enable) = 0;

        // opengl util
        virtual void clear(float r, float g, float b, float a) = 0;
        inline virtual void clear(glm::vec4 color) { clear(color.r, color.g, color.b, color.a); };
        inline virtual void clear(glm::vec3 color) { clear(color.r, color.g, color.b, 1.f); }
        inline virtual void clear(float r, float g, float b) { clear(r, g, b, 1.f); };

        // not used yet, but interesting for the future
        inline virtual void beginFrame() {}
        inline virtual void endFrame() {}

        // virtual void drawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) = 0;

        static Context* create(RenderSurface* renderSurface);
    protected:
        RenderSurface* renderSurface;
    };

}