#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "material.h"
#include "shader/shader.h"
#include "attributes.h"

const char* const Attributes::attributeNames[] = {"POSITION","NORMAL","COLOR","UV1","UV2"};

const char * Attributes::GetAttributeName(Attribute attr)
{
	return attributeNames[(int)attr];
}

int Attributes::AttributeMaskToIndex(AttributeMask mask)
{
	int index=0x00000001;
	while(!((int)mask & index))index <<= 1;
	return index;
}
