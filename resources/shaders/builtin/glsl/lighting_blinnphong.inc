#include "lighting_diffuse.inc"
#include "lighting_specular.inc"

void calcBlinnPhongTermsForLight(in vec4 position, in vec3 normal, in vec3 surfaceNormal, in vec4 lightPosition, in vec3 lightDir, in vec4 eyePosition, float lightIntensity, float baseAttenuation, float specularFactor, out float diffuseTerm, out float specularTerm)
{
	vec3 lightVector = vec3(0, 0, 0);
	float attenuation = 0.0;
	if (LIGHT_TYPE == 4)
	{
		diffuseTerm = lightIntensity;
		specularTerm = 0.0;
	}
	else if (LIGHT_TYPE == 1)
	{
		diffuseTerm = calcDiffuseTerm(normal, lightDir, 1.0, lightIntensity); 
		specularTerm = calcSpecularTerm(1.0, lightDir, normal, surfaceNormal, eyePosition, position, lightIntensity, specularFactor);
	}
	else if (LIGHT_TYPE == 2)
	{
		calcPointLightParameters(position, lightPosition, baseAttenuation, lightVector, attenuation);
		diffuseTerm = calcDiffuseTerm(normal, normalize(lightVector), attenuation, lightIntensity);
		specularTerm = calcSpecularTerm(attenuation, lightVector, normal, surfaceNormal, eyePosition, position, lightIntensity, specularFactor);
	}
	else if (LIGHT_TYPE == 5)
	{
		calcPlanarLightParameters(position, normal, lightPosition, lightDir, baseAttenuation, lightVector, attenuation);
		diffuseTerm = calcDiffuseTermNoVectors(attenuation, lightIntensity);
		specularTerm = calcSpecularTerm(attenuation, lightVector, normal, surfaceNormal, eyePosition, position, lightIntensity, specularFactor);
	}
}