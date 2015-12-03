#include "attributedesc.h"
#include "graphics/materialvardirectory.h"

namespace GTE
{
	AttributeDescriptor::AttributeDescriptor()
	{
		ShaderVarID = -1;		
		Size = -1;		
		Type = AttributeType::Unknown;

		FloatData = nullptr;
		IsSet = false;
		SetSize = 0;
		RegisteredAttributeID = AttributeDirectory::VarID_Invalid;
		RequiresVerification = true;
	}

	AttributeDescriptor::~AttributeDescriptor()
	{

	}
}
