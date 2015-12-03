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

#include "particles_fragment_header.inc"
#include "lighting_diffuse.inc"

in vec4 vPosition;
in vec3 vLightDir;

void main()
{
	vec4 textureColor = texture(PARTICLE_TEXTURE, vUV);
	float DiffuseTerm = 0.0;
	vec4 diffuseColor = textureColor;

	vec3 normal = normalize((LIGHT_POSITION - vPosition).xyz);
	DiffuseTerm = calcDiffuseTermForLight(LIGHT_TYPE, normal, vPosition, LIGHT_POSITION, vLightDir, LIGHT_INTENSITY, LIGHT_ATTENUATION, LIGHT_RANGE, LIGHT_PARALLEL_ATTENUATION, LIGHT_ORTHO_ATTENUATION);
	diffuseColor = vColor * LIGHT_COLOR * textureColor;

	out_color = vec4(diffuseColor.xyz * DiffuseTerm, diffuseColor.a);
}

