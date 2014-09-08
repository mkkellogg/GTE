 
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;
uniform sampler2D Texture;
 
attribute vec4 POSITION;
attribute vec3 COLOR;
attribute vec4 NORMAL;
 
varying vec3 vColor;
varying vec3 vNormal;
varying vec4 vPosition;
 
void main()
{
   	vColor = COLOR;
   	vNormal = MODELVIEW_MATRIX * NORMAL;
   	vPosition = MODELVIEW_MATRIX * POSITION;
    gl_Position = PROJECTION_MATRIX * MODELVIEW_MATRIX * POSITION ;
}
