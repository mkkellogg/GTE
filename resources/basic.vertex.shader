uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;
uniform sampler2D Texture;
 
attribute vec4 POSITION;
attribute vec3 COLOR;
attribute vec2 UVTEXTURE0;
attribute vec4 NORMAL;
 
varying vec3 vColor;
varying vec2 vUVTexture0;
varying vec3 vNormal;
varying vec4 vPosition;
 
void main()
{
   	vColor = COLOR;
   	vUVTexture0 = UVTEXTURE0;
   	vNormal = vec3(MODEL_MATRIX * NORMAL);
   	vPosition = MODEL_MATRIX * POSITION;
    gl_Position = PROJECTION_MATRIX * MODELVIEW_MATRIX * POSITION ;
}
