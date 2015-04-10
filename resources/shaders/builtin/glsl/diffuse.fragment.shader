#version 150

uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform int LIGHT_TYPE;

vec4 outputF;

in vec4 vColor;
in vec3 vNormal;
in vec4 vPosition;

out vec4 out_color;

void main()
{	
	float DiffuseTerm = 0.0;
	
	if(LIGHT_TYPE == 4)
	{
		DiffuseTerm = LIGHT_INTENSITY;
	}
	else if(LIGHT_TYPE == 1)
	{
		vec3 normalized_normal = normalize(vNormal);
		vec3 normalized_vertex_to_light_vector = normalize(-LIGHT_DIRECTION.xyz);
		DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * LIGHT_INTENSITY;
	}
	else if(LIGHT_TYPE == 1)
	{
		vec3 normalized_normal = normalize(vNormal);
		vec3 vertex_to_light_vector = vec3(LIGHT_POSITION - vPosition); 
	    vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	    DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0);
	}    
	
    outputF = vColor * DiffuseTerm;
    out_color = outputF;
}
