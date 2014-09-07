#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "attributedesc.h"

AttributeDescriptor::AttributeDescriptor()
{
	ShaderVarID = -1;
	FloatData = NULL;
	Size = -1;
	IsSet = false;
	Type = AttributeType::Unknown;
}

AttributeDescriptor::~AttributeDescriptor()
{

}
