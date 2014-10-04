#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "uniformdesc.h"

UniformDescriptor::UniformDescriptor()
{
	SamplerUnitIndex = 0;
	ShaderVarID = -1;
	Size = -1;
	Type = UniformType::Float;
	ExtendedFloatData = NULL;
	MatrixData = NULL;
	IsSet = false;
	memset(BasicFloatData, 0, sizeof(float) * 4);
}

UniformDescriptor::~UniformDescriptor()
{

}
