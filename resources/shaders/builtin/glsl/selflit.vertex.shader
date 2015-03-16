#version 130

uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;
attribute vec4 POSITION;
uniform int CLIP_PLANE_COUNT;
uniform vec4 CLIP_PLANE0;

void main()
{
    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
    
    if(CLIP_PLANE_COUNT > 0)
    {
    	gl_ClipDistance[0] = dot(MODEL_MATRIX * POSITION, CLIP_PLANE0);
    }
}
