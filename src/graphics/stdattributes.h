#ifndef _GTE_ATTRIBUTES_H_
#define _GTE_ATTRIBUTES_H_

enum class StandardAttributeMaskComponent
{
    Position=1,
    ShadowPosition=2,
    Normal=4,
    VertexColor=8,
    UVTexture0=16,
    UVTexture1=32,
    UVNormalMap=64
};

enum class StandardAttribute
{
    Position=0,
    ShadowPosition=1,
    Normal=2,
    VertexColor=3,
    UVTexture0=4,
    UVTexture1=5,
    UVNormalMap=6,
    _Last=7, // always keep as last entry
    _None=8
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
