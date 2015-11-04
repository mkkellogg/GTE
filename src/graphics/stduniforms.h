#ifndef _GTE_UNIFORMS_H_
#define _GTE_UNIFORMS_H_

#include "object/enginetypes.h"
#include "base/intmask.h"
#include <unordered_map>
#include <string>

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
		LightRange = 8192,
		LightParallelAngleAttenuation = 16384,
		LightOrthoAngleAttenuation = 32768,
		EmissiveColor = 65536,
		Texture0 = 131072,
		Texture1 = 262144,
		NormalMap = 524288,
		DoShadowVolumeRender = 1048576,
		ClipPlaneCount = 2097152,
		ClipPlane0 = 4194304
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
		LightRange = 13,
		LightParallelAngleAttenuation = 14,
		LightOrthoAngleAttenuation = 15,
		EmissiveColor = 16,
		Texture0 = 17,
		Texture1 = 18,
		NormalMap = 19,
		DoShadowVolumeRender = 20,
		ClipPlaneCount = 21,
		ClipPlane0 = 22,
		_Last = 23, // always keep as last entry (before _None)
		_None = 24
	};

	typedef IntMask StandardUniformSet;

	class StandardUniforms
	{
		static const std::string uniformNames[];
		static std::unordered_map<std::string, StandardUniform> nameToUniform;

	public:
		
		static void RegisterAll();

		static const std::string& GetUniformName(StandardUniform uniform);
		static StandardUniform GetUniformForName(const std::string& name);
		static StandardUniform ForName(const std::string& name);
		static StandardUniform UniformMaskComponentToUniform(StandardUniformMaskComponent component);
		static StandardUniformMaskComponent UniformToUniformMaskComponent(StandardUniform uniform);

		static StandardUniformSet CreateUniformSet();
		static void AddUniform(StandardUniformSet * set, StandardUniform uniform);
		static void RemoveUniform(StandardUniformSet * set, StandardUniform uniform);

		static Bool HasUniform(StandardUniformSet set, StandardUniform uniform);
	};
}

#endif
