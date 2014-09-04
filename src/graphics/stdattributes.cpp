#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "stdattributes.h"
#include "ui/debug.h"

const char* const StandardAttributes::attributeNames[] = {"POSITION","NORMAL","COLOR","UV1","UV2"};

const char * StandardAttributes::GetAttributeName(StandardAttribute attr)
{
	return attributeNames[(int)attr];
}

StandardAttribute StandardAttributes::AttributeMaskComponentToAttribute(StandardAttributeMaskComponent component)
{
	return (StandardAttribute)IntMask::MaskValueToIndex((unsigned int)component);
}

StandardAttributeMaskComponent StandardAttributes::AttributeToAttributeMaskComponent(StandardAttribute attr)
{
	return (StandardAttributeMaskComponent)IntMask::IndexToMaskValue((unsigned int)attr);
}

void StandardAttributes::AddAttribute(StandardAttributeSet * set, StandardAttribute attr)
{
	IntMask::SetBitForIndexMask((unsigned int *)set, (unsigned int )attr);
}

void StandardAttributes::RemoveAttribute(StandardAttributeSet * set, StandardAttribute attr)
{
	IntMask::ClearBitForIndexMask((unsigned int *)set, (unsigned int )attr);
}

bool StandardAttributes::HasAttribute(StandardAttributeSet set, StandardAttribute attr)
{
	return IntMask::IsBitSet((unsigned int)set, (unsigned int)attr);
}

StandardAttributeSet StandardAttributes::CreateAttributeSet()
{
	return (unsigned int)0;
}
