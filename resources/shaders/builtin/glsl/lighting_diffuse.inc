#include "common.inc"
#include "lightcalcs.inc"

float calcDiffuseTerm(in vec3 normal, in vec3 lightVector, in float attenuation, in float lightIntensity)
{
	return clamp(dot(normal, -lightVector), 0.0, 1.0) * attenuation * lightIntensity;
}

float calcDiffuseTermNoVectors(in float attenuation, in float lightIntensity)
{
	return max(attenuation * lightIntensity, 0.0);
}

float calcDiffuseTermForLight(in vec3 normal, in vec4 position, in vec4 lightPosition, in vec3 lightDir, in float lightIntensity, in float baseAttenuation, 
							  in float lightRange, in int parallelAttenuationType, in int orthoAttenuationType)
{
	float diffuseTerm = 0.0;
	vec3 lightVector = vec3(0, 0, 0);
	float attenuation = 0.0;
	if(LIGHT_TYPE == LIGHT_TYPE_AMBIENT)
	{
		diffuseTerm = lightIntensity;
	}
	else if (LIGHT_TYPE == LIGHT_TYPE_DIRECTIONAL)
	{
		diffuseTerm = calcDiffuseTerm(normal, lightDir, 1.0, lightIntensity); 
	}
	else if (LIGHT_TYPE == LIGHT_TYPE_POINT)
	{
		calcPointLightParameters(position, lightPosition, baseAttenuation, lightVector, attenuation);
		diffuseTerm = calcDiffuseTerm(normal, normalize(lightVector), attenuation, lightIntensity);
	}
	else if (LIGHT_TYPE == LIGHT_TYPE_PLANAR)
	{
		calcPlanarLightParameters(position, normal, lightPosition, lightDir, baseAttenuation, lightRange, parallelAttenuationType, orthoAttenuationType, lightVector, attenuation);
		diffuseTerm = calcDiffuseTermNoVectors(attenuation, lightIntensity);
	}

	return diffuseTerm;
}

