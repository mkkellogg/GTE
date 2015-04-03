#version 130

uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
in vec4 POSITION;
in vec4 NORMAL;

uniform sampler2D WATER_HEIGHT_MAP;

out vec4 position;
out vec4 oPos;
out vec4 modLocalPos;
out vec4 normal;
 
void main()
{
	vec2 texCoords = vec2(POSITION.x * 0.5 + 0.5, 0.5 - POSITION.z * 0.5);
	position = MODEL_MATRIX * POSITION ;
	oPos = POSITION; 
	modLocalPos = POSITION;
   // modLocalPos.y += texture(WATER_HEIGHT_MAP, texCoords.st).g * 20;
	normal = MODEL_MATRIX * NORMAL ;
    gl_Position = MODELVIEWPROJECTION_MATRIX * modLocalPos ;
}
