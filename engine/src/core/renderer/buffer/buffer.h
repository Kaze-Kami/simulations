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

        inline unsigned int getId() {
            return id;
        }

        inline void bind() const {
            GL_CALL(glBindBuffer(type, id));
        }

        inline void unbind() const {
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

        inline bool bound() const {
            int binding = 0;
            switch (type) {
                case GL_ARRAY_BUFFER:
                    binding = GL_ARRAY_BUFFER_BINDING;
                    break;
                case GL_ELEMENT_ARRAY_BUFFER:
                    binding = GL_ELEMENT_ARRAY_BUFFER_BINDING;
                    break;
                case GL_SHADER_STORAGE_BUFFER:
                    binding = GL_SHADER_STORAGE_BUFFER_BINDING;
                    break;
            };

            CORE_ASSERT(binding != 0, "Unknown buffer type");

            int activeId;
            GL_CALL(glGetIntegerv(binding, &activeId));
            return activeId == id;
        }

    private:
        GLenum type, access;
        int itemCount, itemSize;
        unsigned int id;
    };

}