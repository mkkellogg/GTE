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

enum class UniformType
{
	Sampler,
	Matrix4x4,
	Float,
	Float2,
	Float3,
	Float4,
	FloatArray,
	Custom
};

class UniformDescriptor
{
	public:

	UniformDescriptor();
	~UniformDescriptor();

	unsigned int ShaderVarID;
	UniformType Type;
	Texture * SamplerData;
	Matrix4x4 * MatrixData;
	float BasicFloatData[4];
	float * ExtendedFloatData;
};

#endif
