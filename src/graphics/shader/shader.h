
#ifndef _SHADER_H_
#define _SHADER_H_

// forward declarations
class VertexAttrBuffer;
class Matrix4x4;
class Point3;
class Vector3;
class Color4;
class Texture;
class UniformDescriptor;
class AttributeDescriptor;

#include "object/engineobject.h"

enum class ShaderType
{
    Vertex,
    Fragment
};

class Shader : EngineObject
{
    protected: 

    public :

    Shader(const char * vertexSourcePath, const char * fragmentSourcePath);
    virtual ~Shader();
    virtual bool Load() = 0;
    virtual bool IsLoaded() = 0;
    virtual int GetAttributeVarID(const char *varName) const = 0;
    virtual int GetUniformVarID(const char *varName) const = 0;
    virtual void SendBufferToShader(int varID, VertexAttrBuffer * buffer) = 0;

    virtual void SendUniformToShader(int varID, const Texture * texture) = 0;
    virtual void SendUniformToShader(int varID, const Matrix4x4 * mat) = 0;
    virtual void SendUniformToShader(int varID, const Point3 * point) = 0;
    virtual void SendUniformToShader(int varID, const Vector3 * vector) = 0;
    virtual void SendUniformToShader(int varID, const Color4 * color) = 0;

    virtual void SendUniformToShader4v(int varID, const float * data) = 0;
    virtual void SendUniformToShader3v(int varID, const float * data) = 0;
    virtual void SendUniformToShader2v(int varID, const float * data) = 0;
    virtual void SendUniformToShader4(int varID, float x, float y, float z, float w) = 0;
    virtual void SendUniformToShader3(int varID, float x, float y, float z) = 0;
    virtual void SendUniformToShader2(int varID, float x, float y) = 0;
    virtual void SendUniformToShader(int varID, float  data) = 0;

    virtual unsigned int GetUniformCount() const = 0;
    virtual const UniformDescriptor * GetUniformDescriptor(unsigned int index) const = 0;

    virtual unsigned int GetAttributeCount() const = 0;
    virtual const AttributeDescriptor * GetAttributeDescriptor(unsigned int index) const = 0;
};

#endif
