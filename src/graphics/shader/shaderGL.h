#ifndef _SHADER_GL_H_
#define _SHADER_GL_H_

#include <GL/glew.h>
#include <GL/glut.h>

//forward declarations
class ShaderSource;
class GraphicsGL;
class Texture;
class AttributeDescriptor;
class UniformDescriptor;

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

    unsigned int attributeCount;
    unsigned int uniformCount;
    AttributeDescriptor ** attributes;
    UniformDescriptor ** uniforms;

    void DestroyShaders();
    void DestroyProgram();
    void DestroyComponents();
    void DestroyUniformAndAttributeInfo();

    char * GetShaderLog(GLuint obj);
    char * GetProgramLog(GLuint obj);
    bool CheckCompilation(int shaderID, ShaderType shaderType);

    bool StoreUniformAndAttributeInfo();

    protected:

    ShaderGL(const char * vertexSourcePath, const char * fragmentSourcePath);
    virtual ~ShaderGL();

    public :

    bool Load();
    bool IsLoaded();
    int GetAttributeVarID(const char *varName) const;
    int GetUniformVarID(const char *varName) const;
    GLuint GetProgramID();

    void SendBufferToShader(int varID, VertexAttrBuffer * buffer);

    void SendUniformToShader(int varID, const Texture * texture);
    void SendUniformToShader(int varID, const Matrix4x4 * mat);
    void SendUniformToShader(int varID, const Point3 * point);
    void SendUniformToShader(int varID, const Vector3 * vector);
    void SendUniformToShader(int varID, const Color4 * color);

    void SendUniformToShader4v(int varID, const float * data);
    void SendUniformToShader3v(int varID, const float * data);
    void SendUniformToShader2v(int varID, const float * data);
    void SendUniformToShader4(int varID, float x, float y, float z, float w);
    void SendUniformToShader3(int varID, float x, float y, float z);
    void SendUniformToShader2(int varID, float x, float y);
    void SendUniformToShader(int varID, float  data);

    unsigned int GetUniformCount() const;
    const UniformDescriptor * GetUniformDescriptor(unsigned int index) const;

    unsigned int GetAttributeCount() const;
    const AttributeDescriptor * GetAttributeDescriptor(unsigned int index) const;
};

#endif
