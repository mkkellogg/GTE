#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "attributedesc.h"

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
	}

	AttributeDescriptor::~AttributeDescriptor()
	{

	}
}
