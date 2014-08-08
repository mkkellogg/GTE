#ifndef _SHADER_GL_H_
#define _SHADER_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "shadersource.h"
#include "shader.h"

class ShaderGL : public Shader
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

    ShaderGL(const char * vertexSourcePath, const char * fragmentSourcePath);
    virtual ~ShaderGL();
    bool Load();
};

#endif
