 
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;
uniform sampler2D Texture;
 
attribute vec4 POSITION;
attribute vec3 COLOR;
attribute vec2 UV1;
attribute vec4 NORMAL;
 
varying vec3 vColor;
varying vec2 vUV1;
varying vec3 vNormal;
varying vec4 vPosition;
 
void main()
{
   	vColor = COLOR;
   	vUV1 = UV1;
   	vNormal = vec3(MODELVIEW_MATRIX * NORMAL);
   	vPosition = MODELVIEW_MATRIX * POSITION;
    gl_Position = PROJECTION_MATRIX * MODELVIEW_MATRIX * POSITION ;
}
