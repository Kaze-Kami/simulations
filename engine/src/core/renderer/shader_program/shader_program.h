#pragma once

/*
 * Created by Kami-Kaze on 3/28/2021.
 */
#include "shader.h"

namespace Engine {

    class ShaderProgram {
    public:
        ~ShaderProgram();
        void bind() const;
        void unbind() const;

        int getUniformLocation(std::string name) const;

        static ShaderProgram* fromShaderList(std::vector<Shader*>& shaders);
    private:
        explicit ShaderProgram(GLuint id);

        GLuint id;

        static GLuint linkShaderProgram(std::vector<Shader*>& shaders);
    };

}
