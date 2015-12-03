#version 150

#include "common.inc"
#include "particles_vertex_header.inc"
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

out vec3 vLightDir[MAX_SHADER_LIGHTS];
out vec4 vPosition;

void main() 
{
	vColor = COLOR;
	vUV = UVTEXTURE0;

	for(int i = 0; i < MAX_SHADER_LIGHTS; i++)
	{
		if(LIGHT_ENABLED[i] == 1)
		{
			if(LIGHT_TYPE[i] == LIGHT_TYPE_DIRECTIONAL || LIGHT_TYPE[i] == LIGHT_TYPE_PLANAR)
			{
				vLightDir[i] = normalize(LIGHT_DIRECTION[i].xyz);
			}
		}
	}

	vec4 quadPos = getQuadPosition();
	vPosition = quadPos;

	gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * quadPos;
}