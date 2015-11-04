#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "stduniforms.h"
#include "debug/gtedebug.h"
#include "materialvardirectory.h"

namespace GTE
{
	const std::string StandardUniforms::uniformNames[] =
	{   
		"MODEL_MATRIX",
		"MODEL_MATRIX_INVERSE_TRANSPOSE",
		"MODELVIEW_MATRIX",
		"MODELVIEWPROJECTION_MATRIX",
		"PROJECTION_MATRIX",
		"VIEW_MATRIX",
		"EYE_POSITION",
		"LIGHT_POSITION",
		"LIGHT_DIRECTION",
		"LIGHT_COLOR",
		"LIGHT_INTENSITY",
		"LIGHT_ATTENUATION",
		"LIGHT_TYPE",
		"LIGHT_RANGE",
		"LIGHT_PARALLEL_ATTENUATION",
		"LIGHT_ORTHO_ATTENUATION",
		"EMISSIVECOLOR",
		"TEXTURE0",
		"TEXTURE1",
		"NORMALMAP",
		"DO_SHADOW_VOLUME_RENDER",
		"CLIP_PLANE_COUNT",
		"CLIP_PLANE0" 
	};

	std::unordered_map<std::string, StandardUniform> StandardUniforms::nameToUniform 
	{
		{uniformNames[(UInt16)StandardUniform::ModelMatrix],StandardUniform::ModelMatrix},
		{uniformNames[(UInt16)StandardUniform::ModelMatrixInverseTranspose],StandardUniform::ModelMatrixInverseTranspose},
		{uniformNames[(UInt16)StandardUniform::ModelViewMatrix],StandardUniform::ModelViewMatrix},
		{uniformNames[(UInt16)StandardUniform::ModelViewProjectionMatrix],StandardUniform::ModelViewProjectionMatrix},
		{uniformNames[(UInt16)StandardUniform::ProjectionMatrix],StandardUniform::ProjectionMatrix},
		{uniformNames[(UInt16)StandardUniform::ViewMatrix],StandardUniform::ViewMatrix},
		{uniformNames[(UInt16)StandardUniform::ModelMatrix],StandardUniform::ModelMatrixInverseTranspose},
		{uniformNames[(UInt16)StandardUniform::EyePosition],StandardUniform::EyePosition},
		{uniformNames[(UInt16)StandardUniform::LightPosition],StandardUniform::LightPosition},
		{uniformNames[(UInt16)StandardUniform::LightDirection],StandardUniform::LightDirection},
		{uniformNames[(UInt16)StandardUniform::LightColor],StandardUniform::LightColor},
		{uniformNames[(UInt16)StandardUniform::LightIntensity],StandardUniform::LightIntensity},
		{uniformNames[(UInt16)StandardUniform::LightAttenuation],StandardUniform::LightAttenuation},
		{uniformNames[(UInt16)StandardUniform::LightType],StandardUniform::LightType},
		{uniformNames[(UInt16)StandardUniform::LightRange],StandardUniform::LightRange},
		{uniformNames[(UInt16)StandardUniform::LightParallelAngleAttenuation],StandardUniform::LightParallelAngleAttenuation},
		{uniformNames[(UInt16)StandardUniform::LightOrthoAngleAttenuation],StandardUniform::LightOrthoAngleAttenuation},
		{uniformNames[(UInt16)StandardUniform::EmissiveColor],StandardUniform::EmissiveColor},
		{uniformNames[(UInt16)StandardUniform::Texture0],StandardUniform::Texture0},
		{uniformNames[(UInt16)StandardUniform::Texture1],StandardUniform::Texture1},
		{uniformNames[(UInt16)StandardUniform::NormalMap],StandardUniform::NormalMap},
		{uniformNames[(UInt16)StandardUniform::DoShadowVolumeRender],StandardUniform::DoShadowVolumeRender},
		{uniformNames[(UInt16)StandardUniform::ClipPlaneCount],StandardUniform::ClipPlaneCount},
		{uniformNames[(UInt16)StandardUniform::ClipPlane0],StandardUniform::ClipPlane0}
	};

	void StandardUniforms::RegisterAll()
	{
		for(UInt32 i = 0; i < (UInt32)StandardUniform::_Last; i++)
		{
			UniformDirectory::RegisterVarID(GetUniformName((StandardUniform)i));
		}
	}

	const std::string& StandardUniforms::GetUniformName(StandardUniform uniform)
	{
		return uniformNames[(UInt16)uniform];
	}

	StandardUniform StandardUniforms::GetUniformForName(const std::string& name)
	{
		auto result = nameToUniform.find(name);
		if(result == nameToUniform.end())
		{
			return StandardUniform::_None;
		}

		return (*result).second;
	}

	StandardUniform StandardUniforms::ForName(const std::string& name)
	{
		return GetUniformForName(name);
	}

	StandardUniform StandardUniforms::UniformMaskComponentToUniform(StandardUniformMaskComponent component)
	{
		return (StandardUniform)IntMaskUtil::MaskValueToIndex((IntMask)component);
	}

	StandardUniformMaskComponent StandardUniforms::UniformToUniformMaskComponent(StandardUniform uniform)
	{
		return (StandardUniformMaskComponent)IntMaskUtil::IndexToMaskValue((UInt16)uniform);
	}

	void StandardUniforms::AddUniform(StandardUniformSet * set, StandardUniform uniform)
	{
		IntMaskUtil::SetBit((IntMask *)set, (IntMask)uniform);
	}

	void StandardUniforms::RemoveUniform(StandardUniformSet * set, StandardUniform uniform)
	{
		IntMaskUtil::ClearBit((IntMask *)set, (IntMask)uniform);
	}

	Bool StandardUniforms::HasUniform(StandardUniformSet set, StandardUniform uniform)
	{
		return IntMaskUtil::IsBitSet((IntMask)set, (IntMask)uniform);
	}

	StandardUniformSet StandardUniforms::CreateUniformSet()
	{
		return (StandardUniformSet)IntMaskUtil::CreateIntMask();
	}
}
