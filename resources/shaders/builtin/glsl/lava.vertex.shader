#version 130

uniform mat4 MODELVIEWPROJECTION_MATRIX;
in vec4 POSITION;
in vec2 UVTEXTURE0;
in vec2 UVTEXTURE1;

uniform vec2 UVTEXTURE0_OFFSET;
uniform vec2 UVTEXTURE1_OFFSET;

out vec2 uv0;
out vec2 uv0Offset;
out vec2 uv1;
out vec2 uv1Offset;
 
void main()
{
	uv0 = UVTEXTURE0;
	uv0Offset = UVTEXTURE0_OFFSET;
	uv1 = UVTEXTURE1;
	uv1Offset = UVTEXTURE1_OFFSET;
    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
}
