#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "stdattributes.h"
#include "debug/gtedebug.h"

const char* const StandardAttributes::attributeNames[] = {"POSITION", "SHADOW_POSITION", "NORMAL","TANGENT","COLOR","UVTEXTURE0","UVTEXTURE1", "UVNORMALMAP"};

const char * StandardAttributes::GetAttributeName(StandardAttribute attr)
{
	return attributeNames[(IntMask)attr];
}

StandardAttribute StandardAttributes::AttributeMaskComponentToAttribute(StandardAttributeMaskComponent component)
{
	return (StandardAttribute)IntMaskUtil::MaskValueToIndex((IntMask)component);
}

StandardAttributeMaskComponent StandardAttributes::AttributeToAttributeMaskComponent(StandardAttribute attr)
{
	return (StandardAttributeMaskComponent)IntMaskUtil::IndexToMaskValue((IntMask)attr);
}

void StandardAttributes::AddAttribute(StandardAttributeSet * set, StandardAttribute attr)
{
	IntMaskUtil::SetBit((IntMask *)set, (IntMask )attr);
}

void StandardAttributes::RemoveAttribute(StandardAttributeSet * set, StandardAttribute attr)
{
	IntMaskUtil::ClearBit((IntMask *)set, (IntMask )attr);
}

bool StandardAttributes::HasAttribute(StandardAttributeSet set, StandardAttribute attr)
{
	return IntMaskUtil::IsBitSet((IntMask)set, (IntMask)attr);
}

StandardAttributeSet StandardAttributes::CreateAttributeSet()
{
	return (StandardAttributeSet)IntMaskUtil::CreateIntMask();
}
