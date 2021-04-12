/*
 * Created by Kami-Kaze on 3/28/2021.
 */


#include "shader.h"

#include <fstream>
#include <iostream>

#include "macros/gl_call.h"

namespace Engine {

    Shader::Shader(ShaderType type, GLint id) : type(type), id(id) {}

    Shader::~Shader() {
        GL_CALL(glDeleteShader(id));
    }

    Shader* Shader::fromString(ShaderType type, const std::string& source) {
        GLuint id = compileShader(type, source);
        if (id) {
            return new Shader(type, id);
        }
        LOG_CORE_ERROR("Failed to compile shader {:x}", type);
        return nullptr;
    }

    GLuint Shader::compileShader(GLenum type, const std::string& source) {
        GLuint shader = glCreateShader(type);

        const GLchar* sourceCStr = source.c_str();
        GL_CALL(glShaderSource(shader, 1, &sourceCStr, nullptr));

        GL_CALL(glCompileShader(shader));

        GLint isCompiled = 0;
        GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled));
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            GL_CALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength));

            std::vector<GLchar> infoLog(maxLength);
            GL_CALL(glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]));

            GL_CALL(glDeleteShader(shader));
            LOG_CORE_ERROR("Compiling shader failed: ", infoLog.data());
            std::cout << infoLog.data() << std::endl;

            // failed
            return 0;
        }

        return shader;
    }

    static std::string readFileAsString(const std::string& filepath) {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            result.resize((size_t)in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&result[0], result.size());
            in.close();
        }
        else
        {
            LOG_CORE_ERROR("Could not open file '{}'", filepath);
        }

        return result;
    }

    Shader* Shader::fromFile(ShaderType type, const std::string& filePath) {
        return Shader::fromString(type, readFileAsString(filePath));
    }

}