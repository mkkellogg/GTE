#version 150

uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;
uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform int LIGHT_TYPE;
uniform int CLIP_PLANE_COUNT;
uniform vec4 CLIP_PLANE0;
uniform float EPSILON;

in vec4 SHADOW_POSITION;

void main()
{	
	vec4 dir = vec4(0.0,0.0,0.0,0.0);
	if(LIGHT_TYPE == 1)
	{
		dir = SHADOW_POSITION.w == 0.0 ? vec4(LIGHT_DIRECTION.xyz, 0.0) : SHADOW_POSITION + normalize(vec4(LIGHT_DIRECTION.xyz, 0.0)) * EPSILON;
	}
	else
	{
		vec4 dirFromLight = vec4(SHADOW_POSITION.xyz - LIGHT_POSITION.xyz, 0.0);
		dir = SHADOW_POSITION.w == 0.0 ? dirFromLight : SHADOW_POSITION + normalize(dirFromLight) * EPSILON;
	}
    gl_Position = MODELVIEWPROJECTION_MATRIX * dir;
    
    if(CLIP_PLANE_COUNT > 0)
    {
    	gl_ClipDistance[0] = dot(MODEL_MATRIX * SHADOW_POSITION, CLIP_PLANE0);
    }
}