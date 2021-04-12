/*
 * Created by Kami-Kaze on 4/11/2021.
 */

#include "shader_list.h"

namespace Engine {

    void ShaderList::pushFromString(ShaderType type, const std::string& code) {
        shaders.push_back(Shader::fromString(type, code));
    }

    void ShaderList::pushFromFile(ShaderType type, const std::string& filePath) {
        shaders.push_back(Shader::fromFile(type, filePath));
    }

    void ShaderList::clear() {
        for (Shader* shader : shaders) {
            delete shader;
        }
        shaders.clear();
    }

    const std::vector<Shader*>& ShaderList::getShaders() const {
        return shaders;
    }

}