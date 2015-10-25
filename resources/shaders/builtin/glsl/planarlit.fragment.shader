#version 150

uniform sampler2D TEXTURE0;
uniform vec4 PLANE_NORMAL;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;

vec4 texColor;
vec4 outputF;

in vec3 vNormal;
in vec4 vPosition;
in vec3 vLightDir;
in vec2 vUVTexture0;

out vec4 out_color;

void main()
{
	texColor = texture(TEXTURE0, vUVTexture0);
	float DiffuseTerm = 0.0;
	
	/*vec3 normalized_normal = normalize(vNormal);
	vec3 vertex_to_light_vector = vec3(LIGHT_POSITION - vPosition); 
	float light_dist = length(vertex_to_light_vector);
	float attenForLength = LIGHT_ATTENUATION * light_dist;
	float attenuationFactor = max(1.0-attenForLength,0.0);
    vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * attenuationFactor * LIGHT_INTENSITY;*/
	
	vec3 normalized_normal = normalize(vNormal);
	vec3 vertex_to_light_vector = (dot(vPosition.xyz, PLANE_NORMAL.xyz) - abs(PLANE_NORMAL.w)) * PLANE_NORMAL.xyz;
	float light_dist = length(vertex_to_light_vector);
	float attenForLength = LIGHT_ATTENUATION * light_dist;
	float attenuationFactor = max(1.0-attenForLength,0.0);
    vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * attenuationFactor * LIGHT_INTENSITY;
	
    outputF = texColor * DiffuseTerm * LIGHT_COLOR;
    out_color = outputF;
}
