#version 130

uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;
attribute vec4 POSITION;
attribute vec2 UVTEXTURE0;
attribute vec2 UVTEXTURE1;
uniform int CLIP_PLANE_COUNT;
uniform vec4 CLIP_PLANE0;

uniform vec2 UVTEXTURE0_OFFSET;
uniform vec2 UVTEXTURE1_OFFSET;

varying vec2 uv0;
varying vec2 uv0Offset;

varying vec2 uv1;
varying vec2 uv1Offset;
 
void main()
{
	uv0 = UVTEXTURE0;
	uv0Offset = UVTEXTURE0_OFFSET;
	uv1 = UVTEXTURE1;
	uv1Offset = UVTEXTURE1_OFFSET;
    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
    
    if(CLIP_PLANE_COUNT > 0)
    {
    	gl_ClipDistance[0] = dot(MODEL_MATRIX * POSITION, CLIP_PLANE0);
    }
}
