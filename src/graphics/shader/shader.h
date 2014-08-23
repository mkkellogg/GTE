
#ifndef _SHADER_H_
#define _SHADER_H_

// forward declarations
class VertexAttrBuffer;
class Matrix4x4;
class Point3;
class Vector3;
class Color4;

enum class ShaderType
{
    Vertex,
    Fragment
};

class Shader
{
    protected: 

    public :

    Shader(const char * vertexSourcePath, const char * fragmentSourcePath);
    virtual ~Shader();
    virtual bool Load() = 0;
    virtual int GetAttributeVarLocation(const char *varName) const = 0;
    virtual int GetUniformVarLocation(const char *varName) const = 0;
    virtual void SendBufferToShader(int loc, VertexAttrBuffer * buffer) = 0;

    virtual void SendUniformToShader(int loc, const Matrix4x4 * mat) = 0;
    virtual void SendUniformToShader(int loc, const Point3 * point) = 0;
    virtual void SendUniformToShader(int loc, const Vector3 * vector) = 0;
    virtual void SendUniformToShader(int loc, const Color4 * color) = 0;

    virtual void SendUniformToShader4v(int loc, const float * data) = 0;
    virtual void SendUniformToShader3v(int loc, const float * data) = 0;
    virtual void SendUniformToShader2v(int loc, const float * data) = 0;
    virtual void SendUniformToShader4(int loc, float x, float y, float z, float w) = 0;
    virtual void SendUniformToShader3(int loc, float x, float y, float z) = 0;
    virtual void SendUniformToShader2(int loc, float x, float y) = 0;
    virtual void SendUniformToShader(int loc, float  data) = 0;
};

#endif
