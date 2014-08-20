#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "uniforms.h"
#include "ui/debug.h"

const char* const Uniforms::uniformNames[] = {"MODELVIEW_MATRIX","MODELVIEWPROJECTION_MATRIX","PROJECTION_MATRIX","LIGHT"};

const char * Uniforms::GetUniformName(Uniform uniform)
{
	return uniformNames[(int)uniform];
}

Uniform Uniforms::UniformMaskComponentToUniform(UniformMaskComponent component)
{
	return (Uniform)IntMask::MaskValueToIndex((unsigned int)component);
}

UniformMaskComponent Uniforms::UniformToUniformMaskComponent(Uniform uniform)
{
	return (UniformMaskComponent)IntMask::IndexToMaskValue((unsigned int)uniform);
}

void Uniforms::AddUniform(UniformSet * set, Uniform uniform)
{
	IntMask::SetBitForIndexMask((unsigned int *)set, (unsigned int )uniform);
}

void Uniforms::RemoveUniform(UniformSet * set, Uniform uniform)
{
	IntMask::ClearBitForIndexMask((unsigned int *)set, (unsigned int )uniform);
}

bool Uniforms::HasUniform(UniformSet set, Uniform uniform)
{
	return IntMask::IsBitSet((unsigned int)set, (unsigned int)uniform);
}

UniformSet Uniforms::CreateUniformSet()
{
	return (unsigned int)0;
}
