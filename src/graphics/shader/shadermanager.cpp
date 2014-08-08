
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
 
#include "shader.h"
#include "shaderGL.h"
#include "shadermanager.h"
#include "shadersource.h"
#include "gte.h"
 
Shader * ShaderManager::CreateShader(const char * vertexShaderPath, const char * fragmentShaderPath)
{
    //TODO: Add switch for different platforms; for now only support OpenGL
    Shader * shader = new ShaderGL(vertexShaderPath, fragmentShaderPath);
    return shader;
}



