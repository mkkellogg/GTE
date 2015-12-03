#include "lighting_diffuse.inc"
#include "lighting_specular.inc"
#include "common.inc"

void calcBlinnPhongTermsForLight(in int lightType, in vec4 position, in vec3 normal, in vec3 surfaceNormal, in vec4 lightPosition, in vec3 lightDir, in vec4 eyePosition, in float lightIntensity, in float baseAttenuation,
								 in float lightRange, in int parallelAttenuationType, in int orthoAttenuationType, in float specularFactor, out float diffuseTerm, out float specularTerm)
{
	vec3 lightVector = vec3(0, 0, 0);
	float attenuation = 0.0;
	if (lightType == LIGHT_TYPE_AMBIENT)
	{
		diffuseTerm = lightIntensity;
		specularTerm = 0.0;
	}
	else if (lightType == LIGHT_TYPE_DIRECTIONAL)
	{
		diffuseTerm = calcDiffuseTerm(normal, lightDir, 1.0, lightIntensity); 
		specularTerm = calcSpecularTerm(1.0, lightDir, normal, surfaceNormal, eyePosition, position, lightIntensity, specularFactor);
	}
	else if (lightType == LIGHT_TYPE_POINT)
	{
		calcPointLightParameters(position, lightPosition, baseAttenuation, lightVector, attenuation);
		diffuseTerm = calcDiffuseTerm(normal, normalize(lightVector), attenuation, lightIntensity);
		specularTerm = calcSpecularTerm(attenuation, lightVector, normal, surfaceNormal, eyePosition, position, lightIntensity, specularFactor);
	}
	else if (lightType == LIGHT_TYPE_PLANAR)
	{
		calcPlanarLightParameters(position, normal, lightPosition, lightDir, baseAttenuation, lightRange, parallelAttenuationType, orthoAttenuationType, lightVector, attenuation);
		diffuseTerm = calcDiffuseTermNoVectors(attenuation, lightIntensity);
		specularTerm = calcSpecularTerm(attenuation, lightVector, normal, surfaceNormal, eyePosition, position, lightIntensity, specularFactor);
	}
}
