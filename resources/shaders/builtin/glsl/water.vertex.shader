uniform mat4 MODELVIEWPROJECTION_MATRIX;

attribute vec4 POSITION;
attribute vec2 UVTEXTURE0;
varying vec2 uv0;
 
void main()
{
	uv0 = UVTEXTURE0;
    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
}
