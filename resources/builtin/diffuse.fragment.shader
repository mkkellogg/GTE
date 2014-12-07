 
varying vec4 vColor;

vec4 outputF;

uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform int LIGHT_ISDIRECTIONAL;

varying vec3 vNormal;
varying vec4 vPosition;

void main()
{	
	float DiffuseTerm = 1.0;
	
	if(LIGHT_ISDIRECTIONAL == 0)
	{
		vec3 normalized_normal = normalize(vNormal);
		vec3 vertex_to_light_vector = vec3(LIGHT_POSITION - vPosition); 
	    vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	    DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0);
	}
	else
	{
		vec3 normalized_normal = normalize(vNormal);
		vec3 normalized_vertex_to_light_vector = normalize(-LIGHT_DIRECTION.xyz);
		DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * LIGHT_INTENSITY;
	}    
	
    outputF = vColor * DiffuseTerm;
    gl_FragColor = outputF;
}
