#pragma once

/*
 * Created by Kami-Kaze on 4/3/2021.
 */

#include <glad/glad.h>

#include "macros/assert.h"
#include "macros/gl_call.h"

namespace Engine {
    template<class T>
    class Buffer {
    public:
        inline Buffer(GLenum type, GLenum access, int itemCount, const void* data)
                : type(type), access(access), itemCount(itemCount), itemSize(sizeof(T)) {
            // create buffer
            GL_CALL(glCreateBuffers(1, &id));

            // buffer data
            bind();
            GL_CALL(glBufferData(type, itemCount * itemSize, data, access));
            unbind();
        }

        inline ~Buffer() {
            GL_CALL(glDeleteBuffers(1, &id));
        }

        inline void bind() {
            GL_CALL(glBindBuffer(type, id));
        }

        inline void unbind() {
            GL_CALL(glBindBuffer(type, 0));
        }

        inline void bufferData(const void* data) {
            CORE_ASSERT(bound(), "Buffer to be written to not bound!");
            GL_CALL(glBufferSubData(type, 0, itemCount * itemSize, data));
        }

        inline void bufferSubData(int count, int startIndex, const void* data) {
            CORE_ASSERT(bound(), "Buffer to be written to not bound!");
            GL_CALL(glBufferSubData(type, startIndex * itemCount, count * itemSize, data));
        }

        inline bool bound() {
            //! todo: when using a new kind of buffer ensure that
            //! GL_[BUFFER_TYPE] +2 == GL_[BUFFER_TYPE]_BINDING
            //! todo: holds true ;; checked:
            //! - GL_ARRAY_BUFFER
            //! - GL_ELEMENT_ARRAY_BUFFER

            int activeId;
            GL_CALL(glGetIntegerv(type + 2, &activeId));
            return activeId == id;
        }

    private:
        GLenum type, access;
        int itemCount, itemSize;
        unsigned int id;
    };

}