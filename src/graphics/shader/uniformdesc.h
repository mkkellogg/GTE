/*
 * class: UniformDescriptor
 *
 * author: Mark Kellogg
 *
 * Describes a uniform shader variable.
 *
 */

#ifndef _UNIFORM_DESCRIPTOR_H_
#define _UNIFORM_DESCRIPTOR_H_

//forward declarations
class Texture;
class Matrix4x4;

#include "object/enginetypes.h"

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
	char Name [128];
	unsigned int ShaderVarID;
	unsigned int SamplerUnitIndex;
	UniformType Type;
	bool IsSet;

	TextureRef SamplerData;
	Matrix4x4 * MatrixData;
	float BasicFloatData[4];
	float * ExtendedFloatData;
};

#endif
