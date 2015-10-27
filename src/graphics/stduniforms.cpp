#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "stduniforms.h"
#include "debug/gtedebug.h"

namespace GTE
{
	const Char* const StandardUniforms::uniformNames[] = 
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

	const Char * StandardUniforms::GetUniformName(StandardUniform uniform)
	{
		return uniformNames[(Int16)uniform];
	}

	StandardUniform StandardUniforms::UniformMaskComponentToUniform(StandardUniformMaskComponent component)
	{
		return (StandardUniform)IntMaskUtil::MaskValueToIndex((IntMask)component);
	}

	StandardUniformMaskComponent StandardUniforms::UniformToUniformMaskComponent(StandardUniform uniform)
	{
		return (StandardUniformMaskComponent)IntMaskUtil::IndexToMaskValue((IntMask)uniform);
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
