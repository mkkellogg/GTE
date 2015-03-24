#version 150

uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;

uniform sampler2D WATER_HEIGHT_MAP;

attribute vec4 POSITION;
attribute vec4 NORMAL;

varying vec4 position;
varying vec4 oPos;
varying vec4 modLocalPos;
varying vec4 normal;
 
void main()
{
	vec2 texCoords = vec2(POSITION.x * 0.5 + 0.5, 0.5 - POSITION.z * 0.5);
	position = MODEL_MATRIX * POSITION ;
	oPos = POSITION; 
	modLocalPos = POSITION;
   // modLocalPos.y += texture2D(WATER_HEIGHT_MAP, texCoords.st).g * 20;
	normal = MODEL_MATRIX * NORMAL ;
    gl_Position = MODELVIEWPROJECTION_MATRIX * modLocalPos ;
}
