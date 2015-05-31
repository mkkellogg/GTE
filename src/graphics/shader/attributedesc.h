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

#include "object/enginetypes.h"

namespace GTE
{
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

		Int32 Size;
		bool IsSet;
		AttributeType Type;
		UInt32 ShaderVarID;
		char Name[128];
		Real * FloatData;
	};
}

#endif
