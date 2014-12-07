uniform mat4 MVP_MATRIX; 
attribute vec4 POSITION;
uniform vec4 LightPos;

vec4 dir;
 
void main()
{	
	dir = POSITION.w == 0.0 ? vec4(POSITION.xyz * LightPos.w - LightPos.xyz, 0.0) : POSITION;
    gl_Position = MVP_MATRIX * dir;
}