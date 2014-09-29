#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attributetransformer.h"
#include "graphics/stdattributes.h"

AttributeTransformer::AttributeTransformer(StandardAttributeSet attributes)
{
	activeAttributes = attributes;
}

AttributeTransformer::~AttributeTransformer()
{

}

StandardAttributeSet AttributeTransformer::GetActiveAttributes()
{
	return activeAttributes;
}
