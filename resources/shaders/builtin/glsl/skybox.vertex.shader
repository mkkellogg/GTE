#version 150

uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 VIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;
in vec4 POSITION;

out vec4 TexCoord0;

void main()
{
	TexCoord0 = POSITION;
	vec3 vWorldPos = mat3(MODELVIEW_MATRIX) * POSITION.xyz;
	 gl_Position = (PROJECTION_MATRIX * vec4(vWorldPos, 1.0)).xyww;
	//vec4 vWorldPos = MODELVIEW_MATRIX * POSITION;
	//gl_Position = (PROJECTION_MATRIX * vWorldPos);
}
	