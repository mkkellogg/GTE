uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;

attribute vec4 POSITION;
attribute vec4 COLOR;
 
varying vec4 vColor;
varying vec4 vPosition;
 
void main()
{
   	vColor = COLOR;
   	vPosition = MODEL_MATRIX * POSITION;
    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
}
