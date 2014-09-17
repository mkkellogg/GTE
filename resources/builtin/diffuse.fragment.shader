 
varying vec4 vColor;

vec4 outputF;

uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;

varying vec3 vNormal;
varying vec4 vPosition;

void main()
{	
	vec3 normalized_normal = normalize(vNormal);
	vec3 vertex_to_light_vector = vec3(LIGHT_POSITION - vPosition); 
    vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
 
    // Calculating The Diffuse Term And Clamping It To [0;1]
    float DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0);
	
    outputF = vColor * DiffuseTerm;
    gl_FragColor = outputF;
}
