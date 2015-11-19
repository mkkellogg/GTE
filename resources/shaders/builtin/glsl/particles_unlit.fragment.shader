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

#include "lighting_diffuse.inc"
#include "particles_fragment_header.inc"

void main()
{
	vec4 textureColor = texture(PARTICLE_TEXTURE, vUV);
	out_color = vColor *textureColor;
}

