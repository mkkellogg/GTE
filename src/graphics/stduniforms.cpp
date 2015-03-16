#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "stduniforms.h"
#include "debug/gtedebug.h"

const char* const StandardUniforms::uniformNames[] = {"MODEL_MATRIX",
													  "MODELVIEW_MATRIX",
													  "MODELVIEWPROJECTION_MATRIX",
													  "PROJECTION_MATRIX",
													  "LIGHT_POSITION",
													  "LIGHT_DIRECTION",
													  "LIGHT_COLOR",
													  "LIGHT_INTENSITY",
													  "LIGHT_ATTENUATION",
													  "LIGHT_TYPE",
													  "EMISSIVECOLOR",
													  "TEXTURE0",
													  "TEXTURE1",
													  "NORMALMAP",
													  "DO_SHADOW_VOLUME_RENDER",
													  "CLIP_PLANE_COUNT",
													  "CLIP_PLANE0"};

const char * StandardUniforms::GetUniformName(StandardUniform uniform)
{
	return uniformNames[(short)uniform];
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
	IntMaskUtil::SetBit((IntMask *)set, (IntMask )uniform);
}

void StandardUniforms::RemoveUniform(StandardUniformSet * set, StandardUniform uniform)
{
	IntMaskUtil::ClearBit((IntMask *)set, (IntMask )uniform);
}

bool StandardUniforms::HasUniform(StandardUniformSet set, StandardUniform uniform)
{
	return IntMaskUtil::IsBitSet((IntMask)set, (IntMask)uniform);
}

StandardUniformSet StandardUniforms::CreateUniformSet()
{
	return (StandardUniformSet)IntMaskUtil::CreateIntMask();
}
