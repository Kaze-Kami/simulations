#pragma once

/*
 * Created by Kami-Kaze on 4/4/2021.
 */

#include <glad/glad.h>

#include "macros/gl_call.h"
#include "vertex_array_layout.h"


namespace Engine {

    class VertexArray {
    public:
        inline VertexArray() {
            GL_CALL(glGenVertexArrays(1, &id));
        }
        inline ~VertexArray() {
            GL_CALL(glDeleteVertexArrays(1, &id));
        }

        template<class T>
        inline void addBuffer(VertexBuffer<T>* buffer, VertexBufferLayout& layout) {
            CORE_ASSERT(bound(), "");
            CORE_ASSERT(buffer->bound(), "");

            const auto& attributes = layout.getAttributes();
            unsigned int offset = 0;
            for (unsigned int i = 0; i < attributes.size(); i++) {
                const auto& attribute = attributes[i];
                GL_CALL(glEnableVertexArrayAttrib(id, i));
                GL_CALL(glVertexAttribPointer(i, attribute.count, attribute.type, attribute.normalize, layout.getStride(), (const void*) offset));
                offset += attribute.count * attribute.size;
            }
        }

        inline void bind() {
            GL_CALL(glBindVertexArray(id));
        }

        inline void unbind() {
            GL_CALL(glBindVertexArray(0));
        }

        inline bool bound() {
            int activeId;
            GL_CALL(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &activeId));
            return id == activeId;
        }

    private:
        unsigned int id;
    };

}