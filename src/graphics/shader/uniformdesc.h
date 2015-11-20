/*
 * class: UniformDescriptor
 *
 * author: Mark Kellogg
 *
 * Describes a uniform shader variable.
 *
 */

#ifndef _GTE_UNIFORM_DESCRIPTOR_H_
#define _GTE_UNIFORM_DESCRIPTOR_H_

#include <vector>
#include "geometry/matrix4x4.h"
#include "graphics/materialvardirectory.h"

#include <string>

namespace GTE
{
	//forward declarations
	class Texture;

	enum class UniformType
	{
		Sampler2D,
		SamplerCube,
		Matrix4x4,
		Matrix3x3,
		Matrix2x3,
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Bool2,
		Bool3,
		Bool4,
		Custom,
		Unknown
	};

	class UniformDescriptor
	{
	public:

		UniformDescriptor();
		~UniformDescriptor();

		Int32 Size;
		std::string Name;		
		UInt32 ShaderVarID;
		UInt32 SamplerUnitIndex;
		UniformType Type;

		Bool IsSet;
		Bool IsDelayedSet;
		UInt32 SetSize;
		UniformID RegisteredUniformID;
		TextureSharedPtr SamplerData;
		Matrix4x4 MatrixData;
		Real BasicFloatData[4];
		Real * ExtendedFloatData;
	};
}

#endif
