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

#include "particles_vertex_header.inc"
#include "lighting_diffuse.inc"

out vec3 vLightDir;
out vec4 vPosition;

void main() 
{
	if(LIGHT_TYPE == LIGHT_TYPE_DIRECTIONAL || LIGHT_TYPE == LIGHT_TYPE_PLANAR)
	{
		vLightDir = normalize(LIGHT_DIRECTION.xyz);
	}

	
	vColor = COLOR;
	vUV = UVTEXTURE0;
	vec4 quadPos = getQuadPosition();
	vPosition = quadPos;
	gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * quadPos;
}