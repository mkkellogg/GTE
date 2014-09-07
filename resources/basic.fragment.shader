 
varying vec3 vColor;
varying vec2 vUV1;

vec4 outputF;
uniform sampler2D Texture;

uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;

vec4 texColor;

varying vec3 vNormal;
varying vec4 vPosition;

void main()
{
	texColor = texture2D(Texture, vUV1);
	
	vec3 normalized_normal = normalize(vNormal);
	vec3 vertex_to_light_vector = vec3(LIGHT_POSITION - vPosition); 
    vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
 
    // Calculating The Diffuse Term And Clamping It To [0;1]
    float DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0);
	
    outputF = vec4(vColor,1.0) * texColor * DiffuseTerm;
    gl_FragColor = outputF;
}
