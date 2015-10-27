#version 150

uniform mat4 MODEL_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 MODEL_MATRIX_INVERSE_TRANSPOSE;
uniform mat4 PROJECTION_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform int CLIP_PLANE_COUNT;
uniform vec4 CLIP_PLANE0;
uniform int LIGHT_TYPE;
uniform vec4 LIGHT_DIRECTION;
in vec4 POSITION;
in vec4 COLOR;
in vec4 NORMAL;

out vec4 vColor;
out vec3 vNormal;
out vec4 vPosition;
out vec3 vLightDir;
 
void main()
{
	if (LIGHT_TYPE == 1 || LIGHT_TYPE == 5)
	{
		vLightDir = normalize(LIGHT_DIRECTION.xyz);
	}
   	vColor = COLOR;
	vNormal = mat3(MODEL_MATRIX_INVERSE_TRANSPOSE) * NORMAL.xyz;
   	vPosition = MODEL_MATRIX * POSITION;
    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
    
    if(CLIP_PLANE_COUNT > 0)
    {
    	gl_ClipDistance[0] = dot(MODEL_MATRIX * POSITION, CLIP_PLANE0);
    }
}
