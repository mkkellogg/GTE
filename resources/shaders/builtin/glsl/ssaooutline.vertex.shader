#version 150

uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform int CLIP_PLANE_COUNT;
uniform vec4 CLIP_PLANE0;
in vec4 POSITION;
in vec4 FACENORMAL;
in vec4 NORMAL;

out vec3 vNormal;
 
void main()
{
	vNormal =  vec3(MODELVIEW_MATRIX * FACENORMAL);
   	vec4 outPos = MODELVIEWPROJECTION_MATRIX * POSITION;
    gl_Position = outPos ;
    
    if(CLIP_PLANE_COUNT > 0)
    {
    	gl_ClipDistance[0] = dot(MODEL_MATRIX * POSITION, CLIP_PLANE0);
    }
}



