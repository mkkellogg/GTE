#version 120

uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
uniform vec4 CAMERA_POSITION;

attribute vec4 POSITION;
attribute vec4 NORMAL;
attribute vec2 UVTEXTURE0;

varying vec2 uv0;
varying vec4 position;
varying vec4 oPos;
varying vec4 camLocalPos;
varying vec3 normal;
 
void main()
{
	uv0 = UVTEXTURE0;
	position = MODEL_MATRIX * POSITION ;
	oPos = POSITION; 
	camLocalPos = inverse(MODEL_MATRIX) * CAMERA_POSITION;
	mat3 normalMat =  mat3(transpose(inverse(MODEL_MATRIX)));
	normal = normalMat * NORMAL.xyz ;
    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
}
