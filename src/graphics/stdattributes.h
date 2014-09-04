#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_

enum class StandardAttributeMaskComponent
{
    Position=1,
    Normal=2,
    Color=4,
    UV1=8,
    UV2=16
};

enum class StandardAttribute
{
    Position=0,
    Normal=1,
    Color=2,
    UV1=3,
    UV2=4,
    _Last=5 // always keep as last entry
};

typedef unsigned int StandardAttributeSet;

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
