uniform mat4 MODELVIEW_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;

attribute vec4 POSITION;
attribute vec4 FACENORMAL;
attribute vec4 NORMAL;

varying vec3 vNormal;
 
void main()
{
	vNormal =  vec3(MODELVIEW_MATRIX * FACENORMAL);
   	vec4 outPos = MODELVIEWPROJECTION_MATRIX * POSITION;
    gl_Position = outPos ;
}



