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

Light::Light()
{
	type = LightType::Directional;
	color.Set(1,1,1,1);
	direction.Set(1,-1,0);
	intensity = 1;
	attenuation = 1;
	SetRange(30);
	shadowsEnabled = false;
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
	attenuation = 1.0/range;
}

const Color4 * Light::GetColorPtr() const
{
	return &color;
}

void Light::SetColor(Color4 color)
{
	this->color = color;
}

void Light::SetColor(float r, float g, float b, float a)
{
	color.Set(r,g,b,a);
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

void Light:: SetDirection(float x, float y, float z)
{
	direction.Set(x,y,z);
}

LightType Light::GetType() const
{
	return type;
}

void Light::SetType(LightType type)
{
	this->type = type;
}

float Light::GetRange() const
{
	return range;
}

void Light::SetRange(float range)
{
	this->range = range;
	CalcAttentuationForCurrentRange();
}

void Light::SetIntensity(float intensity)
{
	this->intensity = intensity;
}

float Light::GetIntensity() const
{
	return intensity;
}

void Light::SetAttenuation(float attenuation)
{
	this->attenuation = attenuation;
}

float Light::GetAttenuation() const
{
	return attenuation;
}

void Light::SetShadowsEnabled(bool enabled)
{
	shadowsEnabled = enabled;
}

bool Light::GetShadowsEnabled() const
{
	return shadowsEnabled;
}

void Light::SetCullingMask(IntMask mask)
{
	cullingMask = mask;
}

void Light::MergeCullingMask(IntMask mask)
{
	cullingMask = IntMaskUtil::MergeMasks(cullingMask, mask);
}

IntMask Light::GetCullingMask() const
{
	return cullingMask;
}

