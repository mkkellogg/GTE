#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_

enum class StandardAttributeMaskComponent
{
    Position=1,
    Normal=2,
    Color=4,
    UVTexture0=8,
    UVTexture1=16,
    UVNormalMap=32
};

enum class StandardAttribute
{
    Position=0,
    Normal=1,
    Color=2,
    UVTexture0=3,
    UVTexture1=4,
    UVNormalMap=5,
    _Last=6, // always keep as last entry
    _None=7
};

#include "base/intmask.h"

typedef IntMask StandardAttributeSet;

class StandardAttributes
{
	static const char* const attributeNames[];

	public:

	static const char * GetAttributeName(StandardAttribute attr);
	static StandardAttribute AttributeMaskComponentToAttribute(StandardAttributeMaskComponent component);
	static StandardAttributeMaskComponent AttributeToAttributeMaskComponent(StandardAttribute attr);

	static StandardAttributeSet CreateAttributeSet();
	static void AddAttribute(StandardAttributeSet * set, StandardAttribute attr);
	static void RemoveAttribute(StandardAttributeSet * set, StandardAttribute attr);

	static bool HasAttribute(StandardAttributeSet set, StandardAttribute attr);
};

#endif
