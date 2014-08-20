 
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;
 
attribute vec4 POSITION;
attribute vec4 COLOR;
 
varying vec3 vColor;
 
void main()
{
    vColor = COLOR;
    gl_Position = PROJECTION_MATRIX * MODELVIEW_MATRIX * POSITION ;
}
