#ifndef _SHADER_GL_H_
#define _SHADER_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

//forward declarations
class ShaderSource;
class GraphicsGL;

#include "shader.h"

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

    void SendUniformToShader(int loc, const Matrix4x4 * mat);
    void SendUniformToShader(int loc, const Point3 * point);
    void SendUniformToShader(int loc, const Vector3 * vector);
    void SendUniformToShader(int loc, const Color4 * color);

    void SendUniformToShader4v(int loc, const float * data);
    void SendUniformToShader3v(int loc, const float * data);
    void SendUniformToShader2v(int loc, const float * data);
    void SendUniformToShader4(int loc, float x, float y, float z, float w);
    void SendUniformToShader3(int loc, float x, float y, float z);
    void SendUniformToShader2(int loc, float x, float y);
    void SendUniformToShader(int loc, float  data);
};

#endif
