#pragma once

/*
 * Created by Kami-Kaze on 3/28/2021.
 */

#include <glad/glad.h>

#include "definitions.h"
#include "core/logging/engine_log.h"

#if defined(_BUILD_RELEASE)
#define GL_CALL(x) x;
#else
#define GL_CALL(x) { \
    ::Engine::internal::glClearErrors(); \
    x; \
    ::Engine::internal::glLogCall(__FILE__, __LINE__); \
}
#endif

namespace Engine::internal {
    inline void glClearErrors() {
        while(glGetError() != GL_NO_ERROR);
    }

    inline bool glLogCall(const char* file, int line) {
        bool nError = true;
        while (GLenum error = glGetError()) {
            LOG_CORE_ERROR("OpenGL Error: {} in {}:{}", error, file, line);
            nError = false;
        }
        return nError;
    }
}