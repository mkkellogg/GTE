uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;

attribute vec4 POSITION;
attribute vec2 UVTEXTURE0;
attribute vec4 NORMAL;
uniform vec4 LIGHT_DIRECTION;
uniform int LIGHT_TYPE;
 
varying vec2 vUVTexture0;
varying vec3 vNormal;
varying vec4 vPosition;
varying vec3 vLightDir;
 
void main()
{
	if(LIGHT_TYPE == 1)
	{
		vLightDir = normalize(LIGHT_DIRECTION.xyz);
	}
   	vUVTexture0 = UVTEXTURE0;
   	vNormal = vec3(MODEL_MATRIX * NORMAL);
   	vPosition = MODEL_MATRIX * POSITION;
    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
}
