#ifndef _SHADER_MANAGER_H_
#define _SHADER_MANAGER_H_

#include "shader.h"

class ShaderManager
{
    public :

    Shader * CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath);
};

#endif
