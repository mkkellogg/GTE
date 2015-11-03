#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "stdattributes.h"
#include "debug/gtedebug.h"

namespace GTE
{
	const std::string StandardAttributes::attributeNames[] = 
	{ 
		"POSITION", 
		"SHADOW_POSITION", 
		"NORMAL", 
		"FACENORMAL", 
		"TANGENT", 
		"COLOR", 
		"UVTEXTURE0", 
		"UVTEXTURE1", 
		"UVNORMALMAP" 
	};

	std::unordered_map<std::string, StandardAttribute> StandardAttributes::nameToAttribute
	{
		{attributeNames[(UInt16)StandardAttribute::Position],StandardAttribute::Position},
		{attributeNames[(UInt16)StandardAttribute::ShadowPosition],StandardAttribute::ShadowPosition},
		{attributeNames[(UInt16)StandardAttribute::Normal],StandardAttribute::Normal},
		{attributeNames[(UInt16)StandardAttribute::FaceNormal],StandardAttribute::FaceNormal},
		{attributeNames[(UInt16)StandardAttribute::Tangent],StandardAttribute::Tangent},
		{attributeNames[(UInt16)StandardAttribute::VertexColor],StandardAttribute::VertexColor},
		{attributeNames[(UInt16)StandardAttribute::UVTexture0],StandardAttribute::UVTexture0},
		{attributeNames[(UInt16)StandardAttribute::UVTexture1],StandardAttribute::UVTexture1},
		{attributeNames[(UInt16)StandardAttribute::UVNormalMap],StandardAttribute::UVNormalMap}
	};

	const std::string& StandardAttributes::GetAttributeName(StandardAttribute attr)
	{
		return attributeNames[(UInt16)attr];
	}

	StandardAttribute StandardAttributes::GetAttributeForName(const std::string& name)
	{
		auto result = nameToAttribute.find(name);
		if(result == nameToAttribute.end())
		{
			return StandardAttribute::_None;
		}

		return (*result).second;
	}

	StandardAttribute StandardAttributes::AttributeMaskComponentToAttribute(StandardAttributeMaskComponent component)
	{
		return (StandardAttribute)IntMaskUtil::MaskValueToIndex((IntMask)component);
	}

	StandardAttributeMaskComponent StandardAttributes::AttributeToAttributeMaskComponent(StandardAttribute attr)
	{
		return (StandardAttributeMaskComponent)IntMaskUtil::IndexToMaskValue((UInt16)attr);
	}

	void StandardAttributes::AddAttribute(StandardAttributeSet * set, StandardAttribute attr)
	{
		IntMaskUtil::SetBit((IntMask *)set, (IntMask)attr);
	}

	void StandardAttributes::RemoveAttribute(StandardAttributeSet * set, StandardAttribute attr)
	{
		IntMaskUtil::ClearBit((IntMask *)set, (IntMask)attr);
	}

	Bool StandardAttributes::HasAttribute(StandardAttributeSet set, StandardAttribute attr)
	{
		return IntMaskUtil::IsBitSet((IntMask)set, (IntMask)attr);
	}

	StandardAttributeSet StandardAttributes::CreateAttributeSet()
	{
		return (StandardAttributeSet)IntMaskUtil::CreateIntMask();
	}
}
