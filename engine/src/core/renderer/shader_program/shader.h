#pragma once

/*
 * Created by Kami-Kaze on 3/28/2021.
 */

#include <glad/glad.h>
#include <string>
#include <vector>

namespace Engine {

    enum ShaderType {
        VERTEX=GL_VERTEX_SHADER, GEOMETRY=GL_GEOMETRY_SHADER, FRAGMENT=GL_FRAGMENT_SHADER, COMPUTE=GL_COMPUTE_SHADER,
    };

    class Shader {
    public:
        ~Shader();

        const ShaderType type;
        const GLuint id;

        static Shader* fromString(ShaderType type, const std::string& code);
        static Shader* fromFile(ShaderType type, const std::string& filePath);

    private:
        Shader(ShaderType type, GLint id);

        static GLuint compileShader(GLenum type, const std::string& source);

    };

}