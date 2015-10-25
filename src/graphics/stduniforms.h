#ifndef _GTE_UNIFORMS_H_
#define _GTE_UNIFORMS_H_

#include "object/enginetypes.h"
#include "base/intmask.h"

namespace GTE
{
	enum class StandardUniformMaskComponent
	{
		ModelMatrix = 1,
		ModelMatrixInverseTranspose = 2,
		ModelViewMatrix = 4,
		ModelViewProjectionMatrix = 8,
		ProjectionMatrix = 16,
		ViewMatrix = 32,
		EyePosition = 64,
		LightPosition = 128,
		LightDirection = 256,
		LightColor = 512,
		LightIntensity = 1024,
		LightAttenuation = 2048,
		LightType = 4096,
		EmissiveColor = 8192,
		Texture0 = 16384,
		Texture1 = 32768,
		NormalMap = 65536,
		DoShadowVolumeRender = 131072,
		ClipPlaneCount = 262144,
		ClipPlane0 = 524288
	};

	enum class StandardUniform
	{
		ModelMatrix = 0,
		ModelMatrixInverseTranspose = 1,
		ModelViewMatrix = 2,
		ModelViewProjectionMatrix = 3,
		ProjectionMatrix = 4,
		ViewMatrix = 5,
		EyePosition = 6,
		LightPosition = 7,
		LightDirection = 8,
		LightColor = 9,
		LightIntensity = 10,
		LightAttenuation = 11,
		LightType = 12,
		EmissiveColor = 13,
		Texture0 = 14,
		Texture1 = 15,
		NormalMap = 16,
		DoShadowVolumeRender = 17,
		ClipPlaneCount = 18,
		ClipPlane0 = 19,
		_Last = 20, // always keep as last entry (before _None)
		_None = 21
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
