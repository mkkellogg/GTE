/*
 * class: Shader
 *
 * Author: Mark Kellogg
 *
 * This class is the base class for host-side shader descriptors. Platform specific shader
 * descriptors should derive from this one one (e.g. ShaderGL for OpenGL). This is only
 * a descriptor; it is not meant to contain any shader code. It is only meant to hold
 * information about a shader.
 *
 * An instance of a Shader object actually means the combination of a vertex and fragment
 * shader. A Shader is not complete unless it has both of those components.
 */

#ifndef _GTE_SHADER_H_
#define _GTE_SHADER_H_

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
#include "object/enginetypes.h"
#include <string>

enum class ShaderType
{
    Vertex,
    Fragment
};

class Shader : public EngineObject
{
    protected: 

    public :

    Shader(const std::string& vertexSourcePath, const std::string& fragmentSourcePath);
    virtual ~Shader();
    virtual bool Load() = 0;
    virtual bool IsLoaded() = 0;
    virtual int GetAttributeVarID(const std::string& varName) const = 0;
    virtual int GetUniformVarID(const std::string& varName) const = 0;
    virtual void SendBufferToShader(int varID, VertexAttrBuffer * buffer) = 0;

    virtual void SendUniformToShader(unsigned int samplerUnitIndex, const TextureRef texture) = 0;
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

    virtual void SendUniformToShader(int varID, int  data) = 0;

    virtual unsigned int GetUniformCount() const = 0;
    virtual const UniformDescriptor * GetUniformDescriptor(unsigned int index) const = 0;

    virtual unsigned int GetAttributeCount() const = 0;
    virtual const AttributeDescriptor * GetAttributeDescriptor(unsigned int index) const = 0;
};

#endif
