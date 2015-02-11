#ifndef _GTE_ATTRIBUTES_H_
#define _GTE_ATTRIBUTES_H_

enum class StandardAttributeMaskComponent
{
    Position=1,
    ShadowPosition=2,
    Normal=4,
    Tangent=8,
    VertexColor=16,
    UVTexture0=32,
    UVTexture1=64,
    UVNormalMap=128
};

enum class StandardAttribute
{
    Position=0,
    ShadowPosition=1,
    Normal=2,
    Tangent=3,
    VertexColor=4,
    UVTexture0=5,
    UVTexture1=6,
    UVNormalMap=7,
    _Last=8, // always keep as last entry
    _None=9
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
