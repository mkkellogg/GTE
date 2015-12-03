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

#include "engine.h"
#include "graphics/materialvardirectory.h"

#include <string>

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
		AttributeType Type;
		UInt32 ShaderVarID;
		std::string Name;

		AttributeID RegisteredAttributeID;
		Bool IsSet;
		UInt32 SetSize;
		Real * FloatData;
		Bool RequiresVerification;
	};
}

#endif
