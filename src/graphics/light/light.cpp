#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <string>
#include <iostream>

#include "light.h"
#include "graphics/color/color4.h"
#include "geometry/point/point3.h"
#include "geometry/vector/vector3.h"

namespace GTE
{
	Light::Light()
	{
		type = LightType::Directional;
		color.Set(1, 1, 1, 1);
		direction.Set(1, -1, 0);
		intensity = 1;
		attenuation = 1;
		SetRange(30);
		shadowsEnabled = false;
		attenuationOverride = false;
		parallelAttenuation = AngleAttenuationType::None;
		orthoAttenuation = AngleAttenuationType::None;
	}

	Light::~Light()
	{
		cullingMask = IntMaskUtil::CreateIntMask();
	}

	Color4 Light::GetColor() const
	{
		return color;
	}

	void Light::CalcAttentuationForCurrentRange()
	{
		if(!attenuationOverride)
		{
			attenuation = 1.0f / range;
		}
	}

	const Color4 * Light::GetColorPtr() const
	{
		return &color;
	}

	void Light::SetColor(Color4 color)
	{
		this->color = color;
	}

	void Light::SetColor(Real r, Real g, Real b, Real a)
	{
		color.Set(r, g, b, a);
	}

	Vector3 Light::GetDirection() const
	{
		return direction;
	}

	const Vector3 * Light::GetDirectionPtr() const
	{
		return &direction;
	}

	void Light::SetDirection(Vector3  direction)
	{
		this->direction = direction;
	}

	void Light::SetDirection(Real x, Real y, Real z)
	{
		direction.Set(x, y, z);
	}

	LightType Light::GetType() const
	{
		return type;
	}

	void Light::SetType(LightType type)
	{
		this->type = type;
	}

	Real Light::GetRange() const
	{
		return range;
	}

	void Light::SetRange(Real range)
	{
		this->range = range;
		CalcAttentuationForCurrentRange();
	}

	void Light::SetIntensity(Real intensity)
	{
		this->intensity = intensity;
	}

	Real Light::GetIntensity() const
	{
		return intensity;
	}

	void Light::SetAttenuation(Real attenuation)
	{
		this->attenuation = attenuation;
		attenuationOverride = true;
	}

	Real Light::GetAttenuation() const
	{
		return attenuation;
	}

	AngleAttenuationType Light::GetParallelAngleAttenuationType() const
	{
		return parallelAttenuation;
	}

	void Light::SetParallelAngleAttenuationType(AngleAttenuationType type)
	{
		parallelAttenuation = type;
	}

	AngleAttenuationType Light::GetOrthoAngleAttenuationType() const
	{
		return orthoAttenuation;
	}

	void Light::SetOrthoAngleAttenuationType(AngleAttenuationType type)
	{
		orthoAttenuation = type;
	}

	void Light::SetShadowsEnabled(Bool enabled)
	{
		shadowsEnabled = enabled;
	}

	Bool Light::GetShadowsEnabled() const
	{
		return shadowsEnabled;
	}

	void Light::SetCullingMask(IntMask mask)
	{
		cullingMask = mask;
	}

	IntMask Light::GetCullingMask() const
	{
		return cullingMask;
	}
}

