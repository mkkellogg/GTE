#version 150
#include "common.inc"

uniform mat4 MODEL_MATRIX;
uniform mat4 MODEL_MATRIX_INVERSE_TRANSPOSE;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;

in vec4 POSITION;
in vec3 COLOR;
in vec2 UVTEXTURE0;
in vec4 NORMAL;
in vec4 TANGENT;
in vec4 FACENORMAL;

uniform float LIGHT_ATTENUATION;
uniform vec4 LIGHT_DIRECTION;
uniform int LIGHT_TYPE;
uniform int CLIP_PLANE_COUNT;
uniform vec4 CLIP_PLANE0;

uniform float USCALE;
uniform float VSCALE;

out vec2 vUVTexture0;
out vec3 vNormal;
out vec3 vFaceNormal;
out vec3 vTangent;
out vec4 vPosition;
out vec3 vLightDir;

void main()
{
	if(LIGHT_TYPE == LIGHT_TYPE_DIRECTIONAL || LIGHT_TYPE == LIGHT_TYPE_PLANAR)
	{
		vLightDir = normalize(LIGHT_DIRECTION.xyz);
	}
	vUVTexture0 = vec2(UVTEXTURE0.s * USCALE, UVTEXTURE0.t * VSCALE);
	vNormal = mat3(MODEL_MATRIX_INVERSE_TRANSPOSE) * NORMAL.xyz;
	vFaceNormal = mat3(MODEL_MATRIX_INVERSE_TRANSPOSE) * FACENORMAL.xyz;
	vTangent = vec3(MODEL_MATRIX * TANGENT);
	vPosition = MODEL_MATRIX * POSITION;
	gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION;

	if(CLIP_PLANE_COUNT > 0)
	{
		gl_ClipDistance[0] = dot(MODEL_MATRIX * POSITION, CLIP_PLANE0);
	}
}
