#version 130

uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;
attribute vec4 POSITION;
attribute vec4 NORMAL;
uniform int CLIP_PLANE_COUNT;
uniform vec4 CLIP_PLANE0;

varying vec3 vNormal;
 
void main()
{
	vNormal =  vec3(MODELVIEW_MATRIX * NORMAL);
   	vec4 outPos = MODELVIEWPROJECTION_MATRIX * POSITION;
    gl_Position = outPos ;
    
    if(CLIP_PLANE_COUNT > 0)
    {
    	gl_ClipDistance[0] = dot(MODEL_MATRIX * POSITION, CLIP_PLANE0);
    }
}



