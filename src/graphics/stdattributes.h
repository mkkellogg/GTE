#ifndef _GTE_ATTRIBUTES_H_
#define _GTE_ATTRIBUTES_H_

#include <unordered_map>
#include <string>

#include "engine.h"
#include "base/bitmask.h"

namespace GTE
{
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

	enum class StandardAttributeMaskComponent
	{
		Position = (UInt32)StandardAttribute::Position << 1,
		ShadowPosition = (UInt32)StandardAttribute::ShadowPosition << 1,
		Normal = (UInt32)StandardAttribute::Normal << 1,
		FaceNormal = (UInt32)StandardAttribute::FaceNormal << 1,
		Tangent = (UInt32)StandardAttribute::Tangent << 1,
		VertexColor = (UInt32)StandardAttribute::VertexColor << 1,
		UVTexture0 = (UInt32)StandardAttribute::UVTexture0 << 1,
		UVTexture1 = (UInt32)StandardAttribute::UVTexture1 << 1,
		UVNormalMap = (UInt32)StandardAttribute::UVNormalMap << 1
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
