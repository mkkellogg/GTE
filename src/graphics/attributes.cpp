#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "material.h"
#include "shader/shader.h"
#include "attributes.h"
#include "ui/debug.h"

const char* const Attributes::attributeNames[] = {"POSITION","NORMAL","COLOR","UV1","UV2"};

const char * Attributes::GetAttributeName(Attribute attr)
{
	return attributeNames[(int)attr];
}

unsigned int Attributes::GetInvertAttributeMask(Attribute attr)
{
	unsigned int uAttr = (unsigned int)AttributeToAttributeMaskComponent(attr);
	unsigned int result = 0;
	unsigned int mask = 0x80000000;

	for(int i=0; i < 32; i++)
	{
		if(mask & uAttr)
		{
			result |= 1;
		}

		result <<= 1;
		mask >>= 1;
	}

	return result;
}

Attribute Attributes::AttributeMaskComponentToAttribute(AttributeMaskComponent component)
{
	int index=0x00000001;
	int count = 0;
	while(!((int)component & index) && count < 64)
	{
		index <<= 1;

		// prevent infinite loop if somehow 'mask' gets corrupted and is zero;
		count++;
	}

	if(count ==64)Debug::PrintError("Infinite loop in AttributeMaskToIndex()");

	return (Attribute)index;
}

AttributeMaskComponent Attributes::AttributeToAttributeMaskComponent(Attribute attr)
{
	return (AttributeMaskComponent)(2 ^ (unsigned int)attr);
}

void Attributes::AddAttribute(AttributeSet * set, Attribute attr)
{
	AttributeMaskComponent maskComponent = AttributeToAttributeMaskComponent(attr);
	unsigned int uPtr = (unsigned int)*set;
	uPtr |= (unsigned int)maskComponent;
	*set = (AttributeSet)uPtr;
}

void Attributes::RemoveAttribute(AttributeSet * set, Attribute attr)
{
	unsigned int uPtr = (unsigned int)*set;
	unsigned int mask = GetInvertAttributeMask(attr);

	uPtr &= mask;
	*set = (AttributeSet)uPtr;
}

bool Attributes::HasAttribute(AttributeSet set, Attribute attr)
{
	unsigned int u = (unsigned int)set;

	AttributeMaskComponent maskComponent = AttributeToAttributeMaskComponent(attr);
	unsigned int uMask = (unsigned int)maskComponent;
	return u & uMask;
}

AttributeSet Attributes::CreateAttributeSet()
{
	return (unsigned int)0;
}
