/*
 * Created by Kami-Kaze on 3/28/2021.
 */

#include "shader_program.h"

#include <iostream>

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
        GL_CALL(int location = glGetUniformLocation(id, name.c_str()));
        return location;
    }

    ShaderProgram *ShaderProgram::fromShaderList(const ShaderList& shaderList) {
        GLuint id = linkShaderProgram(shaderList);
        if (id) {
            return new ShaderProgram(id);
        }

        LOG_CORE_ERROR("Failed to link shader program!");
        return nullptr;
    }

    GLuint ShaderProgram::linkShaderProgram(const ShaderList& shaderList) {
        GLuint program = glCreateProgram();

        for (Shader* shader : shaderList.getShaders()) {
            GL_CALL(glAttachShader(program, shader->id));
        }

        GL_CALL(glLinkProgram(program));

        for (Shader* shader : shaderList.getShaders()) {
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

            LOG_CORE_ERROR("Linking shader program failed: ", infoLog.data());
            std::cout << infoLog.data() << std::endl;
            return 0;
        }

        return program;
    }

}