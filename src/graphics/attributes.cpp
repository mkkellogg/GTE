#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "attributes.h"
#include "ui/debug.h"

const char* const Attributes::attributeNames[] = {"POSITION","NORMAL","COLOR","UV1","UV2"};

const char * Attributes::GetAttributeName(Attribute attr)
{
	return attributeNames[(int)attr];
}

Attribute Attributes::AttributeMaskComponentToAttribute(AttributeMaskComponent component)
{
	return (Attribute)IntMask::MaskValueToIndex((unsigned int)component);
}

AttributeMaskComponent Attributes::AttributeToAttributeMaskComponent(Attribute attr)
{
	return (AttributeMaskComponent)IntMask::IndexToMaskValue((unsigned int)attr);
}

void Attributes::AddAttribute(AttributeSet * set, Attribute attr)
{
	IntMask::SetBitForIndexMask((unsigned int *)set, (unsigned int )attr);
}

void Attributes::RemoveAttribute(AttributeSet * set, Attribute attr)
{
	IntMask::ClearBitForIndexMask((unsigned int *)set, (unsigned int )attr);
}

bool Attributes::HasAttribute(AttributeSet set, Attribute attr)
{
	return IntMask::IsBitSet((unsigned int)set, (unsigned int)attr);
}

AttributeSet Attributes::CreateAttributeSet()
{
	return (unsigned int)0;
}
