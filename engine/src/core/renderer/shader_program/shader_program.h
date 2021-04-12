#pragma once

/*
 * Created by Kami-Kaze on 3/28/2021.
 */
#include "shader_list.h"

namespace Engine {

    class ShaderProgram {
    public:
        ~ShaderProgram();
        void bind() const;
        void unbind() const;

        int getUniformLocation(std::string name) const;

        static ShaderProgram* fromShaderList(const ShaderList& shaderList);
    private:
        explicit ShaderProgram(GLuint id);

        GLuint id;

        static GLuint linkShaderProgram(const ShaderList& shaderList);
    };

}
