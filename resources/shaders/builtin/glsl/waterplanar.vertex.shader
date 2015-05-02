#version 150

uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
in vec4 POSITION;

uniform sampler2D WATER_HEIGHT_MAP;
uniform sampler2D WATER_NORMAL_MAP;
//uniform float PIXEL_DISTANCE;

out vec4 position;
out vec4 oPos;

 
void main()
{
	vec2 texCoords = vec2(POSITION.x * 0.5 + 0.5, 0.5 - POSITION.z * 0.5);
	position = MODEL_MATRIX * POSITION ;
	oPos = POSITION; 
	vec4 modLocalPos = POSITION;
	
	float height = texture(WATER_HEIGHT_MAP, texCoords.st).g;
    vec3 normal = normalize(texture(WATER_NORMAL_MAP, texCoords.st).xyz);
    
    // modLocalPos.y += texture(WATER_HEIGHT_MAP, texCoords.st).g * 20;
    // modLocalPos.y += (normal.x + normal.z) * .01;
    // modLocalPos.y += .5;
    
    vec4 finalPosition = MODELVIEWPROJECTION_MATRIX * modLocalPos ; 
    gl_Position = finalPosition;
}
