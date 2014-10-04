#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attributetransformer.h"
#include "graphics/stdattributes.h"

AttributeTransformer::AttributeTransformer(StandardAttributeSet attributes)
{
	activeAttributes = attributes;
}

AttributeTransformer::AttributeTransformer()
{
	activeAttributes = StandardAttributes::CreateAttributeSet();
}

AttributeTransformer::~AttributeTransformer()
{

}

void AttributeTransformer::SetActiveAttributes (StandardAttributeSet attributes)
{
	activeAttributes = attributes;
}

StandardAttributeSet AttributeTransformer::GetActiveAttributes()
{
	return activeAttributes;
}
