#ifndef _GTE_UNIFORMS_H_
#define _GTE_UNIFORMS_H_

enum class StandardUniformMaskComponent
{
	ModelMatrix=1,
    ModelViewMatrix=2,
    ModelViewProjectionMatrix=4,
    ProjectionMatrix=8,
    EyePosition=16,
    LightPosition=32,
    LightDirection=64,
    LightColor=128,
    LightIntensity=256,
    LightAttenuation=512,
    LightType=1024,
    EmissiveColor=2048,
    Texture0=4096,
    Texture1=8192,
    NormalMap=16384,
    DoShadowVolumeRender=32768,
    ClipPlaneCount=65536,
    ClipPlane0=131072
};

enum class StandardUniform
{
	ModelMatrix=0,
	ModelViewMatrix=1,
	ModelViewProjectionMatrix=2,
	ProjectionMatrix=3,
	EyePosition=4,
	LightPosition=5,
	LightDirection=6,
	LightColor=7,
	LightIntensity=8,
	LightAttenuation=9,
	LightType=10,
	EmissiveColor=11,
	Texture0=12,
	Texture1=13,
	NormalMap=14,
	DoShadowVolumeRender=15,
	ClipPlaneCount=16,
	ClipPlane0=17,
    _Last=18, // always keep as last entry (before _None)
    _None=19
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
