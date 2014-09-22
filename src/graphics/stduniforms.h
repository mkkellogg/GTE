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
    LightIntensity=128,
    LightAttenuation=256,
    EmissiveColor=512,
    Texture0=1024,
    Texture1=2048,
    NormalMap=4096
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
	LightIntensity=7,
	LightAttenuation=8,
	EmissiveColor=9,
	Texture0=10,
	Texture1=11,
	NormalMap=12,
    _Last=13, // always keep as last entry
    _None=14
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
