#pragma once

/*
 * Created by Kami-Kaze on 4/11/2021.
 */

#include <vector>
#include <string>

#include "shader.h"

namespace Engine {

    class ShaderList {
    public:
        void pushFromString(ShaderType type, const std::string& code);
        void pushFromFile(ShaderType type, const std::string& filePath);

        void clear();

        const std::vector<Shader*>& getShaders() const;
    private:
        std::vector<Shader*> shaders;
    };

}



