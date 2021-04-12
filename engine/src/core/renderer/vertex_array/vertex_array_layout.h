#pragma once

/*
 * Created by Kami-Kaze on 4/4/2021.
 */

#include <glad/glad.h>
#include <vector>

#include "core/renderer/buffer/buffer.h"

namespace Engine {

    struct VertexBufferLayoutAttribute {
        // GLuint index; => carried by the layout
        // GLsizei stride; => carried by the layout
        // const void *pointer; => carried by the layout

        int count;
        int size;           // number of T's
        GLenum type;        // basically T but we need to parse it
        bool normalize;    // whether or not our data should be normalized on load
    };


    class VertexBufferLayout {
    private:
        std::vector<VertexBufferLayoutAttribute> attributes = std::vector<VertexBufferLayoutAttribute>();
        int stride = 0;

    public:
        template<class T>
        inline void pushAttribute(int count, bool normalize) {
            int size = sizeof(T);
            GLenum type = toGladType<T>();
            attributes.push_back({count, size, type, normalize});

            // increment stride
            stride += size * count;
        }

        inline std::vector<VertexBufferLayoutAttribute>& getAttributes() {
            return attributes;
        }

        inline int getStride() {
            return stride;
        }

    private:
        template<class T>
        inline static GLenum toGladType() {
            CORE_ASSERT(false, "Call to generic template VertexBufferLayout::toGladType()");
            return 0;
        }

#define IMPL_PUSH(_t_cpp, _t_glad) \
template<> inline static GLenum toGladType<_t_cpp>() { return _t_glad; }

        IMPL_PUSH(float, GL_FLOAT);
        IMPL_PUSH(int, GL_INT);
        IMPL_PUSH(unsigned int, GL_UNSIGNED_INT);
    };

}