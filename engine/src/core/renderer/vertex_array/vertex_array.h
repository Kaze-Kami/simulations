#pragma once

/*
 * Created by Kami-Kaze on 4/4/2021.
 *
 * todo: ? move implementations to .cpp
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
        inline void addBuffer(Buffer<T>* buffer, VertexBufferLayout& layout) {
            CORE_ASSERT(bound(), "Vertex array must be bound to add buffers!");
            CORE_ASSERT(buffer->bound(), "Buffer must be bound to be added to vertex array!");

            const auto& attributes = layout.getAttributes();
            unsigned int offset = 0;
            for (unsigned int i = 0; i < attributes.size(); i++) {
                const auto& attribute = attributes[i];
                GL_CALL(glEnableVertexArrayAttrib(id, i));
#pragma warning( disable : 4312 )
                GL_CALL(glVertexAttribPointer(i, attribute.count, attribute.type, attribute.normalize, layout.getStride(), (const void*) offset));
#pragma warning( pop )
                offset += attribute.count * attribute.size;
            }
        }

        inline void bind() const {
            GL_CALL(glBindVertexArray(id));
        }

        inline void unbind() const {
            GL_CALL(glBindVertexArray(0));
        }

        inline bool bound() const {
            int activeId;
            GL_CALL(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &activeId));
            return id == activeId;
        }

    private:
        unsigned int id;
    };

}