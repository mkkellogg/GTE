#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_

enum class AttributeMaskComponent
{
    Position=1,
    Normal=2,
    Color=4,
    UV1=8,
    UV2=16
};

enum class Attribute
{
    Position=0,
    Normal=1,
    Color=2,
    UV1=3,
    UV2=4,
    _Last=5 // always keep as last entry
};

typedef unsigned int AttributeSet;

class Attributes
{
	static const char* const attributeNames[];

	public:

	static const char * GetAttributeName(Attribute attr);
	static Attribute AttributeMaskComponentToAttribute(AttributeMaskComponent component);
	static AttributeMaskComponent AttributeToAttributeMaskComponent(Attribute attr);

	static AttributeSet CreateAttributeSet();
	static void AddAttribute(AttributeSet * set, Attribute attr);
	static void RemoveAttribute(AttributeSet * set, Attribute attr);

	static bool HasAttribute(AttributeSet set, Attribute attr);
};

#endif
