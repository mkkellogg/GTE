/*
 * class: AttributeDescriptor
 *
 * author: Mark Kellogg
 *
 * Describes an attribute shader variable.
 *
 */

#ifndef _GTE_ATTRIBUTE_DESCRIPTOR_H_
#define _GTE_ATTRIBUTE_DESCRIPTOR_H_

//forward declarations
class Texture;
class Matrix4x4;


enum class AttributeType
{
	Matrix4x4,
	Matrix3x3,
	Matrix2x3,
	Float,
	Float2,
	Float3,
	Float4,
	Unknown
};

class AttributeDescriptor
{
	public:

	AttributeDescriptor();
	~AttributeDescriptor();

	int Size;
	bool IsSet;
	AttributeType Type;
	unsigned int ShaderVarID;
	char Name [128];
	float * FloatData;
};

#endif
