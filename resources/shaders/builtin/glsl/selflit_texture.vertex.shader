uniform mat4 MODELVIEWPROJECTION_MATRIX;

attribute vec4 POSITION;
attribute vec2 UVTEXTURE0;
attribute vec2 UVTEXTURE1;

uniform vec2 UVTEXTURE0_OFFSET;

varying vec2 uv0;
varying vec2 uv0Offset;
 
void main()
{
	uv0 = UVTEXTURE0;
	uv0Offset = UVTEXTURE0_OFFSET;

    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
}
