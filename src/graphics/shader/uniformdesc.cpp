#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "uniformdesc.h"

namespace GTE
{
	UniformDescriptor::UniformDescriptor()
	{
		SamplerUnitIndex = 0;
		ShaderVarID = -1;		
		Size = -1;
		Type = UniformType::Float;

		RegisteredUniformID = -1;
		ExtendedFloatData = nullptr;
		IsSet = false;
		SetSize = 0;

		memset(BasicFloatData, 0, sizeof(Real) * 4);
	}

	UniformDescriptor::~UniformDescriptor()
	{

	}
}
