/*
 * Created by Kami-Kaze on 4/15/2021.
 */

#include "shader_program.h"

namespace Engine {

    ShaderProgram::ShaderProgram(const unsigned int id)  : id(id) {}

    void ShaderProgram::use() const {
        GL_CALL(glUseProgram(id));
    }

    bool ShaderProgram::isBound(const unsigned int id) {
        int active = -1;
        GL_CALL(glGetIntegerv(GL_CURRENT_PROGRAM, &active));
        return active == id;
    }

    int ShaderProgram::getUniformLocation(const std::string& name) {
        auto it = uniformLocations.find(name);
        if (it == uniformLocations.end()) {
            // not registered -> fetch and cache
            GL_CALL(int location = glGetUniformLocation(id, name.c_str()));
            uniformLocations.insert(std::make_pair(name, location));

            return location;
        }

        // registered -> return location
        return it->second;
    }

    ShaderProgram* ShaderProgram::createProgram(const ShaderList& shaderList) {
        const unsigned int id = linkProgram(shaderList);
        if (id) {
            return new ShaderProgram(id);
        }

        LOG_CORE_ERROR("Failed to link shader program!");
        return nullptr;
    }

    unsigned int ShaderProgram::linkProgram(const ShaderList& shaderList) {
        GL_CALL(unsigned int program = glCreateProgram());

        for (Shader* shader : shaderList.getShaders()) {
            GL_CALL(glAttachShader(program, shader->id));
        }

        GL_CALL(glLinkProgram(program));

        for (Shader* shader : shaderList.getShaders()) {
            GL_CALL(glDetachShader(program, shader->id));
        }

        int isLinked = 0;
        GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &isLinked));
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