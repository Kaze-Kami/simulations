#pragma once

/*
 * Created by Kami-Kaze on 4/15/2021.
 */

#include <iostream>

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_list.h"
#include "macros/gl_call.h"
#include "macros/assert.h"

namespace Engine {

    template<typename T>
    struct Uniform {
        std::string name;
        T data;
    };

    class ShaderProgram {
    public:
        void use() const;

        template<typename T>
        inline void uploadUniform(const Uniform<T>& uniform) {
            CORE_ASSERT(isBound(id), "Shader needs to be bound to upload uniforms!");
            uploadUniformImpl(getUniformLocation(uniform.name), uniform);
        }

        static ShaderProgram* createProgram(const ShaderList& shaderList);

    private:
        const unsigned int id;
        std::unordered_map<std::string, int> uniformLocations;

        explicit ShaderProgram(const unsigned int id);

        static bool isBound(const unsigned int id);

        int getUniformLocation(const std::string& name);

        static unsigned int linkProgram(const ShaderList& shaderList);

        template<typename T>
        inline void uploadUniformImpl(const int, const Uniform<T>&) {
            CORE_ASSERT(false, "Call to non-specialized template function!");
        }

        /**
         * Implementations for uploading Uniform<T>
         */

        template<>
        inline void uploadUniformImpl<glm::mat4>(const int location, const Uniform<glm::mat4>& u) {
            GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(u.data)));
        }

        template<>
        inline void uploadUniformImpl<glm::vec4>(const int location, const Uniform<glm::vec4>& u) {
            GL_CALL(glUniform4fv(location, 1, glm::value_ptr(u.data)));
        }

        template<>
        inline void uploadUniformImpl<glm::vec3>(const int location, const Uniform<glm::vec3>& u) {
            GL_CALL(glUniform3fv(location, 1, glm::value_ptr(u.data)));
        }

        template<>
        inline void uploadUniformImpl<glm::vec2>(const int location, const Uniform<glm::vec2>& u) {
            GL_CALL(glUniform2fv(location, 1, glm::value_ptr(u.data)));
        }

        template<>
        inline void uploadUniformImpl<float>(const int location, const Uniform<float>& u) {
            GL_CALL(glUniform1f(location, u.data));
        }
    };

}