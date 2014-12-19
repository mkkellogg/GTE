uniform mat4 MODELVIEWPROJECTION_MATRIX;
attribute vec4 SHADOW_POSITION;
uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform int LIGHT_TYPE;

void main()
{	
	//vec4 dir = SHADOW_POSITION.w == 0.0 ? vec4(SHADOW_POSITION.xyz * LIGHT_POSITION.w - LIGHT_POSITION.xyz, 0.0) : SHADOW_POSITION;
	vec4 dir = vec4(0.0,0.0,0.0,0.0);
	if(LIGHT_TYPE == 1)
	{
		dir = SHADOW_POSITION.w == 0.0 ? vec4(LIGHT_DIRECTION.xyz, 0.0) : SHADOW_POSITION + normalize(vec4(LIGHT_DIRECTION.xyz, 0.0)) * .02;
	}
	else
	{
		vec4 dirFromLight = vec4(SHADOW_POSITION.xyz - LIGHT_POSITION.xyz, 0.0);
		dir = SHADOW_POSITION.w == 0.0 ? dirFromLight : SHADOW_POSITION + normalize(dirFromLight) * 0.02;
	}
    gl_Position = MODELVIEWPROJECTION_MATRIX * dir;
}