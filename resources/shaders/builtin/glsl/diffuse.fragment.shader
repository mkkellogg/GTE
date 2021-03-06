#version 150

uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;
uniform int LIGHT_TYPE;
uniform float LIGHT_RANGE;
uniform int LIGHT_PARALLEL_ATTENUATION;
uniform int LIGHT_ORTHO_ATTENUATION;

vec4 outputF;

in vec4 vColor;
in vec3 vNormal;
in vec4 vPosition;
in vec3 vLightDir;

out vec4 out_color;

#include "lighting_diffuse.inc"

void main()
{
	float DiffuseTerm = 0.0;
	vec4 diffuseColor = vec4(0, 0, 0, 0);
	vec3 normal = normalize(vNormal);

	DiffuseTerm = calcDiffuseTermForLight(LIGHT_TYPE, normal, vPosition, LIGHT_POSITION, vLightDir, LIGHT_INTENSITY, LIGHT_ATTENUATION, LIGHT_RANGE, LIGHT_PARALLEL_ATTENUATION, LIGHT_ORTHO_ATTENUATION);

	diffuseColor = LIGHT_COLOR * vColor;
	outputF = DiffuseTerm * diffuseColor;
	out_color = outputF;
}

