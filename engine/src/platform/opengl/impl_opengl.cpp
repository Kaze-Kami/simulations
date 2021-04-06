/*
 * Created by Kami-Kaze on 4/3/2021.
 */

#include "core/renderer/context/context.h"
#include "opengl_context.h"

namespace Engine {

    Context* Context::create(RenderSurface* renderSurface) {
        return new OpenGlContext(renderSurface);
    }

}