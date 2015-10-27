#include "lightcalcs.inc"

float calcSpecularTerm(float attenuationFactor, vec3 lightVector, vec3 normal, vec3 surfaceNormal, vec4 eyePosition, vec4 position, float lightIntensity, float specularFactor)
{
	vec4 specularColor = vec4(0,0,0,0);
	vec3 VertexToEye = normalize(vec3(eyePosition - position));
    vec3 LightReflect = normalize(reflect(lightVector, normal));
	float specularTerm = dot(VertexToEye, LightReflect);
    specularTerm = pow(max(0.0, specularTerm), 32) * attenuationFactor * (lightIntensity) * specularFactor;
	if(dot(-lightVector,surfaceNormal) <0.0) specularTerm=0.0;
	specularTerm = max(specularTerm, 0.0);
 	return specularTerm;
 }

float calcSpecularTermForLight(in vec4 position, in vec3 normal, in vec3 surfaceNormal, in vec4 lightPosition, in vec3 lightDir, in vec4 eyePosition, float lightIntensity, float baseAttenuation, float specularFactor)
{
	float specularTerm = 0.0;
	vec3 lightVector = vec3(0, 0, 0);
	float attenuation = 0.0;
	if(LIGHT_TYPE == 4)
	{
		specularTerm = 0.0;
	}
	else if (LIGHT_TYPE == 1)
	{
		specularTerm = calcSpecularTerm(1.0, lightDir, normal, surfaceNormal, eyePosition, position, lightIntensity, specularFactor);
	}
	else if (LIGHT_TYPE == 2)
	{
		calcPointLightParameters(position, lightPosition, baseAttenuation, lightVector, attenuation);
		specularTerm = calcSpecularTerm(attenuation, lightVector, normal, surfaceNormal, eyePosition, position, lightIntensity, specularFactor);
	}
	else if (LIGHT_TYPE == 5)
	{
		calcPlanarLightParameters(position, normal, lightPosition, lightDir, baseAttenuation, lightVector, attenuation);
		specularTerm = calcSpecularTerm(attenuation, lightVector, normal, surfaceNormal, eyePosition, position, lightIntensity, specularFactor);
	}

	return specularTerm;
}