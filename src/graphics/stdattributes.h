#ifndef _GTE_ATTRIBUTES_H_
#define _GTE_ATTRIBUTES_H_

enum class StandardAttributeMaskComponent
{
    Position=1,
    ShadowPosition=2,
    Normal=4,
    FaceNormal = 8,
    Tangent=16,
    VertexColor=32,
    UVTexture0=64,
    UVTexture1=128,
    UVNormalMap=256
};

enum class StandardAttribute
{
    Position=0,
    ShadowPosition=1,
    Normal=2,
    FaceNormal=3,
    Tangent=4,
    VertexColor=5,
    UVTexture0=6,
    UVTexture1=7,
    UVNormalMap=8,
    _Last=9, // always keep as second to last entry
    _None=10 // always keep as last entry
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
