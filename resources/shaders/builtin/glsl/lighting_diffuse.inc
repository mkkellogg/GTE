#include "lightcalcs.inc"

float calcDiffuseTerm(in vec3 normal, in vec3 lightVector, in float attenuation, in float lightIntensity)
{
	return clamp(dot(normal, -lightVector), 0.0, 1.0) * attenuation * lightIntensity;
}

float calcDiffuseTermNoVectors(in float attenuation, in float lightIntensity)
{
	return max(attenuation * lightIntensity, 0.0);
}

float calcDiffuseTermForLight(in vec3 normal, in vec4 position, in vec4 lightPosition, in vec3 lightDir, float lightIntensity, float baseAttenuation)
{
	float diffuseTerm = 0.0;
	vec3 lightVector = vec3(0, 0, 0);
	float attenuation = 0.0;
	if(LIGHT_TYPE == 4)
	{
		diffuseTerm = lightIntensity;
	}
	else if (LIGHT_TYPE == 1)
	{
		diffuseTerm = calcDiffuseTerm(normal, lightDir, 1.0, lightIntensity); 
	}
	else if (LIGHT_TYPE == 2)
	{
		calcPointLightParameters(position, lightPosition, baseAttenuation, lightVector, attenuation);
		diffuseTerm = calcDiffuseTerm(normal, normalize(lightVector), attenuation, lightIntensity);
	}
	else if (LIGHT_TYPE == 5)
	{
		calcPlanarLightParameters(position, normal, lightPosition, lightDir, baseAttenuation, lightVector, attenuation);
		diffuseTerm = calcDiffuseTermNoVectors(attenuation, lightIntensity);
	}

	return diffuseTerm;
}

