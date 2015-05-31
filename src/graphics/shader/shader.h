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

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "shadersource.h"
#include <string>

namespace GTE
{
	// forward declarations
	class VertexAttrBuffer;
	class Matrix4x4;
	class Point3;
	class Vector3;
	class Color4;
	class Texture;
	class UniformDescriptor;
	class AttributeDescriptor;

	enum class ShaderType
	{
		Vertex,
		Fragment
	};

	class Shader : public EngineObject
	{
	protected:

		ShaderSource shaderSource;

	public:

		Shader(const ShaderSource& shaderSource);
		virtual ~Shader();
		virtual bool Load() = 0;
		virtual bool IsLoaded() = 0;
		virtual int GetAttributeVarID(const std::string& varName) const = 0;
		virtual int GetUniformVarID(const std::string& varName) const = 0;
		virtual void SendBufferToShader(int varID, VertexAttrBuffer * buffer) = 0;

		virtual void SendUniformToShader(int varID, UInt32 samplerUnitIndex, const TextureRef texture) = 0;
		virtual void SendUniformToShader(int varID, const Matrix4x4 * mat) = 0;
		virtual void SendUniformToShader(int varID, const Point3 * point) = 0;
		virtual void SendUniformToShader(int varID, const Vector3 * vector) = 0;
		virtual void SendUniformToShader(int varID, const Color4 * color) = 0;

		virtual void SendUniformToShader4v(int varID, const Real * data) = 0;
		virtual void SendUniformToShader3v(int varID, const Real * data) = 0;
		virtual void SendUniformToShader2v(int varID, const Real * data) = 0;
		virtual void SendUniformToShader4(int varID, Real x, Real y, Real z, Real w) = 0;
		virtual void SendUniformToShader3(int varID, Real x, Real y, Real z) = 0;
		virtual void SendUniformToShader2(int varID, Real x, Real y) = 0;
		virtual void SendUniformToShader(int varID, Real  data) = 0;

		virtual void SendUniformToShader(int varID, int  data) = 0;

		virtual UInt32 GetUniformCount() const = 0;
		virtual const UniformDescriptor * GetUniformDescriptor(UInt32 index) const = 0;

		virtual UInt32 GetAttributeCount() const = 0;
		virtual const AttributeDescriptor * GetAttributeDescriptor(UInt32 index) const = 0;
	};
}

#endif
