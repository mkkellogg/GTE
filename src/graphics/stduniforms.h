#ifndef _GTE_UNIFORMS_H_
#define _GTE_UNIFORMS_H_

#include "object/enginetypes.h"
#include "base/intmask.h"

namespace GTE
{
	enum class StandardUniformMaskComponent
	{
		ModelMatrix = 1,
		ModelViewMatrix = 2,
		ModelViewProjectionMatrix = 4,
		ProjectionMatrix = 8,
		ViewMatrix = 16,
		EyePosition = 32,
		LightPosition = 64,
		LightDirection = 128,
		LightColor = 256,
		LightIntensity = 512,
		LightAttenuation = 1024,
		LightType = 2048,
		EmissiveColor = 4096,
		Texture0 = 8192,
		Texture1 = 16384,
		NormalMap = 32768,
		DoShadowVolumeRender = 65536,
		ClipPlaneCount = 131072,
		ClipPlane0 = 262144
	};

	enum class StandardUniform
	{
		ModelMatrix = 0,
		ModelViewMatrix = 1,
		ModelViewProjectionMatrix = 2,
		ProjectionMatrix = 3,
		ViewMatrix = 4,
		EyePosition = 5,
		LightPosition = 6,
		LightDirection = 7,
		LightColor = 8,
		LightIntensity = 9,
		LightAttenuation = 10,
		LightType = 11,
		EmissiveColor = 12,
		Texture0 = 13,
		Texture1 = 14,
		NormalMap = 15,
		DoShadowVolumeRender = 16,
		ClipPlaneCount = 17,
		ClipPlane0 = 18,
		_Last = 19, // always keep as last entry (before _None)
		_None = 20
	};

	typedef IntMask StandardUniformSet;

	class StandardUniforms
	{
		static const Char* const uniformNames[];

	public:

		static const Char * GetUniformName(StandardUniform uniform);
		static StandardUniform UniformMaskComponentToUniform(StandardUniformMaskComponent component);
		static StandardUniformMaskComponent UniformToUniformMaskComponent(StandardUniform uniform);

		static StandardUniformSet CreateUniformSet();
		static void AddUniform(StandardUniformSet * set, StandardUniform uniform);
		static void RemoveUniform(StandardUniformSet * set, StandardUniform uniform);

		static Bool HasUniform(StandardUniformSet set, StandardUniform uniform);
	};
}

#endif
