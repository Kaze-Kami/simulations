/*
 * Created by Kami-Kaze on 3/28/2021.
 */

#include "shader_program.h"
#include "macros/gl_call.h"

namespace Engine {

    ShaderProgram::ShaderProgram(GLuint id) : id(id) {}

    ShaderProgram::~ShaderProgram() {
        GL_CALL(glDeleteProgram(id));
    }

    void ShaderProgram::bind() const {
        GL_CALL(glUseProgram(id));
    }

    void ShaderProgram::unbind() const {
        GL_CALL(glUseProgram(0));
    }

    int ShaderProgram::getUniformLocation(std::string name) const {
        return glGetUniformLocation(id, name.c_str());
    }

    ShaderProgram *ShaderProgram::fromShaderList(std::vector<Shader*>& shaders) {
        GLuint id = linkShaderProgram(shaders);
        return new ShaderProgram(id);
    }

    GLuint ShaderProgram::linkShaderProgram(std::vector<Shader*>& shaders) {
        GLuint program = glCreateProgram();

        for (Shader* shader : shaders) {
            GL_CALL(glAttachShader(program, shader->id));
        }

        GL_CALL(glLinkProgram(program));

        for (Shader* shader : shaders) {
            GL_CALL(glDetachShader(program, shader->id));
        }

        GLuint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));

            std::vector<GLchar> infoLog(maxLength);
            GL_CALL(glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]));

            GL_CALL(glDeleteProgram(program));

            LOG_CORE_ERROR("Linking shader program failed:");
            LOG_CORE_ERROR("    {0}", infoLog.data());
            return 0;
        }

        return program;
    }

}