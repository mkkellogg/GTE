#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "uniforms.h"
#include "ui/debug.h"

const char* const StandardUniforms::uniformNames[] = {"MODELVIEW_MATRIX","MODELVIEWPROJECTION_MATRIX","PROJECTION_MATRIX","LIGHT"};

const char * StandardUniforms::GetUniformName(StandardUniform uniform)
{
	return uniformNames[(int)uniform];
}

StandardUniform StandardUniforms::UniformMaskComponentToUniform(StandardUniformMaskComponent component)
{
	return (StandardUniform)IntMask::MaskValueToIndex((unsigned int)component);
}

StandardUniformMaskComponent StandardUniforms::UniformToUniformMaskComponent(StandardUniform uniform)
{
	return (StandardUniformMaskComponent)IntMask::IndexToMaskValue((unsigned int)uniform);
}

void StandardUniforms::AddUniform(StandardUniformSet * set, StandardUniform uniform)
{
	IntMask::SetBitForIndexMask((unsigned int *)set, (unsigned int )uniform);
}

void StandardUniforms::RemoveUniform(StandardUniformSet * set, StandardUniform uniform)
{
	IntMask::ClearBitForIndexMask((unsigned int *)set, (unsigned int )uniform);
}

bool StandardUniforms::HasUniform(StandardUniformSet set, StandardUniform uniform)
{
	return IntMask::IsBitSet((unsigned int)set, (unsigned int)uniform);
}

StandardUniformSet StandardUniforms::CreateUniformSet()
{
	return (unsigned int)0;
}
