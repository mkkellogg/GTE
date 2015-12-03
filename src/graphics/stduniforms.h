#ifndef _GTE_UNIFORMS_H_
#define _GTE_UNIFORMS_H_

#include <unordered_map>
#include <string>

#include "engine.h"
#include "base/intmask.h"

namespace GTE
{
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
		LightEnabled = 16,
		EmissiveColor = 17,
		Texture0 = 18,
		Texture1 = 19,
		NormalMap = 20,
		DoShadowVolumeRender = 21,
		ClipPlaneCount = 22,
		ClipPlane0 = 23,
		_Last = 24, // always keep as last entry (before _None)
		_None = 25
	};

	enum class StandardUniformMaskComponent
	{
		ModelMatrix = (UInt32)StandardUniform::ModelMatrix << 1,
		ModelMatrixInverseTranspose = (UInt32)StandardUniform::ModelMatrixInverseTranspose << 1,
		ModelViewMatrix = (UInt32)StandardUniform::ModelViewMatrix << 1,
		ModelViewProjectionMatrix = (UInt32)StandardUniform::ModelViewProjectionMatrix << 1,
		ProjectionMatrix = (UInt32)StandardUniform::ProjectionMatrix << 1,
		ViewMatrix = (UInt32)StandardUniform::ViewMatrix << 1,
		EyePosition = (UInt32)StandardUniform::EyePosition << 1,
		LightPosition = (UInt32)StandardUniform::LightPosition << 1,
		LightDirection = (UInt32)StandardUniform::LightDirection << 1,
		LightColor = (UInt32)StandardUniform::LightColor << 1,
		LightIntensity = (UInt32)StandardUniform::LightIntensity << 1,
		LightAttenuation = (UInt32)StandardUniform::LightAttenuation << 1,
		LightType = (UInt32)StandardUniform::LightType << 1,
		LightRange = (UInt32)StandardUniform::LightRange << 1,
		LightParallelAngleAttenuation = (UInt32)StandardUniform::LightParallelAngleAttenuation << 1,
		LightOrthoAngleAttenuation = (UInt32)StandardUniform::LightOrthoAngleAttenuation << 1,
		LightEnabled = (UInt32)StandardUniform::LightEnabled << 1,
		EmissiveColor = (UInt32)StandardUniform::EmissiveColor << 1,
		Texture0 = (UInt32)StandardUniform::Texture0 << 1,
		Texture1 = (UInt32)StandardUniform::Texture1 << 1,
		NormalMap = (UInt32)StandardUniform::NormalMap << 1,
		DoShadowVolumeRender = (UInt32)StandardUniform::DoShadowVolumeRender << 1,
		ClipPlaneCount = (UInt32)StandardUniform::ClipPlaneCount << 1,
		ClipPlane0 = (UInt32)StandardUniform::ClipPlane0 << 1
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
