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

#include "object/enginetypes.h"
#include "geometry/matrix4x4.h"

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

		int Size;
		char Name[128];
		UInt32 ShaderVarID;
		UInt32 SamplerUnitIndex;
		UniformType Type;
		bool IsSet;

		TextureRef SamplerData;
		Matrix4x4 MatrixData;
		Real BasicFloatData[4];
		Real * ExtendedFloatData;
	};
}

#endif
