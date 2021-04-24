#pragma once

/*
 * Created by Kami-Kaze on 4/3/2021.
 */

#include <glad/glad.h>

#include "macros/assert.h"
#include "macros/gl_call.h"

namespace Engine {
    enum BufferType {
        ARRAY_BUFFER=GL_ARRAY_BUFFER,
        ELEMENT_ARRAY_BUFFER=GL_ELEMENT_ARRAY_BUFFER,
        SHADER_STORAGE_BUFFER=GL_SHADER_STORAGE_BUFFER,
    };
    enum BufferAccess {
        STATIC_DRAW=GL_STATIC_DRAW,
        DYNAMIC_DRAW=GL_DYNAMIC_DRAW,
    };

    template<class T>
    class Buffer {
    public:
        inline Buffer(BufferType type, BufferAccess accessMode, int itemCount, const void* data)
                : type(type), accessMode(accessMode), itemCount(itemCount), itemSize(sizeof(T)) {
            // create buffer
            GL_CALL(glCreateBuffers(1, &id));

            // buffer data
            //! todo: check if some other buffer was bound before and if so, rebind it after
            bind();
            GL_CALL(glBufferData(type, itemCount * itemSize, data, accessMode));
            unbind();
        }

        inline ~Buffer() {
            GL_CALL(glDeleteBuffers(1, &id));
        }

        inline unsigned int getId() const {
            return id;
        }

        inline void bind() const {
            GL_CALL(glBindBuffer(type, id));
        }
        inline void unbind() const {
            GL_CALL(glBindBuffer(type, 0));
        }

        inline bool bound() const {
            int binding = 0;
            switch (type) {
                case ARRAY_BUFFER:
                    binding = GL_ARRAY_BUFFER_BINDING;
                    break;
                case ELEMENT_ARRAY_BUFFER:
                    binding = GL_ELEMENT_ARRAY_BUFFER_BINDING;
                    break;
                case SHADER_STORAGE_BUFFER:
                    binding = GL_SHADER_STORAGE_BUFFER_BINDING;
                    break;
            };

            CORE_ASSERT(binding != 0, "Unknown buffer type");

            int activeId;
            GL_CALL(glGetIntegerv(binding, &activeId));
            return activeId == id;
        }

        inline void bindBase(const int index = 0) const {
            CORE_ASSERT(bound(), "Buffer not bound!");
            GL_CALL(glBindBufferBase(type, index, id));
        }

        inline void bufferData(const void* data) {
            CORE_ASSERT(bound(), "Buffer not bound!");
            GL_CALL(glBufferSubData(type, 0, itemCount * itemSize, data));
        }
        inline void bufferSubData(int count, int startIndex, const void* data) {
            CORE_ASSERT(bound(), "Buffer not bound!");
            GL_CALL(glBufferSubData(type, startIndex * itemCount, count * itemSize, data));
        }

        inline T* readData(T* out = nullptr) const {
            CORE_ASSERT(bound(), "Buffer not bound!");
            void* ptr = glMapBuffer(type, GL_READ_ONLY);
            if (ptr == nullptr) {
                LOG_CORE_ERROR("Mapping buffer for read failed!");
                return nullptr;
            }
            T* arr = out == nullptr ? new T[itemCount] : out;
            memcpy(arr, ptr, sizeof(T) * itemCount);
            bool unmapSuccess = glUnmapBuffer(type);
            ASSERT(unmapSuccess, "Unmapping buffer failed!");
            return arr;
        }

    private:
        BufferType type;
        BufferAccess accessMode;
        int itemCount, itemSize;
        unsigned int id;
    };

}