#ifndef _SHADER_GL_H_
#define _SHADER_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "shadersource.h"
#include "shader.h"
#include "graphics/vertexattrbuffer.h"

class ShaderGL : public Shader
{
    friend class GraphicsGL;

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

    protected:

    ShaderGL(const char * vertexSourcePath, const char * fragmentSourcePath);
    virtual ~ShaderGL();

    public :

    bool Load();
    int GetAttributeVarLocation(const char *varName) const;
    int GetUniformVarLocation(const char *varName) const;
    GLuint GetProgramID();
    void SendBufferToShader(int loc, VertexAttrBuffer * buffer);
};

#endif
