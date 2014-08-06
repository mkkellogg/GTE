#ifndef _SHADER_H_
#define _SHADER_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "shadersource.h"

enum class ShaderType
{
    Vertex,
    Fragment
};

class Shader
{
    bool ready;
    char * name;
    ShaderSource * vertexShaderSource;
    ShaderSource * fragmentShaderSource;

    GLuint programID;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;

    void DestroyShaders();
    void DestroyProgram();
    void DestroyComponents();

    char * GetShaderLog(GLuint obj);
    char * GetProgramLog(GLuint obj);
    bool CheckCompilation(int shaderID, ShaderType shaderType);

    public :

    Shader(const char * vertexSourcePath, const char * fragmentSourcePath);
    ~Shader();
    bool Load();
};

#endif
