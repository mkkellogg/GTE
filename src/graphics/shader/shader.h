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

#include "engine.h"
#include "object/engineobject.h"
#include "shadersource.h"

#include <string>

namespace GTE {
    // forward declarations
    class VertexAttrBuffer;
    class Matrix4x4;
    class Point3;
    class Vector3;
    class Color4;
    class Texture;
    class UniformDescriptor;
    class AttributeDescriptor;

    enum class ShaderType {
        Vertex,
        Fragment
    };

    class Shader : public EngineObject {
    protected:

        ShaderSource shaderSource;

    public:

        Shader(const ShaderSource& shaderSource);
        virtual ~Shader();
        virtual Bool Load() = 0;
        virtual Bool IsLoaded() const = 0;
        virtual Int32 GetAttributeVarID(const std::string& varName) const = 0;
        virtual Int32 GetUniformVarID(const std::string& varName) const = 0;
        virtual void SendBufferToShader(Int32 varID, const VertexAttrBuffer * buffer) = 0;

        virtual void SendUniformToShader(Int32 varID, UInt32 samplerUnitIndex, const TextureSharedPtr texture) = 0;
        virtual void SendUniformToShader(Int32 varID, const Matrix4x4& mat) = 0;

        virtual void SendUniformToShader(Int32 varID, Real x, Real y, Real z, Real w) = 0;
        virtual void SendUniformToShader(Int32 varID, Real x, Real y, Real z) = 0;
        virtual void SendUniformToShader(Int32 varID, Real x, Real y) = 0;
        virtual void SendUniformToShader(Int32 varID, Real  data) = 0;
        virtual void SendUniformToShader(Int32 varID, Int32  data) = 0;

        virtual void SendUniformToShader4FV(Int32 varID, const Real * data, UInt32 count) = 0;
        virtual void SendUniformToShader3FV(Int32 varID, const Real * data, UInt32 count) = 0;
        virtual void SendUniformToShader2FV(Int32 varID, const Real * data, UInt32 count) = 0;
        virtual void SendUniformToShader1FV(Int32 varID, const Real * data, UInt32 count) = 0;
        virtual void SendUniformToShader4IV(Int32 varID, const Int32 * data, UInt32 count) = 0;
        virtual void SendUniformToShader3IV(Int32 varID, const Int32 * data, UInt32 count) = 0;
        virtual void SendUniformToShader2IV(Int32 varID, const Int32 * data, UInt32 count) = 0;
        virtual void SendUniformToShader1IV(Int32 varID, const Int32 * data, UInt32 count) = 0;
        virtual void SendUniformToShaderM4x4V(Int32 varID, const Matrix4x4 * mat, UInt32 count) = 0;

        virtual UInt32 GetUniformCount() const = 0;
        virtual const UniformDescriptor * GetUniformDescriptor(UInt32 index) const = 0;

        virtual UInt32 GetAttributeCount() const = 0;
        virtual const AttributeDescriptor * GetAttributeDescriptor(UInt32 index) const = 0;
    };
}

#endif
