#version 150

uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;
uniform int CLIP_PLANE_COUNT;
uniform vec4 CLIP_PLANE0;
in vec4 POSITION;
in vec2 UVTEXTURE0;
in vec2 UVTEXTURE1;

uniform vec2 UVTEXTURE0_OFFSET;

out vec2 uv0;
out vec2 uv0Offset;
 
void main()
{
	uv0 = UVTEXTURE0;
	uv0Offset = UVTEXTURE0_OFFSET;

    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
    
    if(CLIP_PLANE_COUNT > 0)
    {
    	gl_ClipDistance[0] = dot(MODEL_MATRIX * POSITION, CLIP_PLANE0);
    }
}
