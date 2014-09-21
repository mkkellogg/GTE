#ifndef _UNIFORMS_H_
#define _UNIFORMS_H_

enum class StandardUniformMaskComponent
{
	ModelMatrix=1,
    ModelViewMatrix=2,
    ModelViewProjectionMatrix=4,
    ProjectionMatrix=8,
    LightPosition=16,
    LightDirection=32,
    LightColor=64,
    EmissiveColor=128,
    Texture0=256,
    Texture1=512,
    NormalMap=1024
};

enum class StandardUniform
{
	ModelMatrix=0,
	ModelViewMatrix=1,
	ModelViewProjectionMatrix=2,
	ProjectionMatrix=3,
	LightPosition=4,
	LightDirection=5,
	LightColor=6,
	EmissiveColor=7,
	Texture0=8,
	Texture1=9,
	NormalMap=10,
    _Last=11, // always keep as last entry
    _None=12
};

#include "base/intmask.h"

typedef IntMask StandardUniformSet;

class StandardUniforms
{
	static const char* const uniformNames[];

	public:

	static const char * GetUniformName(StandardUniform uniform);
	static StandardUniform UniformMaskComponentToUniform(StandardUniformMaskComponent component);
	static StandardUniformMaskComponent UniformToUniformMaskComponent(StandardUniform uniform);

	static StandardUniformSet CreateUniformSet();
	static void AddUniform(StandardUniformSet * set, StandardUniform uniform);
	static void RemoveUniform(StandardUniformSet * set, StandardUniform uniform);

	static bool HasUniform(StandardUniformSet set, StandardUniform uniform);
};

#endif
