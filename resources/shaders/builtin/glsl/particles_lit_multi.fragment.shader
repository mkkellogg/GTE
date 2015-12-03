#version 150

#include "common.inc"
#include "particles_fragment_header.inc"
#include "lighting_diffuse.inc"

uniform vec4 LIGHT_POSITION[MAX_SHADER_LIGHTS];
uniform vec4 LIGHT_DIRECTION[MAX_SHADER_LIGHTS];
uniform vec4 LIGHT_COLOR[MAX_SHADER_LIGHTS];
uniform float LIGHT_INTENSITY[MAX_SHADER_LIGHTS];
uniform float LIGHT_ATTENUATION[MAX_SHADER_LIGHTS];
uniform int LIGHT_TYPE[MAX_SHADER_LIGHTS];
uniform float LIGHT_RANGE[MAX_SHADER_LIGHTS];
uniform int LIGHT_PARALLEL_ATTENUATION[MAX_SHADER_LIGHTS];
uniform int LIGHT_ORTHO_ATTENUATION[MAX_SHADER_LIGHTS];
uniform int LIGHT_ENABLED[MAX_SHADER_LIGHTS];

in vec4 vPosition;
in vec3 vLightDir;

void main()
{
	vec4 textureColor = texture(PARTICLE_TEXTURE, vUV);
	float DiffuseTerm = 0.0;
	vec4 overallDiffuseColor = vec4(0.0, 0.0, 0.0, 0.0);
	
	for(int i = 0; i < MAX_SHADER_LIGHTS; i++)
	{
		if(LIGHT_ENABLED[i] == 1)
		{
			vec3 normal = normalize((LIGHT_POSITION[i] - vPosition).xyz);
			DiffuseTerm = calcDiffuseTermForLight(LIGHT_TYPE[i], normal, vPosition, LIGHT_POSITION[i], vLightDir, LIGHT_INTENSITY[i], LIGHT_ATTENUATION[i],
												  LIGHT_RANGE[i], LIGHT_PARALLEL_ATTENUATION[i], LIGHT_ORTHO_ATTENUATION[i]);
			vec4 diffuseColor = vColor * LIGHT_COLOR[i] * textureColor * DiffuseTerm;

			overallDiffuseColor += diffuseColor;
		}
	}

	out_color = vec4(overallDiffuseColor.rgb, textureColor.a * vColor.a);
}

