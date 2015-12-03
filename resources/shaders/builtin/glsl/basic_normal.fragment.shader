#version 150

uniform sampler2D TEXTURE0;
uniform sampler2D NORMALMAP;
uniform vec4 EYE_POSITION;
uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;
uniform int LIGHT_TYPE;
uniform float LIGHT_RANGE;
uniform int LIGHT_PARALLEL_ATTENUATION;
uniform int LIGHT_ORTHO_ATTENUATION;
uniform float SPECULAR_FACTOR;

vec4 texColor;
vec4 outputF;

in vec2 vUVTexture0;
in vec3 vNormal;
in vec3 vFaceNormal;
in vec3 vTangent;
in vec4 vPosition;
in vec3 vLightDir;

out vec4 out_color;

vec3 calcMappedNormal()
{
	vec3 Normal = normalize(vNormal);
	vec3 Tangent = normalize(vTangent);
	Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
	vec3 Bitangent = cross(Tangent, Normal);
	vec3 BumpMapNormal = texture(NORMALMAP, vUVTexture0).xyz;
	BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
	vec3 NewNormal;
	mat3 TBN = mat3(Tangent, Bitangent, Normal);

	NewNormal = TBN * BumpMapNormal;
	NewNormal = normalize(NewNormal);
	return NewNormal;
}

#include "lighting_blinnphong.inc"

void main()
{
	texColor = texture(TEXTURE0, vUVTexture0);
	float DiffuseTerm = 0.0;
	float SpecularTerm = 0.0;

	vec4 specularColor = vec4(0, 0, 0, 0);
	vec4 diffuseColor = vec4(0, 0, 0, 0);
	vec3 surfaceNormal = normalize(vNormal);
	vec4 position = vPosition;

	calcBlinnPhongTermsForLight(LIGHT_TYPE, position, calcMappedNormal(), surfaceNormal, LIGHT_POSITION, vLightDir, EYE_POSITION, LIGHT_INTENSITY, LIGHT_ATTENUATION, LIGHT_RANGE, 
							    LIGHT_PARALLEL_ATTENUATION, LIGHT_ORTHO_ATTENUATION, SPECULAR_FACTOR, DiffuseTerm, SpecularTerm);

	diffuseColor = LIGHT_COLOR * texColor;
	specularColor = LIGHT_COLOR;
	outputF = ((DiffuseTerm * diffuseColor) + (SpecularTerm * specularColor));
	out_color = outputF;
}
