#version 150

uniform sampler2D TEXTURE0;
uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;
uniform int LIGHT_TYPE;
uniform float LIGHT_RANGE;
uniform int LIGHT_PARALLEL_ATTENUATION;
uniform int LIGHT_ORTHO_ATTENUATION;
uniform vec2 UV_SCALE;

vec4 texColor;
vec4 outputF;

in vec3 vNormal;
in vec4 vPosition;
in vec3 vLightDir;
in vec2 vUVTexture0;

out vec4 out_color;

#include "lighting_diffuse.inc"

void main()
{

	texColor = texture(TEXTURE0, vUVTexture0 * UV_SCALE);
	float DiffuseTerm = 0.0;
	vec4 diffuseColor = vec4(0, 0, 0, 0);
	vec3 normal = normalize(vNormal);

	DiffuseTerm = calcDiffuseTermForLight(normal, vPosition, LIGHT_POSITION, vLightDir, LIGHT_INTENSITY, LIGHT_ATTENUATION, LIGHT_RANGE, LIGHT_PARALLEL_ATTENUATION, LIGHT_ORTHO_ATTENUATION);

	diffuseColor = LIGHT_COLOR * texColor;
	outputF = (DiffuseTerm * diffuseColor);
	out_color = outputF;
}

