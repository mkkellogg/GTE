#ifndef _GTE_ATTRIBUTES_H_
#define _GTE_ATTRIBUTES_H_

#include "engine.h"
#include "base/intmask.h"
#include <unordered_map>
#include <string>

namespace GTE
{
	enum class StandardAttributeMaskComponent
	{
		Position = 1,
		ShadowPosition = 2,
		Normal = 4,
		FaceNormal = 8,
		Tangent = 16,
		VertexColor = 32,
		UVTexture0 = 64,
		UVTexture1 = 128,
		UVNormalMap = 256
	};

	enum class StandardAttribute
	{
		Position = 0,
		ShadowPosition = 1,
		Normal = 2,
		FaceNormal = 3,
		Tangent = 4,
		VertexColor = 5,
		UVTexture0 = 6,
		UVTexture1 = 7,
		UVNormalMap = 8,
		_Last = 9, // always keep as second to last entry
		_None = 10 // always keep as last entry
	};

	typedef IntMask StandardAttributeSet;

	class StandardAttributes
	{
		static const std::string attributeNames[];
		static std::unordered_map<std::string, StandardAttribute> nameToAttribute;

	public:
		static void RegisterAll();

		static const std::string& GetAttributeName(StandardAttribute attr);
		static StandardAttribute GetAttributeForName(const std::string& name);
		static StandardAttribute ForName(const std::string& name);
		static StandardAttribute AttributeMaskComponentToAttribute(StandardAttributeMaskComponent component);
		static StandardAttributeMaskComponent AttributeToAttributeMaskComponent(StandardAttribute attr);

		static StandardAttributeSet CreateAttributeSet();
		static void AddAttribute(StandardAttributeSet * set, StandardAttribute attr);
		static void RemoveAttribute(StandardAttributeSet * set, StandardAttribute attr);

		static Bool HasAttribute(StandardAttributeSet set, StandardAttribute attr);
	};
}

#endif
