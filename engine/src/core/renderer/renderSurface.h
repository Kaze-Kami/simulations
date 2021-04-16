#pragma once

/*
 * Created by Kami-Kaze on 4/3/2021.
 */


namespace Engine {
    class RenderSurface {
    public:
        virtual ~RenderSurface() = default;

        virtual void swapBuffers() = 0;

        virtual void setVsync(bool enable) = 0;

        virtual float getWidth() = 0;
        virtual float getHeight() = 0;
    };
}