#ifndef _GTE_UNIFORMS_H_
#define _GTE_UNIFORMS_H_

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
    LightType=512,
    EmissiveColor=1024,
    Texture0=2048,
    Texture1=4096,
    NormalMap=8192,
    DoShadowVolumeRender=16384,
    ClipPlaneCount=32768,
    ClipPlane0=65536
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
	LightType=9,
	EmissiveColor=10,
	Texture0=11,
	Texture1=12,
	NormalMap=13,
	DoShadowVolumeRender=14,
	ClipPlaneCount=15,
	ClipPlane0=16,
    _Last=17, // always keep as last entry (before _None)
    _None=18
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
