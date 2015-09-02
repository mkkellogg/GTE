#version 150

precision highp float;

uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
in vec4 POSITION;

uniform sampler2D WATER_HEIGHT_MAP;
uniform sampler2D WATER_NORMAL_MAP;

out vec4 position;
out vec4 oPos;

void main()
{
	vec2 texCoords = vec2(POSITION.x * 0.5 + 0.5, 0.5 - POSITION.z * 0.5);
	vec4 modLocalPos = POSITION;
	oPos = POSITION;

	float height = texture(WATER_HEIGHT_MAP, texCoords.st).r;
     
    modLocalPos.y += height;
	
	position = MODEL_MATRIX * modLocalPos ;	 
    gl_Position = MODELVIEWPROJECTION_MATRIX * modLocalPos ; 
}
