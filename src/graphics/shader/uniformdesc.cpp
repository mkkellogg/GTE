#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "uniformdesc.h"

UniformDescriptor::UniformDescriptor()
{
	ShaderVarID = -1;
	Type = UniformType::Float;
	ExtendedFloatData = NULL;
	SamplerData = NULL;
	MatrixData = NULL;

	memset(BasicFloatData, 0, sizeof(float) * 4);
}

UniformDescriptor::~UniformDescriptor()
{

}
